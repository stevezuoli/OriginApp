#ifndef UICOMMENTEXPORTDLG_H
#define UICOMMENTEXPORTDLG_H

#include "GUI/UIModalDialog.h"
#include "GUI/UITextBox.h"
#include "GUI/UIBookReaderProgressBar.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "Common/File_DK.h"
#include "BookReader/IBookReader.h"


class UICommentExportDlg : public UIModalDlg
{
public:

    virtual LPCSTR GetClassName() const
    {
        return "UICommentExportDlg";
    }

    UICommentExportDlg(UIContainer* pParent, LPCSTR title);
    virtual ~UICommentExportDlg();
    
    virtual void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);
    virtual void Popup();
    virtual void EndDialog(INT32 iEndCode);
    virtual void OnTimer(INT32 iId);

private:
    void InitUI();
    void InitPosition();
    void ExportComment();
    static void* ExportThreadFunc(void* param);
    void StopExportThread();
    void UpdateUIText();
    bool ExportCommentByBook(const PCDKFile pcdkBook);

private:
    UIBookReaderProgressBar m_barExportProgress;
    UITouchComplexButton m_btnCancel;
    UITextSingleLine m_statusText;
    UITextSingleLine m_textExportOK;
    pthread_t m_threadExport;
    bool m_bIsThreadCanceled;
    bool m_bIsExportOver;
    int m_iRefreshTimerId;
    int m_iExportPercent;
    string m_strCommentExportPath;
};
#endif//UICOMMENTEXPORTDLG_H

