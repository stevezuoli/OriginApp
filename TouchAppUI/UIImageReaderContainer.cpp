#include <stdlib.h>
#include "TouchAppUI/DkReaderPage.h"
#include "I18n/StringManager.h"
#include "CommonUI/CPageNavigator.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIBookReaderProgressBar.h"
#include "Common/FileManager_DK.h"
#include "Utility.h"
#include "Framework/CDisplay.h"
#include "Framework/CHourglass.h"
#include "BookInfoManager.h"
#include "ArchiveParser/ArchiveFactory.h"
#include "DkStream.h"
#include "SQM.h"
#include "Common/WindowsMetrics.h"
#include "BookReader/PageTable.h"
#include "KernelVersion.h"
#include "TouchAppUI/UITocDlg.h"

using namespace DkFormat;
using namespace WindowsMetrics;

#define TITLE_BAR_HEIGHT                   30
#define BOTTOM_BAR_HEIGHT                  45
#define BAR_TEXT_MARGIN_HORIZON            30
#define READING_PROGRESS_BAR_HEIGHT        2

UIImageReaderContainer::UIImageReaderContainer(UIContainer *pParent)
    : UIContainer(pParent, IDSTATIC)
    , m_bIsDisposed(FALSE)
    , m_pGotoPageDlg(NULL)
    , m_ReaderProgressBar(this,IDSTATIC)
    , m_pImageStream(NULL)
    , m_pImageHandler(NULL)
    , m_iRotateAngle(0)
    , m_eReadingMode(PDF_RM_NormalPage)
    , m_bIsIntelliEdgingOn(FALSE)
    , m_uCurImage(0)
    , m_uTotalImages(0)
    , m_iBookId(-1)
    , m_nFileId(knMainFileId)
    , m_pPassword(NULL)
    , m_fIsDownFinished(FALSE)
    , m_fIsUpFinished(FALSE)
    , m_bExistBookMarkForCurPage(false)
    , m_iIndexOfBookMarkForCurPage(-1)
    , m_dkxBlock(NULL)
    , m_jumpHistory(-1)
    , m_jumpDirection(GO_NONE)
{
    m_gestureListener.SetImageReaderContainer(this);
    m_gestureDetector.SetListener(&m_gestureListener);
    memset(&(m_CartoonRect[0]), 0, sizeof(DK_RECT));
    memset(&(m_CartoonRect[1]), 0, sizeof(DK_RECT));
    memset(&(m_CartoonRect[2]), 0, sizeof(DK_RECT));
    memset(&(m_CartoonRect[3]), 0, sizeof(DK_RECT));
    m_imgOutput.pbData = NULL;
}

UIImageReaderContainer::~UIImageReaderContainer()
{
    if (NULL != m_dkxBlock)
    {
        m_dkxBlock->Serialize();
    }

    DKXManager::DestoryDKXBlock(m_dkxBlock);
    DKXManager::GetInstance()->Reset();
    Dispose();
}

BOOL UIImageReaderContainer::Initialize(UINT nFileId, INT iBookId, IDkStream *pStream, const char *pPassword, int totalImage)
{
    if (!SetImage(nFileId, iBookId, pStream, pPassword))
    {
        return FALSE;
    }
	
    InitUI();
    SetTotalImage(totalImage);


    PageTable pageTableData;
    pageTableData.pageCount = m_uTotalImages;
    CPageTable pageTable(m_strFilePath, DK_GetKernelVersion(), -1);
    pageTableData.strMD5 = 
        SystemSettingInfo::GetInstance()->GetDuokanBookMD5(-1, -1);
    pageTable.SavePageTable(pageTableData);
    return TRUE;
}

BOOL UIImageReaderContainer::SetImage(UINT nFileId, INT iBookId, IDkStream *pStream, const char *pPassword)
{
    if(!pStream)
    {
        return FALSE;
    }

    if (pPassword)
    {
        SafeFreePointer(m_pPassword);
        m_pPassword = (char *)strdup(pPassword);
        if (NULL == m_pPassword)
        {
            return FALSE;
        }
    }

    m_iBookId = iBookId;
    m_nFileId = nFileId;
    m_pImageStream= pStream;

    // 不用释放
    CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
    if(NULL == pFileManager)
    {
        return FALSE;
    }
    
    PCDKFile pDkFile = pFileManager->GetFileById(iBookId);
    if(0 == pDkFile)
    {
        return FALSE;
    }
    
    m_strFilePath.clear();
    m_strFilePath = (PCHAR)strdup(pDkFile->GetFilePath());
    if(m_strFilePath.empty())
    {
        return FALSE;
    }
    if (NULL != m_dkxBlock && m_dkxBlock->GetFilePath() != m_strFilePath)
    {
        DKXManager::DestoryDKXBlock(m_dkxBlock);
        m_dkxBlock = NULL;
    }
    if (NULL == m_dkxBlock)
    {
        m_dkxBlock = DKXManager::CreateDKXBlock(m_strFilePath.c_str());
    }

    IArchiverParser *pArchiver = NULL;
    HRESULT hr = CArchiveFactory::CreateArchiverInstance(m_strFilePath.c_str(), &pArchiver);
    if(FAILED(hr) || NULL == pArchiver)
    {
        return FALSE;
    }

    char* pCurFileName = NULL;
    m_strCurFileName.clear();
    if (SUCCEEDED(pArchiver->GetFileNameByIndex((INT)m_nFileId, &pCurFileName)))
    {
        m_strCurFileName = pCurFileName;
        pArchiver->ReleaseFileName(pCurFileName);
    }
    SafeDeletePointer(pArchiver);

	PdfModeController modeController;
    m_dkxBlock->GetPDFModeController(&modeController);
	m_ctlImage.m_iRotation = modeController.m_iRotation;
	m_ctlImage.m_eReadingMode = modeController.m_eReadingMode;
	m_ctlImage.m_uCurSubPage = modeController.m_uCurSubPage;
	m_ctlImage.m_uSubPageOrder = modeController.m_uSubPageOrder;

    m_strBookName = pDkFile->GetFileName();
    if(!InitMember() || !InitImageHandler())
    {
        m_strFilePath.clear();
        DKXManager::DestoryDKXBlock(m_dkxBlock);
        return FALSE;
    }
    return TRUE;
}

bool UIImageReaderContainer::InitMember()
{

    INT iRotateAngle         = m_ctlImage.m_iRotation; 
    ReadingMode eReadingMode = m_ctlImage.m_eReadingMode;
    m_ReaderProgressBar.SetProgressBarMode(SystemSettingInfo::GetInstance()->GetProgressBarMode());
    if((iRotateAngle == 0 || iRotateAngle == 90 ||
        iRotateAngle == 180 || iRotateAngle == 270) && 
        (eReadingMode >= PDF_RM_NormalPage && eReadingMode <= PDF_RM_Split2Page))
    {        
        if(m_iRotateAngle != iRotateAngle || m_eReadingMode != eReadingMode)
        {
            m_iRotateAngle = iRotateAngle;
            m_eReadingMode = eReadingMode;
        }
        return true;
    }
    return false;
}

bool UIImageReaderContainer::InitImageHandler()
{
    m_pImageHandler = DkImageHelper::CreateImageHandlerInstance(m_pImageStream);
    
    SafeDeletePointer(m_pImageStream);
    
    if(!ApplySettings())
    {
        return false;
    }

    return true;
}

void UIImageReaderContainer::InitUI()
{
    m_BottomBar.MoveWindow(m_iLeft, m_iTop + m_iHeight - BOTTOM_BAR_HEIGHT, m_iWidth, BOTTOM_BAR_HEIGHT);
    m_BottomBar.SetBackTransparent(0);
    m_BottomBar.Show(FALSE);
    AppendChild(&m_BottomBar);

    m_TxtReadingPages.SetFont (0, 0, 18);
    m_TxtReadingPages.SetAlign (ALIGN_RIGHT);
    m_TxtReadingPages.MoveWindow (370, 763, 200, 37);
    m_TxtReadingPages.Show(FALSE);

    m_TxtReadingPercent.SetFont (0, 0, 18);
    m_TxtReadingPercent.MoveWindow (30, 763, 200, 37);
    m_TxtReadingPercent.Show(FALSE);

    m_ReaderProgressBar.SetMaximum(m_uTotalImages);
    m_ReaderProgressBar.SetProgress(m_uCurImage);
    m_ReaderProgressBar.SetProgressBarMode(FALSE);
	int horizontalMargin = SystemSettingInfo::GetInstance()->GetPageHorizontalMargin(READINGLAYOUT_FULLSCREEN);

	m_ReaderProgressBar.SetLeftMargin(horizontalMargin);
	int progressBarHeight = GetWindowMetrics(UIBookReaderProgressBarHeightIndex);
	m_ReaderProgressBar.MoveWindow(m_iLeft, m_iTop + m_iHeight - progressBarHeight, m_iWidth, progressBarHeight);

    AppendChild(&m_TxtReadingPages);
    AppendChild(&m_TxtReadingPercent);
    AppendChild(&m_ReaderProgressBar);
}

void UIImageReaderContainer::MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight)
{
    UIWindow::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);
}

void UIImageReaderContainer::OnEnter()
{
    UpdateSettings();
}

void UIImageReaderContainer::OnLeave()
{
    SaveSettings();
}

void UIImageReaderContainer::OnLoad()
{
    if (!CheckBookExists())
    {
        CPageNavigator::BackToHome();
        return;
    }

    if (CUtility::StringEndWith(m_strFilePath.c_str(), ".zip", TRUE) 
        || CUtility::StringEndWith(m_strFilePath.c_str(), ".rar", TRUE))
    {
        if(!InitStream())
        {
            CPageNavigator::BackToHome();
            return;
        }
    }

    if (!InitImageHandler())
    {
        DebugLog(DLC_ERROR, "InitFormatRender  Return ERROR!");
    }
}

void UIImageReaderContainer::OnUnLoad()
{
    SafeDeletePointer(m_pImageHandler);
    SafeDeletePointer(m_pImageStream);
}

BOOL UIImageReaderContainer::OnKeyPress(INT32 iKeyCode)
{
    return UIWindow::OnKeyPress(iKeyCode);
}

void UIImageReaderContainer::HandlePageUpDown(BOOL fIsUp)
{
    if (m_BottomBar.IsVisible())
    {
        m_BottomBar.Show(FALSE);
    }

    if (m_TxtReadingPages.IsVisible())
    {
        m_TxtReadingPages.Show(FALSE);
    }

    if (m_TxtReadingPercent.IsVisible())
    {
        m_TxtReadingPercent.Show(FALSE);
    }

    m_fIsDownFinished = FALSE;
    m_fIsUpFinished = FALSE;
    if(PDF_RM_NormalPage == m_ctlImage.m_eReadingMode)
    {
        m_fIsDownFinished = TRUE;
        m_fIsUpFinished   = TRUE;
    }
    else if (PDF_RM_Split4Page == m_ctlImage.m_eReadingMode
            || PDF_RM_Split2Page == m_ctlImage.m_eReadingMode)
    {
        unsigned int uPageCount = (PDF_RM_Split4Page == m_ctlImage.m_eReadingMode) ? 4 : 2;
        if(fIsUp)
        {
            if (m_ctlImage.m_uCurSubPage > 0)
            {
                m_ctlImage.m_uCurSubPage--;
                ApplySettings();
                Repaint();
            }
            else
            {
                m_fIsDownFinished = FALSE;
                m_fIsUpFinished   = TRUE;
				m_ctlImage.m_uCurSubPage = uPageCount - 1;
            }
        }
        else
        {
            if (m_ctlImage.m_uCurSubPage < uPageCount - 1)
            {
                m_ctlImage.m_uCurSubPage++;
                ApplySettings();
                Repaint();
            }
            else
            {
                m_fIsDownFinished = TRUE;
                m_fIsUpFinished   = FALSE;
				m_ctlImage.m_uCurSubPage = 0;
            }
        }        
    }
    
    if (m_pParent)
        m_pParent->OnKeyPress(fIsUp ? KEY_LPAGEUP : KEY_LPAGEDOWN);
}

BOOL UIImageReaderContainer::GotoLastPage()
{
    if(PDF_RM_NormalPage == m_ctlImage.m_eReadingMode)
    {
        return TRUE;
    }
    else if((PDF_RM_Split4Page == m_ctlImage.m_eReadingMode || 
        PDF_RM_Split2Page == m_ctlImage.m_eReadingMode) && m_ctlImage.m_uCurSubPage > 0)
    {
        ApplySettings();
        return TRUE;
    }

    return FALSE;
}

bool UIImageReaderContainer::ApplySettings()
{
    if(NULL == m_pImageHandler)
    {
        return false;
    }

    switch(m_ctlImage.m_eReadingMode)
    {
    case PDF_RM_NormalPage:
        {
            memset(&(m_CartoonRect[0]), 0, sizeof(DK_RECT));
            memset(&(m_CartoonRect[1]), 0, sizeof(DK_RECT));
            memset(&(m_CartoonRect[2]), 0, sizeof(DK_RECT));
            memset(&(m_CartoonRect[3]), 0, sizeof(DK_RECT));
        }
        break;
    case PDF_RM_Split4Page:
        {
            const int iImgWidth     = m_pImageHandler->GetWidth();
            const int iImgHeight    = m_pImageHandler->GetHeight();
            const int iSubImgWidth  = (iImgWidth >> 1);
            const int iSubImgHeight = (iImgHeight >> 1);

            DK_RECT clsRect1 = {0,            0,             iSubImgWidth, iSubImgHeight};
            DK_RECT clsRect2 = {iSubImgWidth, 0,             iImgWidth,    iSubImgHeight};
            DK_RECT clsRect3 = {0,            iSubImgHeight, iSubImgWidth, iImgHeight};
            DK_RECT clsRect4 = {iSubImgWidth, iSubImgHeight, iImgWidth,    iImgHeight};

            DK_RECT* pCurRect1 = &clsRect1;
            DK_RECT* pCurRect2 = &clsRect2;
            DK_RECT* pCurRect3 = &clsRect3;
            DK_RECT* pCurRect4 = &clsRect4;
            switch(m_ctlImage.m_uSubPageOrder)
            {
            case 1:
                {
                    pCurRect1 = &clsRect2;
                    pCurRect2 = &clsRect1;
                    pCurRect3 = &clsRect4;
                    pCurRect4 = &clsRect3;
                }
                break;
            case 2:
                {
                    pCurRect1 = &clsRect1;
                    pCurRect2 = &clsRect3;
                    pCurRect3 = &clsRect2;
                    pCurRect4 = &clsRect4;
                }
                break;
            case 3:
                {
                    pCurRect1 = &clsRect2;
                    pCurRect2 = &clsRect4;
                    pCurRect3 = &clsRect1;
                    pCurRect4 = &clsRect3;
                }
                break;
            default:
                {
                }
                break;
            } 
            m_CartoonRect[0] = *pCurRect1;
            m_CartoonRect[1] = *pCurRect2;
            m_CartoonRect[2] = *pCurRect3;
            m_CartoonRect[3] = *pCurRect4;
        }
        break;
    case PDF_RM_Split2Page:
        {
            const int iImgWidth     = m_pImageHandler->GetWidth();
            const int iImgHeight    = m_pImageHandler->GetHeight();
            const int iSubImgWidth  = (iImgWidth >> 1);
            const int iSubImgHeight = (iImgHeight >> 1);

            const DK_RECT clsRectLeft = {0,            0,             iSubImgWidth, iImgHeight};
            const DK_RECT clsRectRight = {iSubImgWidth, 0,             iImgWidth,    iImgHeight};
            const DK_RECT clsRectBottom = {0,          iSubImgHeight, iImgWidth,    iImgHeight};
            const DK_RECT clsRectTop = {0,             0,             iImgWidth,    iSubImgHeight};
            if (m_ctlImage.m_uSubPageOrder)
            {
                m_CartoonRect[0] = clsRectRight;
                m_CartoonRect[1] = clsRectLeft;
            }
            else
            {
                m_CartoonRect[0] = clsRectLeft;
                m_CartoonRect[1] = clsRectRight;
            }
        }
    default:
        break;
    }
    return true;
}

bool UIImageReaderContainer::SaveSettings()
{
	PdfModeController modeController;
	modeController.m_iRotation = m_ctlImage.m_iRotation;
	modeController.m_eReadingMode = m_ctlImage.m_eReadingMode;
	modeController.m_uCurSubPage = m_ctlImage.m_uCurSubPage;
	modeController.m_uSubPageOrder = m_ctlImage.m_uSubPageOrder;
    m_dkxBlock->SetPDFModeController(modeController);
    return true;
}

bool UIImageReaderContainer::UpdateSettings()
{
    INT iRotateAngle = m_ctlImage.m_iRotation; 
    ReadingMode eReadingMode = m_ctlImage.m_eReadingMode;
    m_ReaderProgressBar.SetProgressBarMode(SystemSettingInfo::GetInstance()->GetProgressBarMode());
    if((iRotateAngle == 0 || iRotateAngle == 90 ||
        iRotateAngle == 180 || iRotateAngle == 270) && 
        (eReadingMode >= PDF_RM_NormalPage && eReadingMode <= PDF_RM_Split2Page))
    {        
        if(m_iRotateAngle != iRotateAngle || m_eReadingMode != eReadingMode)
        {
            m_iRotateAngle = iRotateAngle;
            m_eReadingMode = eReadingMode;

            ApplySettings();
        }
        return true;
    }

    return false;
}

void UIImageReaderContainer::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    if (NULL == m_dkxBlock)
    {
        return;
    }
    switch(dwCmdId)
    {
    case ID_BTN_TOUCH_ADD_BOOKMARK:
        SwitchBookMarkOffOn();
        break;
    default:
        break;
    }
}

void UIImageReaderContainer::SwitchBookMarkOffOn()
{
    if (m_bExistBookMarkForCurPage)
    {
        m_dkxBlock->RemoveBookmarkByIndex(m_iIndexOfBookMarkForCurPage);
    }
    else
    {
        time_t _tTime;
        time(&_tTime);
        string _strTime  = CUtility::TransferTimeToString(_tTime);

        CT_ReadingDataItemImpl clsBookMark(ICT_ReadingDataItem::BOOKMARK);
        clsBookMark.SetCreateTime(_strTime);
        clsBookMark.SetLastModifyTime(_strTime);
        clsBookMark.SetCreator("DuoKan");
        clsBookMark.SetBookContent(m_strCurFileName);
        clsBookMark.SetBeginPos(CT_RefPos(m_uCurImage, 0, 0, -1));
        clsBookMark.SetEndPos(CT_RefPos(m_uTotalImages, 0, 0, -1));
        m_dkxBlock->AddBookmark(&clsBookMark);
    }
    m_dkxBlock->Serialize();
    Repaint();

    SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_BOOKMARK_ADD);
}

bool UIImageReaderContainer::DrawBookMark(DK_IMAGE drawingImg)
{
    m_bExistBookMarkForCurPage   = false;
    m_iIndexOfBookMarkForCurPage = -1;
    if (NULL == m_dkxBlock || m_dkxBlock->GetBookmarkCount() == 0)
    {
        return true;
    }

	// 从DKX读取数据
    vector<ICT_ReadingDataItem *> vBookmarks;
	if(!m_dkxBlock->GetAllBookmarks(&vBookmarks))
	{
		return false;
	}

    for (unsigned int i = 0; i < vBookmarks.size(); ++i)
    {
        if (NULL == vBookmarks[i])
        {
            continue;
        }

        switch ((vBookmarks[i])->GetUserDataType())
        {
        case ICT_ReadingDataItem::BOOKMARK:
            // 书签
            {
                ATOM bookmarkPos = (vBookmarks[i])->GetBeginPos();
                DK_FLOWPOSITION flowPos(bookmarkPos.GetChapterIndex(), bookmarkPos.GetParaIndex(), bookmarkPos.GetAtomIndex());
                if ((UINT)bookmarkPos.GetChapterIndex() == m_uCurImage)
                {
                    m_bExistBookMarkForCurPage = true;
                    m_iIndexOfBookMarkForCurPage = i;

                    SPtr<ITpImage> spImageCursor = ImageManager::GetImage(IMAGE_BOOKMARK);
                    DK_RECT rcSelf;
                    rcSelf.right  = m_iWidth - m_iLeftMargin;
                    rcSelf.left   = rcSelf.right - spImageCursor->bmWidth;
                    rcSelf.top    = 0;
                    rcSelf.bottom = rcSelf.top + spImageCursor->bmHeight;
                    DK_IMAGE imgSelf;
                    if (S_OK == CropImage(drawingImg, rcSelf, &imgSelf))
                    {
                        CTpGraphics grf(imgSelf);
                        grf.DrawImageBlend(spImageCursor.Get(), 0, 0, 0, 0, spImageCursor->bmWidth, spImageCursor->bmHeight);
                    }
                }
            }
            break;
        default:
            break;
        }

        delete vBookmarks[i];
    }

    return true;
}

void UIImageReaderContainer::TocHandler(int showTable)
{
    UITocDlg tocDlg(this, m_iBookId, m_dkxBlock, NULL, m_strBookName.c_str());
    tocDlg.MoveWindow(m_iLeft, m_iTop, m_iWidth, m_iHeight);
    tocDlg.SetShowDialog(showTable);
    tocDlg.DoModal();
    if(UITocDlg::GOTOBOOKMARK == tocDlg.GetEndFlag())
    {
        ICT_ReadingDataItem* _pclsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);
        if(m_dkxBlock && _pclsReadingProgress)
        {
            m_dkxBlock->GetProgress(_pclsReadingProgress);
            int _iCurIndex = _pclsReadingProgress->GetBeginPos().GetChapterIndex();
            SafeDeletePointerEx(_pclsReadingProgress);

            DkReaderPage *pReaderPage = dynamic_cast<DkReaderPage *>(m_pParent);
            if(((int)m_uCurImage != _iCurIndex) && pReaderPage)
            {
                pReaderPage->GotoImgPageNo(_iCurIndex);
            }
        }
    }
}

void UIImageReaderContainer::NavigateToPage(int pageIndex, bool isRollBack)
{
	DkReaderPage *pReaderPage = dynamic_cast<DkReaderPage *>(m_pParent);
    if(pReaderPage)
    {
    	if(isRollBack)
		{
			m_jumpDirection = (m_jumpDirection == GO_LEFT ? GO_RIGHT : GO_LEFT);
		}
    	else if(m_jumpDirection == GO_NONE)
		{
			m_jumpDirection = GO_LEFT;
		}
		m_jumpHistory = GetCurrentPageIndex();
        pReaderPage->GotoImgPageNo(pageIndex + 1);
    }
}

HRESULT UIImageReaderContainer::Draw(DK_IMAGE drawingImg)
{
    if(NULL == m_pImageHandler)
    {
        return E_FAIL;
    }
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(DrawBackGround(drawingImg));
	m_BottomBar.Show(FALSE);
	m_TxtReadingPages.Show(FALSE);
	m_TxtReadingPercent.Show(FALSE);

    if(!AdaptScreen(drawingImg))
    {
        return E_FAIL;
    }

    // TODO: 此处待优化
    SafeFreePointer(m_imgOutput.pbData);
    m_imgOutput.iColorChannels = drawingImg.iColorChannels;
    m_imgOutput.iWidth         = drawingImg.iWidth;
    m_imgOutput.iHeight        = drawingImg.iHeight;
    m_imgOutput.iStrideWidth   = drawingImg.iStrideWidth;

    long iDataLen = m_imgOutput.iStrideWidth * m_imgOutput.iHeight;
    m_imgOutput.pbData         = DK_MALLOCZ_OBJ_N(BYTE8, iDataLen);
    if (NULL == m_imgOutput.pbData)
    {
        return E_FAIL;
    }
    memcpy(m_imgOutput.pbData, drawingImg.pbData, iDataLen);
    
    CHAR _cText[50] = {0};
    snprintf(_cText, sizeof(_cText), "%d/%d", m_uCurImage, m_uTotalImages);
    m_ReaderProgressBar.SetProgressText(string(_cText));

    m_ReaderProgressBar.SetMaximum(m_uTotalImages);
    m_ReaderProgressBar.SetProgress(m_uCurImage);

    UpdateReadingProgress();

    CHourglass* pIns = CHourglass::GetInstance();
    if (pIns)
    {
        pIns->Stop();
    }
    
    int iSize = GetChildrenCount();
    for (int i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin)
        {
            RTN_HR_IF_FAILED(pWin->Draw(drawingImg));
        }
        else
        {
        }
    }
    DrawBookMark(drawingImg);
    return hr;
}

HRESULT UIImageReaderContainer::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    grf.EraserBackGround();
    return hr;
}

void UIImageReaderContainer::Dispose()
{
    UIContainer::Dispose();
    OnDispose(m_bIsDisposed);
}

void UIImageReaderContainer::OnDispose(BOOL bIsDisposed)
{
    if(m_bIsDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
    Finalize();
}

void UIImageReaderContainer::Finalize()
{
    SafeDeletePointer(m_pImageHandler);
    SafeDeletePointer(m_pImageStream);
    SafeFreePointer(m_pPassword);
    m_strFilePath.clear();
    m_strBookName.clear();
    m_strCurFileName.clear();
    SafeDeletePointer(m_pGotoPageDlg);
    SafeFreePointer(m_imgOutput.pbData);
}

void UIImageReaderContainer::ResetContainer()
{
    OnLeave();

    Finalize();

    m_iRotateAngle = 0;
    m_bIsIntelliEdgingOn = FALSE;
    m_uCurImage = 0;
    m_uTotalImages = 0;
    m_iBookId = -1;
    m_nFileId = knMainFileId;
    m_fIsDownFinished = FALSE;
    m_fIsUpFinished = FALSE;

    memset(&(m_CartoonRect[0]), 0, sizeof(DK_RECT));
    memset(&(m_CartoonRect[1]), 0, sizeof(DK_RECT));
    memset(&(m_CartoonRect[2]), 0, sizeof(DK_RECT));
    memset(&(m_CartoonRect[3]), 0, sizeof(DK_RECT));
}

void UIImageReaderContainer::UpdateReadingProgress()
{
    CHAR str[50];
    memset(str, 0, 50);
    sprintf(str, "%d/%d", m_uCurImage, m_uTotalImages);
    m_TxtReadingPages.SetText(CString(str));

    memset(str, 0, 50);
    sprintf(str, "%d%s", (UINT)((m_uCurImage*100) / m_uTotalImages), "%");
    m_TxtReadingPercent.SetText(CString(str));
}

bool UIImageReaderContainer::AdaptScreen(DK_IMAGE drawingImg)
{
    DK_RECT cropRect = {0, 0, 0, 0};
    if (PDF_RM_NormalPage == m_ctlImage.m_eReadingMode)
    {
        cropRect.right = m_pImageHandler->GetWidth();
        cropRect.bottom = m_pImageHandler->GetHeight();
    }
    else if (PDF_RM_Split2Page == m_ctlImage.m_eReadingMode)
    {
        switch (m_ctlImage.m_uCurSubPage)
        {
        case 0:
        case 1:
            cropRect = m_CartoonRect[m_ctlImage.m_uCurSubPage];
            break;
        default:
            cropRect.right = m_pImageHandler->GetWidth();
            cropRect.bottom = m_pImageHandler->GetHeight();
            break;
        }
    }
    else if (PDF_RM_Split4Page == m_ctlImage.m_eReadingMode)
    {
        switch (m_ctlImage.m_uCurSubPage)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            cropRect = m_CartoonRect[m_ctlImage.m_uCurSubPage];
            break;
        default:
            cropRect.right = m_pImageHandler->GetWidth();
            cropRect.bottom = m_pImageHandler->GetHeight();
            break;
        }
    }

    int iImgWidth = cropRect.right - cropRect.left;
    int iImgHeight = cropRect.bottom - cropRect.top;
    if (90 == m_iRotateAngle || 270 == m_iRotateAngle)
    {
        iImgWidth = cropRect.bottom - cropRect.top;
        iImgHeight = cropRect.right - cropRect.left;
    }

    double scale = dk_min((double)m_iWidth / iImgWidth, (double)m_iHeight / iImgHeight);
    if (scale > 1.0)
    {
        scale = 1.0;
    }

    int iDrawWidth = (int)(scale * iImgWidth);
    int iDrawHeight = (int)(scale * iImgHeight);

    DK_RECT rcSelf = 
    {
        m_iLeft + ((m_iWidth  - iDrawWidth)  >> 1),
        m_iTop  + ((m_iHeight - iDrawHeight) >> 1),
        m_iLeft + ((m_iWidth  - iDrawWidth)  >> 1) + iDrawWidth,
        m_iTop  + ((m_iHeight - iDrawHeight) >> 1) + iDrawHeight
    };

    DK_IMAGE DestImg;
    if (FAILED(CropImage(drawingImg, rcSelf, &DestImg)))
    {
        return false;
    }

    int iNewWidth = iDrawWidth;
    int iNewHeight = iDrawHeight;
    if (90 == m_iRotateAngle || 270 == m_iRotateAngle)
    {
        iNewWidth = iDrawHeight;
        iNewHeight = iDrawWidth;
    }

    if (!(m_pImageHandler->CropAndResize(cropRect, iNewWidth, iNewHeight)))
    {
        return false;
    }

    if (!(m_pImageHandler->Rotate(m_iRotateAngle)))
    {
        return false;
    }

    if (!(m_pImageHandler->GetImage(&DestImg)))
    {
        return false;
    }
    return true;
}

bool UIImageReaderContainer::CheckBookExists()
{
    CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
    if (pFileManager)
    {
        m_iBookId = pFileManager->GetFileIdByPath(m_strFilePath.c_str());
        return m_iBookId != -1;
    }

    return false;
}

bool UIImageReaderContainer::InitStream()
{
    IArchiverParser *pIArchiverParser = NULL;
    CArchiveFactory::CreateArchiverInstance(m_strFilePath.c_str(), &pIArchiverParser);
    if(NULL == pIArchiverParser)
    {
        return false;
    }

    if (m_pPassword)
    {
        pIArchiverParser->SetPassword(m_pPassword);
    }

    IDkStream *pStream = NULL;
    pIArchiverParser->GetFileStreamByIndex((INT)m_nFileId, &pStream);
    delete pIArchiverParser;
    pIArchiverParser = NULL;
    if(NULL == pStream)
    {
        return false;
    }

    m_pImageStream = pStream;
    return true;
}

bool UIImageReaderContainer::OnSingleTapUp(MoveEvent* moveEvent)
{
    if (HitOnBookMarkShortCutArea(moveEvent->GetX(), moveEvent->GetY()))
    {
        return true;
    }
    TapAction tapAction = GetTapAction(moveEvent->GetX(), moveEvent->GetY());
    switch (tapAction)
    {
    case TA_MENU:
        HandleMenuAction();
        break;
    case TA_PAGEUP:
        HandlePageUpDown(true);
        break;
    case TA_PAGEDOWN:
        HandlePageUpDown(false);
        break;
    default:
        break;
    }
	return true;
}


bool UIImageReaderContainer::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    direction = TransformFlingDirection(direction, m_iRotateAngle); 
    switch (direction)
    {
        case FD_UP:
        case FD_LEFT:
            HandlePageUpDown(false);
            break;
        case FD_DOWN:
        case FD_RIGHT:
            HandlePageUpDown(true);
            break;
        default:
            break;
    }
    return true;
}

UIImageReaderContainer::TapAction UIImageReaderContainer::GetTapAction(int x, int y) const
{
    TapAction action = TA_NONE;
    DK_INT curRotation = m_ctlImage.m_iRotation;
    double heightPercent = 0.2;
    double widthPercent  = 0.2;

    if (curRotation==0)
    {
        if (y>=0 && y<=(m_iHeight*heightPercent + 50))
        {
            action = TA_MENU;
        }
        else
        {
            if (x>=0 && x<=m_iWidth*widthPercent)
            {
                action = TA_PAGEUP;
            } 
            else
            {
                action = TA_PAGEDOWN;
            }
        }
    }
    else if (curRotation==180) 
    {
        if (y>=(m_iHeight*(1.0 - heightPercent) - 50) && y<=m_iHeight)
        {
            action = TA_MENU;
        } 
        else if (x>=m_iWidth*(1.0 - widthPercent) && x<=m_iWidth)
        {
            action = TA_PAGEUP;
        } 
        else
        {
            action = TA_PAGEDOWN;
        }
    }
    else if (curRotation==90)
    {
        if ((y>=m_iHeight*(1.0-heightPercent) && y<=m_iHeight)
            ||  (((x>=0 && x<=m_iWidth*widthPercent)) && (y>=0&&y<=m_iHeight*(1.0-heightPercent))))
        {
            action = TA_PAGEUP;
        } 
        else if(x>widthPercent*m_iWidth && x<=(1.0 - widthPercent)*m_iWidth
            &&  y>heightPercent*m_iHeight&& y<=(1.0 - heightPercent)*m_iHeight)
        {
            action = TA_MENU;
        } 
        else
        {
            action = TA_PAGEDOWN;
        }
    }
    else if (curRotation==270)
    {
        if ((y>=m_iHeight*(1.0-heightPercent) && y<=m_iHeight)
            ||  (((x>=0 && x<=m_iWidth*widthPercent)) && (y>=0&&y<=m_iHeight*(1.0-heightPercent))))
        {
            action = TA_PAGEDOWN;
        }
        else if 
            (x>widthPercent*m_iWidth && x<=(1.0 - widthPercent)*m_iWidth
            &&  y>heightPercent*m_iHeight&& y<=(1.0 - heightPercent)*m_iHeight)
        {
            action = TA_MENU;
        } 
        else
        {
            action = TA_PAGEUP;
        }
    }
    if (action == TA_PAGEUP)
    {
        action == SystemSettingInfo::GetInstance()->GetTurnPageFlag() == 0 ? TA_PAGEDOWN : TA_PAGEUP;
    }
    return action;
}

void UIImageReaderContainer::HandleMenuAction()
{
    UIReaderSettingDlg dlgReaderSetting(
        this,
        NULL,
        m_dkxBlock,
        m_strBookName.c_str(),
        m_strFilePath.c_str());
    dlgReaderSetting.SetBookMarkOn(m_bExistBookMarkForCurPage);
    dlgReaderSetting.DoModal();
    switch (dlgReaderSetting.GetEndFlag()) 
    {
        case UIReaderSettingDlg::SHOWTOCDLG:
            TocHandler(UITocDlg::SHOWBOOKMARK);
            break;
        case UIReaderSettingDlg::Normal:
            {
                UpdateSettings();
            }
            break;
        case UIReaderSettingDlg::GotoPage:
            {
                int iGotoPageNum = dlgReaderSetting.GetGotoPageNum();
                NavigateToPage(iGotoPageNum - 1);
            }
            break;
        case UIReaderSettingDlg::BackToBookShelf:
            CPageNavigator::BackToPrePage();
            return;
    }
    Repaint();
}

int UIImageReaderContainer::GetJumpHistory()
{
	return m_jumpHistory;
}

JumpDirection UIImageReaderContainer::GetJumpDirection()
{
	return m_jumpDirection;
}

bool UIImageReaderContainer::OnTouchEvent(MoveEvent* moveEvent)
{
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}

bool UIImageReaderContainer::HitOnBookMarkShortCutArea(int x, int y)
{
    INT iRotateAngle = m_ctlImage.m_iRotation; 
    const int bookMarkAreaSize = WindowsMetrics::GetWindowMetrics(BookMarkShortCutAreaSizeIndex);
    bool isInBookMarkShortCutArea = false;

    switch (iRotateAngle)
    {
    case 0:
        isInBookMarkShortCutArea = (x >= (m_iWidth - bookMarkAreaSize) && x <= m_iWidth && y >= 0 && y <= bookMarkAreaSize);
        break;
    case 180:
        isInBookMarkShortCutArea = (x >= 0 && x <= bookMarkAreaSize && y >= (m_iHeight - bookMarkAreaSize) && y <= m_iHeight);
        break;
    case 270:
        isInBookMarkShortCutArea = (x >= 0 && x <= bookMarkAreaSize && y >= 0 && y <= bookMarkAreaSize);
        break;
    case 90:
        isInBookMarkShortCutArea = (x >= (m_iWidth - bookMarkAreaSize) && x <= m_iWidth && y >= (m_iHeight - bookMarkAreaSize) && y <= m_iHeight);
        break;
    default:
        break;
    }

    if (isInBookMarkShortCutArea)
    {
        SwitchBookMarkOffOn();
    }

    return isInBookMarkShortCutArea;
}