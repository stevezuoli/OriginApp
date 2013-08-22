////////////////////////////////////////////////////////////////////////
// UITablePage.h
// Contact:
////////////////////////////////////////////////////////////////////////

#ifndef _UITABLEPAGE_H_
#define _UITABLEPAGE_H_

#include "GUI/UIPage.h"
#include "GUI/UIImage.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITabBar.h"
#endif

class UITablePage : public UIPage
{
public:
    UITablePage();
    ~UITablePage();


#ifdef KINDLE_FOR_TOUCH
    void AppendTableBox(LPCSTR Text,const DWORD dwId,UIContainer* Page, BOOL bIsSelected=FALSE);
    bool OnTabIndexChanged(const EventArgs& args);
#else
    void AppendTableBox(LPCSTR Text,CHAR cHotKey,const DWORD dwId,UIContainer* Page, BOOL bIsSelected=FALSE);
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
#endif
    void LayOutTableBox();
    HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
     UIContainer*    m_pShowPage;
	 std::vector< UIContainer* > m_panels;
#ifdef KINDLE_FOR_TOUCH
     UITabBar m_tabBar;
#else
     std::vector< UIComplexButton* > m_titleBtns;
#endif
};

#endif //_BOOKTAGINFOITEM_H_
