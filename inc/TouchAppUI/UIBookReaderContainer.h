///////////////////////////////////////////////////////////////////////
// UIBookReaderContainer.h
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIBOOKREADERPAGE_H__
#define __UIBOOKREADERPAGE_H__


#include "Common/File_DK.h"
#include "BookReader/IBookReader.h"
#include "Bookmark.h"
#include "Common/CAccountManager.h"
#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"
#include "GUI/GestureDetector.h"
#include "GUI/ScaleGestureDetector.h"
#include "GUI/UIBookReaderProgressBar.h"
#include "GUI/UIContainer.h"
#include "GUI/UIMenu.h"
#include "LayoutHelper.h"
#include "SequentialStream.h"
#include "CommonUI/UIBookReaderSelected.h"
#include "TouchAppUI/UIComplexDialog.h"
#include "TouchAppUI/UIDictionaryDialog.h"
#include "TouchAppUI/UIReaderSettingDlg.h"
#include "GUI/UIDotProgress.h"
#include "Wifi/WifiManager.h"

using DkFormat::IDKSequentialStream;

class DKXManager;
class DKXBlock;
class UIBookContentSearchDlg;
class UIBookReaderContainer : public UIContainer
{
public:
    static const int PDF_ZOOM_MOVE_MINVALUE = 5;
    static const DK_DOUBLE PDF_ZOOM_MAXVALUE = 5.0;
    static const int PDF_ZOOM_POSMARGIN = 20;
    static const int PDF_ZOOM_POSHEIGHT = 15;
    static const DK_DOUBLE MOVE_MINZOOMSPAN = 0.3;
    enum RecognizerAction
    {
        ActionInvalid,
        ActionPagedown,
        ActionPageup,
        ActionMenu
    };

    enum GotoChapterType
    {
        GotoPrevChapter = 0,
        GotoNextChapter
    };
    
    enum SCALETYPE
    {
        WAIT_FOR_SCALE = 0,
        START_SCALE,
        SCALING,
        STOP_SCALE
    };

    enum SELECT_AREA
    {
        SELECT_OUTAREA = 0x0,
        SELECT_INAREA = 0x1,
        SELECT_LEFT = 0x2,
        SELECT_RIGHT = 0x4,
        SELECT_INAREA_LEFT = SELECT_INAREA | SELECT_LEFT,
        SELECT_INAREA_RIGHT = SELECT_INAREA | SELECT_RIGHT,
    };

    UIBookReaderContainer(UIContainer *pParent);
    virtual ~UIBookReaderContainer();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookReaderContainer";
    }

    /****************************************************************************
    *         functions public to DKReaderPage.cpp                              *
    ****************************************************************************/
    //初始化 UIBookReaderContainer objects are created.
    BOOL Initialize (LPCSTR pstrFilePath, LPCSTR strBookName, int iBookId);

    //复用  UIBookReaderContainer
    BOOL CreateBookReader();

    void ResetContainer();
   
    bool GotoBookmark(const ICT_ReadingDataItem *pBookmark);
    bool GotoBookmarkAndHighlightKeyword(const ICT_ReadingDataItem* pBookmark);
    BOOL IsPageDownFinished();
    BOOL IsPageUpFinished();
    BOOL GotoLastPage();
    HRESULT HandleBookContentSearch (ICT_ReadingDataItem* searchResultTable[], UINT HightLightStatPosTable[],UINT HightLightEndPosTable[], INT locationsInBook[], int iTableSize, PINT piUsedTableSize, PINT piNeedTableSize, DK_FLOWPOSITION loadPos, PCCH pcchUTF8Keyword, int uTextLength);
    void UpdateAutoPageDownStatus (BOOL fRestartTimer);

    virtual BOOL OnKeyPress(int iKeyCode);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void OnLoad(); 
    virtual void OnUnLoad();
    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnTTSMessage(SNativeMessage* msg);
    bool IsVerticalForms() const;
    bool IsVerticalRTL() const;
    bool IsVerticalLTR() const;
    bool IsHorizontalHTB() const;
    bool IsWritingLTR() const;
    bool IsWritingRTL() const;

    int  GetCurFileId();
    int  GetBookID();
    void SetPageDownFinished(); 
    void SetPageUpFinished();

    void SwitchBookMarkOffOn();
    void NavigateToPage(int);
	int MoveToFlowPosition(const DK_FLOWPOSITION &pos, bool isRollBack = false);
    void NavigateToToc(DK_TOCINFO* pTocInfo);
    void NavigateToBookMark();

    void    ReaderSettingChangedHandler();
    // 判断当前打开文件解析状态，如果没有解析完成则弹出相应的消息对话框
    bool    IsParseFinished();
    void    ShowParseDialog(BOOKREADER_PARSE_STATUS clsStatus);

	bool SyncDKXBlock();
	void GotoChapter(GotoChapterType _ChapterType);
	
	DK_FLOWPOSITION GetJumpHistory();
	JumpDirection GetJumpDirection();
	void StopTTS();
private:

    int GotoPrePage();
    int GotoNextPage();

    bool ActionOfGallery(MoveEvent* moveEvent1, MoveEvent* moveEvent2, DK_POS& pos);
    bool GotoPreFrameInGallery(const DK_POS& pos);
    bool GotoNextFrameInGallery(const DK_POS& pos);
    
    bool SaveBookMark(ICT_ReadingDataItem *_pclsBookmark);
    bool UpdateBookMark(int _iIndex, ICT_ReadingDataItem *_pclsBookmark);
    bool RemoveBookMark(int _iIndex);
    bool SaveReadingInfo();

    bool LoadSystemLayoutInfo(DK_LAYOUTSETTING* pLayout);
    bool LoadSystemPageSizeInfo(int &_iPageWidth, int &_iPageHeight);
    bool LoadSystemDarkenLevel();
    void LoadSystemFontnameChange(bool &bChanged);
    bool IsInstantTurnPageModel();
    void UpdateProgressBarMode();
protected:
//    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual bool OnTouchEvent(MoveEvent* moveEvent);

private:
    bool ShowCommentOrSummary();
    bool DrawSelectionImage(DK_IMAGE drawingImg);
    void HideSelectionImage(bool updateWindow = true);
    bool MoveSelectionImage(int x, int y);
    void UpdateSelectionImage();
    void ClearSelection();
    DK_RECT GetRectOfSelection();

    bool    InitBookReader();
    void    PostLocalReadingDataToCloud();
    bool    DrawBookReaderData(DK_IMAGE drawingImg);
    bool    DrawTitleBar(DK_IMAGE drawingImg);
    bool    UpdateBottomBar();
    bool    DrawSearchData(DK_IMAGE drawingImg);
    bool    OnTouchTap(GESTURE_EVENT _ge, int _x, int _y);
    bool    HandleFootNote(int _x, int _y);
    bool    HandleInteractiveImage(int _x, int _y);
    bool    HandleCommentTouch(int _x, int _y);
    bool    HandleLinkJump(const DK_POS& curPoint);
    bool    OnTouchZoomPdf(DK_DOUBLE zoomSpan, int focusX, int focusY);
    bool    OnTouchMoveZoomPdf(int distanceX, int distanceY);
    bool    FinishZoomPdf(bool updateWindow = true);

    void    InitPageObjectsInCurPage();
    void    ClearPageObjectsInCurPage();
    bool    DrawInteractiveImageSymbol(DK_IMAGE drawingImg);
    bool DrawDotProgress(DK_IMAGE drawingImg);

    int    GetCommentDigestIndex(const CT_RefPos& beginPos, const CT_RefPos& endPos, ICT_ReadingDataItem::UserDataType dataType, bool& isContained);

    // 处理光标的移动和选取
    // 返回“是/否已响应按键”
    std::string GetUserComment(const CT_RefPos& beginPos, const CT_RefPos& endPos);
    bool    DrawSelected(DK_IMAGE drawingImg, bool updateToScreen = true);
    /**
     * @brief DrawBookMark 该函数目前绘制的有书签，书摘，批注
     *
     * @param drawingImg 源img
     * @param drawComment 是否绘制批注图标
     *
     * @return 绘制成功返回ture，否则false
     */
    bool    DrawBookMark(DK_IMAGE drawingImg, bool drawComment = true);
    bool    DrawZoomedPdfPosBar(DK_IMAGE drawingImg);


    bool    InitUI();
    
    void    PlayTTS();
    void    ManualTTS();

    std::string GetCurPageTextPreview(unsigned int maxLength);
    // 使用说明:1.包含子窗口的延时的操作可以调用下面两个接口给出提示信息
    //          2.看书的过程中使用中，只要有延时的动作，调用ShowHourGlass，在Draw后调用StopHourGlass
    void    StopHourGlass();
    bool    MoveToPage(long _lPageIndex);
    
    void    PDFFormatSettingChangedHandler();
    void    OtherFormatsSettingChangedHandler();
    UIBookReaderContainer::RecognizerAction RecognizeTouchArea(int _x, int _y);
    GESTURE_EVENT TransformGestureDirections(GESTURE_EVENT ge);
    void    DoDkxUpdate();

    void  TocHandler(int showTable);
    void  ShowBookContentSearchDlg();

    bool IsPageInfoShowPercent() const;
    void UpdateReadingTitle();
    void InitSelectBasePoint();
    bool IsDotProgressIndexValid() const;

    // web browser
    bool OnWebBrowserClosed(const EventArgs& args);

private:   
    inline bool IsScannedPdf() const;
    inline bool IsZoomedPdf() const;
    inline bool IsSupportedZoomedPara() const;
    bool InitZoomedPdfPara();
    void UpdateZoomedPdfProgressBar();
    bool ConvertImageToBMP(const DK_IMAGE* pSrcImg, char* pDstBMP);
    bool ResetImage(DK_IMAGE *pImage, int colorChannels, int width, int height);
    void ZoomBMP();
    bool OnScale(ScaleGestureDetector* detector);
    bool OnScaleBegin(ScaleGestureDetector* detector);
    bool OnScaleEnd(ScaleGestureDetector* detector);
    bool OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY);
    bool OnDoubleTapEvent(MoveEvent* moveEvent);

    //void SaveDKImage(const DK_IMAGE* pImage, const DK_CHAR* pFilePathName);
    bool OnGetReadingProgressUpdate(const EventArgs& args);
    bool OnGetReadingDataUpdate(const EventArgs& args);
    bool OnAccountLoginEvent(const EventArgs& args);
	bool OnParseFinishedEvent(const EventArgs& args);
    void BeginGetReadingData();
    bool UpdateReadingData(const ReadingBookInfo& readingBookInfo, ReadingBookInfo* localInfo);
    bool HitTestAndMove(GESTURE_EVENT _ge, int _x, int _y);
    bool HandleNextPageInLast();
    void AddDigest();
    bool HitOnBookMarkShortCutArea(int x, int y);
    bool IsComicsChapter() const;
    void InitDotProgress();
    void UpdateDotProgress();
    /**
     * @brief OnReadLocationChanged 翻页，跳转使得页面发生改变时需要更新的数据
     */
    void OnReadLocationChanged();
    bool SwitchComicsFrameMode(const DK_POS* pPos = NULL);
    void ProcessComment(DWORD cmdID, const std::string& comment);
    void SetFullRepaint();
    bool GetFullRepaintFlag();
    bool ValidCommentDlgTopAndLeft(int& left, int& top);
    int PointInSelection(const DK_POS& point);
    int PointInSelectionRects(const DK_POS& point);

    // web functions
    void showWebSearchMenu();

private:
    IBookReader* m_pBookReader;
    PdfModeController m_modeController;
    int m_pageWidth;
    int m_pageHeight;
    SCALETYPE m_scaleFlag;
    int m_curSpan;

    char*     m_pInitBMP;
    IDkStream* m_pImgStream;
    bool m_expandToc;
    DK_LAYOUTSETTING    m_clsLayout;
    string       m_strBookFilePath;
    string       m_strBookName;
    bool         m_bInialized;
    bool         m_bIsAutoPageDown;
    bool         m_bLastPage;
    bool         m_bFirstPage;
    bool         m_bIsFirstOpen;
	bool		 m_isTrialBook;
    int          m_iBookID;
    string       m_localBookID;

    int             m_iTopMargin;
    int             m_iBottomMargin;
    int             m_iHorizontalMargin;    
    DkFileFormat    m_eFormat;

    int         m_iAutoPageDownSecondCount;
    int         m_iAutoPageDownTimerId;
    
    UIBookReaderSelected    m_clsSelected;
    UIBookReaderProgressBar m_clsReaderProgressBar;
    UIBookReaderProgressBar m_zoomedPdfVerticalPos;
    UIBookReaderProgressBar m_zoomedPdfHorizontalPos;

    ReadingMode             m_eLastReadingMode;              // PDF 浏览的阅读模式，由文本模式返回时切换回原有模式
    bool                    m_bExistBookMarkForCurPage;
    unsigned int            m_uIndexOfBookMarkForCurPage;
    bool                    m_bIsPdfTextMode;
    std::vector<DK_RECT> m_vHighlightRects;
    std::vector<PageObjectInfo> m_vPageInteractiveObjects;//可交互的元素，文内注，可交互图等
    std::vector<GalleryObjectInfo> m_vGalleryObjects;
    UIReaderSettingDlg *m_pReaderSettingDlg;
    UIDotProgressList m_vDotProgress;
    bool m_readingDataChanged;

    ATOM          m_clsStartSelected;
    ATOM          m_clsEndSelected;
    std::string   m_strSelected;
    std::string   m_strSelectedOfPdf;
    int           m_iCurCommentIndex;
    int           m_iCurDigestIndex;
    int           m_iReplaceDigestIndex;
    std::string   m_strComment;
    //DK_FLOWPOSITION m_posStartPos;
    DK_POS        m_posJumpLink;
    bool m_isDKBookStoreBook;
    int           m_turnPageNum;
    //是否是刚刚从屏保中出来，用于判断是否该上传数据
    bool          m_isScreenSaverOut;
	bool		  m_isGotoBookMarkFromOpenBook;
	//记录在BeginGetReadingData 时的阅读进度，用于判断是否该弹出进度同步对话框 
	ATOM		  m_getServerProgressPos;

    UITextSingleLine m_txtChapterTitle;

    int m_iCurrentInteractivePageObjectIndex;
    int m_dotProgressFocusIndex;
    bool m_isSelecting;
    bool m_rightSelecter;
    bool m_processMove;
    bool m_downInSelectionImage;
    DK_IMAGE m_imageReader;
    UIImageTransparent m_imageSelectedEnd;
    UIImageTransparent m_imageSelectedStart;

    struct COMMENT_TAP_LOCATION{
        DK_RECT   imgRect;
        CT_RefPos startPos;
        CT_RefPos endPos;
    };
    std::vector<COMMENT_TAP_LOCATION> m_commentTapLocation;
    class UIBookReaderContainerGestureListener:
        public SimpleGestureListener,
        public DoubleTapListener
    {
    public:
        virtual bool OnDown(MoveEvent* moveEvent);
        virtual bool OnSingleTapUp(MoveEvent* moveEvent);
        virtual bool OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY);
        virtual void OnLongPress(MoveEvent* moveEvent);
        virtual void OnSingleTapConfirmed(MoveEvent* moveEvent);
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
        virtual bool OnDoubleTap(MoveEvent* moveEvent);
        virtual bool OnDoubleTapEvent(MoveEvent* moveEvent);
        virtual void OnHolding(int holdingX, int holdingY);
        void SetBookReaderContainer(UIBookReaderContainer* bookReaderContainer)
        {
            m_pBookReaderContainer = bookReaderContainer;
        }
    private:

        UIBookReaderContainer* m_pBookReaderContainer;
    };
    class UIBookReaderContainerScaleGestureListener :
        public ScaleGestureListener
    {
    public:
        virtual bool OnScale(ScaleGestureDetector* detector);
        virtual bool OnScaleBegin(ScaleGestureDetector* detector);
        virtual bool OnScaleEnd(ScaleGestureDetector* detector);
        void SetBookReaderContainer(UIBookReaderContainer* bookReaderContainer)
        {
            m_pBookReaderContainer = bookReaderContainer;
        }
    private:
        UIBookReaderContainer* m_pBookReaderContainer;
    };

    UIBookReaderContainerGestureListener m_gestureListener;
    GestureDetector m_gestureDetector;
    UIBookReaderContainerScaleGestureListener m_scaleGestureListener;
    ScaleGestureDetector m_scaleGestureDetector;
    bool IsInSelectionProgress() const;
    bool OnLongPress(MoveEvent* moveEvent);
    void OnSelectMove(int x, int y);
    static bool ShouldTreatUpDownAsPageSwitch();
    int GetRotationDegree() const;
    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
    bool OnDown(MoveEvent* moveEvent);
    bool OnUp(MoveEvent* moveEvent);
    void OnHolding(int holdingX, int holdingY);
    bool m_pdfZoomMoved;
    bool m_firstDrawPage;
    bool m_lastPageFullScreen;
    DKXBlock* m_dkxBlock;
    bool m_fullRepaint;
	bool m_isPageTurned;
	DK_FLOWPOSITION m_jumpHistory;
	JumpDirection m_jumpDirection;
};

#endif

