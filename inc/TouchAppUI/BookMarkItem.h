////////////////////////////////////////////////////////////////////////
// BookMarkItem.h
// Contact: wanghua
////////////////////////////////////////////////////////////////////////

#ifndef _BOOKMARKTEM_H_
#define _BOOKMARKTEM_H_

#include "../GUI/UIContainer.h"
#include "../GUI/UITextSingleLine.h"
#include "../GUI/UIText.h"
#include "TouchAppUI/UIBookmarkListBox.h"
#include "dkBaseType.h"

class CBookMarkItem : public UIContainer
{
public:
    CBookMarkItem(int iBookId,LPCSTR pPassword = NULL,BOOL bFromReaderPage = FALSE);
    ~CBookMarkItem();

    virtual LPCSTR GetClassName() const
    {
        return ("CBookMarkItem");
    }

    void Init();
    HRESULT Draw(DK_IMAGE drawingImg);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    void SetFocus(BOOL bIsFocus);
	void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

private:
    int m_iBookId;
    int m_iPageNum;
    int m_iCurPage;
    UITextSingleLine m_txtTotal;
    UITextSingleLine m_btnPageNo;
    UIBookmarkListBox m_lstBookmark;
};

#endif //_BOOKTAGINFOITEM_H_

