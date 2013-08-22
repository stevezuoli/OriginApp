#ifndef _DKREADER_UIBOOKSTOREBOOKCHANGELOGITEM_H_
#define _DKREADER_UIBOOKSTOREBOOKCHANGELOGITEM_H_

#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"
#include "GUI/UIContainer.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIModalDialog.h"
#include "BookStore/BookStoreInfoManager.h"
#include "interface.h"
#include "GUI/UIListItemBase.h"

class UIBookStoreBookChangeLogItem : public UIListItemBase
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookChangeLogItem";
    }
    UIBookStoreBookChangeLogItem();
    ~UIBookStoreBookChangeLogItem();

    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr changeLogInfo, int index = -1);
    bool ResponseTouchTap();

private:
    void Init();

private:
    UITextSingleLine m_updatedTime;
    UITextSingleLine m_revision;
    UIText m_log;
};

class UIChangeLogDetailDlg : public UIModalDlg
{
public:
    UIChangeLogDetailDlg(UIContainer* pParent, LPCSTR title);
    virtual ~UIChangeLogDetailDlg();
    void MoveWindow(int left, int top, int width, int height);
    void SetText(const char* pUpdateTime, const char* pLog);

private:
    bool OnCloseClick(const EventArgs&);

private:
    UITextSingleLine m_updateTime;
    UIText m_Log;
};
#endif//_DKREADER_UIBOOKSTOREBOOKCHANGELOGITEM_H_
