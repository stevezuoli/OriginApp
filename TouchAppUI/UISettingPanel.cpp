#include "TouchAppUI/UISettingPanel.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIRadioButtonGroup.h"
#include "Common/SystemSetting_DK.h"
#include "I18n/StringManager.h"
#include "drivers/DeviceInfo.h"
#include "Wifi/WifiBaseImpl.h"
#include "../Common/FileManager_DK.h"
#include "Utility/EncodeUtil.h"
#include "TouchAppUI/LayoutHelper.h"
#include "Wifi/WifiManager.h"
#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "Utility/Misc.h"

using namespace WindowsMetrics;
using namespace dk::utility;

UISettingPanel::UISettingPanel()
{
    m_titleFontSize = GetWindowFontSize(UISettingPanelSectionIndex);
    m_itemFontSize = GetWindowFontSize(UISettingPanelItemTitleIndex);
    m_itemButtonFontSize = GetWindowFontSize(UISettingPanelItemButtonIndex);
    m_buttonLeftMargin = GetWindowMetrics(UISettingPanelButtonLeftMarginIndex);
    m_buttonTopMargin = GetWindowMetrics(UISettingPanelButtonTopMarginIndex);
    m_titleLeftMargin = GetWindowMetrics(UISettingPanelSectionLeftMarginIndex);
    m_itemLeftMagin = GetWindowMetrics(UISettingPanelItemLeftMarginIndex);
    m_leftHalfWidth = GetWindowMetrics(UISettingPanelLeftHalfWidthIndex);
    m_rightHalfWidth = GetWindowMetrics(UISettingPanelRightHalfWidthIndex);
    m_titleUpSpacing = GetWindowMetrics(UISettingPanelTitleUpSpacingIndex);
    m_titleDownSpacing = GetWindowMetrics(UISettingPanelTitleDownSpacingIndex);
    m_itemVertSpacing = GetWindowMetrics(UISettingPanelItemVertSpacingIndex);
    m_itemTipVertSpacing = GetWindowMetrics(UISettingPanelItemTipVertSpacingIndex);
    m_itemHorzSpacing = GetWindowMetrics(UISettingPanelItemHorzSpacingIndex);
    m_systemSettingInfo = SystemSettingInfo::GetInstance();
    m_selectedChineseFontIndex = -1;
    m_selectedEnglishFontIndex = -1;
    m_fontMenuWidth = GetWindowMetrics(FontMenuWidthIndex);
    m_selectedIMEIndex = -1;
    m_selectedLanguage = -1;

    SetMinWidth(m_leftHalfWidth + m_rightHalfWidth);
}

UISettingPanel::~UISettingPanel()
{
    for (size_t i = 0; i < m_dynamicCreatedWindows.size(); ++i)
    {
        delete m_dynamicCreatedWindows[i];
    }
}

//dkSize UISettingPanel::GetMinSize() const
//{
    //return dkSize(m_leftHalfWidth + m_rightHalfWidth, 1);
//}

UIBoxSizer* UISettingPanel::AddSectionTitle(const char* text)
{
    if (!EnsureMainSizerExisted())
    {
        return NULL;
    }
    UISizer* mainSizer = GetSizer();
    UIBoxSizer* sizer = new UIBoxSizer(dkHORIZONTAL);

    UITextSingleLine* titleLine = new UITextSingleLine();
    titleLine->SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    titleLine->SetFontSize(m_titleFontSize);
    titleLine->SetText(text);
    AppendChild(titleLine);
    m_dynamicCreatedWindows.push_back(titleLine);

    sizer->AddSpacer(m_titleLeftMargin);
    sizer->Add(titleLine);

    mainSizer->AddSpacer(m_titleUpSpacing);
    mainSizer->Add(sizer);
    mainSizer->AddSpacer(m_titleDownSpacing);

    return sizer;
}

UIBoxSizer* UISettingPanel::CreateItemTitleSizer(const char* text, UITextSingleLine** textSingleLine)
{
    UITextSingleLine* line = new UITextSingleLine();
    line->SetFontSize(m_itemFontSize);
    line->SetText(text);
    AppendChild(line);
    m_dynamicCreatedWindows.push_back(line);

    UIBoxSizer* sizer = new UIBoxSizer(dkHORIZONTAL);
    sizer->SetMinWidth(m_leftHalfWidth);
    sizer->AddSpacer(m_itemLeftMagin);
    sizer->Add(line, UISizerFlags().Center());

    if (textSingleLine)
    {
        *textSingleLine = line;
    }
    return sizer;
}


UIBoxSizer* UISettingPanel::CreateRightHalfItemSizer()
{
    UIBoxSizer* sizer = new UIBoxSizer(dkHORIZONTAL);
    sizer->SetMinWidth(m_rightHalfWidth);
    return sizer;
}

UIBoxSizer* UISettingPanel::AddPushButton(const char* itemTitle, int itemBtnId, const char* btnText, UISizer** wholeSizer, UITextSingleLine** titleOnRight)
{
    if (!EnsureMainSizerExisted())
    {
        return NULL;
    }
    UISizer* mainSizer = GetSizer();
    UIBoxSizer* leftItemSizer = CreateItemTitleSizer(itemTitle, titleOnRight);
    UITouchComplexButton* btn = CreateButton(itemBtnId, btnText, true);
    UIBoxSizer* rightItemSizer = CreateRightHalfItemSizer();
    rightItemSizer->Add(btn, UISizerFlags().Center());
    UIBoxSizer* itemSizer = new UIBoxSizer(dkHORIZONTAL);
    itemSizer->Add(leftItemSizer, UISizerFlags().Expand());
    itemSizer->Add(rightItemSizer, UISizerFlags().Expand());
    mainSizer->Add(itemSizer);
    if (m_itemVertSpacing > 0)
    {
        mainSizer->AddSpacer(m_itemVertSpacing);
    }
    // return right item sizer so the called can append to it
    if (wholeSizer)
    {
        *wholeSizer = itemSizer;
    }
    return rightItemSizer;
}

bool UISettingPanel::EnsureMainSizerExisted()
{
    if(GetSizer() == NULL)
    {
        UIBoxSizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (NULL == mainSizer)
        {
            return false;
        }
        SetSizer(mainSizer);
    }
    return true;
}

UITouchComplexButton* UISettingPanel::CreateButton(int btnId, const char* btnText, bool addAsChild)
{
    UITouchComplexButton* button = new UITouchComplexButton();
    button->SetId(btnId);
    button->SetText(btnText);
    button->SetFontSize(m_itemButtonFontSize);
    button->SetLeftMargin(m_buttonLeftMargin);
    button->SetTopMargin(m_buttonTopMargin);
    if (addAsChild)
    {
        AppendChild(button);
    }
    m_dynamicCreatedWindows.push_back(button);
    return button;
}

UITextSingleLine* UISettingPanel::CreateTextSingleLine(const char* text, bool addAsChild)
{
    UITextSingleLine* line = new UITextSingleLine();
    line->SetText(text);
    line->SetFontSize(m_itemButtonFontSize);
    if (addAsChild)
    {
        AppendChild(line);
    }

    m_dynamicCreatedWindows.push_back(line);
    return line;
}

UIBoxSizer* UISettingPanel::AddRadioButtonGroups(const char* itemTitle, int groupItemId, int itemButtonCount, int* itemButtonIds, const char** itemButtonTexts,  int initSelected)
{
    if (!EnsureMainSizerExisted())
    {
        return NULL;
    }
    UIBoxSizer* leftItemSizer = CreateItemTitleSizer(itemTitle);
    UIBoxSizer* rightItemSizer = CreateRightHalfItemSizer();
    UIRadioButtonGroup* radioGroup = new UIRadioButtonGroup();
    m_dynamicCreatedWindows.push_back(radioGroup);
    AppendChild(radioGroup);
    for (int i = 0; i < itemButtonCount; ++i)
    {
        UITouchComplexButton* btn = CreateButton(itemButtonIds[i], itemButtonTexts[i], false);
        if (i == initSelected)
        {
            btn->SetFocus(true);
        }
        radioGroup->AddButton(btn);
    }
    rightItemSizer->Add(radioGroup, UISizerFlags().Center());
    UIBoxSizer* itemSizer = new UIBoxSizer(dkHORIZONTAL);
    itemSizer->Add(leftItemSizer, UISizerFlags().Expand());
    itemSizer->Add(rightItemSizer, UISizerFlags().Expand());
    UISizer* mainSizer = GetSizer();
    mainSizer->Add(itemSizer);
    if (m_itemVertSpacing > 0)
    {
        mainSizer->AddSpacer(m_itemVertSpacing);
    }
    return rightItemSizer;
}

UIBoxSizer* UISettingPanel::AddArrowButton(const char* itemTitle, int itemBtnId, const char* btnText)
{
    UIBoxSizer* sizer = AddPushButton(itemTitle, itemBtnId, btnText);
    UITouchComplexButton* btn = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    btn->SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ARROW_BLACK), ImageManager::GetImage(IMAGE_TOUCH_ARROW_WHITE), UIButton::ICON_RIGHT);
    return sizer;
}

void UISettingPanel::MoveWindow(int left, int top, int width, int height)
{
    UIWindow::MoveWindow(left, top, width, height);
    Layout();
}

UIBoxSizer* UISettingPanel::AddLabel(const char* itemTitle, const char* text)
{
    if (!EnsureMainSizerExisted())
    {
        return NULL;
    }
    UISizer* mainSizer = GetSizer();
    UIBoxSizer* leftItemSizer = CreateItemTitleSizer(itemTitle);
    UITextSingleLine* line = CreateTextSingleLine(text, true);
    UIBoxSizer* rightItemSizer = CreateRightHalfItemSizer();
    rightItemSizer->Add(line, UISizerFlags().Center());
    UIBoxSizer* itemSizer = new UIBoxSizer(dkHORIZONTAL);
    itemSizer->Add(leftItemSizer, UISizerFlags().Expand());
    itemSizer->Add(rightItemSizer, UISizerFlags().Expand());
    mainSizer->Add(itemSizer);
    if (m_itemVertSpacing > 0)
    {
        mainSizer->AddSpacer(m_itemVertSpacing);
    }
    // return right item sizer so the called can append to it
    return rightItemSizer;
}

std::string UISettingPanel::GetDiskSpace() const
{
    SystemSettingInfo* systemSettingInfo = SystemSettingInfo::GetInstance();

    int allSize = systemSettingInfo->GetDiskSpace().GetTotalSize();
    int freeSize = systemSettingInfo->GetDiskSpace().GetFreeSize();
    int allG = 0;
    int allM = 0;
    int freeG = 0;
    int freeM = 0;
    char str[128] = {0};

    const int M_MASK = 0x3FF;
    allG =  allSize >> 10;
    allM = allSize & M_MASK;
    freeG = freeSize >> 10;
    freeM = freeSize & M_MASK;

    // 磁盘整体空间小于1G，则按M显示
    if(allG == 0)
    {
        snprintf(str, DK_DIM(str), "%s%dM/%s%dM", StringManager::GetPCSTRById(MEMORY_REMAIN), freeM , StringManager::GetPCSTRById(BOOK_TOTAL),allM);
    }
    else if(freeG == 0 ) 
    {
        snprintf(str, DK_DIM(str), "%s%dM/%s%d.%02dG", StringManager::GetPCSTRById(MEMORY_REMAIN), freeM , StringManager::GetPCSTRById(BOOK_TOTAL),allG, allM*100/1024);
    }
    else 
    {
        snprintf(str, DK_DIM(str), "%s%d.%02dG/%s%d.%02dG", StringManager::GetPCSTRById(MEMORY_REMAIN),freeG, freeM*100/1024, StringManager::GetPCSTRById(BOOK_TOTAL),allG, allM*100/1024);
    }
    return str;
}

std::string UISettingPanel::GetKindleSn() const
{
    return DeviceInfo::SplitSerialNo(DeviceInfo::GetSerialNo());
}

std::string UISettingPanel::GetKindleVersion() const
{
    return DeviceInfo::GetKindleVersion();
}

std::string UISettingPanel::GetKindleMac() const
{
    string kindleMac = SystemSettingInfo::GetInstance()->GetKindleMAC();
    if(kindleMac.empty())
    {
        kindleMac = CWifiBaseImpl::GetInstance()->FindMacAddr();
        SystemSettingInfo::GetInstance()->SetKindleMAC(kindleMac);
    }
    return kindleMac;
}

std::string UISettingPanel::GetVersion() const
{
    return SystemSettingInfo::GetInstance()->GetVersion().GetBuffer();
}

UIBoxSizer* UISettingPanel::AddItemTips(const char* text)
{
    if (!EnsureMainSizerExisted())
    {
        return NULL;
    }
    UITextSingleLine* line = CreateTextSingleLine(text, true);
    line->SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    UIBoxSizer* sizer = new UIBoxSizer(dkHORIZONTAL);
    sizer->AddSpacer(m_itemLeftMagin);
    sizer->Add(line, UISizerFlags().Center());
    UISizer* mainSizer = GetSizer();
    mainSizer->Add(sizer);
    mainSizer->AddSpacer(m_itemVertSpacing);
    return sizer;
}

int UISettingPanel::GetRepaintPage() const
{
    return m_systemSettingInfo->GetRepaintPage();
}
void UISettingPanel::SetRepaintPage(int page)
{
    m_systemSettingInfo->SetRepaintPage(page);
}

int UISettingPanel::GetProgressBarMode() const
{
    return m_systemSettingInfo->GetProgressBarMode();
}

void UISettingPanel::SetProgressBarMode(int mode)
{
    m_systemSettingInfo->SetProgressBarMode(mode);
}

int UISettingPanel::GetFontRender() const
{
    return m_systemSettingInfo->GetFontRender();
}

void UISettingPanel::SetFontRender(int render)
{
    m_systemSettingInfo->SetFontRender(render);
}

int UISettingPanel::GetTextConvertMode() const
{
    int mode = m_systemSettingInfo->GetFontStyle_FamiliarOrTraditional();
    (mode >= 0) || (mode = 0);
    (mode <= 1) || (mode = 1);
    return mode;
}

void UISettingPanel::SetTextConvertMode(int mode)
{
    (mode >= 0) || (mode = 0);
    (mode <= 1) || (mode = 1);
    m_systemSettingInfo->SetFontStyle_FamiliarOrTraditional(mode);
}

int UISettingPanel::GetEpubMobiMetaDataMode() const
{
    return m_systemSettingInfo->GetEpubMobiMetaDataMode();
}

void UISettingPanel::SetEpubMobiMetaDataMode(int mode)
{
    (mode >= 0) || (mode = 0);
    (mode <= 1) || (mode = 1);
    CDKFileManager::GetFileManager()->SetMetaData(1-mode);
    m_systemSettingInfo->SetEpubMobiMetaDataMode( mode);
}

void UISettingPanel::InitFonts()
{
    m_chineseFontNames.clear();
    m_englishFontNames.clear();
    DK_CHARSET_TYPE chineseCharSets[] = {DK_CHARSET_BIG5, DK_CHARSET_SHITJIS, DK_CHARSET_JOHAB, DK_CHARSET_GB};
    DK_CHARSET_TYPE englishCharSets[] = {DK_CHARSET_ANSI};
    GetFontNamesByCharsets(chineseCharSets, DK_DIM(chineseCharSets), &m_chineseFontNames);
    GetFontNamesByCharsets(englishCharSets, DK_DIM(englishCharSets), &m_englishFontNames);
    std::string curChineseFontName = SystemSettingInfo::GetInstance()->GetFontByCharset(DK_CHARSET_GB);
    m_selectedChineseFontIndex = FindFontName(m_chineseFontNames, curChineseFontName);
    if (m_selectedChineseFontIndex >= 0)
    {
        m_curChineseFontName = m_chineseFontNames[m_selectedChineseFontIndex];
    }

    std::string curEnglishFontName = SystemSettingInfo::GetInstance()->GetFontByCharset(DK_CHARSET_ANSI);
    m_selectedEnglishFontIndex = FindFontName(m_englishFontNames, curEnglishFontName);
    if (m_selectedEnglishFontIndex >= 0)
    {
        m_curEnglishFontName = curEnglishFontName;
    }
}


void UISettingPanel::GetFontNamesByCharset(DK_CHARSET_TYPE charSet, std::vector<std::string>* fontNames)
{
    std::vector<DKFont*> fonts;
    g_pFontManager->GetFontListByCharSet(charSet, fonts);
    for (DK_AUTO(begin, fonts.begin()); begin != fonts.end(); ++begin)
    {
        std::string chineseFontName = (*begin)->GetChineseFontName();
        if (!chineseFontName.empty())
        {
            fontNames->push_back((*begin)->GetChineseFontName());
        }
    }
    std::sort(fontNames->begin(), fontNames->end());
    fontNames->erase(std::unique(fontNames->begin(), fontNames->end()),
            fontNames->end());
}

void UISettingPanel::GetFontNamesByCharsets(DK_CHARSET_TYPE* charSets, size_t size, std::vector<std::string>* fontNames)
{
    for (size_t i = 0; i < size; ++i)
    {
        GetFontNamesByCharset(charSets[i], fontNames);
    }
}

int UISettingPanel::FindFontName(const std::vector<std::string>& fontNames, const std::string& curFontName)
{
    DK_AUTO(pos, std::find(fontNames.begin(), fontNames.end(), curFontName));
    if (pos != fontNames.end())
    {
        return pos - fontNames.begin();
    }
    else
    {
        return fontNames.empty() ? -1:0;
    }
}

int UISettingPanel::GetFontSizeIndex() const
{
    return LayoutHelper::GetFontSizeIndex();
}

int UISettingPanel::GetFontSizeCount() const
{
    return LayoutHelper::GetFontSizeTableSize();
}

void UISettingPanel::SetFontSizeIndex(int index)
{
    if (index < 0 || index >= GetFontSizeCount())
    {
        return;
    }
    m_systemSettingInfo->SetFontSize(LayoutHelper::GetFontSizeTable()[index]);
}

int UISettingPanel::GetLayoutMode() const
{
    return m_systemSettingInfo->GetTypeSetting();
}

int UISettingPanel::GetReadingLayout() const
{
    return m_systemSettingInfo->GetReadingLayout();
}

void UISettingPanel::SetReadingLayout(int mode)
{
    m_systemSettingInfo->SetReadingLayout(mode);
}

int UISettingPanel::GetTimeSystem() const
{
    return m_systemSettingInfo->m_Time.GetTimeSystem();
}

void UISettingPanel::SetTimeSystem(int mode)
{
    m_systemSettingInfo->m_Time.SetTimeSystem(mode);
}

int UISettingPanel::GetTimeZoneEast() const
{
    return m_systemSettingInfo->m_Time.GetTimeZoneEast();
}

void UISettingPanel::SetTimeZoneEast(int mode)
{
    m_systemSettingInfo->m_Time.SetTimeZoneEast(mode);
}

int UISettingPanel::GetTimeZoneFull() const
{
    return m_systemSettingInfo->m_Time.GetTimeZoneFull();
}

void UISettingPanel::SetTimeZoneFull(int mode)
{
    m_systemSettingInfo->m_Time.SetTimeZoneFull(mode);
}

int UISettingPanel::GetTimeZoneHalf() const
{
    return m_systemSettingInfo->m_Time.GetTimeZoneHalf();
}

void UISettingPanel::SetTimeZoneHalf(int mode)
{
    m_systemSettingInfo->m_Time.SetTimeZoneHalf(mode);
}


void UISettingPanel::SetLayoutMode(int mode)
{
    mode = MakeInRange(mode, 0, 3);
    m_systemSettingInfo->SetTypeSetting(mode);
    UINT uLineSpacing = 0;
    UINT uParaSpacing =0;
    UINT uIndent = 0;
    UINT uTopMargin = 0;
    UINT uLeftMargin = 0;
    LayoutHelper::LayoutParaFromStyle((LayoutStyle)mode, &uLineSpacing, &uParaSpacing, &uIndent, &uTopMargin, &uLeftMargin);
    m_systemSettingInfo->SetLineGap(uLineSpacing);
    m_systemSettingInfo->SetParaSpacing(uParaSpacing);
    m_systemSettingInfo->SetBookIndent(uIndent);
}

bool UISettingPanel::IsWifiOn() const
{
    return WifiManager::GetInstance()->IsPowerOn();
}
bool UISettingPanel::IsWifiConnected() const
{
    return WifiManager::GetInstance()->IsConnected();
}

std::string UISettingPanel::GetApName() const
{
    AccessPoint* pAP = CWifiBaseImpl::GetInstance()->GetCurAP();
    if (NULL != pAP)
    {
        return pAP->SSID;
    }
    return "";
}


void UISettingPanel::InitIME()
{
    if (m_imeNames.empty())
    {
        const SOURCESTRINGID inputMethodNameIds[] = {JIANTI_PINYIN, WUBI, FANTI_PINYIN};
        int option = m_systemSettingInfo->m_IME.GetOptionIME();
        for (size_t i = 0; i < DK_DIM(inputMethodNameIds); ++i)
        {
            m_imeNames.push_back(StringManager::GetPCSTRById(inputMethodNameIds[i]));
            if (option & (1 << (i + 1)))
            {
                m_selectedIMEIndex = i;
            }
        }
    }
    if (-1 == m_selectedIMEIndex)
    {
        m_selectedIMEIndex = 0;
    }
}

void UISettingPanel::SetSelectedIME(int imeSelected)
{
    if (imeSelected >= 0 && imeSelected < (int)m_imeNames.size())
    {
        for(size_t i = 0; i < m_imeNames.size(); i++)
        {
            m_systemSettingInfo->m_IME.SetOptionIME(i + 1, (imeSelected == (int)i));
        }
    }
}

void UISettingPanel::InitLanguages()
{
    if (!m_languageNames.empty())
    {
        return;
    }
    m_languageNames = m_systemSettingInfo->GetAvailableLanguages();
    std::string curLang = m_systemSettingInfo->GetLanguage();
    DK_AUTO(pos, std::find(m_languageNames.begin(), m_languageNames.end(), curLang));
    if (pos != m_languageNames.end())
    {
        m_selectedLanguage = pos - m_languageNames.begin();
    }
    else
    {
        m_selectedLanguage = 0;
    }
}

void UISettingPanel::SelectLanguage(int index)
{
    m_selectedLanguage = index;
    std::string lang = m_languageNames[index];
    m_systemSettingInfo->SetLanguage(lang.c_str()); 
    if (0 == index)
    {
        StringManager::ReloadLang();
    }
    else
    {
        StringManager::SetLanguage(lang);
    }
}
void UISettingPanel::InitLayout()
{
    if (!m_layoutNames.empty())
    {
        return;
    }
    const char* layoutTexts[] = {
        StringManager::GetPCSTRById(LAYOUT_ELEGANT),
        StringManager::GetPCSTRById(LAYOUT_STANDARD),
        StringManager::GetPCSTRById(LAYOUT_COMPACT),
        StringManager::GetPCSTRById(LAYOUT_ORIGINAL)};
    m_layoutNames.insert(m_layoutNames.end(), layoutTexts, DK_ARRAY_END(layoutTexts));
}
