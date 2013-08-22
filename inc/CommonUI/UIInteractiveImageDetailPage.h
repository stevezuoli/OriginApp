/*
 * =====================================================================================
 *       Filename:  UIInteractiveImageDetailPage.h
 *    Description:  page of interactive image in books
 *
 *        Version:  1.0
 *        Created:  03/12/2013 10:35:57 AM
 * =====================================================================================
 */

#ifndef UIINTERACTIVEIMAGEDETAILPAGE_H
#define UIINTERACTIVEIMAGEDETAILPAGE_H

#include "GUI/UIPage.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif
#include "GUI/UIText.h"
#include "GUI/UISeperator.h"
#include "IDKEBook.h"
#include "GUI/UIDotProgress.h"

class UISizer;
class IDKEGallery;

class UIInteractiveImageDetailPage : public UIPage
{
public:
    UIInteractiveImageDetailPage(string bookName);
    ~UIInteractiveImageDetailPage();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    virtual bool SetHitObjectInfo(DKE_HITTEST_OBJECTINFO objInfo);
    virtual bool SetGallery(IDKEGallery* pGallery);
protected:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
#ifndef KINDLE_FOR_TOUCH
    virtual BOOL OnKeyPress(INT32 iKeyCode);
#endif

private:
    bool Init();
    bool InitImageData(int index = 0);
    void DestroyImageData();
    void ShareToSinaWeibo();
    void FullScreenSwitch();
    void UpdateTitleInfo(int index);
    void ClearArray();
    void PushBackInfo(DKE_HITTEST_OBJECTINFO objInfo);
    bool TurnPageUpDown(bool pageDown);

    const string m_bookName;
    bool m_isFullScreen;
    UISizer* m_pBottomSizer;
    DK_IMAGE m_interactiveImage;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnClose;
    UITouchComplexButton m_btnShare;
#else
    UIComplexButton m_btnClose;
    UIComplexButton m_btnShare;
    UIComplexButton m_btnFullScreen;
#endif
    UIText m_textImageMainTitle;
    UIText m_textImageSubTitle;
    UISeperator m_seperator;
    UIDotProgress m_dotProgress;
    std::vector<std::string> m_vMainTitle;
    std::vector<std::string> m_vSubTitle;
    std::vector<std::string> m_vFileExt;
    std::vector<std::string> m_vFileBuffer;
    int m_curIndex;
#ifdef KINDLE_FOR_TOUCH
    bool OnTouchEvent(MoveEvent* moveEvent);
    bool OnSingleTapUp(MoveEvent* moveEvent);
    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
    class UIInteractiveImageDetailPageGestureListener: public SimpleGestureListener
    {
    public:
        virtual bool OnSingleTapUp(MoveEvent* moveEvent)
        {
            return m_interactiveImageDetailPage
                && m_interactiveImageDetailPage->OnSingleTapUp(moveEvent);
        }
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            return m_interactiveImageDetailPage
                && m_interactiveImageDetailPage->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
        }
        void SetInteractiveImageDetailPage(UIInteractiveImageDetailPage* page)
        {
            m_interactiveImageDetailPage = page;
        }
    private:
        UIInteractiveImageDetailPage* m_interactiveImageDetailPage;
    };
    GestureDetector m_gestureDetector;
    UIInteractiveImageDetailPageGestureListener m_flingGestureListener;
#endif
};
#endif//UIINTERACTIVEIMAGEDETAILPAGE_H
