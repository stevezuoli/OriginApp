//============================================================================== 
// Summary: 
//        UIBookCorrectionDlg: 书城书籍纠错对话框 
// Usage: 
// Remarks: 
// Date: 
//        2012-11-22 
// Author: 
//        juguanhui(juguanhui@duokan.com) 
//============================================================================== 
#include "GUI/UIDialog.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#include "GUI/UIBackButton.h"
#endif
#include "CommonUI/UITitleBar.h"
#include "GUI/UIPlainTextEdit.h"
#include "DKXManager/BaseType/CT_RefPos.h"

class IBookReader;

class UIBookCorrectionDlg : public UIDialog
{
public:
    UIBookCorrectionDlg(UIContainer* pParent, IBookReader* pBookReader);
    ~UIBookCorrectionDlg();
    
    void SetSelectedText(const char* pText, const CT_RefPos* pStartPos, const CT_RefPos* pEndPos);
    void MoveWindow(int left, int top, int width, int height);
    void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);

    bool OnMessageReportBookBugUpdate(const EventArgs& args);

    virtual void EndDialog(INT32 iEndCode,BOOL fRepaintIt = TRUE);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
private:
    void Init();
    void ShowTips(const std::string& tips);
    void HideTips();
private:
    IBookReader* m_pBookReader;
    UITitleBar m_titleBar;
    UITextSingleLine m_barBookName;
    UITextSingleLine m_barOldContent;
    UITextSingleLine m_barNewContent1;
    UITextSingleLine m_barNewContent2;
    UITextSingleLine *m_pTips;
    UIText m_txtOldContent;
    UIPlainTextEdit m_txtNewContent;
#ifdef KINDLE_FOR_TOUCH
    UITouchBackButton m_backButton;
    UITouchComplexButton m_onlineSend;
    UITouchComplexButton m_offlineSend;
#else
    UIBackButton m_backButton;
    UIComplexButton m_onlineSend;
    UIComplexButton m_offlineSend;
#endif

    CT_RefPos m_startPos;
    CT_RefPos m_endPos;
};