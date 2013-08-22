//==============================================================================
// Summary:
//        UIFootNoteDialog: 显示文内注对话框
// Usage:
//
// Remarks:
//      
// Date:
//        2012-06-12
// Author:
//        juguanhui@duokan.com
//==============================================================================

#ifndef _UIFOOTNOTEDIALOG_H_
#define _UIFOOTNOTEDIALOG_H_

#include "GUI/UIDialog.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"
#include "GUI/UIImage.h"
#include "BookReader/EpubBookReader.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#include "GUI/GestureDetector.h"
#endif

class UIFootNoteDialog : public UIDialog
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIFootNoteDialog";
    }

	UIFootNoteDialog(UIContainer* pParent);
    virtual ~UIFootNoteDialog();

    virtual void    MoveWindow(int left, int top, int width, int height);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
#ifdef KINDLE_FOR_TOUCH
    virtual void    OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual bool OnHookTouch(MoveEvent* moveEvent);
#else
    virtual BOOL OnKeyPress(int iKeyCode);
#endif

    void SetText(const char* text);
	void SetSection(IDKESection* pSection);
    void SetArrowPara(bool arrowUp, int x, int y);
    void ShowArrow(bool isShowArrow) { m_isShowArrow = isShowArrow; }
    int  GetMaxHeight() const;
protected:
	void Init();
	void UpdatePageNum();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg); 
    void HandleDlgPageUpDown(bool bPageUp);

private:
    bool ProduceBMPPage(IDKESection* pSection, int pageIndex);
#ifdef KINDLE_FOR_TOUCH
    bool OnTouchEvent(MoveEvent* moveEvent);
    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
	bool OnSingleTapUp(MoveEvent* moveEvent);
    class FootNoteDlgGestureListener: public SimpleGestureListener
    {
    public:
        void SetFootNoteDlg(UIFootNoteDialog* dlg)
        {
            m_pDlg = dlg;
        }
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            return m_pDlg->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
        }

		virtual bool OnSingleTapUp(MoveEvent* moveEvent)
        {
            return m_pDlg->OnSingleTapUp(moveEvent);
        }
    private:
        UIFootNoteDialog* m_pDlg;
    };

    GestureDetector m_gestureDetector;
    FootNoteDlgGestureListener m_gestureListener;
#endif

private:
    int m_leftMargin;
    int m_topMargin;
    int m_bottomMargin;
    int m_maxWidth;
    int m_maxHeight;
    int m_bottomHeight;
    int m_elemSpacing;
#ifdef KINDLE_FOR_TOUCH
	UITouchComplexButton  m_leftPage;
	UITouchComplexButton  m_rightPage;
#else
	UIImage  m_leftPage;
	UIImage  m_rightPage;
#endif

	UITextSingleLine	  m_pageNum;
	UIText				  m_footNote;
	UIImage 			  m_imgArrow;
	int 				  m_totalPage;
	int 				  m_curPage;
	bool				  m_arrowUp;
	bool				  m_isShowArrow;
	UISizer*			  m_bottomSizer;
	UIImage				  m_richTextImage;
	
	int 			  	  m_iTextMaxHeight;
	int 			  	  m_iTextWidth;

	//以下变量仅在富文本注时才有用
    std::vector<DK_IMAGE> m_vImageOfSection;
};

#endif

