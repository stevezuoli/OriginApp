#ifndef _PERSONALUI_SINGLELINE_
#define _PERSONALUI_SINGLELINE_

#include "GUI/UIContainer.h"
#include "GUI/ITpGraphics.h"

class UISingleLine : public UIContainer
{
public:
	UISingleLine(UIContainer* pParent);
	~UISingleLine();
	virtual LPCSTR GetClassName() const
	{
		return "UISingleLine";
	}
private:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
};
#endif

