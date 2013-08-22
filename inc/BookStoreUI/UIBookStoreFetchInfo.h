#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREFETCHINFO_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREFETCHINFO_H_

#include "GUI/UIContainer.h"
#include "GUI/UITextSingleLine.h"
#include "BookStore/BookStoreInfoManager.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif

/********************************************
* UIBookStoreFetchInfo
********************************************/
enum FETCH_STATUS
{
    FETCHING = 0,
    FETCH_SUCCESS,
    FETCH_FAIL,
    FETCH_NOELEM,
    NOFETCH
};
class UISizer;

class UIBookStoreFetchInfo : public UIContainer
{
public:
    UIBookStoreFetchInfo();
    ~UIBookStoreFetchInfo();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreFetchInfo";
    }

    void CustomizeNoElementInfo(const char* noElementNotice, const char* navigatorNotice);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void SetFetchStatus(FETCH_STATUS status);

    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
private:
    void Init();

protected:
    FETCH_STATUS m_fetchStatus;

    UITextSingleLine m_error;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_retry;
#else
    UIComplexButton m_retry;
#endif
    UISizer* m_fetchErrorSizer;

    UITextSingleLine m_loading;
    UISizer* m_loadingSizer;

    UITextSingleLine m_noElem;
    UISizer* m_noElemSizer;

#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnNavigator;
#else
    UIComplexButton m_btnNavigator;
#endif
    UIText m_noElemCustomized;
    UISizer* m_noElemCustomizedSizer;
    bool m_bCustomized;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREFETCHINFO_H_

