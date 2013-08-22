#include "TouchAppUI/UICommentOrSummarySelectDlg.h"
#include "CommonUI/UIIMEManager.h"
#include "CommonUI/UIUtility.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIButton.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UIDialog.h"
#include "GUI/UIImage.h"
#include "GUI/UIProgressBar.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Model/WifiItemModelImpl.h"
#include "singleton.h"
#include "Wifi/WifiBase.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>
#include <vector>
#include <string>
#include "Utility/StringUtil.h"
#include "DkStreamFactory.h"
#include "IDKESection.h"
#include "IDKEPage.h"
#include "DKEAPI.h"

using dk::utility::StringUtil;
using dk::utility::EncodeUtil;
using namespace WindowsMetrics;

/*****************************************************
* UICommentOrSummary
*****************************************************/

SINGLETON_CPP(UICommentOrSummary);

UICommentOrSummary::UICommentOrSummary()
    : UIContainer(NULL,IDSTATIC)
    ,m_bIsShowDictionary(false)
    ,m_bHasComment(false)
    ,m_bHasDigest(false)
    ,m_bIsLink(false)
    ,m_bIsDuokanBook(false)
    ,m_bIsForVerticalForms(false)
    ,m_inGallery(false)
    ,m_szSimpleWord("")
{
    Dictionary::InitDict();
    SetVisible(false);
    SetPropogateTouch(false);
    HookTouch();
}

UICommentOrSummary::~UICommentOrSummary()
{
}

void UICommentOrSummary::SetShowStatusAndText(bool hasComment, bool hasDigest, const char* pWord)
{
	ClearAllChild(false);     //每次MoveWindow时都会重新AppendChild，所以在此ClearAllChild
	m_bHasComment = hasComment;
	m_bHasDigest = hasDigest;
	m_szSimpleWord = (string)pWord;
	string strMean = GetTranslateMean(m_szSimpleWord);
	strMean = StringUtil::ReplaceString(strMean, '\n', " ");
	strMean = StringUtil::ReplaceString(strMean, '\r', " ");
	m_bIsShowDictionary = !strMean.empty();
	m_txtWordMean.SetText(strMean.c_str());
}

string UICommentOrSummary::GetTranslateMean(string& outPutWord)
{
	string strMean = "";
	vector<Dict_Info> vec_dict_info = Dictionary::GetDictInfo();
	int iDictCount = Dictionary::GetDictCount();
	if(!outPutWord.empty() && iDictCount > 0)
	{
		const char* pSimpleWord = outPutWord.c_str();
		for(int i = 0; i < iDictCount; i++)
		{
			string strTmpMean = "";
			if(*pSimpleWord < 0x20 || *pSimpleWord > 0x80)
			{
				strTmpMean = Dictionary::Translate(outPutWord.c_str(), i);
			}
			else
			{
				strTmpMean = Dictionary::TranslateForFuzzyMatch(outPutWord.c_str(), i, &outPutWord);
			}

			if(vec_dict_info[i].dictFormart == DICT_HTML)
			{
				strTmpMean = GetTextFromHtml(strTmpMean);
			}
			strMean += strTmpMean;
			if(!strMean.empty() && strMean.size() > 400)
			{
				break;
			}
		}
	}
	return strMean;
}

string UICommentOrSummary::GetTextFromHtml(const string& strMean)
{
	DKE_SHOWCONTENT_HANDLE contentHandle;
	IDkStream* pDemoStream = DkStreamFactory::GetMemoryStream((DK_VOID*)strMean.c_str(), strMean.length(), strMean.length());
	contentHandle = DKE_OpenHTMLFragment(pDemoStream, L"", DKE_HTMLSTYLE_STANDARD, DKE_SHOWCONTENT_FIXEDPAGE);
	IDKESection* pSection = contentHandle.pSection;
	string strTranslate = "";
	if(pSection)
	{
		pSection->SetFontSize(GetWindowFontSize(FontSize20Index));
		pSection->SetParaSpacing(1.2);
		pSection->SetTextColor(DK_ARGBCOLOR(255, 0, 0, 0));
		DK_BOX pageBox(0, 0, GetWindowMetrics(UICommentOrSummaryImageWidthIndex), GetWindowMetrics(UICommentOrSummaryImageHeihtIndex));
		pSection->SetPageBox(pageBox);
		pSection->ParseSection(false);
		strTranslate = EncodeUtil::ToString(pSection->GetSectionText());
	}
	DKE_CloseHTMLFragment(DKE_SHOWCONTENT_FIXEDPAGE, contentHandle);
	SafeDeletePointer(pDemoStream);
	return strTranslate;
}

void UICommentOrSummary::SetIsLink(bool _bIsLink)
{
	m_bIsLink = _bIsLink;
}

void UICommentOrSummary::SetIsDuokanBook(bool isDuokanBook)
{
    m_bIsDuokanBook = isDuokanBook;
}

void UICommentOrSummary::SetInGallery(bool inGallery)
{
    m_inGallery = inGallery;
}

void UICommentOrSummary::InitUI()
{
    const int elemSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);
    const int fontSizeTitle = GetWindowFontSize(FontSize14Index);
    const int fontSizeBtn = GetWindowFontSize(FontSize16Index);
    int btnHeight = GetWindowMetrics(UICommentOrSummaryBtnHeightIndex);
    const int borderLine = GetWindowMetrics(UIDialogBorderLineIndex);
    const int btnInternalMargin = GetWindowMetrics(UIPixelValue20Index);

    UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* pBtnLstSizer = NULL;
    dkDirection directionNext = dkRIGHT;
    int proportion = 1;
    if (IsDirectionVertical())
    {
        proportion = 0;
        directionNext = dkBOTTOM;
        btnHeight = GetWindowMetrics(UIPixelValue50Index);
        pBtnLstSizer = new UIBoxSizer(dkVERTICAL);
        m_btnLeft.SetLeftMargin(btnInternalMargin);
        m_btnRight.SetLeftMargin(btnInternalMargin);
        m_btnShare.SetLeftMargin(btnInternalMargin);
        m_btnJumpToLink.SetLeftMargin(btnInternalMargin);
        m_btnCorrection.SetLeftMargin(btnInternalMargin);
        m_btnWebSearch.SetLeftMargin(btnInternalMargin);
    }
    else
    {
        proportion = 1;
        directionNext = dkRIGHT;
        pBtnLstSizer = new UIBoxSizer(dkHORIZONTAL);
    }
    if ((NULL == pMainSizer) || (NULL == pBtnLstSizer))
    {
        SafeDeletePointer(pMainSizer);
        SafeDeletePointer(pBtnLstSizer);
        return;
    }

    m_btnTranslateOrMore.Initialize(ID_BTN_TOUCH_MORE_DICTIONARY, NULL, fontSizeBtn);
    AppendChild(&m_btnTranslateOrMore);
    m_btnTranslateOrMore.SetMinHeight(btnHeight);

    if (!m_inGallery)
    {
        DWORD leftID = IDSTATIC, rightID = IDSTATIC ;
        SOURCESTRINGID leftStrID = STRING_COUNT, rightStrID = STRING_COUNT;

        if (m_bHasDigest)
        {
            leftID = ID_BTN_TOUCH_DELETE_DIGEST;
            leftStrID = TOUCH_DELETE_DIGEST;
        }
        else
        {
            leftID = ID_BTN_TOUCH_ADD_DIGEST;
            leftStrID = TOUCH_ADD_DIGEST;
        }

        if (m_bHasComment)
        {
            leftID = ID_BTN_TOUCH_EDIT_COMMENT;
            leftStrID = TOUCH_EDIT_COMMENT;
            rightID = ID_BTN_TOUCH_DELETE_COMMENT;
            rightStrID = TOUCH_DELETE_COMMENT;
        }
        else
        {
            rightID = ID_BTN_TOUCH_ADD_COMMENT;
            rightStrID = TOUCH_COMMENT;
        }

        m_btnLeft.Initialize(leftID, StringManager::GetPCSTRById(leftStrID), fontSizeBtn);
        m_btnRight.Initialize(rightID, StringManager::GetPCSTRById(rightStrID), fontSizeBtn);

        AppendChild(&m_btnLeft);
        AppendChild(&m_btnRight);

        m_btnLeft.SetMinHeight(btnHeight);
        m_btnRight.SetMinHeight(btnHeight);

        pBtnLstSizer->Add(&m_btnLeft, UISizerFlags(proportion).Expand().Border(directionNext , elemSpacing));
        pBtnLstSizer->Add(&m_btnRight, UISizerFlags(proportion).Expand().Border(directionNext, elemSpacing));
    }

    if (m_bIsShowDictionary)
    {
        UISizer* pTopSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pTopSizer)
        {
            m_txtTitle.SetText(m_szSimpleWord.c_str());
            m_txtTitle.SetFontSize(fontSizeTitle);
            m_txtTitle.SetBackColor(ColorManager::GetColor(COLOR_TITLE_BACKGROUND));
            AppendChild(&m_txtTitle);

            pTopSizer->Add(&m_txtTitle, UISizerFlags(1).Align(dkALIGN_CENTRE_VERTICAL).Border(dkRIGHT, elemSpacing));

            SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_TOUCH_BTN_CLOSE);
            if (spImg)
            {
                m_btnCancelIcon.SetIcon(spImg, UIButton::ICON_TOP);
                m_btnCancelIcon.ShowBorder(false);
                AppendChild(&m_btnCancelIcon);
                pTopSizer->Add(&m_btnCancelIcon, UISizerFlags().Border(dkTOP | dkBOTTOM, borderLine).Align(dkALIGN_CENTRE_VERTICAL));
            }

            pMainSizer->Add(pTopSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, elemSpacing));
        }
        m_txtWordMean.SetFontSize(GetWindowFontSize(FontSize20Index));
        m_txtWordMean.SetAlign(ALIGN_JUSTIFY);
        AppendChild(&m_txtWordMean);
        pMainSizer->Add(&m_txtWordMean, UISizerFlags(1).Expand().Border(dkALL, elemSpacing << 1));

        m_btnTranslateOrMore.SetText(StringManager::GetStringById(DICT_SEE_MORE));
        pMainSizer->Add(&m_btnTranslateOrMore, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, elemSpacing));
    }
    else
    {
        m_btnTranslateOrMore.SetText(StringManager::GetStringById(TOUCH_TRANSLATE));
        pBtnLstSizer->Add(&m_btnTranslateOrMore, UISizerFlags(proportion).Expand().Border(directionNext, elemSpacing));
    }

    if(m_bIsLink)
    {
        m_btnJumpToLink.Initialize(ID_BTN_TOUCH_COMMENT_LINK, StringManager::GetStringById(TOUCH_COMMENT_LINK), fontSizeBtn);
        m_btnJumpToLink.SetMinHeight(btnHeight);
        AppendChild(&m_btnJumpToLink);
        pBtnLstSizer->Add(&m_btnJumpToLink, UISizerFlags(proportion).Expand().Border(directionNext, elemSpacing));
    }

    if (m_bIsDuokanBook)
    {
        m_btnCorrection.Initialize(ID_BTN_BOOK_CORRECTION, StringManager::GetPCSTRById(BOOKCORRECTION), fontSizeBtn);
        m_btnCorrection.SetMinHeight(btnHeight);
        AppendChild(&m_btnCorrection);
        pBtnLstSizer->Add(&m_btnCorrection, UISizerFlags(proportion).Expand().Border(directionNext, elemSpacing));
    }

    m_btnShare.Initialize(ID_BTN_SINAWEIBO_SHARE, StringManager::GetPCSTRById(SHARE), fontSizeBtn);
    m_btnShare.SetMinHeight(btnHeight);
    AppendChild(&m_btnShare);
    pBtnLstSizer->Add(&m_btnShare, UISizerFlags(proportion).Expand().Border(directionNext, elemSpacing));

    m_btnWebSearch.Initialize(ID_BTN_WEB_SEARCH, StringManager::GetPCSTRById(WEB_SEARCH), fontSizeBtn);
    m_btnWebSearch.SetMinHeight(btnHeight);
    AppendChild(&m_btnWebSearch);
    pBtnLstSizer->Add(&m_btnWebSearch, UISizerFlags(proportion).Expand().Border(directionNext, elemSpacing));

    if (IsDirectionVertical())
    {
        pMainSizer->Add(pBtnLstSizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, elemSpacing).Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIPixelValue20Index)));
    }
    else
    {
        pMainSizer->Add(pBtnLstSizer, UISizerFlags().Expand().Border(dkALL, elemSpacing));
    }

    SetSizer(pMainSizer);
}

HRESULT UICommentOrSummary::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
        return S_OK;
    
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
        
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    CropImage(drawingImg, rcSelf, &imgSelf);
    CTpGraphics grp(imgSelf);


    UISizerItem* pTitle = m_windowSizer->GetItem((size_t)0);
    int titleHeight = (m_bIsShowDictionary && pTitle) ? pTitle->GetSize().GetHeight() : 0;
    RTN_HR_IF_FAILED(UIUtility::DrawBorder(imgSelf, 0, 0, 0, 0, titleHeight));
    
    if(m_bIsShowDictionary)
    {
        //画包含词典内容的小框
        UISizerItem* pWordMean = m_windowSizer->GetItem((size_t)1);
        if (pWordMean)
        {
            DK_IMAGE imgWordMean;
            const int elemSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);
            const dkRect rtWordMean = pWordMean->GetRect();
            const int top = m_iTop + rtWordMean.GetY() - (elemSpacing << 1);
            DK_RECT rcWordMean = {m_iLeft, top, m_iLeft + m_iWidth, 
                top + rtWordMean.GetHeight() + (elemSpacing << 2)};
            CropImage(drawingImg, rcWordMean, &imgWordMean);
            RTN_HR_IF_FAILED(UIUtility::DrawBorder(imgWordMean, elemSpacing, elemSpacing, elemSpacing, elemSpacing, 0));
        }
    }
    
    int iSize = GetChildrenCount();
    for (int i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin)
        {
            pWin->Draw(imgSelf);
        }
    }
    return hr;
}

dkSize UICommentOrSummary::GetMinSize() const
{
    int minWidth  = m_minWidth;
    int minHeight = m_minHeight;

    if (m_windowSizer && (minWidth == dkDefaultCoord || minHeight == dkDefaultCoord))
    {
        dkSize minSize = m_windowSizer->GetMinSize();
        minWidth = minSize.GetWidth();
        minHeight = m_bIsShowDictionary ? GetWindowMetrics(UICommentOrSummaryMaxHeightIndex) : minSize.GetHeight();
    }
    return dkSize(minWidth, minHeight);
}

void UICommentOrSummary::SetOffsetXY(int offsetX, int offsetY, int maxX, int maxY)
{
    const int horizontalMargin = GetWindowMetrics(UIHorizonMarginIndex);
    const int maxHeight = GetWindowMetrics(UICommentOrSummaryMaxHeightIndex);
    const int btnHeight = GetWindowMetrics(UICommentOrSummaryBtnHeightIndex);
    const int elemSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);
    const int offset = GetWindowMetrics(UIPixelValue50Index);
    int height = maxHeight;
    int top = offsetY;
    if (!m_bIsShowDictionary)
    {
        height = (elemSpacing << 1) + btnHeight;
    }

    if (IsDirectionVertical() && m_windowSizer)
    {
        dkSize minSize = m_windowSizer->GetMinSize();
        int left = offsetX;
        top = (top + minSize.GetHeight()) > maxY ? maxY - minSize.GetHeight() - offset : top;
        UIWindow::MoveWindow(left, top, minSize.GetWidth(), minSize.GetHeight());
    }
    else
    {
        UIWindow::MoveWindow(horizontalMargin, top, maxX - (horizontalMargin << 1), height);
    }
    Layout();
    UpdateWindow();
}

void UICommentOrSummary::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{
    if (m_pParent)
    {
        if (ID_BTN_TOUCH_MORE_DICTIONARY == _dwCmdId || ID_BTN_BOOK_CORRECTION == _dwCmdId)
        {
            // 可能弹出字典框，临时unhook
            UnhookTouch();
        }
        m_pParent->OnCommand(_dwCmdId, _pSender, _dwParam);
        if (ID_BTN_TOUCH_MORE_DICTIONARY == _dwCmdId || ID_BTN_BOOK_CORRECTION == _dwCmdId)
        {
            HookTouch();
        }
    }
}

bool UICommentOrSummary::SetVisible(bool _bVisible)
{
    if (m_bIsVisible == _bVisible)
    {
        return true;
    }
    UIWindow::SetVisible(_bVisible);
    if (m_bIsVisible)
    {
        InitUI();
    }
    else
    {
        if (m_pParent)
        {
            // must save parent, after remove child, m_pParent will be NULL
            UIContainer* parent = m_pParent;
            parent->RemoveChild(this, false);
            //parent->UpdateWindow();
        }
    }
    
    return true;
}

string UICommentOrSummary::GetWords()
{
	return m_szSimpleWord;
}

bool UICommentOrSummary::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
    {
        return false;
    }
    if (IsVisible() && !PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
    {
        SetVisible(false);
        return false;
    }
    return false;
}

