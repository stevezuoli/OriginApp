/*
 * =====================================================================================
 *       Filename:  UIInteractiveFlexPage.h
 *    Description:  page of interactive image in books
 *
 *        Version:  1.0
 *        Created:  03/12/2013 10:35:57 AM
 * =====================================================================================
 */
#ifndef UIINTERACTIVEFLEXPAGE_H
#define UIINTERACTIVEFLEXPAGE_H

#include "GUI/UIPage.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif
#include "GUI/UIText.h"
#include "DKEEPUBDef.h"

class UISizer;
class IDKEFlexPage;

class UIInteractiveFlexPage : public UIPage
{
public:
    UIInteractiveFlexPage(string bookName);
    ~UIInteractiveFlexPage();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    virtual bool SetPreBlockInfo(const DKE_PREBLOCK_INFO& preBlockInfo);
protected:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
#ifndef KINDLE_FOR_TOUCH
    virtual BOOL OnKeyPress(INT32 iKeyCode);
#endif

private:
    bool Init();
    bool InitImageData();
    void DestroyImageData();
    void ShareToSinaWeibo();
    void FullScreenSwitch();
    void GetRenderOption(const DK_IMAGE& image, DK_BITMAPBUFFER_DEV& dev, DK_FLOWRENDEROPTION& renderOption);
    bool UpdateImage(IDKEFlexPage* pFlexPage, DK_IMAGE& image, int initWidth);
    bool TurnPageUpDown(bool turnDown);

    const string m_bookName;
    bool m_isFullScreen;
    bool m_isRotate;
    int m_startPos;
    DK_IMAGE m_image;
    DK_IMAGE m_rotateImage;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnClose;
    UITouchComplexButton m_btnShare;
    UITouchComplexButton m_btnRotate;
#else
    UIComplexButton m_btnClose;
    UIComplexButton m_btnShare;
    UIComplexButton m_btnRotate;
    UIComplexButton m_btnFullScreen;
#endif
    UIButtonGroup m_btnGroup;
#ifdef KINDLE_FOR_TOUCH
    bool OnTouchEvent(MoveEvent* moveEvent);
    bool OnSingleTapUp(MoveEvent* moveEvent);
    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
    class UIInteractiveFlexPageGestureListener: public SimpleGestureListener
    {
    public:
        virtual bool OnSingleTapUp(MoveEvent* moveEvent)
        {
            return m_interactiveFlexPage
                && m_interactiveFlexPage->OnSingleTapUp(moveEvent);
        }
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            return m_interactiveFlexPage
                && m_interactiveFlexPage->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
        }
        void SetInteractiveFlexPage(UIInteractiveFlexPage* page)
        {
            m_interactiveFlexPage = page;
        }
    private:
        UIInteractiveFlexPage* m_interactiveFlexPage;
    };
    GestureDetector m_gestureDetector;
    UIInteractiveFlexPageGestureListener m_flingGestureListener;
#endif
};
#endif//UIINTERACTIVEFLEXPAGE_H
