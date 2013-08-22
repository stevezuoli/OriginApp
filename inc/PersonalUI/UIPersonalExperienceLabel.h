#ifndef TOUCHAPPUI_UIPERSONALEXPERIENCELABEL_
#define TOUCHAPPUI_UIPERSONALEXPERIENCELABEL_

#include <string>
#include "GUI/UIWindow.h"
#include "GUI/ITpGraphics.h"
#include "GUI/UITextSingleLine.h"

class UIPersonalExperienceLabel : public UIContainer
{
public:
	UIPersonalExperienceLabel();
	UIPersonalExperienceLabel(UIContainer* pParent);
	~UIPersonalExperienceLabel();
	virtual LPCSTR GetClassName() const
	{
		return "UIPersonalExperienceLabel";
	}

	void InitItem(CString title, CString number, CString quantifier);
private:
	void Init();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
	
private:
	UITextSingleLine m_title;
    UITextSingleLine m_num;
	UITextSingleLine m_quantifier;
};
#endif

