//===========================================================================
// Summary:
//        IBookReader接口定义
// Version:
//    0.1 draft - creating.
//    0.2 implementing
// Usage:
//        IBookReader的Txt实现
// Remarks:
//        Null
// Date:
//        2011-12-31
// Author:
//        Juntao Liu (liujt@duokan.com)
//===========================================================================

#include "BookReader/TextBookReader.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/SelectedControler.h"
#else
#include "AppUI/CursorControler.h"
#endif
#include "DKTAPI.h"
#include "DKXManager/DKX/DKXBlock.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "FontManager/DKFont.h"
#include "interface.h"
#include "KernelMacro.h"
#include "KernelType.h"
#include "../Common/FileManager_DK.h"
#include "../../Kernel/include/TxtKit/TxtLib/DKTAPI.h"
#include "Utility.h"
#include "KernelVersion.h"
#include <string>
#include "Framework/INativeMessageQueue.h"
#include "DkStreamFactory.h"

using DkFormat::CUtility;

//TODO: static member used only in this implementation
#define PAGE_STEP 20

TextBookReader::TextBookReader()
: m_pBook(NULL)
, m_pCurPage(NULL)
, m_lFileSize(0)
, m_iBookID(-1)
, m_iPageWidth(-1)
, m_iPageHeight(-1)
, m_dTextGamma(1.0f)
, m_eFontSmoothType(DK_FONT_SMOOTH_NORMAL)
, m_bTypeset(true)
, m_uLastPageOffset(0)
, m_bIsInstant(false)
, m_bIsFirstOpen(true)
, m_pParseThread(0)
, m_bIsCancelThread(false)
, m_eParseStatus(BOOKREADER_PARSE_ERROR)
, m_lCurPageIndex(-1)
, m_strDemo("")
{
    DebugPrintf(DLC_LIWEI, "TextBookReader::TextBookReader() Start###");

    m_tCurPageImage.iColorChannels = 4;
    m_tCurPageImage.iWidth         = m_iPageWidth;
    m_tCurPageImage.iHeight        = m_iPageHeight;
    m_tCurPageImage.iStrideWidth        = m_tCurPageImage.iWidth * m_tCurPageImage.iColorChannels;
    m_tCurPageImage.pbData = NULL;

    m_ParserOption.pageBox  = DK_BOX(0, 0, m_iPageWidth, m_iPageHeight);
    m_ParserOption.msType   = DK_MEASURE_PIXEL;
    m_ParserOption.dpi = RenderConst::SCREEN_DPI;

#ifdef KINDLE_FOR_TOUCH
    m_pBookTextController = new SelectedControler();
#else
    m_pBookTextController = new CursorControler();
#endif

    DebugPrintf(DLC_LIWEI, "TextBookReader::TextBookReader FINISHED!!!");
}

TextBookReader::~TextBookReader()
{
    DebugPrintf(DLC_LIWEI, "TextBookReader::~TextBookReader() Start###");
    CloseDocument();
    SafeDeletePointerEx(m_pBookTextController);
    DebugPrintf(DLC_LIWEI, "TextBookReader::~TextBookReader() FINISHED!!!");
}

bool TextBookReader::OpenDocument(const string &_strFilePath)
{
    DebugPrintf(DLC_LIWEI, "TextBookReader::OpenDocument() Start###");
    m_bIsFirstOpen = true;
    if (m_pBook)
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::OpenDocument Again");
        CloseDocument ();
    }

    DebugPrintf(DLC_LIWEI, "TextBookReader::OpenDocument begin to contert char to wchar");
    
    wstring wsFilePath = EncodeUtil::ToWString(_strFilePath);    
    DebugPrintf(DLC_LIWEI, "TextBookReader::OpenDocument begin to open Doc");
    m_pBook = (IDKTBook*)DKT_OpenDocument(wsFilePath.c_str(), DK_NULL);
    if(!m_pBook)
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::OpenDocument Failed to open 04");
        return false;
    }
    DK_ReturnCode res = m_pBook->PrepareParseContent(MAX_CHARS_PER_PAGE);

    if(!DK_SUCCEEDED(res))
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::OpenDocument Failed at PrepareParseContent()");
        return false;
    }
    m_strBookPath = _strFilePath;
    m_lFileSize = CUtility::GetFileLength(m_strBookPath.c_str());
    DebugPrintf(DLC_LIWEI, "TextBookReader::OpenDocument() End!!!");
    return true;
}

bool TextBookReader::OpenDocumentFromString(const string& strDemo)
{
	m_strDemo = strDemo;
	IDkStream* pDemoStream = DkStreamFactory::GetMemoryStream((DK_VOID*)m_strDemo.c_str(), m_strDemo.length(), m_strDemo.length());
	if(NULL == pDemoStream)
	{
		return false;
	}
    
    if (m_pBook)
    {
        CloseDocument();
    }
		
    m_pBook = (IDKTBook*)DKT_OpenDocumentFromStream(pDemoStream, DK_NULL);
	SafeDeletePointer(pDemoStream);
	pDemoStream = NULL;
    if(!m_pBook)
    {
        return false;
    }

    DK_ReturnCode res = m_pBook->PrepareParseContent(MAX_CHARS_PER_PAGE);
    if(!DK_SUCCEEDED(res))
    {
        return false;
    }
    return true;
}

void TextBookReader::SetTypeSetting(const DKTYPESETTING& typeSetting)
{
	if(m_pBook)
	{
    	m_pBook->SetTypeSetting(typeSetting);
	}
}

BOOKREADER_PARSE_STATUS TextBookReader::GetParseStatus()
{
    return m_eParseStatus;
}

void TextBookReader::Parse()
{
    DK_UINT pageOffset = 0;
    m_vPageStartOffset.push_back(pageOffset);
    while(DKR_OK == m_pBook->CalcNextPageOffset(m_ParserOption, pageOffset, &pageOffset))
    {
        m_vPageStartOffset.push_back(pageOffset);
        if(m_bIsCancelThread)
        {
            m_eParseStatus = BOOKREADER_PARSE_ERROR;
            m_bIsCancelThread = false;
            return;
        }
    }
	if(m_strBookPath != "")
	{
		CPageTable cPageTable(m_strBookPath, DK_GetKernelVersion(), PAGE_STEP);
		string strMD5 = SystemSettingInfo::GetInstance()->GetDuokanBookMD5(-1, -1);
		PageTable pageTable;
		for(int i = 0; i < (int)m_vPageStartOffset.size(); i += PAGE_STEP)
		{
			pageTable.vPageOffset.push_back(m_vPageStartOffset[i]);
		}
		pageTable.strMD5 = strMD5;
		pageTable.pageCount = m_vPageStartOffset.size();
		cPageTable.SavePageTable(pageTable);
	}

    m_eParseStatus = BOOKREADER_PARSE_FINISHED;
    pthread_detach(pthread_self());
    m_pParseThread = 0;

	if(m_strBookPath != "")
	{
		UIBookParseStatusListener::GetInstance()->FireBookParseFinishedEvent();
	}
	DebugPrintf(DLC_DIAGNOSTIC, "TextBookReader Send message");

}

void* TextBookReader::ParseThread(void* _pParam)
{
    TextBookReader* pThis = (TextBookReader*)_pParam;
    if(!pThis)
    {
        return NULL;
    }
    pThis->Parse();
    return (void*)0;
}

bool TextBookReader::ReParseContent()
{
    StopParseThread();
	if(GetPageTableData())
	{
		if(m_vPageStartOffset.size() != 0)
		{
			m_eParseStatus = BOOKREADER_PARSE_FINISHED;
			m_lCurPageIndex = GetPageIndexFormParseResult(m_pCurPage->GetOffsetInByte());
		}
		return true;
	}
	
    m_eParseStatus = BOOKREADER_PARSE_START;
    m_vPageStartOffset.clear();
    m_lCurPageIndex = -1;
	
    if (0 != ThreadHelper::CreateThread(&m_pParseThread, ParseThread, this, "TextBookReader @ ReParseContent", false, 32768))
    {
        m_eParseStatus = BOOKREADER_PARSE_ERROR;
        return false;
    }
    return true;
}

void TextBookReader::StopParseThread()
{
    if(m_pParseThread)
    {
        m_bIsCancelThread = true;
        ThreadHelper::JoinThread(m_pParseThread, NULL);
        m_pParseThread = 0;
    }
}

bool TextBookReader::IsPositionInCurrentPage(const DK_FLOWPOSITION &curPos)
{
    if(!m_pBook || !m_pCurPage)
    {
        return false;
    }

    unsigned int startOffset = m_pCurPage->GetOffsetInByte();
    unsigned int endOffset = startOffset + m_pCurPage->GetSizeInByte();
    
    return curPos.nElemIndex >= startOffset && curPos.nElemIndex < endOffset;
}

bool TextBookReader::GotoBookMark(const ICT_ReadingDataItem* readingEntry)
{
    if(!readingEntry || !m_pBook)
    {
        return false;
    }
    CT_RefPos clsRefPos = readingEntry->GetBeginPos();
    unsigned int offset = (unsigned int)clsRefPos.GetAtomIndex();
	m_lCurPageIndex = -1;

	if(m_bIsFirstOpen && !m_strBookPath.empty() && GetPageTableData())
	{
		
		if(m_vPageStartOffset.size() != 0)
		{
			m_eParseStatus = BOOKREADER_PARSE_FINISHED;
			m_bTypeset = false;
		}
	}
    m_bIsFirstOpen = false;

	if(BOOKREADER_PARSE_FINISHED == m_eParseStatus)
	{
		m_lCurPageIndex = GetPageIndexFormParseResult(offset);
		offset = GetOffSetByPageIndex(m_lCurPageIndex);
	}
	
    GotoPage(offset, IDKTBook::NEXT_PAGE);
    return ProduceBMPPage();
}

bool TextBookReader::CloseDocument()
{
    if (m_pBookTextController)
    {
        m_pBookTextController->Release();
    }
    StopParseThread();
    if (m_pBook)
    {
        DestroyCurPage();

        DKT_CloseDocument(m_pBook);
        m_pBook = NULL;
    }

    SafeFreePointer(m_tCurPageImage.pbData);
	for(size_t i = 0; i < m_vTOCList.size(); i++)
	{
		SafeDeletePointerEx(m_vTOCList[i]);
	}
    m_vTOCList.clear();
    //BMP Processor is singleton It's not necessary to release the handle;
    m_vBMPProcessor.clear();
    m_bTypeset = true;
    return true;
}

bool TextBookReader::SetPageSize(int _iLeftMargin, int _iTopMargin, int iWidth, int iHeight)
{
    (void)_iLeftMargin;
    (void)_iTopMargin;
    if(iWidth <= 0 || iHeight <= 0)
    {
        return false;
    }

    m_bTypeset = m_bTypeset || (m_iPageWidth != iWidth || m_iPageHeight != iHeight);

    m_iPageWidth = iWidth;
    m_iPageHeight = iHeight;
    
    m_ParserOption.pageBox = DK_BOX(0, 0, m_iPageWidth, m_iPageHeight);
    m_tCurPageImage.iWidth          = m_iPageWidth;
    m_tCurPageImage.iHeight         = m_iPageHeight;
    m_tCurPageImage.iStrideWidth    = m_tCurPageImage.iWidth * m_tCurPageImage.iColorChannels;

    return true;
}

bool TextBookReader::SetLayoutSettings(const DK_LAYOUTSETTING &_clsLayoutSetting)
{
    DebugPrintf(DLC_LIWEI, "TextBookReader::SetLayoutSettings() Start### m_LayoutSetting.dFontSize %d - _clsLayoutSetting.dFontSize %d", m_LayoutSetting.dFontSize, _clsLayoutSetting.dFontSize);

    bool _bSettingChanged = abs(m_LayoutSetting.dFontSize - _clsLayoutSetting.dFontSize) >= DoublePrecision ||
        abs(m_LayoutSetting.dIndent - _clsLayoutSetting.dIndent) >= DoublePrecision || 
        abs(m_LayoutSetting.dLineGap - _clsLayoutSetting.dLineGap) >= DoublePrecision ||
        abs(m_LayoutSetting.dParaSpacing - _clsLayoutSetting.dParaSpacing) >= DoublePrecision ||
        abs(m_LayoutSetting.dTabStop - _clsLayoutSetting.dTabStop) >= DoublePrecision;
    if(_bSettingChanged)
    {
        m_LayoutSetting.dFontSize = _clsLayoutSetting.dFontSize;
        m_LayoutSetting.dIndent = _clsLayoutSetting.dIndent;
        m_LayoutSetting.dLineGap = _clsLayoutSetting.dLineGap;
        m_LayoutSetting.dParaSpacing = _clsLayoutSetting.dParaSpacing;
        m_LayoutSetting.dTabStop = _clsLayoutSetting.dTabStop;
   
        DebugPrintf(DLC_DIAGNOSTIC, "TextBookReader::SetLayoutSettings font size=%d", m_LayoutSetting.dFontSize);
        DebugPrintf(DLC_DIAGNOSTIC, "TextBookReader::SetLayoutSettings dIndent=%f", m_LayoutSetting.dIndent);
        DebugPrintf(DLC_DIAGNOSTIC, "TextBookReader::SetLayoutSettings dLineGap=%f", m_LayoutSetting.dLineGap);
        DebugPrintf(DLC_DIAGNOSTIC, "TextBookReader::SetLayoutSettings dParaSpacing=%f", m_LayoutSetting.dParaSpacing);
        DebugPrintf(DLC_DIAGNOSTIC, "TextBookReader::SetLayoutSettings dTabStop=%f", m_LayoutSetting.dTabStop);
    }
    m_bTypeset = m_bTypeset || _bSettingChanged || (m_LayoutSetting.bOriginalLayout != _clsLayoutSetting.bOriginalLayout);

    m_LayoutSetting.bOriginalLayout = _clsLayoutSetting.bOriginalLayout;

	m_pBook->SetFirstLineIndent(m_LayoutSetting.dIndent);
	m_pBook->SetFontSize((double)m_LayoutSetting.dFontSize);
	m_pBook->SetLineGap(m_LayoutSetting.dLineGap);
	m_pBook->SetParaSpacing(m_LayoutSetting.dParaSpacing);
	m_pBook->SetTabStop(m_LayoutSetting.dTabStop);
	m_pBook->SetTextColor(DK_ARGBCOLOR(255, 0, 0, 0));
	m_pBook->SetBlankLineStrategy(IDKTBook::BLANKLINE_IGNOREALL);
	DKT_SetUseBookStyle(m_LayoutSetting.bOriginalLayout);
    DebugPrintf(DLC_DIAGNOSTIC, "TextBookReader::SetLayoutSettings originalLayoUt=%d", m_LayoutSetting.bOriginalLayout);
    return true;
}


bool TextBookReader::ReRenderPage()
{
    unsigned int lCurPageOffset = m_pCurPage ? m_pCurPage->GetOffsetInByte() : 0;
    if(m_eParseStatus == BOOKREADER_PARSE_FINISHED)
    {
        m_lCurPageIndex = (m_lCurPageIndex == -1) ? GetPageIndexFormParseResult(lCurPageOffset) : m_lCurPageIndex;
    }
    else
    {
        m_lCurPageIndex = -1;
    }
    GotoPage(lCurPageOffset, IDKTBook::NEXT_PAGE);
    return ProduceBMPPage();
}

long TextBookReader::GetCurrentPageIndex()
{
    DebugPrintf(DLC_LIWEI, "TextBookReader::GetCurrentPageIndex() Start###");
    if(!m_pBook || !m_pCurPage)
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::GetCurrentPageIndex() failed on m_pBook && m_pCurPage");
        return -1;
    }
    DebugPrintf(DLC_LIWEI, "TextBookReader::GetCurrentPageIndex() End###");
	if(m_eParseStatus == BOOKREADER_PARSE_FINISHED && m_lCurPageIndex == -1)
    {
        return GetPageIndexFormParseResult(m_pCurPage->GetOffsetInByte());
    }
    return m_lCurPageIndex;
}

int TextBookReader::GotoPrevPage()
{
    DebugPrintf(DLC_LIWEI, "TextBookReader::GotoPrevPage() Start###");
    unsigned int lCurPageOffset = m_pCurPage? m_pCurPage->GetOffsetInByte() : 0;
    bool bResult = false;
    if(lCurPageOffset <= 0)
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::GotoPrevPage() failed (lCurPageOffset <= 0) ###");
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    if(m_eParseStatus == BOOKREADER_PARSE_FINISHED)
    {
        m_lCurPageIndex = (m_lCurPageIndex == -1) ? GetPageIndexFormParseResult(lCurPageOffset) : m_lCurPageIndex;
        m_lCurPageIndex--;
        if(m_lCurPageIndex < 0)
        {
            m_lCurPageIndex = 0;
        }
        lCurPageOffset = GetOffSetByPageIndex(m_lCurPageIndex);
        bResult = GotoPage(lCurPageOffset, IDKTBook::NEXT_PAGE);
    }
    else
    {
        bResult = GotoPage(lCurPageOffset, IDKTBook::PREV_PAGE);
    }
    if(!bResult)
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::GotoPrevPage() failed (GotoPage ) ###");
        return BOOKREADER_GOTOPAGE_FAIL;
    }  
    
    if(!ProduceBMPPage())
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::GotoPrevPage() failed (ProduceBMPPage ) ###");
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    DebugPrintf(DLC_LIWEI, "TextBookReader::GotoPrevPage() End###");
    return BOOKREADER_GOTOPAGE_SUCCESS;
}

int TextBookReader::GotoNextPage()
{
    DebugPrintf(DLC_LIWEI, "TextBookReader::GotoNextPage() start###");
    unsigned int lPageOffset = m_pCurPage->GetOffsetInByte();
    bool bResult = false;
    if(m_eParseStatus == BOOKREADER_PARSE_FINISHED)
    {
        m_lCurPageIndex = (m_lCurPageIndex == -1) ? GetPageIndexFormParseResult(lPageOffset) : m_lCurPageIndex;
        m_lCurPageIndex++;
        if((int)m_lCurPageIndex > (int)m_vPageStartOffset.size() - 2)
        {
            m_lCurPageIndex = m_vPageStartOffset.size() - 2;
        }
        lPageOffset = GetOffSetByPageIndex(m_lCurPageIndex);
    }
    else
    {
        lPageOffset +=  m_pCurPage->GetSizeInByte();
    }
	bResult = GotoPage(lPageOffset, IDKTBook::NEXT_PAGE);
    if(!bResult)
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }  
    
    if(!ProduceBMPPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }
    
    DebugPrintf(DLC_LIWEI, "TextBookReader::GotoNextPage() End###");
    return BOOKREADER_GOTOPAGE_SUCCESS;
}

bool TextBookReader::SetFamiliarToTraditional(bool toTraditional)
{
	DKT_SetGBToBig5(toTraditional);
	return true;
}

bool TextBookReader::GotoPage(unsigned int pageOffset, IDKTBook::PAGE_POSITION_TYPE position)
{
    DebugPrintf(DLC_LIWEI, "TextBookReader::GotoPage() Begin###");

    if(!m_pBook || pageOffset == (size_t)-1)
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::GotoPage return 000");
        return false;
    }

    if (m_pBookTextController)
    {
        m_pBookTextController->Release();
    }

	if(m_bTypeset)
	{
		m_bTypeset = false;
		ReParseContent();
	}
	
	IDKTPage* page = DK_NULL;
	DK_ReturnCode res = DKR_OK;
	if(m_eParseStatus == BOOKREADER_PARSE_FINISHED && m_lCurPageIndex == -1)
	{
		m_lCurPageIndex =  GetPageIndexFormParseResult(pageOffset);
	}
	res = m_pBook->CreatePage(m_ParserOption, pageOffset, position, &page);

	if(!DK_SUCCEEDED(res))
	{
		return false;
	}

	if(page && !page->GetSizeInByte())
	{
		m_pBook->DestroyPage(page);
		page = NULL;
		return false;
	}

    DestroyCurPage();
    m_pCurPage = page;
    m_uLastPageOffset = pageOffset;
    return true;
}

bool TextBookReader::ProduceBMPPage()
{
    DebugPrintf(DLC_LIWEI, "TextBookReader::ProduceBMPPage Start###");

    if(!m_pCurPage)
    {
        return false;
    }

    // 当前页面的图像信息设置，初始为32色，需要经过BMPProcessor的处理 
    if (DK_NULL != m_tCurPageImage.pbData)
    {        
        SafeFreePointer(m_tCurPageImage.pbData);
    }

    //TODO: assumption comes from Duokan Kernel Render
    m_tCurPageImage.iColorChannels = 4;
    m_tCurPageImage.iWidth         = (int)m_ParserOption.pageBox.Width();
    m_tCurPageImage.iHeight        = (int)m_ParserOption.pageBox.Height();
    m_tCurPageImage.iStrideWidth   = m_tCurPageImage.iWidth * m_tCurPageImage.iColorChannels;
    long iDataLen = m_tCurPageImage.iStrideWidth * m_tCurPageImage.iHeight;
    
    m_tCurPageImage.pbData         = DK_MALLOCZ_OBJ_N(DK_BYTE, iDataLen);
    if (DK_NULL == m_tCurPageImage.pbData)
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::ProduceBMPPage m_tCurPageImage.pbData is NULL!!!");
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
    
    DebugPrintf(DLC_LIWEI, "TextBookReader::ProduceBMPPage Render!!!");
    memset(dev.pbyData, 0xFF, iDataLen); //画布颜色变为白色

    DK_FLOWRENDERRESULT  RenderResult;
    m_pCurPage->Render(renderOption, &RenderResult);

    if (m_pBookTextController)
    {
        m_pBookTextController->Initialize<IDKTPageTraits>(m_pBook, m_pCurPage);
        m_pBookTextController->SetModeController(GetPdfModeController());
    }

    DebugPrintf(DLC_LIWEI, "TextBookReader::ProduceBMPPage begtin to process!!!");
    vector<IBMPProcessor*>::iterator vi;
    for (vi = m_vBMPProcessor.begin(); vi != m_vBMPProcessor.end(); ++vi)
    {
        if(!(*vi))
        {
            continue;
        }
        if (!(*vi)->Process(&m_tCurPageImage))
        {
            DebugPrintf(DLC_LIWEI, "TextBookReader::ProduceBMPPage Process ERROR!!!");
            return false;
        }
    }

    DebugPrintf(DLC_LIWEI, "TextBookReader::ProduceBMPPage End###");
    return true;
}

#ifdef DEBUG
void TextBookReader::ReadTocFromDKX2()
{
    if(m_pTOCRoot)
    {
        return;
    }

    DKXManager * _pclsManager = DKXManager::GetInstance();
    if(!_pclsManager)
    {
        return;
    }

    m_pTOCRoot = new DK_TOCINFO();
    if(!m_pTOCRoot)
    {
        return;
    }

    for(unsigned int i = 0; i < 3; i++)
    {
        DK_TOCINFO * _pclsTOCLevel3 = new DK_TOCINFO();
        if(!_pclsTOCLevel3)
        {
            break;
        }
        DK_FLOWPOSITION _clsPosition3(4-i, 4-i, 4-i);
        string abc(3, 'A'+i);
        _pclsTOCLevel3->SetCurTOC(abc, _clsPosition3);

        DK_TOCINFO * _pclsTOCLevel2 = new DK_TOCINFO();
        if(!_pclsTOCLevel2)
        {
            break;
        }
        string def(2, 'A'+i);
        DK_FLOWPOSITION _clsPosition2(3-i , 3-i, 3-i);
        _pclsTOCLevel2->SetCurTOC(def, _clsPosition2);
        _pclsTOCLevel2->AppendChild(_pclsTOCLevel3);

        DK_TOCINFO * _pclsTOCLevel1 = new DK_TOCINFO();
        if(!_pclsTOCLevel1)
        {
            break;
        }

        DK_FLOWPOSITION _clsPosition1(1-i, 1-i, 1-i);
        string abcd(1, 'A'+i);
        _pclsTOCLevel1->SetCurTOC(abcd, _clsPosition1);
        _pclsTOCLevel1->AppendChild(_pclsTOCLevel2);

        m_pTOCRoot->AppendChild(_pclsTOCLevel1);
    }
}
#endif

void TextBookReader::ReadTocFromDKX(DKXBlock* curDKXBlock)
{
    if(!m_vTOCList.empty() || !curDKXBlock)
    {
        return;
    }

    vector<TxtTocItem> _vTocList = curDKXBlock->GetTxtToc();
    unsigned int _uSize = _vTocList.size();
    for(unsigned int i = 0; i < _uSize; i++)
    {
        TxtTocItem _clsItem = _vTocList[i];
        DK_TOCINFO * _pclsTOC = new DK_TOCINFO();
        if(!_pclsTOC)
        {
            break;
        }
        CT_RefPos _clsPos = _clsItem.GetRefPos();

        DK_FLOWPOSITION _clsPosition(0, _clsPos.GetParaIndex(), _clsPos.GetAtomIndex());
        _pclsTOC->SetCurTOC(_clsItem.GetTitle(), _clsPosition);
		_pclsTOC->SetDepth(1);
        m_vTOCList.push_back(_pclsTOC);
    }
}

vector<DK_TOCINFO*>* TextBookReader::GetTOC(DKXBlock* curDKXBlock)
{
    if(m_pBook && curDKXBlock && m_vTOCList.empty())
    {
         //从内核获取TOC，并存储在DKX中
	    SaveTocToDKX(curDKXBlock);
	    //从文件中获取TOC目录
	    ReadTocFromDKX(curDKXBlock);
    }
	return &m_vTOCList;
}

//判断当前文档是否有TOC，如果没有，则调用接口获得TOC目录后存入DKX
void TextBookReader::SaveTocToDKX(DKXBlock* curDKXBlock)
{
#ifndef __LINUX_X86__

    if(!m_pBook)
    {
        return;
    }

    vector<TxtTocItem> _vTocList = curDKXBlock->GetTxtToc();
    unsigned int _uSize = _vTocList.size();
    if(_uSize) //already saved in DKX
    {
        return;
    }
    bool _bHasExactTOC = curDKXBlock->GetTocExtractedState();
    if(_bHasExactTOC)
    {
        return;
    }

    unsigned int *offsetResult = NULL;
    unsigned int tocCount = 0;

    DK_ReturnCode res = m_pBook->GetTOC(&offsetResult, &tocCount);
    if(DKR_OK != res)
    {
        return;
    }
    
    _vTocList.clear();
    for(unsigned int i = 0; i < tocCount; i++)
    {
        DK_WCHAR _wchTitle[IDKTBook::MAX_CHAPTER_TITLE+1] = {0};
        m_pBook->GetChapterTitle(offsetResult[i], _wchTitle);
        string strTitle = EncodeUtil::ToString(_wchTitle);
        
        TxtTocItem _clsTocItem;
        _clsTocItem.SetTitle(strTitle);
        CT_RefPos _clsPos(i, 0, offsetResult[i], -1);
        _clsTocItem.SetRefPos(_clsPos);
        _vTocList.push_back(_clsTocItem);
    }

    curDKXBlock->SetTxtToc(_vTocList);
    curDKXBlock->SetTocExtractedState(true);
    curDKXBlock->Serialize();
    m_pBook->FreeTOC(offsetResult);
#endif

}

string TextBookReader::GetCurrentPageText()
{
    if(!m_pCurPage)
    {
        return "";
    }
    
    return EncodeUtil::ToString(m_pCurPage->GetAllText());
}

string TextBookReader::GetChapterTitle(const DK_FLOWPOSITION& posChapter, DKXBlock* curDKXBlock)
{
	if(m_vTOCList.empty())
	{
		GetTOC(curDKXBlock);
	}
	int chapterIndex = GetChapterIndex(posChapter.nElemIndex, curDKXBlock);
	if(chapterIndex >= 0 && chapterIndex < (int)m_vTOCList.size())
	{
		DK_TOCINFO* pTocInfo = m_vTOCList.at(chapterIndex);
		if(pTocInfo)
		{
			return pTocInfo->GetChapter();
		}
	}
	return "";
}

bool TextBookReader::GetPosByPageIndex(int pageIndex, DK_FLOWPOSITION* pos)
{
	if(pos && pageIndex >= 0)
	{
		int offset = GetOffSetByPageIndex(pageIndex);
		if(offset != -1)
		{
		    *pos = DK_FLOWPOSITION(0, 0, offset);
			return true;
		}
	}
	return false;
}

long TextBookReader::GetPageIndex(const CT_RefPos& startPos)
{
    return GetPageIndexFormParseResult(startPos.GetAtomIndex());
}

bool TextBookReader::GetPageTableData()
{
	CPageTable cPageTable(m_strBookPath, DK_GetKernelVersion(), PAGE_STEP);
	string strMD5 = SystemSettingInfo::GetInstance()->GetDuokanBookMD5(-1, -1);
	PageTable pageTable = cPageTable.GetPageTable(strMD5);
	if(pageTable.IsNull())
	{
		return false;
	}
	
	m_vPageStartOffset.assign(pageTable.pageCount, -1);
	for(int i = 0 , j = 0; j < (int)pageTable.vPageOffset.size(); i += PAGE_STEP , j++)
	{
		m_vPageStartOffset[i] = pageTable.vPageOffset[j];
	}
	return true;
}

int TextBookReader::GetPageIndexFormParseResult(int iOffset)
{

    if(m_eParseStatus != BOOKREADER_PARSE_FINISHED)
    {
        return -1;
    }
	//计算该书末尾的位置，方便其他计算
	GetOffSetByPageIndex(m_vPageStartOffset.size() - 1);
	
	int startPageIndex = -1;
	int endPageIndex = -1;
    for(int i = 0; i <= (int)m_vPageStartOffset.size() - 1; i++)
    {
		if(m_vPageStartOffset[i] != -1 && iOffset < m_vPageStartOffset[i])
		{
			endPageIndex = i;
			break;
		}
    }
	startPageIndex = (endPageIndex - PAGE_STEP) > 0 ? (endPageIndex - PAGE_STEP) : 0;
	endPageIndex = endPageIndex > (int)m_vPageStartOffset.size() - 1 ? ((int)m_vPageStartOffset.size() - 1) : endPageIndex;
	for(int i = startPageIndex; i <= endPageIndex; i++)
	{
		if(iOffset < GetOffSetByPageIndex(i))
		{
			return i - 1;
		}
	}
    return m_vPageStartOffset.size() - 2;
}

int TextBookReader::GetOffSetByPageIndex(int pageIndex)
{
	if(m_eParseStatus != BOOKREADER_PARSE_FINISHED || pageIndex > (int)m_vPageStartOffset.size() - 1)
	{
		return -1;
	}
	
	if(m_vPageStartOffset[pageIndex] == -1)
	{
		int beforeValidPageIndex = pageIndex - pageIndex % PAGE_STEP;
		int afterValidPageIndex = pageIndex + (PAGE_STEP - pageIndex % PAGE_STEP);
		beforeValidPageIndex = beforeValidPageIndex < 0 ? 0 : beforeValidPageIndex;
		afterValidPageIndex = afterValidPageIndex > (int)m_vPageStartOffset.size() - 1 ? m_vPageStartOffset.size() - 1 : afterValidPageIndex;
		ParsePartContent(beforeValidPageIndex, afterValidPageIndex);
	}
	return m_vPageStartOffset[pageIndex];
}

bool TextBookReader::ParsePartContent(int startPageIndex, int endPageIndex)
{
	int pageOffset = m_vPageStartOffset[startPageIndex];
	if(pageOffset == -1)
	{
		return false;
	}
	DK_UINT parsePageOffset = pageOffset;
	for(int i = startPageIndex; i <= endPageIndex;)
	{
		if(DKR_OK != m_pBook->CalcNextPageOffset(m_ParserOption, parsePageOffset, &parsePageOffset))
		{
			return false;
		}
		i++;
		m_vPageStartOffset[i] = parsePageOffset;
	}
	return true;
}

bool TextBookReader::GetCurrentPageATOM(ATOM & beginATOM, ATOM& endATOM)
{
    if(!m_pBook || !m_pCurPage)
    {
        return false;
    }

    unsigned int pageOffset = m_pCurPage->GetOffsetInByte();
    beginATOM.SetAtomIndex(pageOffset);
    beginATOM.SetChapterIndex(0);
    beginATOM.SetParaIndex(0);
    endATOM.SetAtomIndex(pageOffset + m_pCurPage->GetSizeInByte());
    endATOM.SetChapterIndex(0);
    endATOM.SetParaIndex(0);
    return true;
}
string TextBookReader::GetText(const ATOM & clsStartPos, const string strSearchText, unsigned int & highLightenStartPos, unsigned int & highLightenEndPos)
{
    if(!m_pBook || !m_pCurPage)
    {
        return strSearchText;
    }

    wstring wstrText = EncodeUtil::ToWString(strSearchText.c_str());

    const DK_WCHAR* wchEllipsis = L"...";
    unsigned int _uSnippetLen = 90;
    DK_WCHAR wchResultString[200] = {0};
    
    DK_ReturnCode res = this->m_pBook->GetFindResultSnippet((unsigned int)clsStartPos.GetAtomIndex(), wstrText.c_str(), wchEllipsis, _uSnippetLen, wchResultString, &highLightenStartPos, &highLightenEndPos);
    if(DKR_OK != res)
    {
        return strSearchText;
    }

    return EncodeUtil::ToString(wchResultString);
}

bool TextBookReader::SearchInCurrentBook(const string& _strText, const DK_FLOWPOSITION& _startPos, const int& _iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* _puResultCount) 
{
    if(!m_pBook || _strText.empty() || !_pSearchResultData || !_puResultCount || _iMaxResultCount <= 0)
    {
        return false;
    }

    wstring wstrText = EncodeUtil::ToWString(_strText.c_str());
    unsigned int _uStartOffset = _startPos.nElemIndex;
    unsigned int *_uOffsetResultList(NULL);
        
    // 调用IDKTBook查找接口
    DK_ReturnCode _dkCode = m_pBook->Find(_uStartOffset, wstrText.c_str(), _iMaxResultCount, &_uOffsetResultList, _puResultCount);
    
    if(!_puResultCount || !(*_puResultCount))
    {
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
        CT_RefPos _clsTempPos(0, 0,(int)(_uOffsetResultList[2 * i]), -1);
        _pSearchResultData->pSearchResultTable[i]->SetBeginPos(_clsTempPos);
        _pSearchResultData->pSearchResultTable[i]->SetEndPos(CT_RefPos(0, 0, (int)(_uOffsetResultList[2 * i + 1]), -1));

        // 记录百分比
        long totalPage = GetTotalPageNumber();
        long curPage = m_pBook->GetLogicalPageIndexByOffsetInByte(_uOffsetResultList[2 * i]);
        if (0 < totalPage && 0 <= curPage && curPage < totalPage)
        {
            _pSearchResultData->pLocationTable[i] = 100 * (curPage + 1) / totalPage;
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
        
    m_pBook->FreeFindResults(_uOffsetResultList);
        
    return DK_SUCCEEDED(_dkCode);
}

long TextBookReader::GetTotalPageNumber() const
{
    if(m_eParseStatus == BOOKREADER_PARSE_FINISHED)
    {
        //解析完后m_vPageStartOffset.size()比实际页数大1，该位置存的是该书结束的offset
        return m_pBook ? m_vPageStartOffset.size() - 1 : 0;
    }
    return 0;
}

DK_IMAGE* TextBookReader::GetPageBMP()
{
    if (!m_pCurPage)
    {
        DebugPrintf(DLC_LIWEI, "TextBookReader::GetPageBMP() m_pCurPage null");
        return NULL;
    }
    return &m_tCurPageImage;
 }

long TextBookReader::GetPageNumber4CurrentChapter()
{
    long ret = GetCurrentPageIndex();
    return ret;
}

bool TextBookReader::AddBMPProcessor(IBMPProcessor* pProcessor)
{
    DK_ASSERT (DK_NULL != pProcessor);

    if (DK_NULL == pProcessor)
    {
        return false;
    }

    if (m_vBMPProcessor.end() == find(m_vBMPProcessor.begin(), m_vBMPProcessor.end(), pProcessor))
    {
        m_vBMPProcessor.push_back(pProcessor);
    }

    bool ret = m_vBMPProcessor.size() > 0;
    return ret;
}

bool TextBookReader::RemoveBMPProcessor(IBMPProcessor* pProcessor)
{
    DK_ASSERT (DK_NULL != pProcessor);

    if (DK_NULL == pProcessor)
    {
        return false;
    }

    vector<IBMPProcessor*>::iterator v = find(m_vBMPProcessor.begin(), m_vBMPProcessor.end(), pProcessor);
    if (m_vBMPProcessor.end() != v)
    {
        m_vBMPProcessor.erase(v);
    }

    bool ret = m_vBMPProcessor.size() > 0;
    return ret;
}

bool TextBookReader::GetAllBMPProcessor(vector<IBMPProcessor*> &ProcessorList)
{
    ProcessorList = m_vBMPProcessor;
    return true;
}


bool TextBookReader::SetDefaultFont(const wstring &_strFaceName, DK_CHARSET_TYPE charset)
{
    bool ret = DKT_SetDefaultFont(_strFaceName.c_str(), charset);
    return ret;
};

void TextBookReader::SetFontChange(const bool bFontChange)
{
    m_bTypeset |= bFontChange;
}

int TextBookReader::MoveToPageDirect(long pageOffSet)
{
	if(!m_pBook)
	{
		return BOOKREADER_GOTOPAGE_FAIL;
	}
	m_lCurPageIndex = 0;

	IDKTPage* page = DK_NULL;
	DK_ReturnCode res = DKR_OK;
	res = m_pBook->CreatePage(m_ParserOption, pageOffSet, IDKTBook::NEXT_PAGE, &page);

	if(!DK_SUCCEEDED(res))
	{
		return BOOKREADER_GOTOPAGE_FAIL;
	}

	if(page && !page->GetSizeInByte())
	{
		m_pBook->DestroyPage(page);
		page = NULL;
		return BOOKREADER_GOTOPAGE_FAIL;
    }

    DestroyCurPage();
	m_pCurPage = page;
	if(!ProduceBMPPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }
	return BOOKREADER_GOTOPAGE_SUCCESS;
}

//如果尚未解析完，跳转失败
int TextBookReader::MoveToPage(long pageIndex)
{
    if(!m_pBook)
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    unsigned int lPageOffset = 0;
    if(BOOKREADER_PARSE_FINISHED == m_eParseStatus)
    {
        if( (int)pageIndex > (int)m_vPageStartOffset.size() - 2)
        {
            m_lCurPageIndex = m_vPageStartOffset.size() - 2;
        }
        else
        {
            m_lCurPageIndex = pageIndex;
        }
        lPageOffset =GetOffSetByPageIndex(pageIndex);
    }
    else
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    if(!GotoPage(lPageOffset, IDKTBook::NEXT_PAGE))
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    } 
               
    if(!ProduceBMPPage())
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }
    return BOOKREADER_GOTOPAGE_SUCCESS;
}

int TextBookReader::MoveToFlowPosition(const DK_FLOWPOSITION& targetPos)
{
    unsigned int pageOffset = targetPos.nElemIndex;
    if(m_eParseStatus == BOOKREADER_PARSE_FINISHED)
    {
        m_lCurPageIndex = GetPageIndexFormParseResult(pageOffset);
        pageOffset = GetOffSetByPageIndex(m_lCurPageIndex);
    }
    
    if(!GotoPage(pageOffset, IDKTBook::NEXT_PAGE))
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }
    ProduceBMPPage();
    return BOOKREADER_GOTOPAGE_SUCCESS;
}

//由调用者负责释放返回的对象
ICT_ReadingDataItem* TextBookReader::GetCurPageProgress()
{
    if(!m_pCurPage)
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
    string _strTime  = CUtility::TransferTimeToString(_tTime);
    _pclsReadingProgress->SetCreateTime(_strTime);

    unsigned int _uStartOffset = m_pCurPage->GetOffsetInByte();
    CT_RefPos _clsStartRefPos;
    _clsStartRefPos.SetChapterIndex(0);
    _clsStartRefPos.SetParaIndex(0);
    _clsStartRefPos.SetAtomIndex(_uStartOffset);

    unsigned int _uEndOffset = _uStartOffset + m_pCurPage->GetSizeInByte();
    CT_RefPos _clsEndRefPos;
    _clsEndRefPos.SetChapterIndex(0);
    _clsEndRefPos.SetParaIndex(0);
    _clsEndRefPos.SetAtomIndex(_uEndOffset);

     m_uLastPageOffset = _uStartOffset;
    CompletePosRange(&_clsStartRefPos, &_clsEndRefPos);
    _pclsReadingProgress->SetBeginPos(_clsStartRefPos);
    _pclsReadingProgress->SetEndPos(_clsEndRefPos);

    _pclsReadingProgress->SetLastModifyTime(_strTime);
    return _pclsReadingProgress;
}

//根据TXT的文本偏移量判断其属于哪个章节，返回结果从0开始！ -1表示此文件没有TOC
int TextBookReader::GetChapterIndex(unsigned int pageOffset, DKXBlock* curDKXBlock)
{
    int chapterIndex = -1;
    GetTOC(curDKXBlock);
    if(m_vTOCList.empty() || pageOffset < 0)
    {
        return chapterIndex;
    }

    unsigned int _uSize = m_vTOCList.size();
    bool _bFind = false;
    for(unsigned int i = 0; i < _uSize; i++)
    {
        DK_TOCINFO* _pTOCInfo = m_vTOCList[i];
        if(!_pTOCInfo)
        {
            continue;
        }
        const ICT_ReadingDataItem * _pItem = _pTOCInfo->GetCurrentTOC();
        if(!_pItem)
        {
            continue;
        }
        unsigned int _uAnchor = _pItem->GetBeginPos().GetAtomIndex();
        if(_uAnchor > pageOffset)
        {
            if(0 == i)
            {
                chapterIndex = 0;
            }
            else
            {
                chapterIndex = i-1;
            }
            _bFind = true;
            break;
        }
        else if(_uAnchor == pageOffset)
        {
            chapterIndex = i;
            _bFind = true;
            break;
        }
    }
    if(!_bFind)
    {
        chapterIndex = (int)_uSize-1;
    }
    return chapterIndex;
}

bool TextBookReader::SetTextGrayScaleLevel(double _dGamma)
{
    bool _bSettingChanged = abs(m_dTextGamma - _dGamma) >= DoublePrecision;
    m_bTypeset = m_bTypeset || _bSettingChanged;
    m_dTextGamma = _dGamma;
    return true;
}

bool TextBookReader::SetFontSmoothType(DK_FONT_SMOOTH_TYPE _eFontSmoothType)
{
    m_eFontSmoothType = _eFontSmoothType;
    return true;
}

bool TextBookReader::SetInstantTurnPageModel(bool _bIsInstant)
{
    m_bIsInstant = _bIsInstant;
    return true;
}

void TextBookReader::CompletePosRange(CT_RefPos* startPos, CT_RefPos* endPos)
{
    if (DK_NULL == startPos || DK_NULL == endPos)
    {
        return;
    }
    startPos->SetOffsetInChapter(startPos->GetAtomIndex());
    endPos->SetOffsetInChapter(endPos->GetAtomIndex());
}

double TextBookReader::GetPercent(const CT_RefPos& startPos)
{
    long pageIndex = GetPageIndex(startPos);
    if (pageIndex < 0)
    {
        return -1;
    }
    long pageCount = GetTotalPageNumber();
    if (pageCount <= 0)
    {
        return -1;
    }
    return (double)((pageIndex  + 1.0) * 100) / (double)pageCount; 
}

const DK_TOCINFO* TextBookReader::GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode, DKXBlock* curDKXBlock)
{
	int chapterIndex = GetChapterIndex(posPage.nElemIndex, curDKXBlock);
	chapterIndex += (positionMode - CURRENT_CHAPTER);
	if(m_pBook && chapterIndex >= 0 && chapterIndex < (int)m_vTOCList.size())
	{
		return m_vTOCList.at(chapterIndex);
	}
	return NULL;
}

//bool TextBookReader::GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem *>* vBookmarks)
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

bool TextBookReader::GetDKXProgressForUpgrade(DKXBlock* curDKXBlock, ICT_ReadingDataItem* localReadingDataItem)
{
	if(localReadingDataItem)
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

bool TextBookReader::ProcessCloudBookmarks(ReadingBookInfo& bookInfo)
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

bool TextBookReader::ProcessCloudDKXProgress(ICT_ReadingDataItem* localReadingDataItem)
{
	CT_RefPos beginPos = localReadingDataItem->GetBeginPos();
	beginPos.SetChapterIndex(0);
	CT_RefPos endPos = localReadingDataItem->GetEndPos();
	endPos.SetChapterIndex(0);
	localReadingDataItem->SetBeginPos(beginPos);
	localReadingDataItem->SetEndPos(endPos);
	return false;
}

void TextBookReader::DestroyCurPage()
{
    if (m_pBook)
    {
        if (m_pBookTextController)
        {
            m_pBookTextController->Release();
        }

        if (m_pCurPage)
        {
            m_pBook->DestroyPage(m_pCurPage);
            m_pCurPage = NULL;
        }
    }
}

