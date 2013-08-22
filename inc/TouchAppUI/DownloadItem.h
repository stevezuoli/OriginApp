#ifndef __DOWNLOADITEM_H__
#define __DOWNLOADITEM_H__
#include "GUI/UIWindow.h"
#include "GUI/ITpGraphics.h"
#include "GUI/UIImage.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIBookReaderProgressBar.h"
#include "GUI/UIProgressBar.h"
#include "GUI/UIPage.h"
#include "GUI/UIMenu.h"
#include<vector>
#include "Model/IDownloadItemModel.h"
#include "GUI/UICompoundListBox.h"

class UISizer;
class CDownloadItem : public UIContainer
{
public:
    CDownloadItem();
    CDownloadItem(UIContainer* pParent, const DWORD dwId);
    ~CDownloadItem();
    void SetDownloadItemModel(IDownloadItemModel *pItemModel);

    virtual LPCSTR GetClassName() const
    {
        return ("CDownloadItem");
    }

    HRESULT DrawFocus(DK_IMAGE drawingImg);
    void SetFocus(BOOL bIsFocus);
    void UpdateProgress();
	string GetDownloadStatus();
	void UpdateButtonStatus();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

protected:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
    void InitItem();

private:
    UITextSingleLine    m_textBookNameLabel;
    UITextSingleLine    m_textDownLoadStatus;
    UITextSingleLine    m_textDownLoadProgress;
    UIImage             m_imgDownLoadStatusImg;
    UIBookReaderProgressBar       m_proDownloadSeparatorbar;
	UITouchComplexButton m_btnTaskDownload;
	UITouchComplexButton m_btnTaskDelete;
	UITouchComplexButton m_btnTaskPause;
	UITextSingleLine m_textTaskDownload;
	UITextSingleLine m_textTaskDelete;
	UITextSingleLine m_textTaskPause;
    UISizer* m_pDownloadSizer;
    UISizer* m_pDeleteSizer;
    UISizer* m_pPauseSizer;
    IDownloadItemModel* m_pItemModel;
    BOOL                m_bIsDisposed;
    int                 m_OldPrecent;
    IDownloadTask::DLState  m_OldState;
    std::string         m_szOldBookname;
};


class CDownloadList : public UICompoundListBox
{
public:
    CDownloadList();
    CDownloadList(UIContainer* pParent, const DWORD dwId);
    ~CDownloadList();
    void SetDownloadList(IDownloadItemModel **ppList, INT32 iNum);
    BOOL OnKeyPress(INT32 iKeyCode);
    void SetFocus(BOOL bIsFocus);
    std::vector<IDownloadItemModel *> & GetItemModelList()
    {
        return m_Itemlist;
    }
    static void *UpdateDownloadProgressPthread(void *_pv);
    void *UpdateDownloadProgress(void *_pv);

public:
    virtual bool OnItemClick(INT32 iSelectedItem);
    IDownloadItemModel *GetListItem(INT32 iItemIndex);
    virtual bool DoHandleListTurnPage(bool pageUp);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

private:
    virtual void InitListItem();

private:
    BOOL            m_bIsDisposed;
    UIMenu*         m_menuMain;

    std::vector<IDownloadItemModel *> m_Itemlist;
};
#endif

