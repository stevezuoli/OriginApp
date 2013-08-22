#ifndef _DKREADER_TOUCHAPPUI_UIBOOKCOVERVIEW_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKCOVERVIEW_H_

#include <string>
#include "GUI/UIContainer.h"
#include "GUI/UIText.h"
#include "GUI/EventArgs.h"
#include "Utility/ImageManager.h"
#include "GUI/UIImageTransparent.h"

enum BookCoverFrameStyle
{
	SMALL,
	LARGE,
	CUSTOMIZED
};
#ifndef KINDLE_FOR_TOUCH
class BookCoverViewTouchedEventArgs : public EventArgs
{
public:
	virtual EventArgs* Clone() const
	{
		return new BookCoverViewTouchedEventArgs(*this);
	}
	std::string m_coverUrl;
};
#endif
class UIBookCoverView : public UIContainer
{
public:
#ifndef KINDLE_FOR_TOUCH
    static const char* EventBookCoverViewTouched;
#endif
    UIBookCoverView();
    UIBookCoverView(UIContainer* pParent, const DWORD dwId);
    ~UIBookCoverView();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookCoverView";
    }
    void SetCoverUrl(const std::string& coverUrl);
    const std::string& GetCoverUrl() const
    {
        return m_coverUrl;
    }
    void SetCoverFrameStyle(BookCoverFrameStyle style);
    void Initialize(BookCoverFrameStyle coverStyle, int defaultCoverId = IMAGE_BOOKSTORE_DEFAULTBOOKCOVER, const std::string& coverUrl = "");
    virtual void MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height);
    //only works when default-cover displayed
    void SetBookTitle(const char* title);
    UIText* GetTextWidget()
    {
        return &m_textBookTitle;
    }

    virtual void SetEnable(BOOL bEnable);
    void ShowBorder(bool showBorder);
#ifdef KINDLE_FOR_TOUCH
    bool OnChildClick(UIWindow* child);
#else
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual HRESULT UpdateFocus();
#endif
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
    bool OnMessageCoverImageUpdate(const EventArgs& args);
    void UpdateMyCoverImage();
    void Init();
    void RequestCoverFromNetwork();
private:
    //UIImage m_coverImage;
    UIImageTransparent m_coverImage;
    UIText m_textBookTitle;
    BookCoverFrameStyle m_coverStyle;
    std::string m_coverUrl;
    int m_defaultCoverId;
    SPtr<ITpImage>      m_defaultCoverImage;
    bool m_showBorder;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKCOVERVIEW_H_
