////////////////////////////////////////////////////////////////////////
// BookInfoPage.h
// Contact: Zhangjd
////////////////////////////////////////////////////////////////////////

#ifndef _BOOKINFOITEM_H_
#define _BOOKINFOITEM_H_

#include "../GUI/UIContainer.h"
#include "../GUI/UIImage.h"
#include "../GUI/UIComplexButton.h"
#include "../GUI/UITextSingleLine.h"
#include "../GUI/UIText.h"

class CBookInfoItem : public UIContainer
{
public:
    CBookInfoItem(int iBookId, char *pPassword = NULL, BOOL bFromReadPage = TRUE);
    ~CBookInfoItem();

    virtual LPCSTR GetClassName() const
    {
        return ("CBookInfoItem");
    }
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    void SetFocus(BOOL bIsFocus);
    virtual void OnSerializedBookUpdate(SNativeMessage event);

private:
    void Init();
    void Dispose();

private:
    int m_iBookId;
    UIImage m_imgBookCover;
    UIComplexButton m_btnDeleteBook;
    UIComplexButton m_btnReadBook;
    UIComplexButton m_btnUpdateBook;
    BOOL m_bFromReadPage;
    //нд╠╬©Р
    UIText m_briefIntroduction;
    UITextSingleLine m_lblBookName;
    UITextSingleLine m_lblNewChapterText;
    UITextSingleLine m_lblAuthor;
    UITextSingleLine m_lblFileSize;
    char *m_pPassword;
};

#endif //_BOOKINFOITEM_H_

