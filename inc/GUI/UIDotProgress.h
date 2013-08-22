#ifndef __COMMONUI_UIDOTPROGRESS_H
#define __COMMONUI_UIDOTPROGRESS_H

#include "GUI/UIWindow.h"

class UIDotProgress : public UIWindow
{
public:
    UIDotProgress();
    virtual ~UIDotProgress();

    void SetProgress(int index, int count);
    virtual HRESULT Draw(DK_IMAGE drawingImg);

    virtual dkSize GetMinSize() const;
    void SetSelected(bool selected);
private:
    void UpdateText();
    std::wstring m_strText;
    std::wstring m_strShowText;
    bool m_selected;
};

typedef std::tr1::shared_ptr<UIDotProgress> UIDotProgressSPtr;
typedef std::vector<UIDotProgressSPtr> UIDotProgressList;

#endif //__COMMONUI_UIDOTPROGRESS_H
