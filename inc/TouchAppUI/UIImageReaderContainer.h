#ifndef __UIIMAGEREADERCONTAINER_H__
#define __UIIMAGEREADERCONTAINER_H__

#include "GUI/UIMenu.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UIProgressBar.h"
#include "GUI/UIBookReaderProgressBar.h"
#include "TouchAppUI/UIComplexDialog.h"
#include "TouchAppUI/UIReaderSettingDlg.h"
#include "../ImageHandler/DkImageHelper.h"
#include "BookmarkHelper.h"
#include "BookInfoEntity.h"
#include "Common/Defs.h"


using DkFormat::ImageHandler;
class DKXBlock;

class IDkStream;

class UIImageReaderContainer : public UIContainer
{
public:
    UIImageReaderContainer(UIContainer *pParent);
    virtual ~UIImageReaderContainer();
    virtual LPCSTR GetClassName() const { return "UIImageReaderContainer"; }

    // 第一次打开使用此接口，会初始化UI
    BOOL Initialize(unsigned int nFileId, int iBookId, IDkStream *pStream, const char *pPassword, int totalImage);
    // 之后使用此接口
    BOOL SetImage(unsigned int nFileId, int iBookId, IDkStream *pStream, const char *pPassword);

    // DkReaderPage使用
    void ResetContainer();
    void OnLoad();
    void OnUnLoad();
    void OnEnter();
    void OnLeave();
    DK_IMAGE* GetCurImage()
    {
    	SafeFreePointer(m_imgOutput.pbData);
		long iDataLen = m_imgOutput.iStrideWidth * m_imgOutput.iHeight;
		m_imgOutput.pbData = DK_MALLOCZ_OBJ_N(BYTE8, iDataLen);
		AdaptScreen(m_imgOutput);
        return &m_imgOutput;
    }
    // 当前图片编号
    void SetCurImage(UINT uCurImage) 
    { 
        m_uCurImage = uCurImage;
    } 
    // 总图片数
    void SetTotalImage(UINT uTotalImage)
    { 
        m_uTotalImages = uTotalImage;
    }
    BOOL GotoLastPage();
	int GetCurImageNum()
	{
		return m_uCurImage;
	}
	int GetTotalPageNumber()
	{
		return m_uTotalImages;
	}
	int GetCurrentPageIndex()
	{
		return (int)m_uCurImage - 1;
	}
    unsigned int GetCurFileId() const
    { 
        return m_nFileId;
    }
    int GetBookID() const
    { 
        return m_iBookId;
    }
    BOOL IsPageDownFinished() const
    { 
        return m_fIsDownFinished;
    }
    BOOL IsPageUpFinished() const 
    { 
        return m_fIsUpFinished;
    }
    void SetPageDownFinished()
    { 
        m_fIsDownFinished = TRUE;
    }
    void SetPageUpFinished()
    { 
        m_fIsUpFinished = TRUE;
    }
	ImageController* GetImgControler()
	{
		return &m_ctlImage;
	}
    void NavigateToPage(int pageIndex, bool isRollBack = false);
    virtual void MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual bool OnTouchEvent(MoveEvent* moveEvent);
    DKXBlock* GetDKXBlock()
    {
        return m_dkxBlock;
    }
	int GetJumpHistory();
	JumpDirection GetJumpDirection();

protected:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void Dispose();

private:
    void OnDispose(BOOL bIsDisposed);
    void Finalize();
    void InitUI();
    bool InitMember();
    bool InitImageHandler();
    bool ApplySettings();
    bool SaveSettings();
    bool UpdateSettings();
    bool AdaptScreen(DK_IMAGE drawingImg);
    void UpdateReadingProgress();
    bool CheckBookExists();
    bool InitStream();
    void HandlePageUpDown(BOOL fIsUp);
    bool DrawBookMark(DK_IMAGE drawingImg);
    void TocHandler(int showTable);
    void SwitchBookMarkOffOn();
    bool HitOnBookMarkShortCutArea(int x, int y);
private:
    BOOL m_bIsDisposed;
    std::string m_strFilePath;
    
    UITextSingleLine m_BottomBar;
    UITextSingleLine m_TxtReadingPercent;
    UITextSingleLine m_TxtReadingPages;
    UIComplexDialog *m_pGotoPageDlg;
    UIBookReaderProgressBar m_ReaderProgressBar;

    IDkStream * m_pImageStream;
    ImageHandler *m_pImageHandler;
    ImageController m_ctlImage;
    INT m_iRotateAngle;
    ReadingMode m_eReadingMode;
    DK_RECT m_CartoonRect[4];
    BOOL m_bIsIntelliEdgingOn;
    
    UINT m_uCurImage;
    UINT m_uTotalImages;
    std::string m_strBookName;
    std::string m_strCurFileName;
    INT m_iBookId;
    UINT m_nFileId;
    char * m_pPassword;

    BOOL m_fIsDownFinished;
    BOOL m_fIsUpFinished;

    DK_IMAGE            m_imgOutput;
    BOOL                m_bExistBookMarkForCurPage;
    int                 m_iIndexOfBookMarkForCurPage;
private:
    bool OnSingleTapUp(MoveEvent* moveEvent);
    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);

    class UIImageReaderContainerGestureListener:
        public SimpleGestureListener
    {
        public:
            virtual bool OnSingleTapUp(MoveEvent* moveEvent)
            {
                return m_imageReaderContainer->OnSingleTapUp(moveEvent);
            }
            virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
            {
                return m_imageReaderContainer->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
            }
            void SetImageReaderContainer(UIImageReaderContainer* imageReaderContainer)
            {
                m_imageReaderContainer = imageReaderContainer;
            }
        private:
            UIImageReaderContainer* m_imageReaderContainer;
    };
    GestureDetector m_gestureDetector;
    UIImageReaderContainerGestureListener m_gestureListener;
    enum TapAction
    {
        TA_PAGEUP,
        TA_PAGEDOWN,
        TA_MENU,
        TA_NONE
    };
    TapAction GetTapAction(int x, int y) const;
    void HandleMenuAction();
    DKXBlock* m_dkxBlock;
	int m_jumpHistory;
	JumpDirection m_jumpDirection;
};

#endif
