#ifndef __GUI_UIIMAGETRANFPARENT_H__
#define __GUI_UIIMAGETRANFPARENT_H__

#include "GUI/UIWindow.h"

class UIImageTransparent: public UIWindow
{
public:
    UIImageTransparent(UIContainer* parent);
    void SetImageFile(const char* imageFile);
    void SetImageFile(const std::string& imageFile)
    {
        SetImageFile(imageFile.c_str());
    }
    bool HasImage() const;
	virtual void SetAutoSize(bool autoSize);
	void ShowBorder(bool show);
    virtual HRESULT Draw(DK_IMAGE drawingImg);

    int GetImageWidth() const;
    int GetImageHeight() const;
private:
    std::wstring m_imageFile;
    DK_UINT m_imgWidth, m_imgHeight;
	bool m_showBorder;
	bool m_autoSize;
};

#endif
