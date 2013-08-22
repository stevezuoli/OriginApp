//===========================================================================
// Summary:
//        IBookReader接口定义
// Version:
//    0.1 draft - creating.
//
// Usage:
//        Null
// Remarks:
//        Null
// Date:
//        2011-11-21
// Author:
//        Chen Mao (chenm@duokan.com)
//===========================================================================
#include "BookReader/EpubBookReader.h"
#include "BookReader/PageTable.h"
#include "FontManager/DKFont.h"
#include "KernelType.h"
#include "Common/File_DK.h"
#include "DKEAPI.h"
#include "IDKEComicsFrame.h"
#include "KernelMacro.h"
#include "interface.h"
#include <algorithm>
#include <dlfcn.h>
#include "Utility/ThreadHelper.h"
#include "Utility/PathManager.h"
#include "DKXManager/DKX/DKXBlock.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/SelectedControler.h"
#else
#include "AppUI/CursorControler.h"
#endif
#include "Utility.h"
#include "../ImageHandler/DkImageHelper.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "dkWinError.h"
#include "Common/SystemSetting_DK.h"
#include "Utility/PathManager.h"
#include "Utility/EncodeUtil.h"
#include "Utility/ConfigFile.h"
#include "BookStore/ConstStrings.h"
#include "BookStore/BookStoreInfoManager.h"
#include "drivers/DeviceInfo.h"
#include "KernelVersion.h"
#include "Mutex.h"
#ifndef _RELEASE_VERSION
//#define ENABLE_PROFILE
#endif
#include "Utility/Profiler.h"

using namespace dk::utility;
using namespace dk::common;

using std::vector;
using std::wstring;
using DkFormat::CUtility;
using dk::utility::PathManager;
using dk::utility::ConfigFile;
using dk::utility::EncodeUtil;
using dk::bookstore::ConstStrings;

#define HTMLBOOKREADER_FAILTIMES       9
#define HTMLBOOKREADER_SLEEPSECONDS    1
#define HTMLBOOKREADER_SLEEPMAXSECONDS 15

enum 
{
    MovePagesInSameChapter = 0,
    MovePagesInAdjoiningChapter = 1,
    MovePagesInSeperatedChaptersWithSameAdjoiningChapter = 2,
};

EpubBookReader::EpubBookReader()
    : m_pParseThread(0)
    , m_pDKEBook(NULL)
    , m_pDKEPage(NULL)
    , m_lPageIndex(-1)
    , m_lPageCount(0)
    , m_lChapterCount(-1)
    , m_bReParse(true)
    , m_bStopParse(false)
    , m_bIsInstant(false)
    , m_eParseStatus(BOOKREADER_PARSE_ERROR)
    , m_iTextGamma(1.0f)
    , m_iGraphicsGamma(1.0f)
    , m_eFontSmoothType(DK_FONT_SMOOTH_NORMAL)
    , m_screenWidth(0)
    , m_screenHeight(0)
    , m_pBookTextController(NULL)
    , m_bookInfoDataSet(false)
    , m_familiarToTraditional(false)
    , m_pPageTable(NULL)
    , m_comicsFrame(false)
    , m_globalFrame(-1)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);

    m_strDKEFilePath.clear();
    m_strDKEPageText.clear();
    m_vPageIndexOfChapter.clear();
    m_vPageCountOfChapter.clear();
    m_vChapterParseStatus.clear();
    m_vEndPosOfChapter.clear();

    //TODO: just initialize a wrong DK_BOX, it should be set to right value
    m_clsDKEParseOption.pageBox = DK_BOX(0, 0, 1, 1);

    m_tCurPageImage.iColorChannels = 4;
    m_tCurPageImage.iWidth         = (int) m_clsDKEParseOption.pageBox.Width();
    m_tCurPageImage.iHeight        = (int) m_clsDKEParseOption.pageBox.Height();
    m_tCurPageImage.iStrideWidth   = m_tCurPageImage.iWidth * m_tCurPageImage.iColorChannels;
    m_tCurPageImage.pbData         = NULL;
}

EpubBookReader::~EpubBookReader()
{
    CloseDocument();    
}


bool EpubBookReader::OpenDocument(const string &_strFilename)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    if (NULL != m_pDKEBook)
        CloseDocument();

    DK_WCHAR* wsFilePath = EncodingUtil::CharToWChar(_strFilename.c_str());  
    if (NULL == wsFilePath)
    {
        DebugLog(DLC_JUGH, "HtmlBaseBookReade::OpenDocument CharToWChar ERROR!!!");
        return false;
    }  

    if (NeedsDrmOpen(_strFilename.c_str()))
    {
        m_pDKEBook = DrmOpen(_strFilename.c_str());
    }
    else
    {
        m_pDKEBook = (IDKEBook*)DKE_OpenDocument(wsFilePath, DKE_CONTAINER_ZIP, NULL);
    }
    if (NULL == m_pDKEBook)
    {
        DebugLog(DLC_JUGH, "HtmlBaseBookReade::OpenDocument failed with m_pDKEBook == NULL!!!");
        DK_FREE(wsFilePath);
        return false;
    }
    DK_FREE(wsFilePath);   

    m_strDKEFilePath = _strFilename;
    if (PathManager::IsUnderBookStore(m_strDKEFilePath.c_str()))
    {
        std::string baseName = PathManager::GetFileName(m_strDKEFilePath.c_str());
        // 4: "xx.epub"
        if (baseName.size() <= 5)
        {
            return false; 
        }
        m_bookId = baseName.substr(0, baseName.size() - 5);
    }

#ifdef KINDLE_FOR_TOUCH
    m_pBookTextController = new SelectedControler();
#else
    m_pBookTextController = new CursorControler();
#endif
    if (NULL == m_pBookTextController)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::OpenDocument new CursorControler ERROR!!!");
        return CloseDocument();
    }
    DK_ARGBCOLOR tTextColor;
    m_pDKEBook->SetTextColor(tTextColor);

    if (DK_SUCCEEDED(m_pDKEBook->PrepareParseContent(DK_NULL)))
    {
        m_lChapterCount = m_pDKEBook->GetChapterCount();
        DebugPrintf(DLC_TIME_TEST, "m_pDKEBook->GetChapterCount() End For Kernel");
        DebugPrintf(DLC_JUGH, "EpubBookReader::OpenDocument m_lChapterCount = %d", m_lChapterCount);
        m_clsDKEParseOption.ptMode = DKE_PARSER_PAGETABLE_CHAPTER;
        m_lPageIndex = 0;
        return true;
    }
    DebugLog(DLC_JUGH, "EpubBookReader::OpenDocument ERROR!!!");
    return false;
};

bool EpubBookReader::CloseDocument()
{
    // must wait the thread to be ended first, otherwise there may be memory error
    if (0 != m_pParseThread)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::CloseDocument CancelThread START!!!");
        SetStopParsingFlag();
        ThreadHelper::JoinThread(m_pParseThread, NULL);
        m_pParseThread = 0;
        DebugLog(DLC_JUGH, "EpubBookReader::CloseDocument CancelThread END!!!");
    }

    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    for(size_t i = 0; i < m_vTOCList.size(); i++)
    {
        SafeDeletePointer(m_vTOCList[i]);
    }
    m_vTOCList.clear();
    SafeDeletePointer(m_pBookTextController);
    SafeDeletePointer(m_pPageTable);


    m_lPageIndex    = -1;
    m_lChapterCount = -1;

    m_strDKEFilePath.clear();
    m_strDKEPageText.clear();
    m_lstBMPProcessor.clear();
    ClearVector();

    m_pDKEPage    = NULL;
    SafeFreePointer(m_tCurPageImage.pbData);

    if (NULL != m_pDKEBook)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::CloseDocument DKE_CloseDocument START!!!");
        DKE_CloseDocument(m_pDKEBook);
        m_pDKEBook = NULL;
        DebugLog(DLC_JUGH, "EpubBookReader::CloseDocument DKE_CloseDocument END!!!");
    }
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s End", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    return true;
};

void EpubBookReader::SetTypeSetting(const DKTYPESETTING& typeSetting)
{
	if(m_pDKEBook)
	{
    	m_pDKEBook->SetTypeSetting(typeSetting);
	}
}

bool EpubBookReader::SetFamiliarToTraditional(bool toTraditional)
{
    m_familiarToTraditional = toTraditional;
    //set it once, not each page 
    DKE_SetGBToBig5(m_familiarToTraditional);
    return true;
}

bool EpubBookReader::SetPageSize(int _iLeftMargin, int _iTopMargin, int _iWidth, int _iHeight)
{
    if(_iWidth <= 0 || _iHeight <= 0)
    {
        return false;
    }

    DK_BOX &clsPaddingBox   = m_clsDKEParseOption.paddingBox;

    bool bSizeChanged = ABS(_iLeftMargin + _iWidth - clsPaddingBox.X1) >= DoublePrecision 
        || ABS(_iTopMargin + _iHeight - clsPaddingBox.Y1) >= DoublePrecision 
        || ABS(clsPaddingBox.X0 - _iLeftMargin) >= DoublePrecision 
        || ABS(clsPaddingBox.Y0 - _iTopMargin) >= DoublePrecision;

    if(bSizeChanged)
    {
        clsPaddingBox.X0 = _iLeftMargin;
        clsPaddingBox.Y0 = _iTopMargin;
        clsPaddingBox.X1 = _iWidth + _iLeftMargin;
        clsPaddingBox.Y1 = _iHeight + _iTopMargin;
        DebugPrintf(DLC_JUGH, "EpubBookReader::Parse box(%f, %f)-(%f, %f)",
            m_clsDKEParseOption.paddingBox.X0,
            m_clsDKEParseOption.paddingBox.Y0,
            m_clsDKEParseOption.paddingBox.X1,
            m_clsDKEParseOption.paddingBox.Y1);
    }

    m_bReParse = m_bReParse || bSizeChanged;
    return true;
};

bool EpubBookReader::SetDefaultFont(const wstring &_strFaceName, DK_CHARSET_TYPE _clsCharSet)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    m_bReParse = true;
    return DKE_SetDefaultFont(_strFaceName.c_str(), _clsCharSet);
};

void EpubBookReader::SetFontChange(const bool bFontChange)
{
    m_bReParse = m_bReParse || bFontChange;
}

long EpubBookReader::GetCurrentPageIndex()
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook);

    const long& nChapterIndex = m_tDKEFlowPos.nChapterIndex;
    long lPageIndex = -1;
    if ((long)m_vPageIndexOfChapter.size() > (long)nChapterIndex)
    {
        lPageIndex = m_vPageIndexOfChapter[nChapterIndex];
    }
    DebugPrintf(DLC_JUGH, "EpubBookReader::GetCurrentPageIndex FINISHED [%d, %d]", lPageIndex, m_lPageIndex);
    return (lPageIndex >= 0) ? (lPageIndex + m_lPageIndex) : -1;
}

string EpubBookReader::GetChapterTitle(const DK_FLOWPOSITION& posChapter, DKXBlock* curDKXBlock)
{
    const DK_TOCINFO* curTocInfo = GetChapterInfo(posChapter, CURRENT_CHAPTER);
    return curTocInfo ? curTocInfo->GetChapter() : "";
}

bool EpubBookReader::UpdateAnchorChapter(DK_TOCINFO* pTocInfo)
{
    if(pTocInfo && pTocInfo->HasAnchor() && !pTocInfo->IsAnchorAnalysised())
    {
        pTocInfo->SetAnchorAnalysised(true);
        DK_FLOWPOSITION clsFlowPos;
        DK_FLOWPOSITION posChapter = pTocInfo->GetBeginPos();
        wstring wstrAnchor = pTocInfo->GetAnchor();
        if(DK_SUCCEEDED(m_pDKEBook->GetAnchorPosition(posChapter.nChapterIndex, wstrAnchor.c_str(), &clsFlowPos)))
        {
            pTocInfo->SetBeginPos(clsFlowPos);
            return true;
        }
    }
    return false;
}

const DK_TOCINFO* EpubBookReader::GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode, DKXBlock* curDKXBlock)
{
    DebugPrintf(DLC_LIWEI, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    GetTOC(curDKXBlock);
    std::vector<int> tocIndexList;
    for(size_t i = 0; i < m_vTOCList.size(); i++)
    {
        DK_TOCINFO* pTocInfo = m_vTOCList.at(i);
        if(pTocInfo->IsValid())
        {
            if(posPage.nChapterIndex == pTocInfo->GetBeginPos().nChapterIndex)
            {
                UpdateAnchorChapter(pTocInfo);
            }
            tocIndexList.push_back(i);
        }
    }

    SortingVector comparer(m_vTOCList);
    std::sort(tocIndexList.begin(),tocIndexList.end(), comparer);

    int tocIndex = -1;
    for(int i = (int)tocIndexList.size() - 1; i >= 0; i--)
    {
        DK_TOCINFO* pTocInfo = m_vTOCList.at(tocIndexList[i]);
        if(pTocInfo->IsValid() && posPage >= pTocInfo->GetBeginPos())
        {
            tocIndex = i;
            break;
        }
    }
    tocIndex += (positionMode - CURRENT_CHAPTER);
    if(tocIndex < 0 || tocIndex >= (int)tocIndexList.size())
    {
        return NULL;
    }
    DK_TOCINFO* pTocInfo = m_vTOCList.at(tocIndexList[tocIndex]);
    DebugPrintf(DLC_LIWEI, "%s, %d, %s, %s end", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    return pTocInfo;
}

int EpubBookReader::GotoPrevPage()
{
    // Move RenderPage() to GotoPage()/PrevFrame()
    return ComicsFrameModeValid() ? PrevFrame() : PrevPage();
}

int EpubBookReader::PrevPage()
{
    DK_PROFILE_FUNCTION;
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook);

    unsigned int uChapterIndex = m_tDKEFlowPos.nChapterIndex;
    long lPageCountOfChapter   = m_vPageCountOfChapter[uChapterIndex];//m_pDKEBook->GetPageCountOfChapter(uChapterIndex);
    DebugPrintf(DLC_JUGH, "(%d, %d/%d)", uChapterIndex, m_lPageIndex, lPageCountOfChapter);

    if (m_lPageIndex < 0 ||  m_lPageIndex >= lPageCountOfChapter)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GetPrevPage m_lPageIndex ERROR");
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    long lPageIndexInChapter  = m_lPageIndex - 1;
    while (lPageIndexInChapter < 0)
    {
        if (uChapterIndex == 0)
        {
            DebugLog(DLC_JUGH, "EpubBookReader::GotoPrevPage is the first page!!!");
            return BOOKREADER_GOTOPAGE_FAIL;;
        }

        uChapterIndex--;
        if (!m_vChapterParseStatus[uChapterIndex])
        {
            DebugLog(DLC_JUGH, "EpubBookReader::GotoPrevPage parse not finish");  
            return BOOKREADER_GOTOPAGE_WAIT;;
        }
        lPageCountOfChapter = m_vPageCountOfChapter[uChapterIndex];//m_pDKEBook->GetPageCountOfChapter(uChapterIndex);
        lPageIndexInChapter = lPageCountOfChapter - 1;
        DebugPrintf(DLC_JUGH, "(%d, %d/%d)", uChapterIndex, lPageIndexInChapter, lPageCountOfChapter);
        if (lPageIndexInChapter >= 0)
        {
            DebugLog(DLC_JUGH, "EpubBookReader::GetPrevPage SUCCESS");
            break;
        }
    }

    DebugPrintf(DLC_JUGH, "[%d, %d/%d]", uChapterIndex, lPageIndexInChapter, lPageCountOfChapter);
    return GotoPage(uChapterIndex, lPageIndexInChapter, true) ? BOOKREADER_GOTOPAGE_SUCCESS : BOOKREADER_GOTOPAGE_WAIT;
}

int EpubBookReader::PrevFrame()
{
    DK_PROFILE_FUNCTION;
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook && NULL != m_pDKEPage && ComicsFrameModeValid());

    const long globalFrame = m_globalFrame - 1;
    const long startFrameIndex = m_pDKEPage->GetGlobalFrameIndex(0);
    if (globalFrame < 0)
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }
    else if (globalFrame >= startFrameIndex)
    {
        m_globalFrame = globalFrame;
        RenderPage();
        return BOOKREADER_GOTOPAGE_SUCCESS;
    }
    return PrevPage();
}

int EpubBookReader::GotoNextPage()
{
    DK_PROFILE_FUNCTION;
    // Move RenderPage() to GotoPage()/NextFrame()
    return ComicsFrameModeValid() ? NextFrame() : NextPage();
}

int EpubBookReader::NextPage()
{
    DK_PROFILE_FUNCTION;
    assert (NULL != m_pDKEBook);

    unsigned int uChapterIndex = m_tDKEFlowPos.nChapterIndex;
    long lPageCountOfChapter   = m_vPageCountOfChapter[uChapterIndex];//m_pDKEBook->GetPageCountOfChapter(uChapterIndex);

    if (m_lPageIndex < 0 || m_lPageIndex >= lPageCountOfChapter)
    {
        DebugPrintf(DLC_JUGH, "EpubBookReader::GotoNextPage m_lPageIndex ERROR!!!");
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    long lPageIndexInChapter = m_lPageIndex + 1;
    while (lPageIndexInChapter >= lPageCountOfChapter)
    {
        uChapterIndex++;
        if (uChapterIndex >= (unsigned int)m_lChapterCount)
        {
            DebugPrintf(DLC_JUGH, "EpubBookReader::GotoNextPage is the final page!!!");
            DebugPrintf(DLC_JUGH, "uChapterIndex   = %d", uChapterIndex);
            DebugPrintf(DLC_JUGH, "m_lChapterCount = %d", m_lChapterCount);
            return BOOKREADER_GOTOPAGE_FAIL;;
        }
        if(!m_vChapterParseStatus[uChapterIndex])
        {
            return BOOKREADER_GOTOPAGE_WAIT;
        }
        lPageCountOfChapter = m_vPageCountOfChapter[uChapterIndex];//m_pDKEBook->GetPageCountOfChapter(uChapterIndex);
        lPageIndexInChapter = 0;
        if (lPageCountOfChapter > 0)
        {
            break;
        }
    }

    return GotoPage(uChapterIndex, lPageIndexInChapter) ? BOOKREADER_GOTOPAGE_SUCCESS : BOOKREADER_GOTOPAGE_WAIT;
}

int EpubBookReader::NextFrame()
{
    DK_PROFILE_FUNCTION;
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook && NULL != m_pDKEPage && ComicsFrameModeValid());

    const long globalFrame = m_globalFrame + 1;
    const long globalFrameCount = m_pDKEBook->GetComicsFrameCount();
    const long maxFrameInPage = m_pDKEPage->GetGlobalFrameIndex(0) + m_pDKEPage->GetFrameCount() - 1;
    if (globalFrame >= globalFrameCount)
    {
        return BOOKREADER_GOTOPAGE_FAIL;
    }
    else if (globalFrame <= maxFrameInPage)
    {
        m_globalFrame = globalFrame;
        RenderPage();
        return BOOKREADER_GOTOPAGE_SUCCESS;
    }
    return NextPage();
}

vector<DK_TOCINFO*>* EpubBookReader::GetTOC(DKXBlock* curDKXBlock)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook);

    if (m_vTOCList.empty())
    {
        IDKETocPoint* pTocRoot = m_pDKEBook->GetTocRoot();
        if (pTocRoot && !ParseTocInfo(pTocRoot))
        {
            DebugPrintf(DLC_LIWEI, "EpubBookReader::GetTOC ERROR!!!");
        }
    }
    DebugPrintf(DLC_LIWEI, "%s, %d, %s, %s end", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    return &m_vTOCList;
}

int EpubBookReader::MoveToChapter(int _iChapterNumber)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook);

    if (_iChapterNumber < 0 || _iChapterNumber >= m_lChapterCount)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::MoveToChapter _iChapterNumber ERROR!!!");
        return BOOKREADER_GOTOPAGE_SUCCESS;
    }

    if (!ReParseContent(_iChapterNumber))
    {
        DebugLog(DLC_JUGH, "EpubBookReader::MoveToChapter ReParse ERROR!!!");
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    return GotoPage(_iChapterNumber, 0) ? BOOKREADER_GOTOPAGE_SUCCESS : BOOKREADER_GOTOPAGE_FAIL;
}

int EpubBookReader::MoveToPage(long _lPageNumber)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook);

    _lPageNumber = (_lPageNumber < 0) ? 0 : ((m_lPageCount > 0 && _lPageNumber >= m_lPageCount) ? (m_lPageCount - 1) : _lPageNumber);

    long lChapterIndex = m_lChapterCount - 1;
    do
    {
        if (lChapterIndex < 0)
        {
            DebugLog(DLC_JUGH, "EpubBookReader::MoveToPage lChapterIndex ERROR!!!");
            return BOOKREADER_GOTOPAGE_FAIL;
        }

        long lPageIndex = m_vPageIndexOfChapter[lChapterIndex];
        if (lPageIndex < 0)
        {
            DebugLog(DLC_JUGH, "EpubBookReader::MoveToPage lPageIndex ERROR!!!");
            return BOOKREADER_GOTOPAGE_FAIL;
        }
        else if (lPageIndex <= _lPageNumber)
        {
            DebugLog(DLC_JUGH, "EpubBookReader::MoveToPage success!!!");
            _lPageNumber -= lPageIndex;
            break;
        }
        DebugPrintf(DLC_JUGH, "lChapterIndex = %d", lChapterIndex);
        DebugPrintf(DLC_JUGH, "lPageIndex    = %d", lPageIndex);
        lChapterIndex--;
    } while (lChapterIndex >= 0);

    if (lChapterIndex < 0)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::MoveToPage lChapterIndex ERROR!!!");
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    if (!m_vChapterParseStatus[lChapterIndex])
    {
        if(!ReParseContent(lChapterIndex))
        {
            DebugLog(DLC_JUGH, "EpubBookReader::MoveToPage ReParse ERROR!!!");
            return BOOKREADER_GOTOPAGE_FAIL;
        }
    }

    DebugPrintf(DLC_JUGH, "[%d, %d]", lChapterIndex, _lPageNumber);
    return GotoPage(lChapterIndex, _lPageNumber) ? BOOKREADER_GOTOPAGE_SUCCESS : BOOKREADER_GOTOPAGE_WAIT;
}

int EpubBookReader::MoveToFlowPosition(const DK_FLOWPOSITION &_pos)
{
    DebugPrintf(DLC_LIWEI, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook);
    if (!ReParseContent(_pos.nChapterIndex))
    {
        DebugLog(DLC_LIWEI, "EpubBookReader::MoveToChapter ReParse ERROR!!!");
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    m_globalFrame = -1;
    return GotoPage(_pos) ? BOOKREADER_GOTOPAGE_SUCCESS : BOOKREADER_GOTOPAGE_FAIL;
}

int EpubBookReader::MoveToTOC(DK_TOCINFO* pCurToc)
{
    if(pCurToc)
    {
        UpdateAnchorChapter(pCurToc);
        DK_FLOWPOSITION posCur = pCurToc->GetBeginPos();
        return MoveToFlowPosition(posCur);
    }
    return BOOKREADER_GOTOPAGE_FAIL;
}

string EpubBookReader::GetCurrentPageText()
{
    assert (NULL != m_pDKEBook && NULL != m_pDKEPage);

    m_strDKEPageText.clear();

    DK_WCHAR* pwchContent = m_pDKEPage->GetTextContent();
    if(pwchContent)
    {
        DK_CHAR*  pstrContent = EncodingUtil::WCharToChar(pwchContent, EncodingUtil::UTF8);
        if (NULL != pstrContent)
        {
            m_strDKEPageText = pstrContent;
            DK_FREE(pstrContent);
        }
        m_pDKEPage->FreeTextContent(pwchContent);
    }

    return m_strDKEPageText;
}

bool EpubBookReader::SearchInCurrentBook(const string& _strText, const DK_FLOWPOSITION& _startPos, const int& _iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* _puResultCount)
{
    if(!m_pDKEBook || !_pSearchResultData || !_puResultCount || _iMaxResultCount <= 0)
    {
        return false;
    }
    DK_WCHAR* pText = EncodingUtil::CharToWChar(_strText.c_str());
    if (NULL == pText)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::SearchInCurrentBook CharToWChar IS NULL!!!");
        return false;
    }

    // 调用IDKEBook查找接口
    DK_FLOWPOSITION* _pclsPositionList = NULL;
    DebugPrintf(DLC_LIUJT, "EpubBookReader::SearchInCurrentBook start to search at pos[%d, %d, %d] for %s", _startPos.nChapterIndex, _startPos.nParaIndex, _startPos.nElemIndex, _strText.c_str());
    if (DK_FAILED(m_pDKEBook->FindTextInBook(_startPos, pText, _iMaxResultCount, &_pclsPositionList, _puResultCount))
        || !_pclsPositionList || !(*_puResultCount))
    {
        DebugLog(DLC_JUGH, "EpubBookReader::SearchInCurrentBook FindTextInChapter FAILED!!!");
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
        CT_RefPos _clsTempPos((int)(_pclsPositionList[2 * i].nChapterIndex), (int)(_pclsPositionList[2 * i].nParaIndex),(int)(_pclsPositionList[2 * i].nElemIndex), -1);
        _pSearchResultData->pSearchResultTable[i]->SetBeginPos(_clsTempPos);
        _pSearchResultData->pSearchResultTable[i]->SetEndPos(CT_RefPos((int)(_pclsPositionList[2 * i + 1].nChapterIndex), (int)(_pclsPositionList[2 * i + 1].nParaIndex),(int)(_pclsPositionList[2 * i + 1].nElemIndex), -1));
        _pSearchResultData->pLocationTable[i] = -1; // 未实现

        // 记录文本信息
        unsigned int _uHighLightenStartPos = 0, _uHighLightenEndPos = 0;
        string _str = GetText(_clsTempPos, _strText, _uHighLightenStartPos, _uHighLightenEndPos);
        _pSearchResultData->pHighlightStartPosTable[i] = _uHighLightenStartPos;
        _pSearchResultData->pHighlightEndPosTable[i] = _uHighLightenEndPos;
        _pSearchResultData->pSearchResultTable[i]->SetBookContent(_str);
    }

    m_pDKEBook->FreeFindTextResults(_pclsPositionList);

    DK_FREE(pText);
    return true;
}

//bool EpubBookReader::GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem *>* vBookmarks)
//{
//	if(curDKXBlock && vBookmarks)
//	{
//		if(curDKXBlock->GetAllBookmarks(vBookmarks))
//		{
//			return true;
//		}
//	}
//	return false;
//}

bool EpubBookReader::GetDKXProgressForUpgrade(DKXBlock* curDKXBlock, ICT_ReadingDataItem* localReadingDataItem)
{
    if(curDKXBlock && localReadingDataItem)
    {
        return curDKXBlock->GetProgress(localReadingDataItem);
    }
    return false;
}

bool EpubBookReader::GetCurrentPageATOM(ATOM & beginATOM, ATOM& endATOM)
{
    if(m_pDKEBook && m_pDKEPage)
    {
        DK_FLOWPOSITION startPos = m_pDKEPage->GetBeginPosition();
        DK_FLOWPOSITION endPos = m_pDKEPage->GetEndPosition();
        beginATOM.SetChapterIndex(startPos.nChapterIndex);
        beginATOM.SetParaIndex(startPos.nParaIndex);
        beginATOM.SetAtomIndex(startPos.nElemIndex);

        endATOM.SetChapterIndex(endPos.nChapterIndex);
        endATOM.SetParaIndex(endPos.nParaIndex);
        endATOM.SetAtomIndex(endPos.nElemIndex);
        return true;
    }
    return false;
}

ICT_ReadingDataItem* EpubBookReader::GetCurPageProgress()
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    if(!m_pDKEBook || !m_pDKEPage)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GetCurPageProgress m_pDKEBook||m_pDKEPage IS NULL!!!");
        return NULL;
    }

    ICT_ReadingDataItem* _pclsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);
    if(!_pclsReadingProgress)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GetCurPageProgress _pclsReadingProgress IS NULL");
        return NULL;
    }

    time_t _tTime;
    time(&_tTime);
    string _strTime  = CUtility::TransferTimeToString(_tTime);
    _pclsReadingProgress->SetCreateTime(_strTime);
    _pclsReadingProgress->SetCreator("DuoKan");

    DebugPrintf(DLC_JUGH, "m_tDKEFlowPos [%d, %d, %d]!!!", m_tDKEFlowPos.nChapterIndex, m_tDKEFlowPos.nParaIndex, m_tDKEFlowPos.nElemIndex);

    CT_RefPos clsBeginPos, clsEndPos;
    clsBeginPos.SetChapterIndex(m_tDKEFlowPos.nChapterIndex);
    clsBeginPos.SetParaIndex(m_tDKEFlowPos.nParaIndex);
    if (ComicsFrameModeValid() && (m_globalFrame >= 0) && m_globalFrame < m_pDKEBook->GetComicsFrameCount())
    {
        _pclsReadingProgress->SetComicsFrameMode(m_comicsFrame);
        clsBeginPos.SetAtomIndex(m_globalFrame);
    }
    else
    {
        clsBeginPos.SetAtomIndex(m_tDKEFlowPos.nElemIndex);
    }
    DK_FLOWPOSITION endPos = m_pDKEPage->GetEndPosition();
    clsEndPos.SetChapterIndex(endPos.nChapterIndex);
    clsEndPos.SetParaIndex(endPos.nParaIndex);
    clsEndPos.SetAtomIndex(endPos.nElemIndex);
    CompletePosRange(&clsBeginPos, &clsEndPos);

    _pclsReadingProgress->SetBeginPos(clsBeginPos);
    _pclsReadingProgress->SetEndPos(clsEndPos);
    _pclsReadingProgress->SetLastModifyTime(_strTime);
    DebugPrintf(DLC_LIUJT, "EpubBookReader::GetCurPageProgress save Position[%u,%u,%u] to DKX", clsBeginPos.GetChapterIndex(), clsBeginPos.GetParaIndex(), clsBeginPos.GetAtomIndex());
    return _pclsReadingProgress;
}

bool EpubBookReader::GotoBookMark(const ICT_ReadingDataItem* _pReadingEntry)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);

    if(!_pReadingEntry || !m_pDKEBook)
    {
        DebugLog(DLC_LIUJT, "EpubBookReader::GotoBookMark End0");
        return false;
    }

    DK_FLOWPOSITION flowPos;
    CT_RefPos clsRefPos = _pReadingEntry->GetBeginPos();
    flowPos.nChapterIndex = clsRefPos.GetChapterIndex();
    flowPos.nParaIndex    = clsRefPos.GetParaIndex();
    flowPos.nElemIndex    = clsRefPos.GetAtomIndex();

    clsRefPos = _pReadingEntry->GetEndPos();

    DebugPrintf(DLC_JUGH, "m_tDKEFlowPos [%d, %d, %d]!!!", m_tDKEFlowPos.nChapterIndex, m_tDKEFlowPos.nParaIndex, m_tDKEFlowPos.nElemIndex);
    DebugPrintf(DLC_JUGH, "flowPos [%d, %d, %d]!!!", flowPos.nChapterIndex, flowPos.nParaIndex, flowPos.nElemIndex);

    // ReParseContent内部根据m_bReParse是否为true来判定是否重新解析文件
    if (!ReParseContent(flowPos.nChapterIndex))
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GotoBookMark ReParseContent ERROR!!!");
        return false;
    }

    m_globalFrame = -1;
    if ((ICT_ReadingDataItem::PROGRESS == _pReadingEntry->GetUserDataType())
        && (IsComicsChapter(flowPos.nChapterIndex)))
    {
        m_comicsFrame = _pReadingEntry->IsComicsFrameMode();
        if (m_comicsFrame)
        {
            m_globalFrame = flowPos.nElemIndex;
        }
        // TODO(JUGH):ios将该值存为了帧号，为了阅读同步，kindle也需要这样处理
        flowPos.nElemIndex = 0;
    }
    return  GotoPage(flowPos);
}

string EpubBookReader::GetText(const ATOM & _clsStartPos, const string _strSearchText, unsigned int & _uHighLightenStartPos, unsigned int & _uHighLightenEndPos)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    if(!m_pDKEBook)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GetText m_pDKEBook IS NULL!!!");
        return _strSearchText;
    }

    DK_WCHAR* pSearchText = EncodingUtil::CharToWChar(_strSearchText.c_str());
    if (NULL == pSearchText)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GetText SEARCH CharToWChar IS NULL!!!");
        return _strSearchText;
    }

    const DK_WCHAR* pEllipsis          = L"...";
    unsigned int    uSnippetLen        = 90;
    DK_WCHAR        pResultString[200] = {0};
    DK_FLOWPOSITION clsFlowPos(_clsStartPos.GetChapterIndex(), _clsStartPos.GetParaIndex(), _clsStartPos.GetAtomIndex());

    if(DK_FAILED(this->m_pDKEBook->GetFindTextSnippet(clsFlowPos, pSearchText, pEllipsis, uSnippetLen, pResultString, &_uHighLightenStartPos, &_uHighLightenEndPos)))
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GetText GetFindTextSnippet IS FAILED!!!");
        DK_FREE(pSearchText);
        return _strSearchText;
    }
    DK_FREE(pSearchText);

    char* pResult = EncodingUtil::WCharToChar(pResultString);
    if (NULL == pResult)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GetText RESULT WCharToChar NULL!!!");
        return _strSearchText;
    }
    string strResult(pResult);
    DK_FREE(pResult);

    return strResult;
}

bool EpubBookReader::IsPositionInCurrentPage(const DK_FLOWPOSITION &_pos)
{
    if (NULL == m_pDKEPage)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::IsPositionInCurrentPage m_pDKEPage IS NULL!!!");
        return false;
    }

    DK_FLOWPOSITION beginPosOfPage = m_pDKEPage->GetBeginPosition();
    if (_pos < beginPosOfPage)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::IsPositionInCurrentPage _pos < beginPosOfPage!!!");
        return false;
    }

    DK_FLOWPOSITION endPosOfPage = m_pDKEPage->GetEndPosition();
    return (_pos < endPosOfPage);
}

long EpubBookReader::GetTotalPageNumber() const
{
    assert (NULL != m_pDKEBook);
    DebugPrintf(DLC_JUGH, "EpubBookReader::GetTotalPageNumber FINISHED (%d)", m_lPageCount);
    return m_lPageCount;
}

long EpubBookReader::GetPageNumber4CurrentChapter()
{
    assert (NULL != m_pDKEBook);

    return m_vPageCountOfChapter[m_tDKEFlowPos.nChapterIndex];//m_pDKEBook->GetPageCountOfChapter(m_tDKEFlowPos.nChapterIndex);
}

bool EpubBookReader::AddBMPProcessor(IBMPProcessor* _pProcessor)
{
    assert (NULL != _pProcessor);
    if (m_lstBMPProcessor.end() != find(m_lstBMPProcessor.begin(), m_lstBMPProcessor.end(), _pProcessor))
    {
        DebugLog(DLC_JUGH, "EpubBookReader::AddBMPProcessor IBMPProcessor IS EXISTING!!!");
        return true;
    }

    m_lstBMPProcessor.push_back(_pProcessor);
    return m_lstBMPProcessor.size() > 0;
}

bool EpubBookReader::RemoveBMPProcessor(IBMPProcessor* _pProcessor)
{
    assert (NULL != _pProcessor);
    vector<IBMPProcessor*>::iterator vi;
    vi = find(m_lstBMPProcessor.begin(), m_lstBMPProcessor.end(), _pProcessor);
    if (vi == m_lstBMPProcessor.end())
        return true;

    m_lstBMPProcessor.erase(vi);
    return true;
}

bool EpubBookReader::GetAllBMPProcessor(vector<IBMPProcessor*> &_ProcessorList)
{
    _ProcessorList = m_lstBMPProcessor;
    return true;
}

bool EpubBookReader::SetLayoutSettings(const DK_LAYOUTSETTING &_clsLayoutSetting)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);

    bool bSettingChanged = ABS(m_tLayoutSetting.dFontSize - _clsLayoutSetting.dFontSize) >= DoublePrecision 
        || ABS(m_tLayoutSetting.dIndent - _clsLayoutSetting.dIndent) >= DoublePrecision 
        || ABS(m_tLayoutSetting.dLineGap - _clsLayoutSetting.dLineGap) >= DoublePrecision 
        || ABS(m_tLayoutSetting.dParaSpacing - _clsLayoutSetting.dParaSpacing) >= DoublePrecision 
        || ABS(m_tLayoutSetting.dTabStop - _clsLayoutSetting.dTabStop) >= DoublePrecision ;

    if(bSettingChanged)
    {
        m_tLayoutSetting.dFontSize    = _clsLayoutSetting.dFontSize;
        m_tLayoutSetting.dIndent      = _clsLayoutSetting.dIndent;
        m_tLayoutSetting.dLineGap     = _clsLayoutSetting.dLineGap;
        m_tLayoutSetting.dParaSpacing = _clsLayoutSetting.dParaSpacing;
        m_tLayoutSetting.dTabStop     = _clsLayoutSetting.dTabStop;

        DebugPrintf(DLC_JUGH, "EpubBookReader::SetLayoutSettings font size=%d", m_tLayoutSetting.dFontSize);
        DebugPrintf(DLC_JUGH, "EpubBookReader::SetLayoutSettings dIndent=%f", m_tLayoutSetting.dIndent);
        DebugPrintf(DLC_JUGH, "EpubBookReader::SetLayoutSettings dLineGap=%f", m_tLayoutSetting.dLineGap);
        DebugPrintf(DLC_JUGH, "EpubBookReader::SetLayoutSettings dParaSpacing=%f", m_tLayoutSetting.dParaSpacing);
        DebugPrintf(DLC_JUGH, "EpubBookReader::SetLayoutSettings dTabStop=%f", m_tLayoutSetting.dTabStop);
        DebugPrintf(DLC_JUGH, "EpubBookReader::SetLayoutSettings dpi=%d", m_clsDKEParseOption.dpi);
    }

    m_bReParse = m_bReParse || bSettingChanged || m_tLayoutSetting.bOriginalLayout != _clsLayoutSetting.bOriginalLayout;

    m_tLayoutSetting.bOriginalLayout = _clsLayoutSetting.bOriginalLayout;
    DebugPrintf(DLC_JUGH, "EpubBookReader::SetLayoutSettings originalLayoUt=%d", m_tLayoutSetting.bOriginalLayout);

    return  true;
}

DK_IMAGE* EpubBookReader::GetPageBMP()
{
    return &m_tCurPageImage;
}

DKE_CHAPTER_TYPE EpubBookReader::GetPageChapterType(IDKEPage* page)
{
    if (NULL == page || NULL == m_pDKEBook)
    {
        return DKE_CHAPTER_UNKNOWN;
    }
    DK_FLOWPOSITION beginPos = page->GetBeginPosition();
    return m_pDKEBook->GetChapterType(beginPos.nChapterIndex);
}

bool EpubBookReader::RenderPage()
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    DK_PROFILE_FUNCTION;
    if(!m_pDKEPage)
    {
        return false;
    }

    if(ProduceBMPPage(m_pDKEPage))
    {
        DebugPrintf(DLC_JUGH, "%s, %d, %s, %s ProduceBMPPage end", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
        if (m_pBookTextController)
        {
            m_pBookTextController->Initialize<IDKEPageTraits>(m_pDKEBook, m_pDKEPage);
            m_pBookTextController->SetModeController(GetPdfModeController());
        }
        return true;
    }
    return false;

}

bool EpubBookReader::ProduceBMPPage(IDKEPage* pPage)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s start", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    if (NULL == pPage)
    {
        DebugPrintf(DLC_JUGH, "EpubBookReader::RenderPage is NULL!!!");
        return false;
    }

    // 当前页面的图像信息设置，初始为32色，需要经过BMPProcessor的处理
    SafeFreePointer(m_tCurPageImage.pbData);
    m_tCurPageImage.iColorChannels = 4;
    if (IsCurrentPageFullScreen())
    {
        m_tCurPageImage.iWidth = m_screenWidth;
        m_tCurPageImage.iHeight = m_screenHeight;
    }
    else
    {
        m_tCurPageImage.iWidth         = (int) m_clsDKEParseOption.pageBox.Width();
        m_tCurPageImage.iHeight        = (int) m_clsDKEParseOption.pageBox.Height();
    }
    m_tCurPageImage.iStrideWidth   = m_tCurPageImage.iWidth * m_tCurPageImage.iColorChannels;

    long iDataLen = m_tCurPageImage.iStrideWidth * m_tCurPageImage.iHeight;
    m_tCurPageImage.pbData         = DK_MALLOCZ_OBJ_N(BYTE8, iDataLen);
    if (NULL == m_tCurPageImage.pbData)
    {
        DebugPrintf(DLC_JUGH, "EpubBookReader::SetPageSize m_tCurPageImage.pbData is NULL!!!");
        return false;
    }

    // DEV信息根据DK_IMAGE来生成
    DK_BITMAPBUFFER_DEV dev;
    DK_FLOWRENDEROPTION renderOption;
    GetRenderOption(m_tCurPageImage, dev, renderOption);

    DK_FLOWRENDERRESULT  clsDKERenderResult;
    bool processComics = false;
    if (ComicsFrameModeValid())
    {
        DK_ASSERT(m_globalFrame >= 0);
        IDKEComicsFrame* pFrame = m_pDKEBook->GetComicsFrame(m_globalFrame);
        if (pFrame && DK_SUCCEEDED(pFrame->RenderFrame(renderOption, &clsDKERenderResult)))
        {
            processComics = true;
        }
    }

    if (!processComics && DK_FAILED(pPage->Render(renderOption, &clsDKERenderResult)))
    {
        DebugPrintf(DLC_JUGH, "EpubBookReader::RenderPage Render ERROR!!!");
        return false;
    }
    // 抖动当前图像
    DitherImage(pPage);

    if (!ProcessImage(m_tCurPageImage))
    {
        DebugPrintf(DLC_JUGH, "EpubBookReader::RenderPage Process ERROR!!!");
        return false;
    }

    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s edn", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    return true;
}

bool EpubBookReader::ProcessImage(DK_IMAGE& image)
{
    vector<IBMPProcessor*>::iterator vi;
    for (vi = m_lstBMPProcessor.begin(); vi != m_lstBMPProcessor.end(); ++vi)
    {
        if (NULL == (*vi) || !(*vi)->Process(&image))
        {
            return false;
        }
    }
    return true;
}

void EpubBookReader::DitherImage(IDKEPage* pPage)
{
    DebugPrintf(DLC_ZHAIGH, "EpubBookReader::%s() ConvertRGB32ToGray256", __FUNCTION__);
    if (!pPage || !DkFormat::DkImageHelper::ConvertRGB32ToGray256(&m_tCurPageImage))
    {
        return;
    }

    DebugPrintf(DLC_ZHAIGH, "EpubBookReader::%s() GetImageIterator", __FUNCTION__);
    IDKEImageIterator *pImageIterator = pPage->GetImageIterator();
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
                    if (InstantValid())
                    {
                        DebugPrintf(DLC_ZHAIGH, "EpubBookReader::%s() DitherGray256ToBinary", __FUNCTION__);
                        DkFormat::DkImageHelper::DitherGray256ToBinary(toDither);
                    }
                    else
                    {
                        DebugPrintf(DLC_ZHAIGH, "EpubBookReader::%s() DitherGray256ToGray16", __FUNCTION__);
                        DkFormat::DkImageHelper::DitherGray256ToGray16(toDither);
                    }
                }
            }
            else
            {
                break;
            }
        }

        pPage->FreeImageIterator(pImageIterator);
        pImageIterator = NULL;
    }

    if (InstantValid())
    {
        DebugPrintf(DLC_ZHAIGH, "EpubBookReader::%s() ConvertGray256ToBinary", __FUNCTION__);
        DkFormat::DkImageHelper::ConvertGray256ToBinary(m_tCurPageImage);
    }
    else
    {
        DebugPrintf(DLC_ZHAIGH, "EpubBookReader::%s() ConvertGray256ToGray16", __FUNCTION__);
        DkFormat::DkImageHelper::ConvertGray256ToGray16(m_tCurPageImage);
    }
}

void EpubBookReader::SetStopParsingFlag()
{
    m_bStopParse = true;
    m_pDKEBook->SetStopParsingFlag();
}

bool EpubBookReader::ReParseContent(int chapter)
{
    DK_PROFILE_FUNCTION;
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook);

    // CursorControler需要在释放Page之前释放,ParseContent会释放page
    DebugLog(DLC_JUGH, "EpubBookReader::ReParseContent Release m_pBookTextController  START!!!");
    if (m_pBookTextController)
    {
        m_pBookTextController->Release();
    }

    if (0 != m_pParseThread)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::ReParseContent CancelThread START!!!");
        SetStopParsingFlag();
        ThreadHelper::JoinThread(m_pParseThread, NULL);
        m_pParseThread = 0;
        DebugLog(DLC_JUGH, "EpubBookReader::ReParseContent CancelThread END!!!");
    }
    m_bStopParse = false;
    if (chapter < 0 || chapter >= m_lChapterCount)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::ReParseContent chapter ERROR!!!");
        return false;
    }

    if (m_bReParse)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::ReParseContent m_bReParse == TRUE!!!");
        m_pDKEPage = NULL;
        m_pDKEBook->ClearAllParsedPages();
        ClearVector();
        vector<CT_RefPos> temp;
        for (int i = 0; i < m_lChapterCount; ++i)
        {
            m_vPageIndexOfChapter.push_back(-1);
            m_vPageCountOfChapter.push_back(-1);
            m_vChapterParseStatus.push_back(false);
            m_vEndPosOfChapter.push_back(temp);
        }

        m_bReParse   = false;
        m_pDKEBook->SetBodyFontSize(m_tLayoutSetting.dFontSize);
        m_pDKEBook->SetLineGap(m_tLayoutSetting.dLineGap);
        double paraSpacing = m_tLayoutSetting.dParaSpacing - m_tLayoutSetting.dLineGap + 1;
        if (paraSpacing < 0)
        {
            paraSpacing = 0;
        }
        m_pDKEBook->SetParaSpacing(paraSpacing);
        m_pDKEBook->SetTabStop(m_tLayoutSetting.dTabStop);
        m_pDKEBook->SetFirstLineIndent(m_tLayoutSetting.dIndent);
        DKE_SetUseBookStyle(m_tLayoutSetting.bOriginalLayout);
        m_lPageCount   = 0;
    }

    DebugLog(DLC_JUGH, "EpubBookReader::ReParseContent CurChapterIndex START!!!");
    m_clsDKEParseOption.chapter = chapter;

    string strThreadName("EpubBookReader");
    if (0 != ThreadHelper::CreateThread(&m_pParseThread, ParseThread, this, strThreadName, false, 1024 * 1024, SERIALIZEDBOOK_THREAD_PRIORITY))
    {
        DebugLog(DLC_JUGH, "EpubBookReader::ReParseContent ERROR");
        return false;
    }

    DebugLog(DLC_JUGH, "EpubBookReader::ReParseContent FINISHED!!!");
    return true;
}

void* EpubBookReader::ParseThread(void* _pParam)
{
    if(!_pParam)
    {
        ThreadHelper::ThreadExit(0);
        return NULL;
    }
    //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    //pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    EpubBookReader* pReader = (EpubBookReader*)_pParam;
    pReader->m_eParseStatus = BOOKREADER_PARSE_START;
    if(!pReader->m_pDKEBook)
    {
        ThreadHelper::ThreadExit(0);
        return NULL;
    }

    const long&       lChapterCount  = pReader->m_lChapterCount;
    vector<long>&     lstPageCount   = pReader->m_vPageCountOfChapter;
    vector<bool>&     lstParseStatus = pReader->m_vChapterParseStatus;

    long lCurChapter = pReader->m_clsDKEParseOption.chapter;//clsParseOption.chapter;
    if (lCurChapter < 0 || lCurChapter >= lChapterCount || pReader->m_bStopParse)
    {
        ThreadHelper::ThreadExit(0);
        return NULL;
    }

    // 先解析当前章节
    pReader->ParseChapter(lCurChapter);
    if (pReader->m_bStopParse)
    {
        ThreadHelper::ThreadExit(0);
        return NULL;
    }

    // 解析前后章节
    long lNext = lCurChapter + 1;
    while (lNext < lChapterCount)
    {
        pReader->ParseChapter(lNext);
        ++lNext;  
        if (pReader->m_bStopParse)
        {
            ThreadHelper::ThreadExit(0);
            return NULL;
        }

        if (lstPageCount[lNext - 1] > 0)
        {
            break;
        }
    }

    long lPrev = lCurChapter - 1;
    while (lPrev >= 0)
    {
        pReader->ParseChapter(lPrev);
        --lPrev;  
        if (pReader->m_bStopParse)
        {
            ThreadHelper::ThreadExit(0);
            return NULL;
        }

        if (lstPageCount[lPrev + 1] > 0)
        {
            break;
        }
    }

    // 如果设置发生改变则解析其他章节
    int majorVersion = -1, minorVersion = -1;
    pReader->m_pDKEBook->GetDuokanBookVersion(majorVersion, minorVersion);
    string strMD5 = SystemSettingInfo::GetInstance()->GetDuokanBookMD5(majorVersion, minorVersion);
    if (!pReader->m_pPageTable)
    {
        pReader->m_pPageTable = new CPageTable(pReader->m_strDKEFilePath, DK_GetKernelVersion());
    }
    //CPageTable pageTable(pReader->m_strDKEFilePath, DK_GetKernelVersion());
    //PageTable tmpPageTable = pageTable.GetPageTable(strMD5);
    PageTable tmpPageTable = pReader->m_pPageTable->GetPageTable(strMD5);
    if (!tmpPageTable.IsNull() && tmpPageTable.chapterCont != pReader->m_lChapterCount)
    {
        tmpPageTable.Clear();
        pReader->m_pPageTable->Clear();
    }
    if(tmpPageTable.IsNull())
    {
        while ((lPrev >= 0 || lNext < lChapterCount))
        {
            if (lNext < lChapterCount && lstPageCount[lNext] <= 0)
            {
                pReader->ParseChapter(lNext);
                if (pReader->m_bStopParse)
                {
                    ThreadHelper::ThreadExit(0);
                    return NULL;
                }
            }

            if (lPrev >= 0 && lstPageCount[lPrev] <= 0)
            {
                pReader->ParseChapter(lPrev);
                if (pReader->m_bStopParse)
                {
                    ThreadHelper::ThreadExit(0);
                    return NULL;
                }
            }

            // 清除page缓存数据
            if (lNext < lChapterCount && (pReader->m_tDKEFlowPos.nChapterIndex + 1) < lNext)
            {
                pReader->m_pDKEBook->ClearChapterData(lNext);
                lstParseStatus[lNext] = false;
            }
            if (lPrev >= 0 && (lPrev + 1) < pReader->m_tDKEFlowPos.nChapterIndex)
            {
                pReader->m_pDKEBook->ClearChapterData(lPrev);
                lstParseStatus[lPrev] = false;
            }
            ++lNext, --lPrev;
        }

        // 将最新分页表存入CPageTable中
        long lPageCount = 0;
        vector<long>& vPageTable = pReader->m_vPageIndexOfChapter;
        for (int i = 0; i < lChapterCount; ++i)
        {
            vPageTable[i] = lPageCount;
            lPageCount += lstPageCount[i];
        }
        pReader->m_lPageCount = lPageCount;

        PageTable sPageTable;
        sPageTable.chapterCont = pReader->m_lChapterCount;
        sPageTable.pageCount = pReader->m_lPageCount;
        sPageTable.strMD5 = strMD5;
        if(pReader->m_lChapterCount == (int)pReader->m_vEndPosOfChapter.size()
            && pReader->m_lChapterCount == (int)pReader->m_vPageIndexOfChapter.size())
        {
            for(int i = 0; i < pReader->m_lChapterCount; i++)
            {
                sPageTable.vEndPosOfChapter.push_back(pReader->m_vEndPosOfChapter[i]);
            }
            for(int i = 0; i < pReader->m_lChapterCount; i++)
            {
                sPageTable.vPageIndexOfChapter.push_back(pReader->m_vPageIndexOfChapter[i]);
            }
            pReader->m_pPageTable->SavePageTable(sPageTable);
        }
		UIBookParseStatusListener::GetInstance()->FireBookParseFinishedEvent();
    }
    else if(pReader->m_lPageCount == 0)
    {
        pReader->m_lPageCount = tmpPageTable.pageCount;
        pReader->m_vPageIndexOfChapter.clear();
        for(int i = 0; i < (int)tmpPageTable.vPageIndexOfChapter.size(); i++)
        {
            pReader->m_vPageIndexOfChapter.push_back(tmpPageTable.vPageIndexOfChapter[i]);
        }
        pReader->m_vEndPosOfChapter.clear();
        for(int i = 0; i < (int)tmpPageTable.vEndPosOfChapter.size(); i++)
        {
            pReader->m_vEndPosOfChapter.push_back(tmpPageTable.vEndPosOfChapter[i]);
        }
        UIBookParseStatusListener::GetInstance()->FireBookParseFinishedEvent();
    }
    pReader->m_eParseStatus = BOOKREADER_PARSE_FINISHED;
    pthread_detach(pthread_self());
    pReader->m_pParseThread = 0;
    ThreadHelper::ThreadExit(0);
    return NULL;
}

void EpubBookReader::ParseChapter(long _lChapterIndex)
{
    assert (NULL != m_pDKEBook);
    if (_lChapterIndex < 0 || _lChapterIndex >= m_lChapterCount || m_vChapterParseStatus[_lChapterIndex])
    {
        return;
    }

    DKE_PARSER_OPTION clsParseOption = m_clsDKEParseOption;
    // 全屏页需要设置为屏幕宽高
    if (m_pDKEBook->GetChapterType(_lChapterIndex) == DKE_CHAPTER_FULLSCREEN)
    {
        clsParseOption.pageBox = DK_BOX(0, 0, m_screenWidth, m_screenHeight);
    }
    clsParseOption.chapter = _lChapterIndex;
    if (!m_bStopParse && DK_FAILED(m_pDKEBook->ParseContent(clsParseOption)))
    {
        m_eParseStatus = BOOKREADER_PARSE_ERROR;
    }

    if (!m_bStopParse)
    {
        LockScope lockScope(m_parseLock);
        m_vChapterParseStatus[_lChapterIndex] = true;
        const long pageCount = m_pDKEBook->GetPageCountOfChapter(_lChapterIndex);
        m_vPageCountOfChapter[_lChapterIndex] = pageCount;

        vector<CT_RefPos> endPosOfCurChapter;
        for (int i = 0; i < pageCount; i++)
        {
            DK_FLOWPOSITION posKernelEnd;
            IDKEPage* pPage = m_pDKEBook->GetPageOfChapter(_lChapterIndex, i);
            if (pPage)
            {
                posKernelEnd = pPage->GetEndPosition();
            }
            CT_RefPos posAppEnd(posKernelEnd.nChapterIndex, posKernelEnd.nParaIndex, posKernelEnd.nElemIndex, -1);
            endPosOfCurChapter.push_back(posAppEnd);
        }
        m_vEndPosOfChapter[_lChapterIndex] = endPosOfCurChapter;
        endPosOfCurChapter.clear();
    }
}

bool EpubBookReader::GotoPage(const DK_FLOWPOSITION& pos)
{
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook);

    DK_FLOWPOSITION flowPos = pos;
    unsigned int uToChapter = flowPos.nChapterIndex;
    if ((long)uToChapter >= m_lChapterCount)
    {
        uToChapter   = m_lChapterCount - 1;
        m_lPageIndex = 0;
        flowPos = DK_FLOWPOSITION(uToChapter, 0, 0);
    }

    int iTryTime = 100;
    int iSleepMilliSecond = 200;

    int pageNumInChapter = -1;
    while(iTryTime > 0)
    {
        if (m_bStopParse)
        {
            DebugPrintf(DLC_JUGH, "EpubBookReader::GotoPage stop is true, return");
            return false;
        }
        DebugPrintf(DLC_JUGH, "EpubBookReader::GotoPage trytime: %d, bStop: %d", iTryTime, m_bStopParse);
        m_pDKEPage = m_pDKEBook->GetPage(flowPos);
        pageNumInChapter = m_pDKEBook->GetPageNumOfChapter(flowPos);
        if (NULL == m_pDKEPage || pageNumInChapter < 0)
        {
            iTryTime--;
            //某些书的首章是空html，所以需要在此判断是否已经解析完
            if(!m_vChapterParseStatus[flowPos.nChapterIndex])
            {
                usleep(iSleepMilliSecond * 1000);
                continue;
            }
            else
            {
                flowPos.nChapterIndex++;
                continue;
            }

        }
        break;
    }
    if(!m_pDKEPage)
    {
        return false;
    }
    m_lPageIndex = pageNumInChapter;

    if (IsComicsFrameMode())
    {
        const long startFrame = m_pDKEPage->GetGlobalFrameIndex(0);
        const long endFrame = startFrame + m_pDKEPage->GetFrameCount();
        if (m_globalFrame < startFrame || m_globalFrame >= endFrame)
        {
            m_globalFrame = startFrame;
        }
    }
    ClearParsedPages();
    m_tDKEFlowPos = m_pDKEPage->GetBeginPosition();
    DebugPrintf(DLC_JUGH, "In EpubBookReader::GotoPage before RenderPage, stop : %d", m_bStopParse);
    return RenderPage();
}

bool EpubBookReader::ParseTocInfo(IDKETocPoint* _pTocPoint)
{
    if (!_pTocPoint)
    {
        DebugPrintf(DLC_LIWEI, "EpubBookReader::ParseTocInfo _pTocPoint is NULL!!!");
        return false;
    }
    const DK_WCHAR* pTitle = _pTocPoint->GetTitle();
    if (NULL != pTitle)
    {
        string strTitle = EncodeUtil::ToString(pTitle);
        if(!strTitle.empty())
        {
            wstring wstrDest = _pTocPoint->GetDest();
            DK_FLOWPOSITION clsFlowPos;
            DK_TOCINFO* pChildToc = new DK_TOCINFO;
            if (!wstrDest.empty() && DK_SUCCEEDED(_pTocPoint->GetDestPosition(clsFlowPos)))
            {
                if(wstrDest.find(L"#") != std::wstring::npos)
                {
                    pChildToc->SetAnchor(_pTocPoint->GetDestAnchor());
                }
            }
            pChildToc->SetValid(_pTocPoint->IsContentValid());
            pChildToc->SetCurTOC(strTitle, clsFlowPos);
            pChildToc->SetDepth(_pTocPoint->GetDepth());
            m_vTOCList.push_back(pChildToc);
        }
    }

    for (int i = 0; i < _pTocPoint->GetChildCount(); ++i)
    {
        IDKETocPoint* pChild = _pTocPoint->GetChildByIndex(i);
        if (pChild)
        {
            ParseTocInfo(pChild);
        }
    }
    return true;
}


bool EpubBookReader::SetTextGrayScaleLevel(double _dGamma)
{
    m_iTextGamma = _dGamma;
    return true;
}

bool EpubBookReader::SetFontSmoothType(DK_FONT_SMOOTH_TYPE _eFontSmoothType)
{
    m_eFontSmoothType = _eFontSmoothType;
    return true;
}

bool EpubBookReader::SetGraphicsGrayScaleLevel(double _dGamma)
{
    m_iGraphicsGamma = _dGamma;
    return true;
}

bool EpubBookReader::GotoPage(long _lChapterIndex, long _lPageIndex, bool bReverse)
{
    DK_PROFILE_FUNCTION;
    DebugPrintf(DLC_JUGH, "%s, %d, %s, %s", __FILE__,  __LINE__, "EpubBookReader", __FUNCTION__);
    assert (NULL != m_pDKEBook);
    DebugPrintf(DLC_JUGH, "EpubBookReader::GotoPage() BEFORE: (%d, %d)!!!", _lChapterIndex, _lPageIndex);
    _lChapterIndex = (m_lChapterCount > 0 && _lChapterIndex >= m_lChapterCount) ? m_lChapterCount - 1 : _lChapterIndex;  //判断章节索引是否合法
    _lChapterIndex = (_lChapterIndex < 0) ? 0 : _lChapterIndex;

    bool bPreChapterParse = _lChapterIndex > 0 && !m_vChapterParseStatus[_lChapterIndex - 1];						//判断前一章节是否解析完
    bool bNextChapterParse = _lChapterIndex < (m_lChapterCount - 1) && !m_vChapterParseStatus[_lChapterIndex + 1];  //判断后一章节时候解析完
    if ((bPreChapterParse || bNextChapterParse) && !ReParseContent(_lChapterIndex))
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GotoPage() ReParse ERROR!!!");
        return false;
    }

    _lPageIndex  = (_lPageIndex < 0) ? 0 : _lPageIndex;
    int  iTryTime          = HTMLBOOKREADER_FAILTIMES;
    int  iSleepSeconds     = HTMLBOOKREADER_SLEEPSECONDS;
    long lPageCountOfChapter = m_pDKEBook->GetPageCountOfChapter(_lChapterIndex);

    // sleep时间每次增加1s，以减少时间片轮询消耗，增加gotopage成功的机会
    while (!m_vChapterParseStatus[_lChapterIndex] && iTryTime-- > 0 && _lPageIndex >= lPageCountOfChapter)
    {
        sleep(iSleepSeconds);
        lPageCountOfChapter = m_pDKEBook->GetPageCountOfChapter(_lChapterIndex);
        DebugPrintf(DLC_JUGH, "EpubBookReader::GotoPage() wait for parse finished %d", lPageCountOfChapter);
    }

    if (iTryTime <= 0 || lPageCountOfChapter < 0)
    {
        DebugLog(DLC_JUGH, "EpubBookReader::GotoPage() wait for parse finished error");
        return false;
    }
    else if (m_vChapterParseStatus[_lChapterIndex] && _lPageIndex >= (lPageCountOfChapter - 1))
    {
        _lPageIndex = (_lPageIndex > (lPageCountOfChapter - 1)) ? (lPageCountOfChapter - 1) : _lPageIndex;
    }

    if (iSleepSeconds >= HTMLBOOKREADER_SLEEPMAXSECONDS)
    {
        iSleepSeconds = HTMLBOOKREADER_SLEEPMAXSECONDS;
    }
    DebugPrintf(DLC_JUGH, "EpubBookReader::GotoPage() AFTER:  (%d, %d/%d)!!!", _lChapterIndex, _lPageIndex, lPageCountOfChapter);

    while (iTryTime-- > 0)
    {
        m_pDKEPage = m_pDKEBook->GetPageOfChapter(_lChapterIndex, _lPageIndex);
        if (NULL != m_pDKEPage)
        {
            DebugLog(DLC_JUGH, "EpubBookReader::GotoPage() SUCCESS!!!");
            ClearParsedPages();
            m_tDKEFlowPos = m_pDKEPage->GetBeginPosition();
            m_lPageIndex  = _lPageIndex;
            if (IsComicsFrameMode())
            {
                m_globalFrame = m_pDKEPage->GetGlobalFrameIndex(0);
                const long frameCount = m_pDKEPage->GetFrameCount();
                if (bReverse)
                {
                    m_globalFrame += (frameCount - 1);
                }
            }
            RenderPage();
            return true;
        }
        sleep(iSleepSeconds);
    }

    DebugLog(DLC_JUGH, "EpubBookReader::GotoPage() ERROR!!!");
    return false;
}

void EpubBookReader::ClearParsedPages()
{
    DebugLog(DLC_JUGH, "EpubBookReader::ClearParsedPages() start");
    LockScope lockScope(m_parseLock);
    const int preChapter = m_tDKEFlowPos.nChapterIndex;
    const int curChapter = m_clsDKEParseOption.chapter;
    const int diff = preChapter - curChapter;
    DebugPrintf(DLC_JUGH, "preChapter = %d, curChapter = %d, diff = %d", preChapter, curChapter, diff);
    int startChapter = -1, endChapter = -1;
    switch(abs(diff))
    {
    case MovePagesInSameChapter:
        // do nothing
        break;
    case MovePagesInAdjoiningChapter:
        // delete parsedPages for one chapter
        {
            startChapter = endChapter = preChapter + diff;
        }
        break;
    case MovePagesInSeperatedChaptersWithSameAdjoiningChapter:
        // delete parsedPages for two chapters
        {
            startChapter = (diff > 0) ? preChapter : ((preChapter == 0) ? 0 : preChapter - 1);
            endChapter = (diff > 0) ? ((preChapter == m_lPageCount - 1) ? m_lPageCount - 1 : preChapter + 1) : preChapter;
        }
        break;
    default:
        // delete parsedPages for three chapters
        {
            startChapter = (preChapter == 0) ? 0 : preChapter - 1;
            endChapter = (preChapter == m_lPageCount - 1) ? m_lPageCount - 1 : preChapter + 1;
        }
        break;
    }

    while (startChapter <= endChapter)
    {
        DebugPrintf(DLC_JUGH, "startChapter = %d", startChapter);
        if (startChapter >= 0 && startChapter < m_lChapterCount)
        {
            m_pDKEBook->ClearParsedPagesForChapter(startChapter);
            m_vChapterParseStatus[startChapter] = false;
        }
        startChapter++;
    }
}

bool EpubBookReader::SetInstantTurnPageModel(bool _bIsInstant)
{
    m_bIsInstant = _bIsInstant;
    return true;
}

void EpubBookReader::SetScreenSize(int screenWidth, int screenHeight)
{
    m_screenWidth = screenWidth > 0 ? screenWidth : 0;
    m_screenHeight = screenHeight > 0 ? screenHeight : 0;

    DK_BOX &clsPageBox   = m_clsDKEParseOption.pageBox;

    bool bSizeChanged = ABS(screenWidth - clsPageBox.X1) >= DoublePrecision 
        || ABS(screenHeight - clsPageBox.Y1) >= DoublePrecision 
        || ABS(clsPageBox.X0) >= DoublePrecision 
        || ABS(clsPageBox.Y0) >= DoublePrecision;

    if(bSizeChanged)
    {
        clsPageBox.X0 = 0;
        clsPageBox.Y0 = 0;
        clsPageBox.X1 = screenWidth;
        clsPageBox.Y1 = screenHeight;
    }

    m_bReParse = m_bReParse || bSizeChanged;
}

bool EpubBookReader::IsCurrentPageFullScreen()
{
    return GetPageChapterType(m_pDKEPage) == DKE_CHAPTER_FULLSCREEN;
}

// 数据升级有如下几种情况，
// 1：升级了书籍，原来硬盘上的数据需要升级
// 2. 获取服务器阅读数据，比本地版本老，需要升级
// 3. 数据存储格式变更（如添加chapter id),需要补全数据
// 4. 内核升级，需要更新flow position
bool EpubBookReader::UpdateReadingData(
    const char* oldBookRevision,
    const std::vector<ICT_ReadingDataItem *> * _pvOld,
    const char* newBookRevision,
    std::vector<ICT_ReadingDataItem *> * _pvNew)
{

    // 不用转化，返回true
    if (NULL != _pvOld && _pvOld->size() == 0)
    {
        return true;
    }
    if (NULL == _pvOld || NULL == _pvNew || 0 == _pvOld->size() || 0 != _pvNew->size())
    {
        return false;
    }

    if (NULL == m_pDKEBook)
    {
        return false;
    }

    IDKEBook* pBook = m_pDKEBook;
    bool needProcessBookRevision = false;
    if (oldBookRevision && newBookRevision && strcmp(oldBookRevision, newBookRevision) < 0)
    {
        needProcessBookRevision = true;
    }

    for (unsigned int i = 0; i < _pvOld->size(); ++i)
    {
        CT_RefPos begin = (*_pvOld)[i]->GetBeginPos();
        CT_RefPos end = (*_pvOld)[i]->GetEndPos();
        bool needFillChapterIdOrOffset = false;
        if (!(begin.HasChapterId()
            && begin.HasOffsetInChapter()
            && end.HasChapterId()
            && end.HasOffsetInChapter()))
        {
            needFillChapterIdOrOffset = true;
        }
        // 没有(chapterId, offsetInChapter)对，无法做书籍改变相关的升级
        if (needProcessBookRevision && needFillChapterIdOrOffset)
        {
            continue;
        }

        if (!begin.HasChapterId() || !end.HasChapterId())
        {
            begin.SetChapterId(pBook->GetChapterID(begin.GetChapterIndex()));
            end.SetChapterId(pBook->GetChapterID(end.GetChapterIndex()));
        }

        // 不需要处理书籍升级
        if (!needProcessBookRevision)
        {
            // 有offsetInChapter，说明内核升级，需要反算flow position
            if (begin.HasOffsetInChapter() && end.HasOffsetInChapter())
            {
                DK_FLOWPOSITION beginPos, endPos;
                int endChapterIndex = end.GetChapterIndex();
                int endOffset = end.GetOffsetInChapter();
                // 两个值相同DKOffsetRange2FlowPositionRange会失败
                // 需要调整
                if (begin.GetChapterIndex() == end.GetChapterIndex()
                    && begin.GetOffsetInChapter() == end.GetOffsetInChapter())
                {
                    ++endOffset;
                }
                if (DKR_OK == pBook->DKOffsetRange2FlowPositionRange(
                    begin.GetChapterIndex(),
                    begin.GetOffsetInChapter(),
                    endChapterIndex,
                    endOffset,
                    &beginPos, 
                    &endPos))
                {
                    ICT_ReadingDataItem *readingData = DKXReadingDataItemFactory::CreateReadingDataItem();
                    if (readingData)
                    {
                        readingData->SetCreateTime((*_pvOld)[i]->GetCreateTime());
                        readingData->SetCreator("DuoKan");

                        begin.SetParaIndex(beginPos.nParaIndex);
                        begin.SetAtomIndex(beginPos.nElemIndex);
                        readingData->SetBeginPos(begin);

                        end.SetParaIndex(endPos.nParaIndex);
                        end.SetAtomIndex(endPos.nElemIndex);
                        readingData->SetEndPos(end);

                        readingData->SetLastModifyTime((*_pvOld)[i]->GetLastModifyTime());
                        readingData->SetBookContent((*_pvOld)[i]->GetBookContent());
                        readingData->SetUserContent((*_pvOld)[i]->GetUserContent());
                        readingData->SetUserDataType((*_pvOld)[i]->GetUserDataType());
                        readingData->SetUploaded((*_pvOld)[i]->IsUploaded());

                        _pvNew->push_back(readingData);
                    }
                }

            }
            else
            {
                DK_FLOWPOSITION beginPos = begin.ToFlowPosition();
                DK_FLOWPOSITION endPos = end.ToFlowPosition();

                DK_UINT beginChapter = 0;
                DK_OFFSET beginOffset = 0;
                DK_UINT endChapter = 0;
                DK_OFFSET endOffset = 0;
                if (DKR_OK == pBook->DKFlowPositionRange2OffsetRange(beginPos, endPos, &beginChapter, &beginOffset, &endChapter, &endOffset))
                {
                    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() beginChapter %d, beginOffset %d", __FUNCTION__, beginChapter, beginOffset);
                    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() endChapter %d, endOffset %d", __FUNCTION__, endChapter, endOffset);

                    ICT_ReadingDataItem *readingData = DKXReadingDataItemFactory::CreateReadingDataItem();
                    if (readingData)
                    {
                        readingData->SetCreateTime((*_pvOld)[i]->GetCreateTime());
                        readingData->SetCreator("DuoKan");

                        begin.SetChapterIndex(beginPos.nChapterIndex);
                        begin.SetParaIndex(beginPos.nParaIndex);
                        begin.SetAtomIndex(beginPos.nElemIndex);
                        begin.SetOffsetInChapter(beginOffset);
                        readingData->SetBeginPos(begin);

                        end.SetChapterIndex(endPos.nChapterIndex);
                        end.SetParaIndex(endPos.nParaIndex);
                        end.SetAtomIndex(endPos.nElemIndex);
                        end.SetOffsetInChapter(endOffset);
                        readingData->SetEndPos(end);

                        readingData->SetLastModifyTime((*_pvOld)[i]->GetLastModifyTime());
                        readingData->SetBookContent((*_pvOld)[i]->GetBookContent());
                        readingData->SetUserContent((*_pvOld)[i]->GetUserContent());
                        readingData->SetUserDataType((*_pvOld)[i]->GetUserDataType());
                        readingData->SetUploaded((*_pvOld)[i]->IsUploaded());

                        _pvNew->push_back(readingData);
                    }
                }
            }
        }
        else  // 只需要处理数据升级
        {
            std::wstring wOldBookRevision = EncodeUtil::ToWString(oldBookRevision);
            std::wstring wNewBookRevision = EncodeUtil::ToWString(newBookRevision);
            std::wstring wBeginChapterId = EncodeUtil::ToWString(begin.GetChapterId());
            std::wstring wEndChapterId = EncodeUtil::ToWString(end.GetChapterId());


            DK_OFFSET newBeginOffset = -1, newEndOffset = -1;
            DK_BOOL deleted = false;
            if (DKR_OK != pBook->UpdateOffsetByPatch(
                wOldBookRevision.c_str(),
                wNewBookRevision.c_str(), 
                wBeginChapterId.c_str(),
                begin.GetOffsetInChapter(),
                &newBeginOffset,
                &deleted))
            {
                newBeginOffset = begin.GetOffsetInChapter();
            }
            if (DKR_OK != pBook->UpdateOffsetByPatch(
                wOldBookRevision.c_str(),
                wNewBookRevision.c_str(), 
                wEndChapterId.c_str(),
                end.GetOffsetInChapter(),
                &newEndOffset,
                &deleted))
            {
                newEndOffset = end.GetOffsetInChapter();
            }
            int newBeginChapterIndex = pBook->GetChapterIndex(wBeginChapterId.c_str());
            int newEndChapterIndex = pBook->GetChapterIndex(wEndChapterId.c_str());
            if (newBeginChapterIndex < 0 || newEndChapterIndex < 0)
            {
                continue;
            }
            // 两个值相同DKOffsetRange2FlowPositionRange会失败
            // 需要调整
            if (newBeginChapterIndex == newEndChapterIndex
                && newBeginOffset == newEndOffset)
            {
                ++newEndOffset;
            }
            DK_FLOWPOSITION beginPos, endPos;
            if (DKR_OK != pBook->DKOffsetRange2FlowPositionRange(
                newBeginChapterIndex,
                newBeginOffset,
                newEndChapterIndex,
                newEndOffset,
                &beginPos, 
                &endPos))
            {
                continue;
            }
            ICT_ReadingDataItem *readingData = DKXReadingDataItemFactory::CreateReadingDataItem();
            if (readingData)
            {
                readingData->SetCreateTime((*_pvOld)[i]->GetCreateTime());
                readingData->SetCreator("DuoKan");

                begin.SetChapterIndex(newBeginChapterIndex);
                begin.SetOffsetInChapter(newBeginOffset);
                begin.SetParaIndex(beginPos.nParaIndex);
                begin.SetAtomIndex(beginPos.nElemIndex);
                readingData->SetBeginPos(begin);

                end.SetChapterIndex(newEndChapterIndex);
                end.SetOffsetInChapter(newEndOffset);
                end.SetParaIndex(endPos.nParaIndex);
                end.SetAtomIndex(endPos.nElemIndex);
                readingData->SetEndPos(end);

                readingData->SetLastModifyTime((*_pvOld)[i]->GetLastModifyTime());

                DK_WCHAR* wBookContent;
                if(DKR_OK != pBook->GetChapterTextOfRange(beginPos, endPos, &wBookContent))
                {
                    continue;
                }

                std::string bookContent = EncodeUtil::ToString(wBookContent);
                pBook->FreeChapterText(wBookContent);

                readingData->SetBookContent(bookContent.c_str());
                readingData->SetUserContent((*_pvOld)[i]->GetUserContent());
                readingData->SetUserDataType((*_pvOld)[i]->GetUserDataType());
                readingData->SetUploaded((*_pvOld)[i]->IsUploaded());

                _pvNew->push_back(readingData);
            }
        }
    }

    DebugPrintf(DLC_ZHAIGH, "MobiBookReader::%s() Finish", __FUNCTION__);
    return true;
}

void EpubBookReader::CompletePosRange(CT_RefPos* startPos, CT_RefPos* endPos)
{
    if (DK_NULL == startPos || DK_NULL == endPos)
    {
        return;
    }
    if (DK_NULL == m_pDKEBook)
    {
        return;
    }
    DK_UINT beginChapter = 0;
    DK_OFFSET beginOffset = 0;
    DK_UINT endChapter = 0;
    DK_OFFSET endOffset = 0;
    if (DKR_OK == m_pDKEBook->DKFlowPositionRange2OffsetRange(
        startPos->ToFlowPosition(),
        endPos->ToFlowPosition(),
        &beginChapter,
        &beginOffset,
        &endChapter,
        &endOffset))
    {
        startPos->SetOffsetInChapter(beginOffset);
        endPos->SetOffsetInChapter(endOffset);
    }
    const DK_WCHAR* beginChapterId = m_pDKEBook->GetChapterID(beginChapter);
    if (beginChapterId != NULL)
    {
        startPos->SetChapterId(beginChapterId);
    }
    const DK_WCHAR* endChapterId = m_pDKEBook->GetChapterID(endChapter);
    if (endChapterId != NULL)
    {
        endPos->SetChapterId(endChapterId);
    }
}

BOOK_INFO_DATA EpubBookReader::GetBookInfo()
{
    if (m_bookInfoDataSet)
    {
        return m_bookInfoData;
    }
    m_bookInfoDataSet = true;
    if (NULL == m_pDKEBook)
    {
        return m_bookInfoData;
    }

    const DKEBOOKINFO* dkeBookInfo = m_pDKEBook->GetBookInfo();
    if (dkeBookInfo->pAuthor && 0 != wcslen(dkeBookInfo->pAuthor))
    {
        char* utf8Author = EncodingUtil::WCharToChar(dkeBookInfo->pAuthor, EncodingUtil::UTF8);
        if (utf8Author)
        {
            m_bookInfoData.author = utf8Author;
            DK_FREE(utf8Author);
        }
    }
    else if (dkeBookInfo->pCreator && 0 != wcslen(dkeBookInfo->pCreator))
    {
        char* utf8Creator = EncodingUtil::WCharToChar(dkeBookInfo->pCreator, EncodingUtil::UTF8);
        if (utf8Creator)
        {
            m_bookInfoData.author = utf8Creator;
            DK_FREE(utf8Creator);
        }
    }
    else if(dkeBookInfo->pPublisher && 0 != wcslen(dkeBookInfo->pPublisher))
    {
        char* utf8Publisher = EncodingUtil::WCharToChar(dkeBookInfo->pPublisher, EncodingUtil::UTF8);
        if (utf8Publisher)
        {
            m_bookInfoData.author = utf8Publisher;
            DK_FREE(utf8Publisher);
        }
    }

    if (dkeBookInfo->pTitle)
    {
        char* utf8Title = EncodingUtil::WCharToChar(dkeBookInfo->pTitle, EncodingUtil::UTF8);
        if (utf8Title)
        {
            m_bookInfoData.title = utf8Title;
            DK_FREE(utf8Title);
        }
    }

    const DKFILEINFO* coverFileInfo = m_pDKEBook->GetBookCover();
    if (NULL != coverFileInfo) 
    {
        m_bookInfoData.coverData = coverFileInfo->pFileDataBuf;
        m_bookInfoData.coverDataLen = coverFileInfo->lFileDataLength;
    }
    return m_bookInfoData;
}

IDKEBook* EpubBookReader::DrmOpen(const char* fileName)
{
    assert(NeedsDrmOpen(fileName));
    ConfigFile config;
    if (!config.LoadFromFile(PathManager::EpubFileToInfoFile(fileName).c_str()))
    {
        return NULL;
    }

    std::string userId, deviceId, orderId, bookId, magicId, cert;
    if (!config.GetString(ConstStrings::USER_ID, &userId)
        || !config.GetString(ConstStrings::ORDER_UUID, &orderId)
        || !config.GetString(ConstStrings::BOOK_UUID, &bookId)
        || !config.GetString(ConstStrings::MAGIC_ID, &magicId)
        || !config.GetString(ConstStrings::CERT, &cert))
    {
        return NULL;
    }
    deviceId = DeviceInfo::GetDeviceID();
    std::vector<unsigned char> certHex;
    if (!EncodeUtil::HexToBin(cert.c_str(), &certHex))
    {
        return NULL;
    }
    std::vector<unsigned char> rsaDecodeResult;
    if (!EncodeUtil::RsaDecode(&certHex[0], certHex.size(), &rsaDecodeResult))
    {
        return NULL;
    }
    std::vector<unsigned char> md5Digest = EncodeUtil::Md5(userId.c_str(), userId.size());
    std::vector<unsigned char> md5Tmp =  EncodeUtil::Md5(deviceId.c_str(), deviceId.size());
    md5Digest.insert(md5Digest.end(), md5Tmp.begin(), md5Tmp.end());
    md5Tmp = EncodeUtil::Md5(orderId.c_str(), orderId.size());
    md5Digest.insert(md5Digest.end(), md5Tmp.begin(), md5Tmp.end());
    md5Tmp = EncodeUtil::Md5(bookId.c_str(), bookId.size());
    md5Digest.insert(md5Digest.end(), md5Tmp.begin(), md5Tmp.end());
    md5Tmp = EncodeUtil::Md5(magicId.c_str(), magicId.size());
    md5Digest.insert(md5Digest.end(), md5Tmp.begin(), md5Tmp.end());

    std::vector<unsigned char> md5Key = EncodeUtil::Md5(&md5Digest[0], md5Digest.size());
    std::vector<unsigned char> realKey;
    EncodeUtil::AesDecode(&md5Key[0], md5Key.size(), &rsaDecodeResult[0], rsaDecodeResult.size(), &realKey);

    DKE_DRMKEY dkeDrmKey;
    dkeDrmKey.key = &realKey[0];
    dkeDrmKey.length = realKey.size();

    std::wstring wFileName = EncodeUtil::ToWString(fileName);
    if (wFileName.empty())
    {
        return NULL;
    }
    IDKEBook* dkeBook = (IDKEBook*)DKE_OpenDRMDocument(wFileName.c_str(), DKE_CONTAINER_ZIP, NULL, &dkeDrmKey);
    return dkeBook;
}

bool EpubBookReader::NeedsDrmOpen(const char* fileName)
{
    if (!PathManager::IsUnderBookStore(fileName))
    {
        return false;
    }

    std::string infoFile = PathManager::EpubFileToInfoFile(fileName);
    if (infoFile.empty())
    {
        return false;
    }
    ConfigFile config;
    std::string cert;
    if (config.LoadFromFile(infoFile.c_str()) 
        && config.GetString("cert", &cert) 
        && !cert.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

long EpubBookReader::GetPageIndex(const CT_RefPos& startPos)
{
    if (NULL == m_pDKEBook)
    {
        return -1;
    }
    long nChapterIndex = startPos.GetChapterIndex();
    if (nChapterIndex >= (int)m_vEndPosOfChapter.size() || nChapterIndex >= (int)m_vPageIndexOfChapter.size())
    {
        return -1;
    }

    const vector<CT_RefPos>& endPosOfCurChapter = m_vEndPosOfChapter[nChapterIndex];
    long pageIndex = m_vPageIndexOfChapter[nChapterIndex];
    long pageCount = GetTotalPageNumber();
    if (endPosOfCurChapter.size() <= 0 || pageIndex < 0 || pageCount <= 0)
    {
        return -1;
    }

    for (size_t i = 0; i < endPosOfCurChapter.size(); ++i)
    {
        CT_RefPos endPos = endPosOfCurChapter[i];
        if (endPos.CompareTo(startPos) > 0)
        {
            return pageIndex + i;
        }
    }
    return -1;
}

double EpubBookReader::GetPercent(const CT_RefPos& startPos)
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

bool EpubBookReader::GetPosByPageIndex(int pageIndex, DK_FLOWPOSITION* pos)
{
	if(pos && pageIndex >= 0)
	{
		long chapterIndex = m_lChapterCount - 1;
		while(chapterIndex >= 0)
		{
			if(m_vPageIndexOfChapter[chapterIndex] <= pageIndex)
			{
				break;
			}
			chapterIndex --;
		}
		if(chapterIndex >= 0)
		{
			int pageIndexInChapter = pageIndex - m_vPageIndexOfChapter[chapterIndex];
			const CT_RefPos& pagePos = m_vEndPosOfChapter[chapterIndex][pageIndexInChapter];
			*pos = pagePos.ToFlowPosition();
			return true;
		}
		
	}
	return false;
}

void EpubBookReader::ClearVector()
{
    m_vPageIndexOfChapter.clear();
    m_vPageCountOfChapter.clear();
    m_vChapterParseStatus.clear();
    m_vEndPosOfChapter.clear();
}

std::string EpubBookReader::GetBookRevision()
{
    if (NULL == m_pDKEBook)
    {
        return "";
    }

    return EncodeUtil::ToString(m_pDKEBook->GetBookRevision());
}

using namespace dk::bookstore;
bool EpubBookReader::IsPurchasedDuoKanBook()
{
    if (m_bookId.empty())
    {
        return false;
    }
    bool isTrial = BookStoreInfoManager::GetInstance()->IsLocalFileTrial(m_bookId.c_str());
    if (isTrial)
    {
        return false;
    }
    return true;
}

bool EpubBookReader::RenderSinglePage(IDKEPage* pPage)
{
    return ProduceBMPPage(pPage);
}

BOOK_DIRECTION_TYPE EpubBookReader::GetBookDirection() const
{
    assert (NULL != m_pDKEBook);
    BOOK_DIRECTION_TYPE directionType = BOOK_MODE_HTB_DIRECTION_LTR;
    if (m_pDKEBook)
    {
        DKEWRITINGOPT opt = m_pDKEBook->GetBookWritingOpt();
        DKE_WRITING_MODE_TYPE modeType = opt.writingMode;
        bool directionLTR = (opt.writingDirection == DKE_WRITING_DIRECTION_LTR);
        switch (modeType)
        {
        case DKE_WRITING_MODE_HTB:
            directionType = directionLTR ? BOOK_MODE_HTB_DIRECTION_LTR : BOOK_MODE_HTB_DIRECTION_RTL;
            break;
        case DKE_WRITING_MODE_VRL:
            directionType = directionLTR ? BOOK_MODE_VRL_DIRECTION_LTR : BOOK_MODE_VRL_DIRECTION_RTL;
            break;
        case DKE_WRITING_MODE_VLR:
            directionType = directionLTR ? BOOK_MODE_VLR_DIRECTION_LTR : BOOK_MODE_VLR_DIRECTION_RTL;
            break;
        default:
            break;
        }
    }

    return directionType;
}

bool EpubBookReader::IsComicsChapter(long chapterIndex) const
{
    return m_pDKEBook && m_pDKEBook->IsComicsChapter(chapterIndex);
}

bool EpubBookReader::SetComicsFrameMode(bool comicsFrame, long frameIndex)
{
    if (m_pDKEPage && m_comicsFrame != comicsFrame)
    {
        m_comicsFrame = comicsFrame;
        if (comicsFrame && frameIndex < 0)
        {
            m_globalFrame = m_pDKEPage->GetGlobalFrameIndex(0);
        }
        else
        {
            m_globalFrame = frameIndex;
        }
        return RenderPage();
    }
    return true;
}

bool EpubBookReader::IsComicsFrameMode() const
{
    return m_comicsFrame;
}

bool EpubBookReader::ComicsFrameModeValid() const
{
    return IsComicsChapter(m_tDKEFlowPos.nChapterIndex) && IsComicsFrameMode();
}

bool EpubBookReader::InstantValid()
{
    return m_bIsInstant && !IsComicsChapter(m_tDKEFlowPos.nChapterIndex) && !IsCurrentPageFullScreen();
}

void EpubBookReader::GetRenderOption(const DK_IMAGE& image, DK_BITMAPBUFFER_DEV& dev, DK_FLOWRENDEROPTION& renderOption)
{
    dev.lWidth       = image.iWidth;
    dev.lHeight      = image.iHeight;
    dev.nPixelFormat = DK_PIXELFORMAT_RGB32;
    dev.lStride      = image.iStrideWidth;
    dev.nDPI         = RenderConst::SCREEN_DPI;
    dev.pbyData      = image.pbData;

    DK_RENDERGAMMA  gma;
    gma.dTextGamma = m_iTextGamma;

    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice     = &dev;
    renderOption.gamma       = gma;
    renderOption.fontSmoothType = m_eFontSmoothType;
    if (InstantValid())
    {
        renderOption.fontSmoothType = DK_FONT_SMOOTH_BINARY;
    }

    memset(dev.pbyData, 0xFF, image.iStrideWidth * image.iHeight); //画布颜色变为白色
}

void EpubBookReader::UpdateGallery(IDKEGallery* pGallery)
{
    DebugPrintf(DLC_JUGH, "EpubBookReader::UpdateGallery start");
    if (pGallery)
    {
        const DK_BOX box = pGallery->GetBoundary();
        const int cellIndex = pGallery->GetCurActiveCell();
        if (cellIndex >= 0 && cellIndex < pGallery->GetCellCount())
        {
            DK_IMAGE gallery;
            gallery.iColorChannels = 4;
            gallery.iWidth         = box.Width();
            gallery.iHeight        = box.Height();
            gallery.iStrideWidth   = gallery.iWidth * gallery.iColorChannels;

            long iDataLen = gallery.iStrideWidth * gallery.iHeight;
            gallery.pbData         = DK_MALLOCZ_OBJ_N(BYTE8, iDataLen);
            if (NULL == gallery.pbData)
            {
                DebugPrintf(DLC_JUGH, "EpubBookReader::UpdateGallery gallery.pbData is NULL!!!");
                return;
            }

            // DEV信息根据DK_IMAGE来生成
            DK_BITMAPBUFFER_DEV dev;
            DK_FLOWRENDEROPTION renderOption;
            GetRenderOption(gallery, dev, renderOption);

            pGallery->RenderCell(cellIndex, renderOption);
            ProcessImage(gallery);

            CopyImageEx(m_tCurPageImage, box.X0, box.Y0, gallery, 0, 0, box.Width(), box.Height());
            SafeFreePointer(gallery.pbData);
        }
    }
}

bool EpubBookReader::UpdateFrameInGallery(const DK_POS& pos, GALLERY_TURNPAGE turnPage)
{
    DebugPrintf(DLC_JUGH, "EpubBookReader::UpdateFrameInGallery start");
    bool ret = false;
    if (m_pBookTextController && m_pBookTextController->IsInitialized())
    {
        IDKEGallery* pGallery = NULL;
        if (m_pBookTextController->HitTestGallery(pos, &pGallery) && pGallery)
        {
            const int offset = (turnPage == PAGEDOWN) ? 1 : ((turnPage == PAGEUP) ? -1 : 0);
            const int count = pGallery->GetCellCount();
            int cell = pGallery->GetCurActiveCell() + offset;
#ifndef KINDLE_FOR_TOUCH
            if (turnPage == PAGEDOWN && cell >= count)
            {
                cell = 0;
            }
            else if (turnPage == PAGEUP && cell < 0)
            {
                cell = count - 1;
            }
#endif
            if (cell >= 0 && cell < count)
            {
                ret = true;
                pGallery->SetCurActiveCell(cell);
                UpdateGallery(pGallery);
            }
            m_pBookTextController->FreeHitTestGallery(pGallery);
        }
    }
    return ret;
}

bool EpubBookReader::GotoNextFrameInGallery(const DK_POS& pos)
{
    return UpdateFrameInGallery(pos, PAGEDOWN);
}

bool EpubBookReader::GotoPreFrameInGallery(const DK_POS& pos)
{
    return UpdateFrameInGallery(pos, PAGEUP);
}

bool EpubBookReader::IsFirstPageInChapter()
{
    return (m_lPageIndex == 0);
}

bool EpubBookReader::IsLastPageInChapter()
{
    const long& nChapterIndex = m_tDKEFlowPos.nChapterIndex;
    long lPageCount = -1;
    if ((long)m_vPageCountOfChapter.size() > (long)nChapterIndex)
    {
        lPageCount = m_vPageCountOfChapter[nChapterIndex];
    }
    return m_lPageIndex == (lPageCount - 1);
}

