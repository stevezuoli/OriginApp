////////////////////////////////////////////////////////////////////////
// UIBookmarkListBox.h
// Contact: zhangjd
// Copyright (c) Duokan Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////

#ifndef __UIBOOKMARKLISTBOX_H__
#define __UIBOOKMARKLISTBOX_H__

#include "GUI/UICompoundListBox.h"
#include "Model/IBookmarkItemModel.h"
#include "TouchAppUI/UIBookmarkListItem.h"
#include "Common/File_DK.h"

class IBookReader;
class DKXBlock;

class UIBookmarkListBox : public UICompoundListBox
{
public:
    UIBookmarkListBox(
            int iBookId,
            const char *pPassword = NULL,
            DKXBlock* dkxBlock = NULL,
            IBookReader* pBookReader = NULL,
            int iType = (UIBookmarkListItem::UIBOOKMARKLISTTYPE_BOOKMARK | UIBookmarkListItem::UIBOOKMARKLISTTYPE_DIGEST | UIBookmarkListItem::UIBOOKMARKLISTTYPE_COMMENT));
    virtual ~UIBookmarkListBox();

    virtual LPCSTR GetClassName() const
    {
        return ("UIBookmarkListBox");
    }

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    int GetCurPageNum();
    int GetTotalPage();
    int GetCurModeIndex();
    int GetClassNum();
    void ClearFocus();
    void SetBookmarkListType(int iType);
    int GetCurBookmarkNum();

    // iPageNum based 0
    bool GotoPage(int iPageNum);
    void LoadBookmark();
    void UpdateList();

    virtual bool OnItemClick(INT32 iSelectedItem);
protected:
    virtual void InitListItem();
    bool DoHandleListTurnPage(bool fKeyUp);
    void Init();
    virtual BOOL HandleLongTap(INT32 selectedItem);

private:
    void DeleteKeyHander(INT32 deleteIndex);
    void ClearModelList();
    BOOL OpenArchive(INT iBookmarkIndex, CDKFile *pDkFile);
	bool RemoveBookmarkFromDKX(int _iBookmarkIndex);

	void HandlePageDown();
	void HandlePageUp();
	void HandleGoToBookMark(int selectedIndex);

private:

    std::vector< IBookmarkItemModel *> m_curModeList;
    std::vector< IBookmarkItemModel *> m_allModeList;
    int m_iCurModelIndex;
    bool m_bNeedUpdate;
    int m_iBookId;
    char *m_pPassword;
    BOOL m_bFromReaderPage;

    int m_listType;
    IBookReader* m_pBookReader;
    DKXBlock* m_dkxBlock;
};

#endif  // __UIBOOKMARKLISTBOX_H__

