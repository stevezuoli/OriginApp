//===========================================================================
//  Summary:
//      MobiBookReader接口实现
//  Version:
//      
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2012-03-21
//  Author:
//      Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#include "BookReader/MobiBookReader.h"
#include "DKMAPI.h"
#include "IDKMPage.h"
#include "IDKMTocPoint.h"
#include "interface.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/SelectedControler.h"
#else
#include "AppUI/CursorControler.h"
#endif
#include "Utility.h"
#include "../ImageHandler/DkImageHelper.h"
#include "DKXManager/ReadingInfo/CT_ReadingDataItemImpl.h"
#include "dkWinError.h"
// 用旧接口做数据升级
#include "DKEAPI.h"
#include "IDKEBook.h"
#include "I18n/StringManager.h"
#include "DKXManager/DKX/DKXBlock.h"
#include "KernelVersion.h"
#include "BookReader/PageTable.h"
#include "../inc/Common/SystemSetting_DK.h"

#define MAX_FILE_LENGTH_PER_PAGE (256)
#define MOBI_SETTING_EQUAL(x, y) ((-DoublePrecision < ((x) - (y))) && ((x) - (y)) < DoublePrecision)

MobiBookReader::MobiBookReader()
    : m_pBook(NULL)
    , m_pPage(NULL)
    , m_uFileLength(0)
    , m_lLogicPageCount(0)
    , m_uNewPagePosition(0)
    , m_ePositionType(IDKMBook::LOCATION_PAGE)
    , m_dTextGamma(1.0)
    , m_eFontSmoothType(DK_FONT_SMOOTH_NORMAL)
    , m_dGraphicsGamma(1.0) 
    , m_bIsInstant(false)
    , m_pBookTextController(NULL)
    , m_bookInfoDataSet(false)
    , m_familiarToTraditional(false)
{
    m_tCurPageImage.iColorChannels = 0;
    m_tCurPageImage.iWidth         = 0;
    m_tCurPageImage.iHeight        = 0;
    m_tCurPageImage.iStrideWidth   = 0;
    m_tCurPageImage.pbData         = NULL;
    m_parseOption.msType           = DK_MEASURE_PIXEL;
    m_parseOption.dpi              = RenderConst::SCREEN_DPI;
}

MobiBookReader::~MobiBookReader()
{
    CloseDocument();
}

bool MobiBookReader::OpenDocument(const string &_strFilename)
{
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);
    if (m_pBook)
    {
        CloseDocument();
    }

    m_strBookPath = _strFilename;
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() DKM_OpenDocument %s", __FUNCTION__, m_strBookPath.c_str());
    DK_WCHAR *wsFilePath = EncodingUtil::CharToWChar(m_strBookPath.c_str());
    if (NULL == wsFilePath)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() CharToWChar Error", __FUNCTION__);
        return false;
    }

    m_pBook = (IDKMBook *)DKM_OpenDocument(wsFilePath, NULL);
    DK_FREE(wsFilePath);
    if (NULL == m_pBook)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() DKM_OpenDocument fail", __FUNCTION__);
        return false;
    }
#ifdef KINDLE_FOR_TOUCH
    m_pBookTextController = new SelectedControler();
#else
    m_pBookTextController = new CursorControler();
#endif
    if (NULL == m_pBookTextController)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Get CursorControler Fail", __FUNCTION__);
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Initialize", __FUNCTION__);
    if (DK_FAILED(m_pBook->Initialize()))
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Initialize fail", __FUNCTION__);
        CloseDocument();
        return false;
    }

    DK_ARGBCOLOR tTextColor;
    m_pBook->SetTextColor(tTextColor);

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() GetMainFileLength", __FUNCTION__);
    m_uFileLength = m_pBook->GetMainFileLength();
    m_lLogicPageCount = (long)(m_uFileLength / MAX_FILE_LENGTH_PER_PAGE);
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() m_uFileLength %d", __FUNCTION__, m_uFileLength);

    m_uNewPagePosition = m_pBook->GetTextOffset();
    m_ePositionType = IDKMBook::LOCATION_PAGE;

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);

    PageTable pageTableData;
    pageTableData.pageCount = GetTotalPageNumber();
    CPageTable pageTable(m_strBookPath, DK_GetKernelVersion(), -1);
    pageTableData.strMD5 = 
        SystemSettingInfo::GetInstance()->GetDuokanBookMD5(-1, -1);
    pageTable.SavePageTable(pageTableData);
    return true;
};

bool MobiBookReader::CloseDocument()
{
    SafeDeletePointer(m_pBookTextController);
    SafeFreePointer(m_tCurPageImage.pbData);
	for(size_t i = 0; i < m_vTOCList.size(); i++)
	{
		SafeDeletePointer(m_vTOCList[i]);
	}
    m_vTOCList.clear();
    m_dGraphicsGamma = 1.0;
    m_dTextGamma = 1.0;
    m_lLogicPageCount = 0;
    m_uFileLength = 0;
    DestroyCurPage();
    if (m_pBook)
    {
        DKM_CloseDocument(m_pBook);
        m_pBook = NULL;
    }

    return true;
};

void MobiBookReader::SetTypeSetting(const DKTYPESETTING& typeSetting)
{
	if(m_pBook)
	{
    	m_pBook->SetTypeSetting(typeSetting);
	}
}

bool MobiBookReader::SetPageSize(int _iLeftMargin, int _iTopMargin, int _iWidth, int _iHeight)
{
    (void)_iLeftMargin;
    (void)_iTopMargin;
    if(_iWidth <= 0 || _iHeight <= 0)
    {
        return false;
    }

    m_parseOption.pageBox.X0 = 0.0;
    m_parseOption.pageBox.Y0 = 0.0;
    m_parseOption.pageBox.X1 = _iWidth;
    m_parseOption.pageBox.Y1 = _iHeight;
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() _iWidth  %d", __FUNCTION__, _iWidth);
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() _iHeight %d", __FUNCTION__, _iHeight);
    return true;
}

bool MobiBookReader::SetDefaultFont(const wstring &_strFaceName, DK_CHARSET_TYPE _clsCharSet)
{
    return DKM_SetDefaultFont(_strFaceName.c_str(), _clsCharSet);
}

//因为mobi没有真实页码，故此值仅供估算百分比使用
long MobiBookReader::GetCurrentPageIndex()
{
    if (NULL == m_pBook || NULL == m_pPage)
    {
        return -1;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);

    long lLogicPageIndex = (long)(m_pPage->GetEndPosition() / MAX_FILE_LENGTH_PER_PAGE);
    if (lLogicPageIndex >= m_lLogicPageCount)
    {
        lLogicPageIndex = m_lLogicPageCount - 1;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return lLogicPageIndex;
}

int MobiBookReader::GotoPrevPage()
{
    if (NULL == m_pPage || m_pPage->IsFirstPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);

    m_uNewPagePosition = m_pPage->GetBeginPosition();
    m_ePositionType = IDKMBook::PREV_PAGE;

    if (!CreateCurPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    if (!RenderCurPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return BOOKREADER_GOTOPAGE_SUCCESS;
}

int MobiBookReader::GotoNextPage()
{
    if (NULL == m_pPage || m_pPage->IsLastPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);

    m_uNewPagePosition = m_pPage->GetEndPosition();
    m_ePositionType = IDKMBook::NEXT_PAGE;

    if (!CreateCurPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    if (!RenderCurPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return BOOKREADER_GOTOPAGE_SUCCESS;
}

vector<DK_TOCINFO*>* MobiBookReader::GetTOC(DKXBlock* curDKXBlock)
{
	if(m_pBook && curDKXBlock && m_vTOCList.empty())
	{
		 //从文件中获取TOC目录
	    ReadTocFromDKX(curDKXBlock);
		if(m_vTOCList.empty())
		{
	        const IDKMTocPoint *pTocRoot = m_pBook->GetTocRoot();
			if(pTocRoot)
			{
				if (ParseTocInfo(pTocRoot))
	            {
					SaveTocToDKX(curDKXBlock);
	            }
			}
		}

		//mobi只支持一级目录
		for(unsigned int i = 0; i < m_vTOCList.size(); i++)
	    {
	    	m_vTOCList[i]->SetDepth(1);
	    }
	}
	return &m_vTOCList;
}

void MobiBookReader::ReadTocFromDKX(DKXBlock* curDKXBlock)
{
    if(curDKXBlock && m_vTOCList.empty())
    {
        vector<TxtTocItem> vTocList = curDKXBlock->GetTxtToc();
	    unsigned int uSize = vTocList.size();
		if(curDKXBlock->GetTocExtractedState() && uSize > 0)
		{
		    for(unsigned int i = 0; i < uSize; i++)
		    {
		        TxtTocItem clsItem = vTocList[i];
		        DK_TOCINFO * pclsTOC = new DK_TOCINFO();
		        if(!pclsTOC)
		        {
		            break;
		        }
		        CT_RefPos clsPos = clsItem.GetRefPos();

		        DK_FLOWPOSITION _clsPosition(0, clsPos.GetParaIndex(), clsPos.GetAtomIndex());
		        pclsTOC->SetCurTOC(clsItem.GetTitle(), _clsPosition);
		        m_vTOCList.push_back(pclsTOC);
		    }
		}
    }
	return;
}

void MobiBookReader::SaveTocToDKX(DKXBlock* curDKXBlock)
{
    if(!m_pBook || !curDKXBlock)
    {
        return;
    }

	if(curDKXBlock->GetTocExtractedState())
	{
		return;
	}

    if(curDKXBlock->GetTxtToc().size() > 0)
    {
        return;
    }

	vector<TxtTocItem> vTocList;
	for(unsigned int i = 0; i < m_vTOCList.size(); i++)
    {
        TxtTocItem _clsTocItem;
		DK_FLOWPOSITION beginPos = m_vTOCList[i]->GetBeginPos();
        _clsTocItem.SetTitle(m_vTOCList[i]->GetChapter());
        _clsTocItem.SetRefPos(CT_RefPos(0, beginPos.nParaIndex, beginPos.nElemIndex, 0));
        vTocList.push_back(_clsTocItem);
    }

    curDKXBlock->SetTxtToc(vTocList);
    curDKXBlock->SetTocExtractedState(true);
    curDKXBlock->Serialize();
}

int MobiBookReader::MoveToPage(long _lPageNumber)
{
    if (NULL == m_pBook)
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s(%ld / %ld) Start", __FUNCTION__, _lPageNumber, m_lLogicPageCount);

    if (_lPageNumber < 0)
    {
        _lPageNumber = 0;
    }

    if (_lPageNumber >= m_lLogicPageCount)
    {
        _lPageNumber = m_lLogicPageCount - 1;
    }

    m_uNewPagePosition = (unsigned int)((double)m_uFileLength * _lPageNumber / m_lLogicPageCount);
    m_ePositionType = IDKMBook::LOCATION_PAGE;

    if (!CreateCurPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    if (!RenderCurPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return BOOKREADER_GOTOPAGE_SUCCESS;
}

int MobiBookReader::MoveToFlowPosition(const DK_FLOWPOSITION &_pos)
{
	if(NULL == m_pBook)
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    m_uNewPagePosition = _pos.nElemIndex;
    m_ePositionType = IDKMBook::LOCATION_PAGE;

    if (!CreateCurPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    if (!RenderCurPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return BOOKREADER_GOTOPAGE_SUCCESS;
}

string MobiBookReader::GetCurrentPageText()
{
    if (NULL == m_pBook || NULL == m_pPage)
    {
        return "";
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);
    string ret = "";
    DK_WCHAR *pContent = DK_NULL;
    if (DKR_OK == m_pBook->GetTextOfRange(m_pPage->GetBeginPosition(), m_pPage->GetEndPosition(), &pContent))
    {
        DK_CHAR *pstrContent = EncodingUtil::WCharToChar(pContent, EncodingUtil::UTF8);
        if (NULL != pstrContent)
        {
            ret = pstrContent;
            DK_FREE(pstrContent);
        }

        m_pBook->FreeText(pContent);
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return ret;
}

bool MobiBookReader::SearchInCurrentBook(const string& _strText, const DK_FLOWPOSITION& _startPos, const int& _iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* _puResultCount)
{
    if(NULL == m_pBook || !_pSearchResultData || !_puResultCount || _iMaxResultCount <= 0)
    {
        return false;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);

    DK_WCHAR* pText = EncodingUtil::CharToWChar(_strText.c_str());
    if (NULL == pText)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() CharToWChar Fail", __FUNCTION__);
        return false;
    }

    // 调用IDKMBook查找接口
    DK_UINT *pPositionList = NULL;
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() start to search at %d for %s", __FUNCTION__, _startPos.nElemIndex, _strText.c_str());
    if (DK_FAILED(m_pBook->FindText(_startPos.nElemIndex, pText, _iMaxResultCount, &pPositionList, _puResultCount))
        || !pPositionList || !(*_puResultCount))
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() FindText Fail", __FUNCTION__);
        DK_FREE(pText);
        return false;
    }

    // 遍历查找结果，存入_pSearchResultData
    for(unsigned int i = 0; i < *_puResultCount; i++)
    {
        SafeDeletePointer(_pSearchResultData->pSearchResultTable[i]);
        _pSearchResultData->pSearchResultTable[i] = new CT_ReadingDataItemImpl(ICT_ReadingDataItem::BOOKMARK);
		if(!_pSearchResultData->pSearchResultTable[i])
		{
			continue;
		}

        // 记录位置
        CT_RefPos _clsTempPos(0, 0,(int)(pPositionList[2 * i]), -1);
		_pSearchResultData->pSearchResultTable[i]->SetBeginPos(_clsTempPos);
        _pSearchResultData->pSearchResultTable[i]->SetEndPos(CT_RefPos(0, 0, (int)(pPositionList[2 * i + 1]), -1));

        // 记录百分比
        if (0 < m_uFileLength)
        {
            _pSearchResultData->pLocationTable[i] = 100 * (pPositionList[2 * i] + 1) / m_uFileLength;
        }
        else
        {
            _pSearchResultData->pLocationTable[i] = -1;
        }

        // 记录文本信息
		unsigned int _uHighLightenStartPos = 0, _uHighLightenEndPos = 0;
		string _str = GetText(_clsTempPos, _strText, _uHighLightenStartPos, _uHighLightenEndPos);
        _pSearchResultData->pHighlightStartPosTable[i] = _uHighLightenStartPos;
        _pSearchResultData->pHighlightEndPosTable[i] = _uHighLightenEndPos;
        _pSearchResultData->pSearchResultTable[i]->SetBookContent(_str);
    }

    m_pBook->FreeFindTextResults(pPositionList);
    DK_FREE(pText);

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return true;
}

bool MobiBookReader::GetCurrentPageATOM(ATOM & beginATOM, ATOM& endATOM)
{
    if (NULL == m_pPage)
    {
        return false;
    }

    beginATOM.SetChapterIndex(0);
    beginATOM.SetParaIndex(0);
    beginATOM.SetAtomIndex(m_pPage->GetBeginPosition());
    endATOM.SetChapterIndex(0);
    endATOM.SetParaIndex(0);
    endATOM.SetAtomIndex(m_pPage->GetEndPosition());
    return true;
}

string MobiBookReader::GetChapterTitle(const DK_FLOWPOSITION& posChapter, DKXBlock* curDKXBlock)
{
	GetTOC(curDKXBlock);
	int chapterIndex = GetChapterIndexByPosition(posChapter.nElemIndex);
	if(chapterIndex > 0 && chapterIndex < (int)m_vTOCList.size())
	{
		DK_TOCINFO* pTocInfo = m_vTOCList.at(chapterIndex);
		if(pTocInfo)
		{
			return pTocInfo->GetChapter();
		}
	}
	return "";
}

ICT_ReadingDataItem * MobiBookReader::GetCurPageProgress()
{
	if(!m_pPage)
	{
		return NULL;
	}
    ICT_ReadingDataItem* _pclsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);
    if(!_pclsReadingProgress)
    {
        return NULL;
    }

    time_t _tTime;
    time(&_tTime);
    string _strTime  = DkFormat::CUtility::TransferTimeToString(_tTime);
    _pclsReadingProgress->SetCreateTime(_strTime);
    _pclsReadingProgress->SetCreator("DuoKan");

	CT_RefPos clsBeginPos(0, 0, 0, -1);
	unsigned int uStartOffset = m_pPage->GetBeginPosition();
	clsBeginPos.SetAtomIndex(uStartOffset);
	unsigned int endOffset = m_pPage->GetEndPosition();
	//因为区间为左闭右开，计算end 时要- 1
	if(endOffset != 0)
	{
		--endOffset;
	}
    CT_RefPos clsEndPos;
    clsEndPos.SetChapterIndex(0);
    clsEndPos.SetParaIndex(0);
    clsEndPos.SetAtomIndex(endOffset);
    CompletePosRange(&clsBeginPos, &clsEndPos);
	
    _pclsReadingProgress->SetBeginPos(clsBeginPos);
    _pclsReadingProgress->SetEndPos(clsEndPos);
    _pclsReadingProgress->SetLastModifyTime(_strTime);
	return _pclsReadingProgress;
}

bool MobiBookReader::GotoBookMark(const ICT_ReadingDataItem *_pReadingEntry)
{
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);

    if(!_pReadingEntry || !m_pBook)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Fail", __FUNCTION__);
        return false;
    }

    m_uNewPagePosition = (_pReadingEntry->GetKernelVersion().empty())? 0: _pReadingEntry->GetBeginPos().GetAtomIndex();
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() m_uNewPagePosition %d", __FUNCTION__, m_uNewPagePosition);
    m_ePositionType = IDKMBook::LOCATION_PAGE;

    if (!CreateCurPage())
    {
        return false;
    }

    if (!RenderCurPage())
    {
        return false;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return true;
}

string MobiBookReader::GetText(const ATOM & _clsStartPos, const string _strSearchText, unsigned int & _uHighLightenStartPos, unsigned int & _uHighLightenEndPos)
{
    if (NULL == m_pBook)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() m_pBook is Null", __FUNCTION__);
        return _strSearchText;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);

    DK_WCHAR *pSearchText = EncodingUtil::CharToWChar(_strSearchText.c_str());
    if (NULL == pSearchText)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() CharToWChar Fail", __FUNCTION__);
        return _strSearchText;
    }

    const DK_WCHAR *pEllipsis          = L"...";
    unsigned int    uSnippetLen        = 90;
    DK_WCHAR        pResultString[200] = {0};
    if(DK_FAILED(m_pBook->GetFindTextSnippet((unsigned int)(_clsStartPos.GetAtomIndex()), 
                                             pSearchText, 
                                             pEllipsis, 
                                             uSnippetLen, 
                                             pResultString, 
                                             &_uHighLightenStartPos, 
                                             &_uHighLightenEndPos)))
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() GetFindTextSnippet Fail", __FUNCTION__);
        DK_FREE(pSearchText);
        return _strSearchText;
    }

    DK_FREE(pSearchText);

    char *pResult = EncodingUtil::WCharToChar(pResultString);
    if (NULL == pResult)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() WCharToChar Fail", __FUNCTION__);
        return _strSearchText;
    }

    string strResult(pResult);
    DK_FREE(pResult);

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return strResult;
}

bool MobiBookReader::IsPositionInCurrentPage(const DK_FLOWPOSITION &_pos)
{
    if (NULL == m_pBook || NULL == m_pPage)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() m_pPage is Null", __FUNCTION__);
        return false;
    }

	unsigned int offset = _pos.nElemIndex;
	//区间为左闭右开
	return (offset >= m_pPage->GetBeginPosition() && offset < m_pPage->GetEndPosition());
}

long MobiBookReader::GetPageNumber4CurrentChapter()
{
	return -1;
}

bool MobiBookReader::AddBMPProcessor(IBMPProcessor* _pProcessor)
{
    assert (NULL != _pProcessor);
    if (m_lstBMPProcessor.end() != find(m_lstBMPProcessor.begin(), m_lstBMPProcessor.end(), _pProcessor))
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() IBMPProcessor is Existing", __FUNCTION__);
        return true;
    }

    m_lstBMPProcessor.push_back(_pProcessor);
    return m_lstBMPProcessor.size() > 0;
}

bool MobiBookReader::RemoveBMPProcessor(IBMPProcessor* _pProcessor)
{
    assert (NULL != _pProcessor);
    std::vector<IBMPProcessor*>::iterator vi;
    vi = find(m_lstBMPProcessor.begin(), m_lstBMPProcessor.end(), _pProcessor);
    if (vi == m_lstBMPProcessor.end())
        return true;

    m_lstBMPProcessor.erase(vi);
    return true;
}

bool MobiBookReader::GetAllBMPProcessor(std::vector<IBMPProcessor*> &_ProcessorList)
{
    _ProcessorList = m_lstBMPProcessor;
    return true;
}

bool MobiBookReader::SetLayoutSettings(const DK_LAYOUTSETTING &_clsLayoutSetting)
{
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);

    m_parseOption.bodyFontSize    = _clsLayoutSetting.dFontSize;
    m_parseOption.lineGap         = _clsLayoutSetting.dLineGap;
    double paraSpacing = _clsLayoutSetting.dParaSpacing - (m_parseOption.lineGap - 1);
    if (paraSpacing < 0)
    {
        paraSpacing = 0;
    }
    m_parseOption.paraSpacing     = paraSpacing;
    m_parseOption.tabStop         = _clsLayoutSetting.dTabStop;
    m_parseOption.firstLineIndent = _clsLayoutSetting.dIndent;
	DKM_SetUseBookStyle(_clsLayoutSetting.bOriginalLayout);
    DebugPrintf(DLC_DIAGNOSTIC, "MobiBookReader::%s() bodyFontSize    %lf", __FUNCTION__, m_parseOption.bodyFontSize);
    DebugPrintf(DLC_DIAGNOSTIC, "MobiBookReader::%s() lineGap         %lf", __FUNCTION__, m_parseOption.lineGap);
    DebugPrintf(DLC_DIAGNOSTIC, "MobiBookReader::%s() paraSpacing     %lf", __FUNCTION__, m_parseOption.paraSpacing);
    DebugPrintf(DLC_DIAGNOSTIC, "MobiBookReader::%s() tabStop         %lf", __FUNCTION__, m_parseOption.tabStop);
    DebugPrintf(DLC_DIAGNOSTIC, "MobiBookReader::%s() firstLineIndent %lf", __FUNCTION__, m_parseOption.firstLineIndent);
    DebugPrintf(DLC_DIAGNOSTIC, "MobiBookReader::%s() msType %ld", __FUNCTION__, m_parseOption.msType);
    DebugPrintf(DLC_DIAGNOSTIC, "MobiBookReader::%s() dpi %ld", __FUNCTION__, m_parseOption.dpi);
    DebugPrintf(DLC_DIAGNOSTIC, "MobiBookReader::%s() pageBox (%f, %f)-(%f, %f)",
            __FUNCTION__,
            m_parseOption.pageBox.X0,
            m_parseOption.pageBox.Y0,
            m_parseOption.pageBox.X1,
            m_parseOption.pageBox.Y1
            );

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return true;
}

bool MobiBookReader::UpdateReadingData(
        const char*,
        const std::vector<ICT_ReadingDataItem *> *_pvOld,
        const char*,
        std::vector<ICT_ReadingDataItem *> *_pvNew)
{
    if (NULL == _pvOld || NULL == _pvNew || 0 == _pvOld->size() || 0 != _pvNew->size())
    {
        return false;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() DKM_OpenDocument %s", __FUNCTION__, m_strBookPath.c_str());
    DK_WCHAR *wsFilePath = EncodingUtil::CharToWChar(m_strBookPath.c_str());
    if (NULL == wsFilePath)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() CharToWChar Error", __FUNCTION__);
        return false;
    }

    IDKEBook *pBook = (IDKEBook *)DKM_OpenDocument(wsFilePath, NULL);
    DK_FREE(wsFilePath);
    if (NULL == pBook)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() DKE_OpenMOBIDocument fail", __FUNCTION__);
        return false;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() PrepareParseContent", __FUNCTION__);
    if (DK_FAILED(pBook->PrepareParseContent(DK_NULL)))
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() PrepareParseContent fail", __FUNCTION__);
        DKM_CloseDocument(pBook);
        return false;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Begin to update", __FUNCTION__);
    for (unsigned int i = 0; i < _pvOld->size(); ++i)
    {
        CT_RefPos begin = (*_pvOld)[i]->GetBeginPos();
        DK_FLOWPOSITION beginPos((unsigned int)(begin.GetChapterIndex()), 
                                 (unsigned int)(begin.GetParaIndex()), 
                                 (unsigned int)(begin.GetAtomIndex()));

        CT_RefPos end = (*_pvOld)[i]->GetEndPos();
        DK_FLOWPOSITION endPos((unsigned int)(end.GetChapterIndex()), 
                               (unsigned int)(end.GetParaIndex()), 
                               (unsigned int)(end.GetAtomIndex()));

        if (ICT_ReadingDataItem::PROGRESS == (*_pvOld)[i]->GetUserDataType())
        {
            --(endPos.nChapterIndex);
        }

        DK_UINT beginChapter = 0;
        DK_OFFSET beginOffset = 0;
        DK_UINT endChapter = 0;
        DK_OFFSET endOffset = 0;
        if (DKR_OK == pBook->DKFlowPositionRange2OffsetRange(beginPos, endPos, &beginChapter, &beginOffset, &endChapter, &endOffset))
        {
            DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() beginChapter %d, beginOffset %d", __FUNCTION__, beginChapter, beginOffset);
            DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() endChapter %d, endOffset %d", __FUNCTION__, endChapter, endOffset);

            ICT_ReadingDataItem *pclsReadingDate = DKXReadingDataItemFactory::CreateReadingDataItem();
            if (pclsReadingDate)
            {
                pclsReadingDate->SetCreateTime((*_pvOld)[i]->GetCreateTime());
                pclsReadingDate->SetCreator("DuoKan");

                begin.SetChapterIndex(0);
                begin.SetParaIndex(0);
                begin.SetAtomIndex(beginOffset);
                pclsReadingDate->SetBeginPos(begin);

                end.SetChapterIndex(0);
                end.SetParaIndex(0);
                end.SetAtomIndex(endOffset);
                pclsReadingDate->SetEndPos(end);

                pclsReadingDate->SetLastModifyTime((*_pvOld)[i]->GetLastModifyTime());
                pclsReadingDate->SetBookContent((*_pvOld)[i]->GetBookContent());
                pclsReadingDate->SetUserContent((*_pvOld)[i]->GetUserContent());
                pclsReadingDate->SetUserDataType((*_pvOld)[i]->GetUserDataType());
                pclsReadingDate->SetUploaded((*_pvOld)[i]->IsUploaded());

                _pvNew->push_back(pclsReadingDate);
            }
        }
    }

    DKM_CloseDocument(pBook);
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return true;
}

bool MobiBookReader::CreateCurPage()
{
    if (NULL == m_pBook)
    {
        return false;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);

    m_pBook->SetParseOption(m_parseOption);

    DebugPrintf(DLC_PENGF, "MobiBookReader::%s() m_uNewPagePosition %d", __FUNCTION__, m_uNewPagePosition);
    DebugPrintf(DLC_PENGF, "MobiBookReader::%s() m_ePositionType %d [P %d, L %d, N %d]", 
                            __FUNCTION__, m_ePositionType, 
                            IDKMBook::PREV_PAGE, IDKMBook::LOCATION_PAGE, IDKMBook::NEXT_PAGE);
    IDKMPage *pNewPage = m_pBook->CreatePage(m_uNewPagePosition, m_ePositionType);
    if (NULL == pNewPage)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() CreatePage fail", __FUNCTION__);
        return false;
    }

    DestroyCurPage();
    m_pPage = pNewPage;
	DebugPrintf(DLC_PENGF, "MobiBookReader::%s() m_uNewPagePosition %d..............", __FUNCTION__, m_pPage->GetBeginPosition());

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return true;
}

void MobiBookReader::DestroyCurPage()
{
    if (m_pBook)
    {
        if (m_pBookTextController)
        {
            m_pBookTextController->Release();
        }

        if (m_pPage)
        {
            m_pBook->DestroyPage(m_pPage);
            m_pPage = NULL;
        }
    }
}

bool MobiBookReader::RenderCurPage()
{
    if (NULL == m_pBook || NULL == m_pPage)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() m_pPage is NULL", __FUNCTION__);
        return false;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);
    DKE_SetGBToBig5(m_familiarToTraditional);
    // 当前页面的图像信息设置，初始为32色，需要经过BMPProcessor的处理
    SafeFreePointer(m_tCurPageImage.pbData);
    m_tCurPageImage.iColorChannels = 4;
    m_tCurPageImage.iWidth         = (int)m_parseOption.pageBox.Width();
    m_tCurPageImage.iHeight        = (int)m_parseOption.pageBox.Height();
    m_tCurPageImage.iStrideWidth   = m_tCurPageImage.iWidth * m_tCurPageImage.iColorChannels;

    long iDataLen = m_tCurPageImage.iStrideWidth * m_tCurPageImage.iHeight;
    m_tCurPageImage.pbData = DK_MALLOCZ_OBJ_N(BYTE8, iDataLen);
    if (NULL == m_tCurPageImage.pbData)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() m_tCurPageImage.pbData is NULL", __FUNCTION__);
        return false;
    }

    // DEV信息根据DK_IMAGE来生成
    DK_BITMAPBUFFER_DEV dev;
    dev.lWidth       = m_tCurPageImage.iWidth;
    dev.lHeight      = m_tCurPageImage.iHeight;
    dev.nPixelFormat = DK_PIXELFORMAT_RGB32;
    dev.lStride      = m_tCurPageImage.iStrideWidth;
    dev.nDPI         = RenderConst::SCREEN_DPI;
    dev.pbyData      = m_tCurPageImage.pbData;

    DK_RENDERGAMMA  gma;
    gma.dTextGamma = m_dTextGamma;

    DK_FLOWRENDEROPTION renderOption;
    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice     = &dev;
    renderOption.gamma       = gma;
	renderOption.fontSmoothType = m_eFontSmoothType;
    if (m_bIsInstant)
    {
        renderOption.fontSmoothType = DK_FONT_SMOOTH_BINARY;
    }
    
    memset(dev.pbyData, 0xFF, iDataLen); //画布颜色变为白色
    DK_FLOWRENDERRESULT clsDKERenderResult;

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::RenderPage Render!!!");
    if (DK_FAILED(m_pPage->Render(renderOption, &clsDKERenderResult)))
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::RenderPage Render ERROR!!!");
        return false;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::RenderPage Render SUCCESS!!!");

    // 抖动当前图像
    DitherImage();

    if (m_pBookTextController)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::RenderPage m_pBookTextController Initialize!!!");
        m_pBookTextController->Initialize<IDKMPageTraits>(m_pBook, m_pPage);
        m_pBookTextController->SetModeController(GetPdfModeController());
    }

    std::vector<IBMPProcessor *>::iterator vi;
    for (vi = m_lstBMPProcessor.begin(); vi != m_lstBMPProcessor.end(); ++vi)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::RenderPage Process Image!!!");
        if (NULL == (*vi) || !(*vi)->Process(&m_tCurPageImage))
        {
            DebugPrintf(DLC_ZHAIGH, "MobiBookReader::RenderPage Process ERROR!!!");
            return false;
        }
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::RenderPage FINISHED!!!");
    return true;
}

void MobiBookReader::DitherImage()
{
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() ConvertRGB32ToGray256", __FUNCTION__);
    if (!DkFormat::DkImageHelper::ConvertRGB32ToGray256(&m_tCurPageImage))
    {
        return;
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() GetImageIterator", __FUNCTION__);
    IDKEImageIterator *pImageIterator = m_pPage->GetImageIterator();
    if (pImageIterator)
    {
        // 避免死循环
        for (int i = 0; i < 1000; ++i)
        {
            if (pImageIterator->MoveToNext())
            {
                DK_BOX cropBox = pImageIterator->GetCurrentImageBox();
                DK_RECT cropRect;
                cropRect.left   = (int)(cropBox.X0);
                cropRect.right  = (int)(cropBox.X1);
                cropRect.top    = (int)(cropBox.Y0);
                cropRect.bottom = (int)(cropBox.Y1);
                DK_IMAGE toDither;
                if (S_OK == CropImage(m_tCurPageImage, cropRect, &toDither))
                {
                    if (m_bIsInstant)
                    {
                        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() DitherGray256ToBinary", __FUNCTION__);
                        DkFormat::DkImageHelper::DitherGray256ToBinary(toDither);
                    }
                    else
                    {
                        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() DitherGray256ToGray16", __FUNCTION__);
                        DkFormat::DkImageHelper::DitherGray256ToGray16(toDither);
                    }
                }
            }
            else
            {
                break;
            }
        }

        m_pPage->FreeImageIterator(pImageIterator);
        pImageIterator = NULL;
    }

    if (m_bIsInstant)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() ConvertGray256ToBinary", __FUNCTION__);
        DkFormat::DkImageHelper::ConvertGray256ToBinary(m_tCurPageImage);
    }
    else
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() ConvertGray256ToGray16", __FUNCTION__);
        DkFormat::DkImageHelper::ConvertGray256ToGray16(m_tCurPageImage);
    }
}

bool MobiBookReader::ParseTocInfo(const IDKMTocPoint *_pTocPoint)
{
    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Start", __FUNCTION__);

    if (!_pTocPoint)
    {
        DebugPrintf(DLC_ZHAIGH, "MobiBookReader::ParseTocInfo _pTocPoint is NULL!!!");
        return false;
    }

    const DK_WCHAR *pTitle = _pTocPoint->GetTitle();
    if (NULL != pTitle)
    {
    	string title = EncodeUtil::ToString(pTitle);
        if (!title.empty())
        {
            DK_FLOWPOSITION clsFlowPos;
            clsFlowPos.nElemIndex = _pTocPoint->GetDestPosition();
			DK_TOCINFO *pChildToc = new DK_TOCINFO();
			if(pChildToc)
			{
				pChildToc->SetCurTOC(title, clsFlowPos);
            	m_vTOCList.push_back(pChildToc);
			}
        }
    }
	
    unsigned int iCount = _pTocPoint->GetChildCount();
    for (unsigned int i = 0; i < iCount; ++i)
    {
        const IDKMTocPoint *pChild = _pTocPoint->GetChildByIndex(i);
        if (pChild)
		{
			ParseTocInfo(pChild);
		}
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return true;
}

unsigned int MobiBookReader::GetChapterIndexByPosition(unsigned int _uPosition)
{
	unsigned int uChapterIndex = 0;
    if(!m_vTOCList.empty())
	{
		for (uChapterIndex = 0; uChapterIndex + 1 < m_vTOCList.size(); ++uChapterIndex)
	    {
	        if (_uPosition < (unsigned int)m_vTOCList.at(uChapterIndex + 1)->GetBeginPos().nElemIndex)
	        {
	            break;
	        }
	    }
	}
    return uChapterIndex;
}

void MobiBookReader::CompletePosRange(CT_RefPos* startPos, CT_RefPos* endPos)
{
    if (DK_NULL == startPos || DK_NULL == endPos)
    {
        return;
    }
    startPos->SetOffsetInChapter(startPos->GetAtomIndex());
    endPos->SetOffsetInChapter(endPos->GetAtomIndex());
}

BOOK_INFO_DATA MobiBookReader::GetBookInfo()
{
    if (m_bookInfoDataSet)
    {
        return m_bookInfoData;
    }
    m_bookInfoDataSet = true;
    if (NULL == m_pBook)
    {
        return m_bookInfoData;
    }

    const DKMBookInfo* bookInfo = m_pBook->GetBookInfo();
    if (bookInfo->pAuthor && 0 != wcslen(bookInfo->pAuthor))
    {
        char* utf8Author = EncodingUtil::WCharToChar(bookInfo->pAuthor, EncodingUtil::UTF8);
        if (utf8Author)
        {
            m_bookInfoData.author = utf8Author;
            DK_FREE(utf8Author);
        }
    }

    if (bookInfo->pTitle)
    {
        char* utf8Title = EncodingUtil::WCharToChar(bookInfo->pTitle, EncodingUtil::UTF8);
        if (utf8Title)
        {
            m_bookInfoData.title = utf8Title;
            DK_FREE(utf8Title);
        }
    }

    const DKFILEINFO* coverFileInfo = m_pBook->GetBookCover();
    if (NULL != coverFileInfo) 
    {
        m_bookInfoData.coverData = coverFileInfo->pFileDataBuf;
        m_bookInfoData.coverDataLen = coverFileInfo->lFileDataLength;
    }
    return m_bookInfoData;
}



double MobiBookReader::GetPercent(const CT_RefPos& startPos) 
{
    double ret = (double)-1;
    if (m_uFileLength > 0)
    {
        const double atomIndex = (double)((startPos.GetAtomIndex() + 1) * 100);
        const double atomCount = (double)m_uFileLength;
        ret = atomIndex / atomCount;
    }
    return ret;
}

bool MobiBookReader::GetPosByPageIndex(int pageIndex, DK_FLOWPOSITION* pos)
{
	if(pos && pageIndex >= 0)
	{
		*pos = DK_FLOWPOSITION(0, 0, (unsigned int)((double)m_uFileLength*pageIndex/m_lLogicPageCount));
		return true;
	}
	return false;
}

const DK_TOCINFO* MobiBookReader::GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode, DKXBlock* curDKXBlock)
{
	GetTOC(curDKXBlock);
	int chapterIndex = GetChapterIndexByPosition(posPage.nElemIndex);
	chapterIndex += (positionMode - CURRENT_CHAPTER);
	if(m_pBook && chapterIndex >= 0 && chapterIndex < (int)m_vTOCList.size())
	{
		DK_TOCINFO* pChapterTocInfo = m_vTOCList.at(chapterIndex);
		if(pChapterTocInfo)
		{
			DK_FLOWPOSITION newChapterPos;
			DK_UINT uTmpPagePosition = pChapterTocInfo->GetBeginPos().nElemIndex;
			IDKMPage *pNewPage = m_pBook->CreatePage(uTmpPagePosition, IDKMBook::LOCATION_PAGE);
			if(!pNewPage)
			{
				return false;
			}
	//		newChapterPos.nChapterIndex = chapterIndex;
			newChapterPos.nElemIndex = pNewPage->GetBeginPosition();
			pChapterTocInfo->SetBeginPos(newChapterPos);
			m_pBook->DestroyPage(pNewPage);
			return pChapterTocInfo;
		}
	}
	return NULL;
}

bool MobiBookReader::SetFontSmoothType(DK_FONT_SMOOTH_TYPE fontSmoothType)
{
    m_eFontSmoothType = fontSmoothType;
    return true;
}

//bool MobiBookReader::GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem *>* vBookmarks)
//{
//	if(curDKXBlock && vBookmarks)
//	{
//		if(curDKXBlock->GetAllBookmarks(vBookmarks))
//		{
//			for(size_t i = 0; i < vBookmarks->size(); i++)
//			{
//				ICT_ReadingDataItem* readingData = vBookmarks->at(i);
//				CT_RefPos beginPos = readingData->GetBeginPos();
//				beginPos.SetChapterIndex(1);
//				CT_RefPos endPos = readingData->GetEndPos();
//				endPos.SetChapterIndex(1);
//				readingData->SetBeginPos(beginPos);
//				readingData->SetEndPos(endPos);
//			}
//			return true;
//		}
//	}
//	return false;
//}

bool MobiBookReader::GetDKXProgressForUpgrade(DKXBlock* curDKXBlock, ICT_ReadingDataItem* localReadingDataItem)
{
	if(curDKXBlock && localReadingDataItem)
	{
		if(curDKXBlock->GetProgress(localReadingDataItem))
		{
			CT_RefPos beginPos = localReadingDataItem->GetBeginPos();
			beginPos.SetChapterIndex(1);
			CT_RefPos endPos = localReadingDataItem->GetEndPos();
			endPos.SetChapterIndex(1);
			localReadingDataItem->SetBeginPos(beginPos);
			localReadingDataItem->SetEndPos(endPos);
			return true;
		}
	}
	return false;
}

bool MobiBookReader::ProcessCloudBookmarks(ReadingBookInfo& bookInfo)
{
	for(size_t i = 0; i < bookInfo.GetReadingDataItemCount(); i++)
	{
		CT_RefPos beginPos = bookInfo.GetReadingDataItem(i).GetBeginRefPos();
		CT_RefPos endPos = bookInfo.GetReadingDataItem(i).GetEndRefPos();
		CT_RefPos refPos = bookInfo.GetReadingDataItem(i).GetRefPos();
		beginPos.SetChapterIndex(0);
		endPos.SetChapterIndex(0);
		refPos.SetChapterIndex(0);
		bookInfo.UpdateItemPos(i, beginPos, endPos, refPos);
	}
	return true;
}

bool MobiBookReader::ProcessCloudDKXProgress(ICT_ReadingDataItem* localReadingDataItem)
{
	CT_RefPos beginPos = localReadingDataItem->GetBeginPos();
	beginPos.SetChapterIndex(0);
	CT_RefPos endPos = localReadingDataItem->GetEndPos();
	endPos.SetChapterIndex(0);
	localReadingDataItem->SetBeginPos(beginPos);
	localReadingDataItem->SetEndPos(endPos);
	return false;
}

