//===========================================================================
// Summary:
//    PdfBookReader实现.
// Version:
//    0.1 draft - creating.
// Usage:
//    Null
// Remarks:
//    Null
// Date:
//    2011-12-6
// Author:
//    Null
//===========================================================================

#include "BookReader/PdfBookReader.h"
#include <dlfcn.h>
#include "DkStreamFactory.h"
#include "KernelType.h"
#include "KernelMacro.h"
#include "KernelVersion.h"
#include "KernelEncoding.h"
#include "interface.h"
#include "BookReader/PdfKernelHandle.h"
#include "FontManager/DKFont.h"
#include "Utility.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "DKXManager/DKX/DKXManager.h"
#include "SQM.h"
#include "BookReader/PageTable.h"

using DkFormat::CUtility;

PdfBookReader::PdfBookReader()
    : m_bInitialized(false)
    , m_strFilePath("")
{
    m_pPdfHandler = new PdfKernelHandle();
}

PdfBookReader::~PdfBookReader()
{
    CloseDocument();
}

bool PdfBookReader::OpenDocument(const string& strFilename)
{
    if (NULL == m_pPdfHandler)
    {
        m_pPdfHandler = new PdfKernelHandle();
        if (NULL == m_pPdfHandler)
        {
            m_bInitialized = false;
            return FALSE;
        }
    }

	m_strFilePath = strFilename;
	wstring filePath = EncodeUtil::ToWString(m_strFilePath);
    if (filePath.empty())
    {
        m_bInitialized = false;
        return false;
    }

    bool ret = m_pPdfHandler->Initialize(filePath.c_str());
    if (ret)
    {
        InitFont(); // 初始化字体, PDF Kernel 需满足 IDKPDoc 对象已被创建
    }
    m_bInitialized = ret;

    PageTable pageTableData;
    pageTableData.pageCount = GetTotalPageNumber();
    CPageTable pageTable(m_strFilePath, DK_GetKernelVersion(), -1);
    pageTableData.strMD5 = 
        SystemSettingInfo::GetInstance()->GetDuokanBookMD5(-1, -1);
    pageTable.SavePageTable(pageTableData);

    return ret;
}

bool PdfBookReader::CloseDocument()
{
    SafeDeletePointer(m_pPdfHandler);
    return true;
}

bool PdfBookReader::IsFirstPageInChapter()
{
    return m_pPdfHandler && m_pPdfHandler->IsFirstPageInChapter();
}

bool PdfBookReader::IsLastPageInChapter()
{
    return m_pPdfHandler && m_pPdfHandler->IsLastPageInChapter();
}

void PdfBookReader::SetTypeSetting(const DKTYPESETTING& typeSetting)
{
	if(m_pPdfHandler)
	{
    	m_pPdfHandler->SetTypeSetting(typeSetting);
	}
}

bool PdfBookReader::SetPageSize(int _iLeftMargin, int _iTopMargin, int width, int height)
{
    (void)_iLeftMargin;
    (void)_iTopMargin;
    bool ret = false;
    if (m_pPdfHandler)
    {
        ret = m_pPdfHandler->SetScreenRes((unsigned int)width, (unsigned int)height);
    }

    return ret;
}

bool PdfBookReader::SetDefaultFont(const wstring &_strFaceName, DK_CHARSET_TYPE charset)
{
    if (m_pPdfHandler)
    {
        m_pPdfHandler->SetDefaultFont (_strFaceName.c_str (), charset);
    }

    return (0 != m_pPdfHandler);
}

void PdfBookReader::SetFontChange(const bool bFontChange)
{
    DebugPrintf(DLC_LIUHJ, "PdfBookReader::SetFontChange() ENTRANCE");
    if (m_pPdfHandler)
    {
		InitFont();
    }
    DebugPrintf(DLC_LIUHJ, "PdfBookReader::SetFontChange() SUCCEEDED");
}

long PdfBookReader::GetPageIndex(const CT_RefPos& startPos)
{
	//pdf 三级索引的ChapterIndex 即是页码
	return startPos.GetChapterIndex();
}

int PdfBookReader::GotoPrevPage()
{
    DebugPrintf(DLC_LIUHJ, "PdfBookReader::GotoPrevPage() Start");
    return (m_bInitialized && m_pPdfHandler && m_pPdfHandler->Prev() && m_pPdfHandler->GetPage()) ? BOOKREADER_GOTOPAGE_SUCCESS : BOOKREADER_GOTOPAGE_FAIL;
}

int PdfBookReader::GotoNextPage()
{
    return (m_bInitialized && m_pPdfHandler && m_pPdfHandler->Next() && m_pPdfHandler->GetPage()) ? BOOKREADER_GOTOPAGE_SUCCESS : BOOKREADER_GOTOPAGE_FAIL;
}

int PdfBookReader::MoveToPage(long pageNum)
{
    DebugPrintf (DLC_LIUHJ, "PdfBookReader Move To Page %d", pageNum);
    return (m_bInitialized && m_pPdfHandler && m_pPdfHandler->SetPage(DK_FLOWPOSITION(pageNum, 0, 0))) ? BOOKREADER_GOTOPAGE_SUCCESS : BOOKREADER_GOTOPAGE_FAIL;
}

int PdfBookReader::MoveToFlowPosition(const DK_FLOWPOSITION &_pos)
{
    return MoveToPage(_pos.nChapterIndex);
}

vector<DK_TOCINFO*>* PdfBookReader::GetTOC(DKXBlock* curDKXBlock)
{
	return m_pPdfHandler ? m_pPdfHandler->GetTOC() : NULL;
}

string PdfBookReader::GetCurrentPageText()
{
    char* pText (NULL);
    string text;
    if (m_pPdfHandler)
    {
        bool ret = m_pPdfHandler->GetPageText (m_pPdfHandler->GetCurPageNum (), &pText);
        text = (ret && pText) ? pText : string ();
        SafeFreePointer (pText);
    }

    return text;
}

long PdfBookReader::GetTotalPageNumber() const
{
    if (false == m_bInitialized)
    {
        return -1;
    }

    return m_pPdfHandler->GetPageCount();
}

long PdfBookReader::GetPageNumber4CurrentChapter()
{
    //TODO: implement it
    return false;
}

bool PdfBookReader::AddBMPProcessor(IBMPProcessor* pProcessor)
{
    if (NULL == m_pPdfHandler)
    {
        return false;
    }

    return m_pPdfHandler->AddBMPProcessor(pProcessor);
}

bool PdfBookReader::RemoveBMPProcessor(IBMPProcessor* pProcessor)
{
    return m_pPdfHandler->RemoveBMPProcessor(pProcessor);
}

bool PdfBookReader::GetAllBMPProcessor(vector<IBMPProcessor*> &rProcessorList)
{
    return m_pPdfHandler->GetAllBMPProcessor(rProcessorList);
}


DK_IMAGE* PdfBookReader::GetPageBMP()
{
    if (NULL == m_pPdfHandler)
    {
        return NULL;
    }

    return m_pPdfHandler->GetPage(false);
}

long PdfBookReader::GetCurrentPageIndex()
{
    if (NULL == m_pPdfHandler)
    {
        return 0;
    }

    return static_cast<long>(m_pPdfHandler->GetCurPageNum());
}

bool PdfBookReader::SetPdfModeController(const PdfModeController* pPdfModeCtl)
{
    if (NULL == m_pPdfHandler)
    {
        return false;
    }

    bool ret (false);
    if (pPdfModeCtl)
    {
        // SQM of Reading mode
        if (PDF_RM_NormalPage == pPdfModeCtl->m_eReadingMode)
        {
			SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_PDF_NORMAL_MODE);
        }
        else if (PDF_RM_Rearrange == pPdfModeCtl->m_eReadingMode)
        {
            SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_PDF_REARRANGE);
        }
        else if (PDF_RM_AdaptiveWidth == pPdfModeCtl->m_eReadingMode)
        {
            SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_PDF_ADAPTIVE_WIDTH);
        }
        
        // SQM of Cutting mode
        if (PDF_CEM_Smart == pPdfModeCtl->m_eCuttingEdgeMode)
        {
            SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_PDF_SMART_STRIP_EDGE);
        }
        else if (PDF_CEM_Custom == pPdfModeCtl->m_eCuttingEdgeMode)
        {
            SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_PDF_CUSTOM_STRIP_EDGE);
        }

        // SQM of rotate screen
        if (90 == pPdfModeCtl->m_iRotation || 270 == pPdfModeCtl->m_iRotation)
        {
            SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_PDF_SCREEN_HORIZONTAL);
        }

        ret = m_pPdfHandler->SetModeController(*pPdfModeCtl);
    }
    
    return ret;
}

void PdfBookReader::InitFont()
{
    if (!g_pFontManager || !m_pPdfHandler)
    {
        return;
    }

    DebugPrintf(DLC_LIUHJ, "PdfBookReader::InitFont START");

    // 注册系统默认字体，特别是系统针对不同字符集指定不同字体时，如自定义中文字体，自定义英文字体
    DK_CHARSET_TYPE lstCharset[] = {DK_CHARSET_ANSI, DK_CHARSET_GB, DK_CHARSET_BIG5, DK_CHARSET_SHITJIS, DK_CHARSET_JOHAB};
    int iSize = sizeof(lstCharset) / sizeof(DK_CHARSET_TYPE);
    for (int i = 0; i < iSize; ++i)
    {
        DK_CHARSET_TYPE &eCT = lstCharset[i];
        wstring strFaceName (L"");
        if (!g_pFontManager->GetDefaultFontFaceNameForCharSet(eCT, &strFaceName))
        {
            g_pFontManager->GetSystemDefaultFontFaceName(&strFaceName);
        }

        if (strFaceName.length () != 0)
        {
            m_pPdfHandler->SetDefaultFont(strFaceName.c_str (), eCT);
        }
    }

    DebugPrintf(DLC_LIUHJ, "PdfBookReader::InitFont FINISH");
}

bool PdfBookReader::GotoBookMark(const ICT_ReadingDataItem* _pReadingEntry)
{
    DebugPrintf(DLC_LIUHJ, "PdfBookReader::GotoBookMark START");
    if (NULL == m_pPdfHandler || NULL == _pReadingEntry)
    {
        return false;
    }
    
    DebugPrintf(DLC_LIUHJ, "PdfBookReader::GotoBookMark (%d, %d, %d)", _pReadingEntry->GetBeginPos().GetChapterIndex(), 
                                                                    _pReadingEntry->GetBeginPos().GetParaIndex(),
                                                                    _pReadingEntry->GetBeginPos().GetAtomIndex());

    return m_pPdfHandler->SetPage(DK_FLOWPOSITION(_pReadingEntry->GetBeginPos().GetChapterIndex(), 
                                                    _pReadingEntry->GetBeginPos().GetParaIndex(),
                                                    _pReadingEntry->GetBeginPos().GetAtomIndex()));
}

ICT_ReadingDataItem* PdfBookReader::GetCurPageProgress()
{
    if (NULL == m_pPdfHandler)
    {
        return NULL;
    }

    DK_FLOWPOSITION curPos = m_pPdfHandler->GetCurPageLocation();
	DK_FLOWPOSITION pageEndPos = m_pPdfHandler->GetCurPageEndLocation();

    ICT_ReadingDataItem* pClsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);
    if(NULL == pClsReadingProgress)
    {
        return NULL;
    }

    time_t tTime;
    time(&tTime);
    string strTime  = CUtility::TransferTimeToString(tTime);
    pClsReadingProgress->SetCreateTime(strTime);
    
    DebugPrintf (DLC_LIUHJ, "PdfBookReader::GetCurPageProgress BeginPos ChapterIndex %d", curPos.nChapterIndex);
    CT_RefPos clsBeginPos;
    clsBeginPos.SetChapterIndex(curPos.nChapterIndex);//页
    clsBeginPos.SetParaIndex(curPos.nParaIndex);//段落
    clsBeginPos.SetAtomIndex(curPos.nElemIndex);//偏移

    CT_RefPos clsEndPos;
    clsEndPos.SetChapterIndex(pageEndPos.nChapterIndex);//页
    clsEndPos.SetParaIndex(pageEndPos.nParaIndex);//段落
    clsEndPos.SetAtomIndex(pageEndPos.nElemIndex);//偏移

    CompletePosRange(&clsBeginPos, &clsEndPos);
    pClsReadingProgress->SetBeginPos(clsBeginPos);
    pClsReadingProgress->SetEndPos(clsEndPos);
    pClsReadingProgress->SetLastModifyTime(strTime);
    return pClsReadingProgress;
}

bool PdfBookReader::SetLayoutSettings(const DK_LAYOUTSETTING &_clsLayoutSetting) 
{
    if (m_pPdfHandler)
    {
        m_pPdfHandler->SetLayoutSettings (_clsLayoutSetting);
        DebugPrintf (DLC_LIUHJ, "PdfBookReader::SetLayoutSettings set layout setting fontsize %d, dParaSpacing %d",
                    _clsLayoutSetting.dFontSize,
                    _clsLayoutSetting.dParaSpacing);
    }
    return true; 
}

bool PdfBookReader::IsPositionInCurrentPage(const DK_FLOWPOSITION &_pos)
{
    DebugPrintf (DLC_LIUHJ, "IsPositionInCurrentPage _pos.nChapterIndex %d, _pos.nParaIndex %d, _pos.nElemIndex %d", _pos.nChapterIndex, _pos.nParaIndex, _pos.nElemIndex);
    return m_pPdfHandler->IsPositionInCurrentPage(_pos);
}

bool PdfBookReader::SetInstantTurnPageModel(bool _bIsInstant)
{
    if (m_pPdfHandler)
    {
        m_pPdfHandler->SetInstantPageModel (_bIsInstant);
    }
    return true;
}

double PdfBookReader::GetPercent(const CT_RefPos& startPos) 
{
    double ret = (double)-1;
    if (m_pPdfHandler)
    {
        const double startPage = (double)((startPos.GetChapterIndex() + 1) * 100);
        const double totalPage = (double)m_pPdfHandler->GetPageCount();
        ret = startPage / totalPage;
    }
    return ret;
}

string PdfBookReader::GetChapterTitle(const DK_FLOWPOSITION& posChapter, DKXBlock* curDKXBlock)
{
    const DK_TOCINFO* curTocInfo = GetChapterInfo(posChapter, CURRENT_CHAPTER, curDKXBlock);
	return curTocInfo ? curTocInfo->GetChapter() : "";
}

const DK_TOCINFO* PdfBookReader::GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode, DKXBlock* curDKXBlock)
{
	return m_pPdfHandler ? m_pPdfHandler->GetChapterInfo(posPage, positionMode) : NULL;
}

//bool PdfBookReader::GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem *>* vBookmarks)
//{
//	if(curDKXBlock && vBookmarks)
//	{
//		if(curDKXBlock->GetAllBookmarks(vBookmarks))
//		{
//			for(size_t i = 0; i < vBookmarks->size(); i++)
//			{
//				ICT_ReadingDataItem* readingData = vBookmarks->at(i);
//				CT_RefPos beginPos = readingData->GetBeginPos();
//				beginPos.SetChapterIndex(beginPos.GetChapterIndex() + 1);
//				CT_RefPos endPos = readingData->GetEndPos();
//				endPos.SetChapterIndex(endPos.GetChapterIndex() + 1);
//				readingData->SetBeginPos(beginPos);
//				readingData->SetEndPos(endPos);
//			}
//			return true;
//		}
//	}
//	return false;
//}

bool PdfBookReader::GetDKXProgressForUpgrade(DKXBlock* curDKXBlock, ICT_ReadingDataItem* localReadingDataItem)
{
	if(curDKXBlock && localReadingDataItem)
	{
		if(curDKXBlock->GetProgress(localReadingDataItem))
		{
			CT_RefPos beginPos = localReadingDataItem->GetBeginPos();
			beginPos.SetChapterIndex(beginPos.GetChapterIndex() + 1);
			CT_RefPos endPos = localReadingDataItem->GetEndPos();
			endPos.SetChapterIndex(endPos.GetChapterIndex() + 1);
			localReadingDataItem->SetBeginPos(beginPos);
			localReadingDataItem->SetEndPos(endPos);
			return true;
		}
	}
	return false;
}

bool PdfBookReader::ProcessCloudBookmarks(ReadingBookInfo& bookInfo)
{
	for(size_t i = 0; i < bookInfo.GetReadingDataItemCount(); i++)
	{
		CT_RefPos beginPos = bookInfo.GetReadingDataItem(i).GetBeginRefPos();
		CT_RefPos endPos = bookInfo.GetReadingDataItem(i).GetEndRefPos();
		CT_RefPos refPos = bookInfo.GetReadingDataItem(i).GetRefPos();
		beginPos.SetChapterIndex(beginPos.GetChapterIndex() - 1);
		endPos.SetChapterIndex(endPos.GetChapterIndex() - 1);
		refPos.SetChapterIndex(refPos.GetChapterIndex() - 1);
		bookInfo.UpdateItemPos(i, beginPos, endPos, refPos);
	}
	return true;
}

bool PdfBookReader::ProcessCloudDKXProgress(ICT_ReadingDataItem* localReadingDataItem)
{
	CT_RefPos beginPos = localReadingDataItem->GetBeginPos();
	beginPos.SetChapterIndex(beginPos.GetChapterIndex() - 1);
	CT_RefPos endPos = localReadingDataItem->GetEndPos();
	endPos.SetChapterIndex(endPos.GetChapterIndex() - 1);
	localReadingDataItem->SetBeginPos(beginPos);
	localReadingDataItem->SetEndPos(endPos);
	return false;
}

void PdfBookReader::ParseSinglePage(bool parseSingle)
{
    if (m_pPdfHandler)
    {
        m_pPdfHandler->ParseSinglePage(parseSingle);
    }
}

BOOK_DIRECTION_TYPE PdfBookReader::GetBookDirection() const
{
    BOOK_DIRECTION_TYPE directionType = BOOK_MODE_HTB_DIRECTION_LTR;
    if (m_pPdfHandler)
    {
        const PdfModeController* pModeController = m_pPdfHandler->GetModeController();
        if (pModeController)
        {
            if (PDF_RM_Rearrange == pModeController->m_eReadingMode)
            {
                return directionType;
            }

            switch (pModeController->m_iRotation)
            {
            case 90:
                directionType = BOOK_MODE_VLR_DIRECTION_LTR;
                break;
            case 180:
                directionType = BOOK_MODE_HBT_DIRECTION_LTR;
                break;
            case 270:
                directionType = BOOK_MODE_VRL_DIRECTION_LTR;
                break;
            default:
                break;
            }
        }
    }
    return directionType;
}

