#include "TouchAppUI/UITabBarView.h"
#include "GUI/CTpGraphics.h"

using namespace std;


UITabBarItem::UITabBarItem()
	: UIContainer(NULL,IDSTATIC)
{

}

UITabBarItem::~UITabBarItem()
{

}

void UITabBarItem::Initialize(const std::string& titel,SPtr<ITpImage> icon)
{
	m_titelString = titel;
	m_iconImage = icon;
}

HRESULT UITabBarItem::Draw(DK_IMAGE drawingImg)
{
	int textWidth = 0;
	int textHeight = 0;
	int ImageWidth = 0;
	int ImageHeight = 0;
	if(!m_titelString.empty())
	{
		m_titel.SetFontSize(20);
		m_titel.SetText(m_titelString.c_str());
		textWidth = m_titel.GetTextWidth();
		textHeight = m_titel.GetTextHeight();
		AppendChild(&m_titel);
	}

	if(m_iconImage)
	{
		m_icon.SetImage(m_iconImage);
		ImageWidth = m_icon.GetImageWidth();
		ImageHeight = m_icon.GetImageHeight();
		AppendChild(&m_icon);
	}
	int contentWidth = textWidth + ImageWidth;
	if(0 != textWidth && 0 != ImageWidth)
	{
		contentWidth += ICONMARGINTOTITEL;
	}

	int leftOffset = 0;
	if(contentWidth <= m_iWidth)
	{
		leftOffset = (m_iWidth - contentWidth) / 2;
		m_icon.MoveWindow(leftOffset, (m_iHeight - ImageHeight) / 2, ImageWidth, ImageHeight);
		leftOffset += (ImageWidth + ICONMARGINTOTITEL);
		m_titel.MoveWindow(leftOffset, (m_iHeight - textHeight) / 2, textWidth, textHeight);
	}
	else
	{
		m_icon.SetVisible(FALSE);
		if(textWidth > m_iWidth)
		{
			m_titel.SetVisible(FALSE);
		}
		else
		{
			leftOffset = (m_iWidth - textWidth) / 2;
			m_titel.MoveWindow(leftOffset, (m_iHeight - textHeight) / 2, textWidth, textHeight);
		}
	}
	return UIContainer::Draw(drawingImg);
}

HRESULT UITabBarItem::DrawFocus(DK_IMAGE drawingImg)
{
	HRESULT hr(S_OK);
	if(m_bIsFocus)
	{
		CTpGraphics grf(drawingImg);
		RTN_HR_IF_FAILED(grf.DrawLine(0,m_iHeight - 2,m_iWidth,2,SOLID_STROKE));
	}
	return hr;
}

const char* UITabBarView::EventTabBarSelectedItemChange = "TabBarSelectedItemChange";

UITabBarView::UITabBarView()
	: UIContainer(NULL,IDSTATIC)
	, m_selectedLine(0)
{
}

UITabBarView::~UITabBarView()
{

}

void UITabBarView::AddItemToTabBar(const std::string& titel, SPtr<ITpImage> icon)
{
	UITabBarItem *pItem = new UITabBarItem();
	pItem->Initialize(titel,icon);
	m_itemList.push_back(pItem);
}

int UITabBarView::GetSelectedLine()
{
	return m_selectedLine;
}

HRESULT UITabBarView::DrawBackGround(DK_IMAGE drawingImg)
{
	// TODO: (Xukai) 将数字替换为变量
	HRESULT hr(S_OK);
	hr = UIContainer::DrawBackGround(drawingImg);
	CTpGraphics grf(drawingImg);
	int itemCount = m_itemList.size();
	if(itemCount > 0)
	{
		int leftOffset = 30;
		m_itemWidth = (m_iWidth - 60 - ITEMSSPACE * (itemCount - 1)) / itemCount;
		for(int i = 0; i < itemCount; i++)
		{
			m_itemList[i]->MoveWindow(leftOffset,0,m_itemWidth,m_iHeight);
			AppendChild((m_itemList[i]));
			if(i != m_selectedLine)
			{
				m_itemList[i]->SetFocus(FALSE);
			}
			else
			{
				m_itemList[i]->SetFocus(true);
			}
			leftOffset += m_itemWidth;
			if(i < itemCount - 1)
			{
				RTN_HR_IF_FAILED(grf.DrawLine(leftOffset,10,ITEMSSPACE,m_iHeight - 20,DOTTED_STROKE));
				leftOffset += ITEMSSPACE;
			}
		}
	}
	RTN_HR_IF_FAILED(grf.DrawLine(30,m_iHeight - 2,m_iWidth - 2 * 30,2,DOTTED_STROKE));
	return hr;
}

HRESULT UITabBarView::Draw(DK_IMAGE drawingImg)
{
	return UIContainer::Draw(drawingImg);
}

void UITabBarView::SelectedLine(int selectLine)
{
	if(selectLine != m_selectedLine)
	{
		m_selectedLine = selectLine;
		TabBarSelectedItemChangeEventArgs args;
		args.m_selectedLine = m_selectedLine;
		FireEvent(EventTabBarSelectedItemChange,args);
	}
}
