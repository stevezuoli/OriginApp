////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/core/inc/Framework/CDisplay.h $
// $Revision: 17 $
// Contact: yuzhan
// Latest submission: $Date: 2008/06/01 21:36:15 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __CDISPLAY_INCLUDED__
#define __CDISPLAY_INCLUDED__

#include "../Common/Mutex.h"
#include "Framework/ISystemMessageListener.h"
#include "Framework/CMessageHandler.h"
#include "Framework/INativeDisplay.h"
#include "GUI/UIPage.h"
#include "Utility/IRunnable.h"
#include "DkSPtr.h"
#include "Framework/INativeMessageQueue.h"
#include "GUI/dkImage.h"
#include "drivers/CFbBitDc.h"
#include "ValueScope.h"
#include <string>

using DkFormat::ValueScope;

class CDisplay: public MutexBase
{
    // Expose inner class ISystemMessageListener to message loop
    friend CMessageHandler::EHandlerReturnType CMessageHandler::HandleMessage();

public:
    class CacheDisabler
    {
        public:
            CacheDisabler();
        private:
            CacheDisabler(const CacheDisabler&);
            CacheDisabler& operator=(const CacheDisabler&);
            ValueScope<bool> m_scope;
    };
    class FullRepainter
    {
    public:
        FullRepainter();
    private:
        FullRepainter(const FullRepainter&);
        FullRepainter& operator=(const FullRepainter&);
        ValueScope<bool> m_scope;
    };


/**********************************************  public class *********************************/
public:
    ~CDisplay();

    static CDisplay* GetDisplay();

    void SetFullRepaint(bool fullRepaint)
    {
        m_fullRepaint = fullRepaint;
    }

    //set Page
    void SetCurrentPage(UIPage* spPage);
    //get Page
    UIPage* GetCurrentPage();

    //get screen parameter
    INT32 GetScreenWidth();
    INT32 GetScreenHeight();

    ISystemMessageListener& GetMessageDispatcher();

    void SetScreenDC(CFbBitDc *pScreenDC);


	void StopFullRepaintOnce();
    void Repaint(paintflag_t flag = PAINT_FLAG_NONE);
    void ScreenRepaint(paintflag_t paintFlag = PAINT_FLAG_NONE, bool drawImmediately = false); 
    void ScreenRepaint(int x,int y,int w,int h, paintflag_t paintFlag = PAINT_FLAG_NONE, bool drawImmediately = false); 
    void ScreenShowImageRepaint(int x,int y, int w, int h, ITpImage *pImg, int srcx, int srcy); 
    void ScreenRefresh();

    void ForceDraw(paintflag_t paintFlag);
    void ForceDraw(const DkRect& rect, paintflag_t paintFlag);

    void PrintSrceen();

    bool DrawFullScreenPicture(const char *pImgPath);
    
    // Deprecated due to memory deallocation issue, use SynchronizedCall unless you know
    // exactly what you are doing.
    void SynchronousCall(IRunnable& rRunnable);

    // This call is for other thread to insert a runnable which later runs within UI thread
    // UI thread coud also use this to schedule a runnable to be running later
    void SynchronizedCall(SPtr<IRunnable> spRunnable);

    //get DK_IMAGE, 用 DK_IMAGE操作内存
    DK_IMAGE GetMemDC();

    INT GetScreenDpi() const;

    void EndCache()
    {
        EndCache(true);
    }

    /**
     * @brief SetScreenTips set the tips which will display on the screen for a while
     *
     * @param tips the text displayed
     * @param displayMSec the display duration
     */
    void SetScreenTips(const char* tips, unsigned int displayMSec = 5000);
    void ClearScreen();
    ///无用 api，清理
    //void UpdateUI();
    //应该没有必要的
    //void ShowRebootTip();
    //disable this function, no one use it.
    //CFbBitDc *GetScreenDC();

    // suspend & resume
    void Suspend();
    void Resume();

private:
    void InitMemDC();

    void DoCache(const DkRect& rect, paintflag_t paintFlag);
    void EndCache(bool flushToFrameBuffer);
    bool IsCaching() const;

    void DoRepaint(int x,int y,int w,int h,int srcx,int srcy, paintflag_t paintFlag, bool flushToFrameBuffer); 
    CDisplay();
    static void ClearScreenTips(void*);   

private:
    class CInnerMessageDispatcher : public ISystemMessageListener
    {
    public:
        CInnerMessageDispatcher(CDisplay&);
        virtual ~CInnerMessageDispatcher()
        {
        }

        virtual void OnKeyEvent(INT32 keyCode, EKeyboardEventType eventType);

        virtual void OnPointerPressed(int x, int y);
        virtual void OnPointerDragged(int x, int y);
        virtual void OnPointerReleased(int x, int y);

        virtual void OnPaint(DK_IMAGE g);

    private:
        CDisplay& m_Parent;
    };

private:
    UIPage* m_spCurrentPage;
    CInnerMessageDispatcher m_MessageDispatcher;
    DK_IMAGE m_memDC;
    DK_IMAGE m_memSecDC;
    DkRect   m_rectUndraw;
    DkRect   m_rectUnflush;
    paintflag_t m_cachePaintFlag;
    bool m_cacheEnabled;
    bool m_inEndCache;
    bool m_inRepaint;
    static CDisplay s_Instance;
	int m_paintTimes;
    int m_stopFullPaintOnce;
    bool m_fullRepaint;
    std::string m_tips;
};



#endif // def __CDISPLAY_INCLUDED__
