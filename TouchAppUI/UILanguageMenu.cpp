#include "TouchAppUI/UILanguageMenu.h"
#include "GUI/UIPage.h"
#include "Utility/ImageManager.h"
#include "GUI/FontManager.h"
#include "Utility/ColorManager.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "interface.h"


#define MENU_ID_CANCEL (101)
#define MENU_ID_SELECT (100)

#define COLOR_HILIGHT (0xFFFFFF)
#define COLOR_OUTBORDER (0xAAAAAA)
#define COLOR_BACKGROUND (0xEAEAEA)
#define COLOR_SHADOW (0x888888)
#define COLOR_BORDER (0xAAAAAA)
#define COLOR_HILIGHT_BACK (0x5555FF)

//MENU_ITEM_HEIGHT
#define MENU_IMAGE_HEIGHT (23)
#define MENU_ROW_HEIGHT (12)
#define MENU_ITEM_HEIGHT  (MENU_IMAGE_HEIGHT + MENU_ROW_HEIGHT)

#define MENU_SEPARATOR_HEIGHT (10)
#define MARGIN (24)
#define MARGIN_TXT_ITEM (5)
#define MENU_ITEM_SPACE (2)
#define ITEM_KEY_X (6)
#define ITEM_KEY_Y (2)
#define MENU_ITEM_IMAGE_TO_TEXT  (12)
#define MENU_TOP (30)
#define MENU_RIGHT (580)
#define MENU_HOTKEY_ICON_SIZE (24)

#define MENU_SOLID_LINE_HEIGHT (5)
#define MENU_DOT_LINE_HEIGHT (1)

#define MENU_ICON_WIDTH (12)
#define MENU_ITEM_COUNT (9)

UILanguageMenu::UILanguageMenu(UIWindow* pEventHandler, DWORD Id)
	: UIMenu(pEventHandler)
	, m_recentCount(1)
	, m_iFirstItem(0)
	, m_iTempSelectItem(0)
	, m_iPage(0)
	, m_iCurPage(0)
	, m_isLArrow(0)
	, m_isRArrow(0)
	, m_menuId(Id)
	, m_bIsDisposed(FALSE)
{
	m_recentIndex[0] = -1;
	m_recentIndex[1] = -1;
	m_recentIndex[2] = -1;
	Init();
}

UILanguageMenu::~UILanguageMenu()
{
	Dispose();
}

void UILanguageMenu::Dispose()
{
	OnDispose(m_bIsDisposed);
}

void UILanguageMenu::OnDispose(BOOL bIsDisposed)
{
	if (bIsDisposed)
	{
		return;
	}

	m_bIsDisposed = TRUE;
	Finalize();
}

void UILanguageMenu::Finalize()
{
	UIMenu::Dispose();
}

void UILanguageMenu::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
	if(m_style == Vertical)
	{
		int _iTop = iTop;
		int _iHeight = 2 * MARGIN;

		INT32 iSize = 0;
		if(this->m_menuId == ID_LANGMNU_CHINESE)
		{
			iSize = m_menuItems.size() - m_recentCount - 1;
		}
		else
		{
			iSize = m_menuItems.size() - m_recentCount;
		}
		if(iSize > MENU_ITEM_COUNT)
		{
			iSize = MENU_ITEM_COUNT;
		}
		for (INT32 i = 0; i < iSize; i++)
		{
			MenuItem* pItem = m_menuItems[i];
			if(pItem)
			{
				if(!(pItem->IsVisible()))
				{
					continue;
				}
				else if (pItem->IsStatic())
				{
					_iHeight += MENU_ITEM_HEIGHT;
				}
				else
				{
					_iHeight += MENU_ITEM_HEIGHT;
				}
			}
		}
		if(iSize > 0)
		{
			_iHeight -= MENU_ROW_HEIGHT;
		}

		if(iHeight == -1)
		{
			_iTop = _iTop - _iHeight - MENU_ROW_HEIGHT;
		}
		else if(iHeight == 1)
		{
			_iTop = _iTop + MENU_ITEM_HEIGHT;
		}

        UIWindow::MoveWindow(iLeft, _iTop, iWidth, _iHeight);
	}
}

HRESULT UILanguageMenu::Draw(DK_IMAGE drawingImg)
{
	if (!m_bIsVisible)
	{
		return S_OK;
	}

	if (drawingImg.pbData == NULL)
	{
		return E_POINTER;
	}

	HRESULT hr(S_OK);
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
	RTN_HR_IF_FAILED(CropImage(
		drawingImg,
		rcSelf,
		&imgSelf
		));

	DrawBackground(imgSelf);
	CTpGraphics prg(drawingImg);
	int iLRArrowWidth = 15;
	int iLRArrowHeight = 15;
	int iLRArrowTop = (m_iHeight - iLRArrowHeight) / 2;
	int iLArrowLeft = 2;
	int iRArrowLeft = m_iWidth - iLRArrowWidth - iLArrowLeft;
	SPtr<ITpImage> spArrow = ImageManager::GetImage(IMAGE_FOUR_ARROW);
	if(spArrow)
	{
		if(m_isLArrow == 1)
		{
			prg.DrawImageBlend(spArrow.Get(), m_iLeft + iLArrowLeft, m_iTop + iLRArrowTop, 0, 30, iLRArrowWidth, iLRArrowHeight);
		}
		if(m_isRArrow == 1)
		{
			prg.DrawImageBlend(spArrow.Get(), m_iLeft + iRArrowLeft, m_iTop + iLRArrowTop, 0, 45, iLRArrowWidth, iLRArrowHeight);
		}
	}

	INT32 iMenuItemX =  MARGIN;
	INT32 iMenuItemY =  MARGIN;

	INT32 iSize = m_menuItems.size() - m_recentCount - 1;
	if(m_iPage == 1)
	{
		if(this->m_menuId != ID_LANGMNU_CHINESE)
		{
			MenuItem* pItem = m_menuItems[iSize];
			if(pItem && pItem->IsVisible())
			{
				iMenuItemY  += DrawMenuItem(imgSelf, pItem, iSize, iMenuItemX, iMenuItemY);
			}
		}
		for (INT32 i = 0; i < iSize; i++)
		{
			MenuItem* pItem = m_menuItems[i];
			if(pItem && pItem->IsVisible())
			{
				iMenuItemY  += DrawMenuItem(imgSelf, pItem, i, iMenuItemX, iMenuItemY);
			}
		}
	}
	else
	{
		INT32 iCurPageSize = iSize - m_iFirstItem;
		if(iCurPageSize > MENU_ITEM_COUNT)
		{
			iCurPageSize = MENU_ITEM_COUNT;
		}
		if(m_iCurPage == 0)
		{
			iCurPageSize = MENU_ITEM_COUNT - m_recentCount - 1;
		}

		if(m_iCurPage == 0)
		{
			for (INT32 i = 0; i < m_recentCount + 1; i++)
			{
				MenuItem* pItem = m_menuItems[iSize + i];
				if(pItem && pItem->IsVisible())
				{
					iMenuItemY  += DrawMenuItem(imgSelf, pItem, iSize + i, iMenuItemX, iMenuItemY);
				}
			}
			for (INT32 i = 0; i < iCurPageSize; i++)
			{
				MenuItem* pItem = m_menuItems[i];
				if(pItem && pItem->IsVisible())
				{
					iMenuItemY  += DrawMenuItem(imgSelf, pItem, i, iMenuItemX, iMenuItemY);
				}

			}
		}
		else
		{
			for (INT32 i = m_iFirstItem; i < iCurPageSize + m_iFirstItem; i++)
			{
				MenuItem* pItem = m_menuItems[i];
				if(pItem && pItem->IsVisible())
				{
					iMenuItemY  += DrawMenuItem(imgSelf, pItem, i, iMenuItemX, iMenuItemY);
				}
			}
		}
	}

	return hr;
}

INT32 UILanguageMenu::DrawMenuItem(DK_IMAGE drawingImg, MenuItem* pItem, INT32 iItemIndex, INT32 iMenuItemX, INT32 iMenuItemY)
{
	INT32 iItemHeight = 0;
	INT32 iItemWidth  = 0;
	CTpGraphics prg(drawingImg);

	if (pItem && pItem->IsStatic())
	{
		iItemHeight = MENU_ITEM_HEIGHT;
		m_ifontAttr.SetFontAttributes(0, 0, 24);
		ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
		if(pFont)
		{
			prg.DrawStringUtf8(pItem->GetMenuLabel(), iMenuItemX + MENU_ICON_WIDTH + MENU_ITEM_IMAGE_TO_TEXT, iMenuItemY, m_pFont);
			iItemWidth = pFont->StringWidth(pItem->GetMenuLabel());
		}
	}
	else
	{
		iItemHeight = MENU_ITEM_HEIGHT;

		INT32 iColor = 0;
		if (pItem->IsActive())
		{
			iColor = ColorManager::GetColor(/*isSelected ? COLOR_SELECTION_TXT :*/ COLOR_SOFTKEY_TXT_HIT);
		}
		else
		{
			iColor = ColorManager::GetColor(COLOR_MENUITEM_INACTIVE);
		}
		m_ifontAttr.SetFontAttributes(0, 0, 18);

		ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, iColor);
		if(pFont == NULL)
		{
			return 0;
		}

		//draw lable
		CString lable = pItem->GetMenuLabel();
		if(pFont->StringWidth(lable.GetBuffer()) >=  m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH)
		{
			lable += CString("...");
			while (pFont->StringWidth(lable.GetBuffer()) > m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH)
			{
				lable = lable.SubString(0, lable.Length() - 4);
				lable += CString("...");
			}
		}
		prg.DrawStringUtf8(lable, iMenuItemX + MENU_ICON_WIDTH + MENU_ITEM_IMAGE_TO_TEXT, iMenuItemY + MENU_ITEM_SPACE, pFont);
		iItemWidth += pFont->StringWidth(lable);

		//draw select icon
		int iSelectId = pItem->GetItemSelectImage();
		if(iSelectId > 0)
		{
			SPtr<ITpImage> spImg = ImageManager::GetImage(iSelectId);
			if(spImg)
			{
				int selectIconWidth = spImg.Get()->GetWidth();
				int selectIconHeight = spImg.Get()->GetHeight();
				prg.DrawImageBlend(spImg.Get(), iMenuItemX, iMenuItemY + 3 * MENU_ITEM_SPACE, 0, 0, selectIconWidth,  selectIconHeight);
			}
		}


		// draw select line
		int iSelectLine_x = iMenuItemX + MENU_ICON_WIDTH + MENU_ITEM_IMAGE_TO_TEXT;
		int iSelectLine_y = iMenuItemY + MENU_IMAGE_HEIGHT ;
		int iSelectLine_w = 0;
		if (m_style == Vertical)
		{
			iSelectLine_w  =  m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH - MENU_ITEM_IMAGE_TO_TEXT;
		}
		else
		{
			iSelectLine_w = iItemWidth;
		}

        BOOL isSelected = (iItemIndex == UIMenu::GetSelectedMenuItem());
		if (isSelected)
		{
			prg.DrawLine(iSelectLine_x, iSelectLine_y,  iSelectLine_w , MENU_SOLID_LINE_HEIGHT, SOLID_STROKE);
		}
		else
		{
			prg.DrawLine(iSelectLine_x, iSelectLine_y,  iSelectLine_w , MENU_DOT_LINE_HEIGHT, DOTTED_STROKE);
		}
	}
	if (m_style == Vertical)
	{
		return iItemHeight;
	}
	else
	{
		iItemWidth += MARGIN;
		return iItemWidth;
	}
}

BOOL UILanguageMenu::OnKeyPress(INT32 iKeyCode)
{/*
	INT32 iOldItem = UIMenu::GetSelectedMenuItem();
	INT32 iSize = m_menuItems.size() - m_recentCount - 1;
	INT32 iCurPageSize = iSize - m_iFirstItem;
	if(iCurPageSize > MENU_ITEM_COUNT)
	{
		iCurPageSize = MENU_ITEM_COUNT;
	}
	switch (iKeyCode)
	{
	case KEY_UP:
		if(Vertical == m_style)
		{
			if(m_iPage == 1)
			{
				if(this->m_menuId == ID_LANGMNU_CHINESE)
				{
					if(UIMenu::GetSelectedMenuItem() > 0)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem()-1);
						DrawSelectItem(iOldItem);    
					}
				}
				else
				{
					if(UIMenu::GetSelectedMenuItem() >= 0)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem()-1);
						DrawSelectItem(iOldItem);    
					}
				}
			}
			else
			{
				if(m_iCurPage != 0)
				{
					if(UIMenu::GetSelectedMenuItem() - m_iFirstItem > 0)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem()-1);
						DrawSelectItem(iOldItem);
					}
				}
				else
				{
					if(UIMenu::GetSelectedMenuItem() > 0)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem()-1);
						DrawSelectItem(iOldItem);
					}
					else if(UIMenu::GetSelectedMenuItem() == 0)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem()-2);
						DrawSelectItem(iOldItem);
					}
					else if(UIMenu::GetSelectedMenuItem() >= 0 - m_recentCount)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem()-1);
						DrawSelectItem(iOldItem);
					}
				}
			}
		}
		break;
	case KEY_DOWN:
		if(Vertical == m_style)
		{
			if(m_iPage == 1)
			{
				if(UIMenu::GetSelectedMenuItem() < iSize - 1)
				{
                    UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem() +1);
					DrawSelectItem(iOldItem);    
				}
			}
			else
			{
				if(m_iCurPage != 0)
				{
					if(UIMenu::GetSelectedMenuItem() - m_iFirstItem < iCurPageSize - 1)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem() +1);
						DrawSelectItem(iOldItem);
					}
				}
				else
				{
					if(UIMenu::GetSelectedMenuItem() < -2)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem() +1);
						DrawSelectItem(iOldItem);
					}
					else if(UIMenu::GetSelectedMenuItem() == -2)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem() +2);
						DrawSelectItem(iOldItem);
					}
					else if(UIMenu::GetSelectedMenuItem() < MENU_ITEM_COUNT - m_recentCount - 1 - 1)
					{
                        UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem() +1);
						DrawSelectItem(iOldItem);
					}
				}
			}
			break;
	case KEY_LPAGEDOWN:
	case KEY_RPAGEDOWN:
	case KEY_RIGHT:
		if(m_iCurPage < m_iPage - 1)
		{
			if(m_iCurPage == 0)
			{
				m_iFirstItem += MENU_ITEM_COUNT - m_recentCount - 1;
			}
			else
			{
				m_iFirstItem += MENU_ITEM_COUNT;
			}
			if(m_iFirstItem >= iSize)
			{
				m_iFirstItem = iSize - 1;
			}
            UIMenu::SetSelectedMenuItem(m_iFirstItem);

			m_iCurPage += 1;
			if(m_iCurPage < m_iPage - 1)
			{
				m_isRArrow = 1;
			}
			else
			{
				m_isRArrow = 0;
			}
			if(m_iCurPage > 0)
			{
				m_isLArrow = 1;
			}
			else
			{
				m_isLArrow = 0;
			}
		}
		Repaint();
		break;
	case KEY_LPAGEUP:
	case KEY_RPAGEUP:
	case KEY_LEFT:
		if(0 < m_iCurPage)
		{
			if(m_iCurPage == 1)
			{
				m_iFirstItem -= MENU_ITEM_COUNT - m_recentCount - 1;
			}
			else
			{
				m_iFirstItem -= MENU_ITEM_COUNT;
			}
			if(m_iFirstItem < 0)
			{
				m_iFirstItem = 0;
			}
            UIMenu::SetSelectedMenuItem(m_iFirstItem);
			m_iCurPage -= 1;
			if(m_iCurPage < m_iPage - 1)
			{
				m_isRArrow = 1;
			}
			else
			{
				m_isRArrow = 0;
			}
			if(m_iCurPage > 0)
			{
				m_isLArrow = 1;
			}
			else
			{
				m_isLArrow = 0;
			}
		}
		Repaint();
		break;
	case KEY_OKAY:
		{
			Show(FALSE);
			MenuItem *pItem = NULL;
			if(UIMenu::GetSelectedMenuItem() < 0)
			{
				if(this->m_menuId == ID_LANGMNU_CHINESE)
				{
					pItem = m_menuItems[m_menuItems.Size(] + UIMenu::GetSelectedMenuItem());
				}
				else
				{
					if(m_iPage == 1)
					{
						pItem = m_menuItems[iSize];
					}
					else
					{
						pItem = m_menuItems[m_menuItems.Size(] + UIMenu::GetSelectedMenuItem());
					}
				}
			}
			else
			{
				pItem = m_menuItems[UIMenu::GetSelectedMenuItem(]);
			}
			if(pItem != NULL)
			{
				SysButton * pBn = NULL;
				pBn =  ((UISystemSettingPage *)(m_pParent))->GetFocusBn();
				pBn->SetText(pItem->GetMenuLabel());
				int ileft = 0;
				ileft = pBn->GetX();
				int itop = 0;
				itop = pBn->GetY();
				int iwidth = 0;
				iwidth = pBn->GetTextLenth() + 30;
				int iheight = 0;
				iheight =    pBn->GetHeight();
				pBn->MoveWindow(ileft, itop, iwidth, iheight);
				ClickMenuItem();
			}
			m_iFirstItem = 0;
			m_iCurPage = 0;
		}
		break;
	case KEY_BACK:
        UIMenu::SetSelectedMenuItem(m_iTempSelectItem);
		Show(FALSE);
		m_iFirstItem = 0;
		m_iCurPage = 0;
		break;
	default:
		return UIWindow::OnKeyPress(iKeyCode);
		break;
		}
	}*/
	return FALSE;
}

void UILanguageMenu::DrawBackground(DK_IMAGE drawingImg)
{
	CTpGraphics prg(drawingImg);
	SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_MENU_BACK);
	if (spImg)
	{
		prg.DrawImageStretchBlend(spImg.Get(), 0, 0, m_iWidth, m_iHeight);
	}
}


BOOL UILanguageMenu::DrawSelectItem(INT32 iOldItem)
{
	if(iOldItem != UIMenu::GetSelectedMenuItem())
	{
		CDisplay* display = CDisplay::GetDisplay();
		if(NULL == display)
		{
			return FALSE;
		}
		DK_IMAGE pGraphics = display->GetMemDC();
		CTpGraphics prg(pGraphics);
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		GetMenuItemSelectLine(x,y,w,h,iOldItem - m_iFirstItem);
		prg.FillRect(m_iLeft + x, m_iTop + y, m_iLeft + x + w, m_iTop + y + h, 0x00000000);
		prg.DrawLine(x + m_iLeft, y + m_iTop + MENU_SOLID_LINE_HEIGHT / 2 -MENU_DOT_LINE_HEIGHT,  w, MENU_DOT_LINE_HEIGHT,DOTTED_STROKE);
		//display->ScreenRepaint(x + m_iLeft,y + m_iTop,w,h);
		//display->ScreenRepaint(m_iLeft, m_iTop, m_iWidth, m_iHeight, m_iLeft, m_iTop);
		GetMenuItemSelectLine(x,y,w,h,UIMenu::GetSelectedMenuItem() - m_iFirstItem);
		prg.DrawLine(x + m_iLeft,y + m_iTop, w, MENU_SOLID_LINE_HEIGHT,SOLID_STROKE);
		
        Repaint();
        //display->ScreenRepaint(x + m_iLeft,y + m_iTop,w,h);
	}
	return FALSE;
}

INT32 UILanguageMenu::GetMenuItemSelectLine(int &x,int &y,int &w,int &h,int indexItem)
{
	INT32 iSize = m_menuItems.size() - m_recentCount - 1;
	x = MARGIN + MENU_ICON_WIDTH + MENU_ITEM_IMAGE_TO_TEXT;
	y = MARGIN;
	if (indexItem>=iSize)
	{
		indexItem = iSize-1;
	}
	if(this->m_menuId == ID_LANGMNU_CHINESE)
	{
		if(m_iPage == 1 || (m_iPage > 1 && m_iCurPage > 0))
		{
			for (INT32 i = 0; i < indexItem; i++)
			{
				MenuItem*pItem = m_menuItems[i];

				if (pItem->IsStatic())
				{
					y  += MENU_ITEM_HEIGHT;
				}
				else if(pItem->IsVisible())
				{
					y  += MENU_ITEM_HEIGHT;
				}
			}
			y += (MENU_IMAGE_HEIGHT);
			w =  m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH - MENU_ITEM_IMAGE_TO_TEXT;
			h = MENU_SOLID_LINE_HEIGHT;
		}
		else
		{
			for(INT32 i = 0 - m_recentCount - 1; i < indexItem; i++)
			{
				MenuItem  *pItem = NULL;
				if(i < 0)
				{
					pItem = m_menuItems[i+(int)m_menuItems.size()];
				}
				else
				{
					pItem= m_menuItems[i];
				}

				if (pItem && pItem->IsStatic())
				{
					y  += MENU_ITEM_HEIGHT;
				}
				else if(pItem && pItem->IsVisible())
				{
					y  += MENU_ITEM_HEIGHT;
				}
			}
			y += (MENU_IMAGE_HEIGHT);
			w =  m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH - MENU_ITEM_IMAGE_TO_TEXT;
			h = MENU_SOLID_LINE_HEIGHT;
		}
	}
	else
	{
		if(m_iPage == 1)
		{
			for(INT32 i =  - 1; i < indexItem; i++)
			{
				MenuItem *pItem = NULL;
				if(i < 0)
				{
					pItem = m_menuItems[iSize];
				}
				else
				{
					pItem= m_menuItems[i];
				}
				if (pItem && pItem->IsStatic())
				{
					y  += MENU_ITEM_HEIGHT;
				}
				else if(pItem && pItem->IsVisible())
				{
					y  += MENU_ITEM_HEIGHT;
				}
			}
			y += (MENU_IMAGE_HEIGHT);
			w =  m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH - MENU_ITEM_IMAGE_TO_TEXT;
			h = MENU_SOLID_LINE_HEIGHT;
		}
		else
		{
			if(m_iCurPage == 0)
			{
				for(INT32 i = 0 - m_recentCount - 1; i < indexItem; i++)
				{
					MenuItem *pItem = NULL;
					if(i < 0)
					{
						pItem = m_menuItems[i+(int)m_menuItems.size()];
					}
					else
					{
						pItem= m_menuItems[i];
					}
					if (pItem && pItem->IsStatic())
					{
						y  += MENU_ITEM_HEIGHT;
					}
					else if(pItem && pItem->IsVisible())
					{
						y  += MENU_ITEM_HEIGHT;
					}
				}
				y += (MENU_IMAGE_HEIGHT);
				w =  m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH - MENU_ITEM_IMAGE_TO_TEXT;
				h = MENU_SOLID_LINE_HEIGHT;
			}
			else
			{
				for(INT32 i = 0; i < indexItem; i++)
				{
					MenuItem *pItem = m_menuItems[i];

					if (pItem && pItem->IsStatic())
					{
						y += MENU_ITEM_HEIGHT;
					}
					else if(pItem && pItem->IsVisible())
					{
						y += MENU_ITEM_HEIGHT;
					}
				}
				y += (MENU_IMAGE_HEIGHT);
				w = m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH - MENU_ITEM_IMAGE_TO_TEXT;
				h = MENU_SOLID_LINE_HEIGHT;
			}
		}
	}
	return 0;
}

void UILanguageMenu::PopupMenu(INT32 iX, INT32 iY, INT32 iScreenWidth, INT32 iScreenHeight)
{
/*	if (!IsVisible())
	{
		Update(TRUE);
	}

	//实际字体数目
	int iSize = 0;
	if(this->m_menuId == ID_LANGMNU_CHINESE)
	{
		iSize = m_menuItems.size() - m_recentCount - 1;
	}
	else
	{
		iSize = m_menuItems.size() - m_recentCount;
	}
	//设置被选中的字体icon
	if(iSize <= MENU_ITEM_COUNT)
	{
		m_iPage = 1;
		if(strcmp(((UISystemSettingPage *)(m_pParent))->GetFocusBn()->GetText(), 
			StringManager::GetStringById(CUSTOM_FONT_SAME_AS_CNS)) == 0)
		{
			this->SetItemSelectIcon(m_menuItems[m_menuItems.Size(] - m_recentCount - 1)->GetMenuId(), IMAGE_SELECTICON);
		}
	}
	else
	{
		iSize = m_menuItems.size() - m_recentCount - 1;
		m_iPage = (iSize + m_recentCount) / MENU_ITEM_COUNT + 1;

		// 英文字体菜单不再需要“使用中文字体设置”选项，所以此处不区分中英文字体菜单 (modified by liuhj@duokan.com 2011.12.28)
		this->SetItemSelectIcon(m_menuItems[m_menuItems.Size(] - m_recentCount - 1)->GetMenuId(), IMAGE_SELECTICON);
	}
	m_iCurPage = 0;
	m_isLArrow = 0;
	if(m_iCurPage < m_iPage - 1)
	{
		m_isRArrow = 1;
	}
	m_iTempSelectItem = UIMenu::GetSelectedMenuItem();
    UIMenu::SetSelectedMenuItem(m_iFirstItem);
    UIMenu::SetScreenWidth(iScreenWidth);
    UIMenu::SetScreenHeight(iScreenHeight);
	m_bIsVisible = TRUE;
	Repaint();*/
}


void UILanguageMenu::ClickMenuItem()
{
	INT32 iSize = m_menuItems.size() - m_recentCount - 1;
	MenuItem* pItem = NULL;
	if(UIMenu::GetSelectedMenuItem() < 0)
	{
		if(this->m_menuId == ID_LANGMNU_CHINESE)
		{
			pItem = m_menuItems[UIMenu::GetSelectedMenuItem() + m_menuItems.size()];
		}
		else
		{
			if(m_iPage == 1)
			{
				pItem = m_menuItems[iSize];
			}
			else
			{
				pItem = m_menuItems[UIMenu::GetSelectedMenuItem() + m_menuItems.size()];
			}
		}
	}
	else
	{
		pItem = m_menuItems[UIMenu::GetSelectedMenuItem()];
	}
	if(pItem != NULL)
	{
		if (pItem->GetSubMenu() == NULL)
		{
			if (!pItem->IsActive())
			{
				return;
			}

			if (m_pEventHandler)
			{
				m_pEventHandler->OnEvent(UIEvent(COMMAND_EVENT, this, pItem->GetMenuId(), this->m_menuId));
			}
		}
		else
		{
			PopSubMenu(pItem);
		}
	}
}


// UIPopMenu
UIPopMenu::UIPopMenu(UIWindow* pEventHandler, DWORD Id)
	: UIMenu(pEventHandler)
	, m_iFirstItem(0)
	, m_iTempSelectItem(0)
	, m_iPageCount(0)
	, m_iCurPage(0)
	, m_isLArrow(0)
	, m_isRArrow(0)
	, m_menuId(Id)
	, m_bIsDisposed(FALSE)
{
	Init();
}


UIPopMenu::~UIPopMenu()
{
	Dispose();
}

void UIPopMenu::Dispose()
{
	OnDispose(m_bIsDisposed);
}

void UIPopMenu::OnDispose(BOOL bIsDisposed)
{
	if (bIsDisposed)
	{
		return;
	}

	m_bIsDisposed = TRUE;
	Finalize();
}

void UIPopMenu::Finalize()
{
	UIMenu::Dispose();
}

void UIPopMenu::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
	if(m_style == Vertical)
	{
        int _iTop = iTop;
		int _iHeight = 2 * MARGIN;
		INT32 iSize = m_menuItems.size();
		if(iSize > MENU_ITEM_COUNT)
		{
			iSize = MENU_ITEM_COUNT;
		}
		for (INT32 i = 0; i < iSize; i++)
		{
			MenuItem* pItem = m_menuItems[i];
			if(pItem)
			{
				if(!(pItem->IsVisible()))
				{
					continue;
				}
				else 
				{
					_iHeight += MENU_ITEM_HEIGHT;
				}
			}
		}
		if(iSize > 0)
		{
			_iHeight -= MENU_ROW_HEIGHT;
		}

		if(iHeight == -1)
		{
			_iTop = _iTop - _iHeight - MENU_ROW_HEIGHT;
		}
		else if(iHeight == 1)
		{
			_iTop = _iTop + MENU_ITEM_HEIGHT;
		}

        UIWindow::MoveWindow(iLeft, _iTop, iWidth, _iHeight);
	}
    DebugPrintf(DLC_JUGH, "[%d, %d, %d, %d]\n", m_iTop, m_iLeft, m_iWidth, m_iHeight);
}

HRESULT UIPopMenu::Draw(DK_IMAGE drawingImg)
{
	if (!m_bIsVisible)
	{
		return S_OK;
	}

	if (drawingImg.pbData == NULL)
	{
		return E_POINTER;
	}

	HRESULT hr(S_OK);
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
	RTN_HR_IF_FAILED(CropImage(
		drawingImg,
		rcSelf,
		&imgSelf
		));

	DrawBackground(imgSelf);
	CTpGraphics prg(drawingImg);
	int iLRArrowWidth = 15;
	int iLRArrowHeight = 15;
	int iLRArrowTop = (m_iHeight - iLRArrowHeight) / 2;
	int iLArrowLeft = 2;
	int iRArrowLeft = m_iWidth - iLRArrowWidth - iLArrowLeft;
	SPtr<ITpImage> spArrow = ImageManager::GetImage(IMAGE_FOUR_ARROW);
	if(spArrow)
	{
		if(m_isLArrow == 1)
		{
			prg.DrawImageBlend(spArrow.Get(), m_iLeft + iLArrowLeft, m_iTop + iLRArrowTop, 0, 30, iLRArrowWidth, iLRArrowHeight);
		}
		if(m_isRArrow == 1)
		{
			prg.DrawImageBlend(spArrow.Get(), m_iLeft + iRArrowLeft, m_iTop + iLRArrowTop, 0, 45, iLRArrowWidth, iLRArrowHeight);
		}
	}

	INT32 iMenuItemX =  MARGIN;
	INT32 iMenuItemY =  MARGIN;

	INT32 iSize = m_menuItems.size() - m_iFirstItem;
	if(iSize > MENU_ITEM_COUNT)
	{
		iSize = MENU_ITEM_COUNT;
	}

	for (INT32 i = m_iFirstItem; i < iSize + m_iFirstItem; i++)
	{
		MenuItem* pItem = m_menuItems[i];
		if(pItem && pItem->IsVisible())
		{
			iMenuItemY  += DrawMenuItem(imgSelf, pItem, i, iMenuItemX, iMenuItemY);
		}
	}

	return hr;
}

INT32 UIPopMenu::DrawMenuItem(DK_IMAGE drawingImg, MenuItem* pItem, INT32 iItemIndex, INT32 iMenuItemX, INT32 iMenuItemY)
{
	INT32 iItemHeight = 0;
	INT32 iItemWidth  = 0;
	CTpGraphics prg(drawingImg);

	if (pItem && pItem->IsStatic())
	{
		iItemHeight = MENU_ITEM_HEIGHT;
		m_ifontAttr.SetFontAttributes(0, 0, 24);
		ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
		if(pFont)
		{
			prg.DrawStringUtf8(pItem->GetMenuLabel(), iMenuItemX + MENU_ICON_WIDTH + MENU_ITEM_IMAGE_TO_TEXT, iMenuItemY, m_pFont);
			iItemWidth = pFont->StringWidth(pItem->GetMenuLabel());
		}
	}
	else
	{
		iItemHeight = MENU_ITEM_HEIGHT;

		INT32 iColor = 0;
		if (pItem->IsActive())
		{
			iColor = ColorManager::GetColor(COLOR_SOFTKEY_TXT_HIT);
		}
		else
		{
			iColor = ColorManager::GetColor(COLOR_MENUITEM_INACTIVE);
		}
		m_ifontAttr.SetFontAttributes(0, 0, 18);

		ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, iColor);
		if(pFont == NULL)
		{
			return 0;
		}

		//draw lable
		CString lable = pItem->GetMenuLabel();
		if(pFont->StringWidth(lable.GetBuffer()) >=  m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH)
		{
			lable += CString("...");
			while (pFont->StringWidth(lable.GetBuffer()) > m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH)
			{
				lable = lable.SubString(0, lable.Length() - 4);
				lable += CString("...");
			}
		}
		prg.DrawStringUtf8(lable, iMenuItemX + MENU_ICON_WIDTH + MENU_ITEM_IMAGE_TO_TEXT, iMenuItemY + MENU_ITEM_SPACE, pFont);
		iItemWidth += pFont->StringWidth(lable);

		//draw select icon
		int iSelectId = pItem->GetItemSelectImage();
		if(iSelectId > 0)
		{
			SPtr<ITpImage> spImg = ImageManager::GetImage(iSelectId);
			if(spImg)
			{
				int selectIconWidth = spImg.Get()->GetWidth();
				int selectIconHeight = spImg.Get()->GetHeight();
				prg.DrawImageBlend(spImg.Get(), iMenuItemX, iMenuItemY + 3 * MENU_ITEM_SPACE, 0, 0, selectIconWidth,  selectIconHeight);
			}
		}


		// draw select line
		int iSelectLine_x = iMenuItemX + MENU_ICON_WIDTH + MENU_ITEM_IMAGE_TO_TEXT;
		int iSelectLine_y = iMenuItemY + MENU_IMAGE_HEIGHT ;
		int iSelectLine_w = 0;
		if (m_style == Vertical)
		{
			iSelectLine_w  =  m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH - MENU_ITEM_IMAGE_TO_TEXT;
		}
		else
		{
			iSelectLine_w = iItemWidth;
		}

		BOOL isSelected = (iItemIndex == UIMenu::GetSelectedMenuItem());
		if (isSelected)
		{
			prg.DrawLine(iSelectLine_x, iSelectLine_y,  iSelectLine_w , MENU_SOLID_LINE_HEIGHT, SOLID_STROKE);
		}
		else
		{
			prg.DrawLine(iSelectLine_x, iSelectLine_y,  iSelectLine_w , MENU_DOT_LINE_HEIGHT, DOTTED_STROKE);
		}
	}
	if (m_style == Vertical)
	{
		return iItemHeight;
	}
	else
	{
		iItemWidth += MARGIN;
		return iItemWidth;
	}
}

BOOL UIPopMenu::OnKeyPress(INT32 iKeyCode)
{
/*	INT32 iOldItem = UIMenu::GetSelectedMenuItem();
	INT32 iSize = m_menuItems.size();
	INT32 iCurPageSize = iSize - m_iFirstItem;
	if(iCurPageSize > MENU_ITEM_COUNT)
	{
		iCurPageSize = MENU_ITEM_COUNT;
	}
	switch (iKeyCode)
	{
	case KEY_UP:
        DebugPrintf(DLC_CHENM, "OnKeypress @ UILanguageMenu # 1 %d , %d ",UIMenu::GetSelectedMenuItem(),m_iFirstItem);
		if((Vertical == m_style) && (UIMenu::GetSelectedMenuItem() - m_iFirstItem > 0))
		{
            UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem() -1);
            DebugPrintf(DLC_CHENM, "OnKeypress @ UILanguageMenu # 2 %d , %d ",UIMenu::GetSelectedMenuItem(),m_iFirstItem);
            DrawSelectItem(iOldItem);
		}
		break;
	case KEY_DOWN:
        DebugPrintf(DLC_CHENM, "OnKeypress @ UILanguageMenu # 3 %d , %d ",UIMenu::GetSelectedMenuItem(),m_iFirstItem);

		if((Vertical == m_style) && (UIMenu::GetSelectedMenuItem() - m_iFirstItem < iCurPageSize - 1))
		{
            UIMenu::SetSelectedMenuItem(UIMenu::GetSelectedMenuItem() + 1);
            DebugPrintf(DLC_CHENM, "OnKeypress @ UILanguageMenu # 4 %d , %d ",UIMenu::GetSelectedMenuItem(),m_iFirstItem);

			DrawSelectItem(iOldItem);
		}
		break;
	case KEY_LPAGEDOWN:
	case KEY_RPAGEDOWN:
	case KEY_RIGHT:
		if(m_iCurPage < m_iPageCount - 1)
		{
			m_iFirstItem += MENU_ITEM_COUNT;
            
			if(m_iFirstItem >= iSize)
			{
				m_iFirstItem = iSize - 1;
			}
            UIMenu::SetSelectedMenuItem(m_iFirstItem);
			m_iCurPage++;
			m_isRArrow = (m_iCurPage < m_iPageCount - 1) ? 1 : 0;
			m_isLArrow = (m_iCurPage > 0) ? 1 : 0;
		}
		Repaint();
		break;
	case KEY_LPAGEUP:
	case KEY_RPAGEUP:
	case KEY_LEFT:
		if(0 < m_iCurPage)
		{
			m_iFirstItem -= MENU_ITEM_COUNT;
			if(m_iFirstItem < 0)
			{
				m_iFirstItem = 0;
			}
            UIMenu::SetSelectedMenuItem(m_iFirstItem);
			m_iCurPage--;
			m_isRArrow = (m_iCurPage < m_iPageCount - 1) ? 1 : 0;
			m_isLArrow = (m_iCurPage > 0) ? 1 : 0;
		}
		Repaint();
		break;
	case KEY_OKAY:
		{
			Show(FALSE);
            DebugPrintf(DLC_CHENM, "OnKeypress @ UILanguageMenu",UIMenu::GetSelectedMenuItem());
			MenuItem *pItem = m_menuItems[UIMenu::GetSelectedMenuItem(]);
			if(pItem != NULL)
			{
				SysButton * pBn = NULL;
				pBn =  ((UISystemSettingPage *)(m_pParent))->GetFocusBn();
				pBn->SetText(pItem->GetMenuLabel());
				int ileft   = pBn->GetX();
				int itop    = pBn->GetY();
				int iwidth  = pBn->GetTextLenth() + 30;
				int iheight = pBn->GetHeight();
				pBn->MoveWindow(ileft, itop, iwidth, iheight);
				ClickMenuItem();
			}
			m_iFirstItem = 0;
			m_iCurPage = 0;
		}
		break;
	case KEY_BACK:
        UIMenu::SetSelectedMenuItem(m_iTempSelectItem);
		Show(FALSE);
		m_iFirstItem = 0;
		m_iCurPage = 0;
		break;
	default:
		return UIWindow::OnKeyPress(iKeyCode);
		break;
	}*/
	return FALSE;
}

void UIPopMenu::DrawBackground(DK_IMAGE drawingImg)
{
	CTpGraphics prg(drawingImg);
	SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_MENU_BACK);
	if (spImg)
	{
		prg.DrawImageStretchBlend(spImg.Get(), 0, 0, m_iWidth, m_iHeight);
	}
}


BOOL UIPopMenu::DrawSelectItem(INT32 iOldItem)
{
	if(iOldItem != UIMenu::GetSelectedMenuItem())
	{
		CDisplay* display = CDisplay::GetDisplay();
		if(NULL == display)
		{
			return FALSE;
		}
		DK_IMAGE pGraphics = display->GetMemDC();
		CTpGraphics prg(pGraphics);
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		GetMenuItemSelectLine(x,y,w,h,iOldItem - m_iFirstItem);
		prg.FillRect(m_iLeft + x, m_iTop + y, m_iLeft + x + w, m_iTop + y + h, 0x00000000);
		prg.DrawLine(x + m_iLeft, y + m_iTop + MENU_SOLID_LINE_HEIGHT / 2 -MENU_DOT_LINE_HEIGHT,  w, MENU_DOT_LINE_HEIGHT,DOTTED_STROKE);
        //display->ScreenRepaint(x + m_iLeft,y + m_iTop,w,h);
		//display->ScreenRepaint(m_iLeft, m_iTop, m_iWidth, m_iHeight, m_iLeft, m_iTop);
		GetMenuItemSelectLine(x,y,w,h,UIMenu::GetSelectedMenuItem() - m_iFirstItem);
		prg.DrawLine(x + m_iLeft,y + m_iTop, w, MENU_SOLID_LINE_HEIGHT,SOLID_STROKE);

        Repaint();
		//display->ScreenRepaint(x + m_iLeft,y + m_iTop,w,h);
	}
	return FALSE;
}

INT32 UIPopMenu::GetMenuItemSelectLine(int &x,int &y,int &w,int &h,int indexItem)
{
	INT32 iSize = m_menuItems.size();
	x = MARGIN + MENU_ICON_WIDTH + MENU_ITEM_IMAGE_TO_TEXT;
	y = MARGIN;
	if (indexItem >= iSize)
	{
		indexItem = iSize - 1;
	}
    
	for (INT32 i = 0; i < indexItem; i++)
	{
		MenuItem*pItem = m_menuItems[i];

		if (pItem->IsVisible() || pItem->IsStatic())
		{
			y  += MENU_ITEM_HEIGHT;
		}
	}
	y += (MENU_IMAGE_HEIGHT);
	w =  m_iWidth - MARGIN * 2 - MENU_ICON_WIDTH - MENU_ITEM_IMAGE_TO_TEXT;
	h = MENU_SOLID_LINE_HEIGHT;
	return 0;
}

void UIPopMenu::PopupMenu(INT32 iX, INT32 iY, INT32 iScreenWidth, INT32 iScreenHeight)
{
	if (!IsVisible())
	{
		Update(TRUE);
	}

	//实际字体数目
	int iSize = m_menuItems.size();
    
	//设置被选中的字体icon
	if(iSize <= MENU_ITEM_COUNT)
	{
		m_iPageCount = 1;
	}
	else
	{
		m_iPageCount = iSize / MENU_ITEM_COUNT + 1;
	}
	m_iCurPage = 0;
	m_isLArrow = 0;
	if(m_iCurPage < m_iPageCount - 1)
	{
		m_isRArrow = 1;
	}
	m_iTempSelectItem = UIMenu::GetSelectedMenuItem();
    UIMenu::SetSelectedMenuItem(m_iFirstItem);
    UIMenu::SetScreenWidth(iScreenWidth);
    UIMenu::SetScreenHeight(iScreenHeight);
	m_bIsVisible = TRUE;
	Repaint();
}


void UIPopMenu::ClickMenuItem()
{
	MenuItem* pItem = m_menuItems[UIMenu::GetSelectedMenuItem()];
	if(pItem != NULL)
	{
		if (pItem->GetSubMenu() == NULL)
		{
			if (!pItem->IsActive())
			{
				return;
			}

			if (m_pEventHandler)
			{
				m_pEventHandler->OnEvent(UIEvent(COMMAND_EVENT, this, pItem->GetMenuId(), this->m_menuId));
			}
		}
		else
		{
			PopSubMenu(pItem);
		}
	}
}


