#include "BookStoreUI/UIBookStoreCategoryItem.h"
#include "BookStoreUI/UIBookStoreCategoryPage.h"
#include "BookStoreUI/UIBookStoreClassifyPage.h"
#include "BookStore/CategoryInfo.h"
#include "CommonUI/CPageNavigator.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "Utility/ImageManager.h"
#include "Utility/ColorManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "interface.h"
#include "Common/WindowsMetrics.h"
using namespace std;
using namespace WindowsMetrics;

using namespace dk::bookstore;

UIBookStoreCategoryItem::UIBookStoreCategoryItem()
{
    Init();
}

UIBookStoreCategoryItem::~UIBookStoreCategoryItem()
{

}

void UIBookStoreCategoryItem::Init()
{
    const int countFontSize = GetWindowFontSize(UIListItemBaseSmallFontSizeIndex);
    const int briefFontSize = GetWindowFontSize(UIListItemBaseMidFontSizeIndex);
    const int titleFontSize = GetWindowFontSize(UIListItemBaseTitleFontSizeIndex);
    m_categoryCover.Initialize(SMALL);
    m_categoryCover.SetEnable(false);
    AppendChild(&m_categoryCover);

    m_categoryTitel.SetFontSize(titleFontSize);
    AppendChild(&m_categoryTitel);

    m_internalSorting.SetFontSize(briefFontSize);
    AppendChild(&m_internalSorting);

    m_bookCount.SetFontSize(countFontSize);
    AppendChild(&m_bookCount);

    SPtr<ITpImage> passIntoImage = ImageManager::GetImage(IMAGE_ARROW_STRONG_RIGHT);
    m_passIntoImage.SetImage(passIntoImage);
    m_passIntoImage.SetVAlign(ALIGN_CENTER);
    AppendChild(&m_passIntoImage);

    if (!m_windowSizer)
    {
        UISizer* windowSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* mainSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* briefSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* middleSizer = new UIBoxSizer(dkVERTICAL);
        if (windowSizer && mainSizer && middleSizer && briefSizer)
        {
            const int midSizerLeftMargin = GetWindowMetrics(UIPixelValue23Index);
            const int midSizerRightMargin = GetWindowMetrics(UIPixelValue45Index);
            const int briefTopMargin = GetWindowMetrics(UIPixelValue12Index);
            const int briefBottomMargin = GetWindowMetrics(UIPixelValue8Index);

            briefSizer->Add(&m_internalSorting, UISizerFlags(1).Border(dkRIGHT, midSizerRightMargin));
            briefSizer->Add(&m_passIntoImage, UISizerFlags().Expand());

            middleSizer->Add(&m_categoryTitel, UISizerFlags().Expand().Border(dkBOTTOM, briefTopMargin));
            middleSizer->Add(briefSizer, UISizerFlags().Expand());
            middleSizer->Add(&m_bookCount, UISizerFlags().Expand().Border(dkTOP, briefBottomMargin));

            mainSizer->Add(&m_categoryCover);
            mainSizer->Add(middleSizer, UISizerFlags(1).Border(dkLEFT, midSizerLeftMargin).Expand());

            windowSizer->AddStretchSpacer();
            windowSizer->Add(mainSizer, UISizerFlags().Expand());
            windowSizer->AddStretchSpacer();

            SetSizer(windowSizer);
        }
        else
        {
            SafeDeletePointer(windowSizer);
            SafeDeletePointer(mainSizer);
            SafeDeletePointer(briefSizer);
            SafeDeletePointer(middleSizer);
        }
    }
}

void UIBookStoreCategoryItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr categoryInfoSptr, int index)
{
    m_categoryInfo.reset((model::CategoryInfo*)categoryInfoSptr.get()->Clone());
    model::CategoryInfo* categoryInfo = (model::CategoryInfo*)(m_categoryInfo.get());
    if(NULL == categoryInfo)
    {
        return;
    }

    m_internalSorting.SetText(categoryInfo->GetTitles().c_str());
    m_categoryTitel.SetText(categoryInfo->GetLabel().c_str());
    int bookCount = categoryInfo->GetBookCount();
    m_categoryCover.SetCoverUrl(categoryInfo->GetCoverUrl());
    SetBookCountLine(bookCount);
    UpdateWindow();
}

void UIBookStoreCategoryItem::SetBookCountLine(int bookCount)
{
    char szBookCount[64] = {0};
    sprintf(szBookCount,"%s%d%s",StringManager::GetPCSTRById(BOOK_TOTAL),bookCount,StringManager::GetPCSTRById(BOOK_BEN));
    m_bookCount.SetText(szBookCount);
}


HRESULT UIBookStoreCategoryItem::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(grf.EraserBackGround());

#ifdef KINDLE_FOR_TOUCH
    const int lineHeight = 1;
    const StrokeStyle style = DOTTED_STROKE;
#else
    const int lineHeight = IsFocus() ? 3 : 1;
    const StrokeStyle style = IsFocus() ? SOLID_STROKE : DOTTED_STROKE;
#endif
    const int left = m_categoryTitel.GetX();
    const int lineTop = m_categoryCover.GetY() + m_categoryCover.GetHeight() - lineHeight;//m_iHeight - lineHeight;
    RTN_HR_IF_FAILED(grf.DrawLine(left, lineTop, m_iWidth - left, lineHeight, style));
    return hr;
}

bool UIBookStoreCategoryItem::ResponseTouchTap()
{
    if(m_categoryInfo)
    {
        model::CategoryInfo* categoryInfo = m_categoryInfo.get();
        int depth = 0;
        while (categoryInfo)
        {
            categoryInfo = categoryInfo->GetParentNode();
            depth++;
        }
        if (m_categoryInfo->GetBookCount() >= s_maxBookCountsFor2ndCategory && depth == 1)//只有1级分类下有2级分类数据
        {
            UIBookStoreCategoryPage *page = new UIBookStoreCategoryPage();
            if (page)
            {
                page->SetCategoryId(m_categoryInfo->GetCategoryId());
                page->SetTitle(m_categoryInfo->GetLabel());
                page->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
                CPageNavigator::Goto(page);
            }
        }
        else
        {
            UIBookStoreClassifyPage *page = new UIBookStoreClassifyPage(m_categoryInfo->GetCategoryId(), m_categoryInfo->GetLabel());
            if (page)
            {
                page->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
                CPageNavigator::Goto(page);
            }
        }
    }
    return true;
}

