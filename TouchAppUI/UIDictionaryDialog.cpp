///////////////////////////////////////////////////////////////////////
// UIDictionaryDialog.cpp
// Contact:wang mingyin
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIDictionaryDialog.h"
#include "TouchAppUI/UIDictionarySetDialog.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Framework/CDisplay.h"
#include "Framework/CHourglass.h"
#include "Utility.h"
#include "KernelEncoding.h"
#include "Common/WindowsMetrics.h"
#include "DkStreamFactory.h"
#include "DKEEPUBDef.h"
#include "CommonUI/UIIMEManager.h"

using namespace DkFormat;
using namespace WindowsMetrics;

UIDictionaryDialog::UIDictionaryDialog(UIContainer* pParent, const char* pWord)
    : UIDialog(pParent)
    , m_btnBack(this, ID_BTN_TOUCH_BACK)
    , m_strWord(pWord)
    , m_iCurPageIndex(0)
    , m_iCurDictIndex(0)
{
	Init();
}

UIDictionaryDialog::UIDictionaryDialog(UIContainer* pParent)
    : UIDialog(pParent)
    , m_btnBack(this, ID_BTN_TOUCH_BACK)
	, m_strWord("")
    , m_iCurPageIndex(0)
    , m_iCurDictIndex(0)
{
	Init();
}

UIDictionaryDialog::~UIDictionaryDialog()
{
	Clear();
	Dispose();
}

void UIDictionaryDialog::Init()
{
    m_gestureListener.SetDictionDialog(this);
    m_gestureDetector.SetListener(&m_gestureListener);
    AppendChild(&m_titleBar);
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    const int horizontalMargin = GetWindowMetrics(UIHorizonMarginIndex);
	const int translateMeanWidth = CDisplay::GetDisplay()->GetScreenWidth() - 2*horizontalMargin;
	if(!m_strWord.empty())
	{
		m_txtLookupWord.SetTextUTF8(m_strWord.c_str());
	}
    m_txtLookupWord.SetFontSize(fontsize18); 
    AppendChild(&m_txtLookupWord);

	CDisplay* pDisplay = CDisplay::GetDisplay();
    if (NULL != pDisplay)
    {
        m_txtTranslate.SetMaxWidth(pDisplay->GetScreenWidth() - (horizontalMargin << 1));
    }
    m_txtTranslate.SetFontSize(GetWindowFontSize(FontSize22Index));
    m_txtTranslate.SetEnable(FALSE);
    m_txtTranslate.SetAlign(ALIGN_JUSTIFY);
	m_txtTranslate.SetVAlign(VALIGN_TOP);
    m_txtTranslate.SetLineSpace(1);
    AppendChild(&m_txtTranslate);

    m_txtDictName.SetFontSize(fontsize18);
    m_txtDictName.SetAlign(ALIGN_LEFT);
	m_txtDictName.SetMinWidth(translateMeanWidth*0.75);
    AppendChild(&m_txtDictName);

    m_txtPageNo.SetFontSize(fontsize18);
	m_txtPageNo.SetEnglishGothic();
    m_txtPageNo.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_txtPageNo);

    if (NULL == m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pTopSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pBottomSizer = new UIBoxSizer(dkHORIZONTAL);
        if ((NULL != pMainSizer) && (NULL != pTopSizer) && (NULL != pBottomSizer))
        {
            const int elementSpacing = GetWindowMetrics(UIElementSpacingIndex);
            pMainSizer->Add(&m_titleBar);
            m_txtPageNo.SetMinWidth(translateMeanWidth*0.25);
            pTopSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            pTopSizer->Add(&m_txtLookupWord, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT | dkTOP | dkBOTTOM, elementSpacing));
            pTopSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));

            pBottomSizer->Add(&m_txtDictName, UISizerFlags().Expand().Align(dkALIGN_LEFT));
			pBottomSizer->AddStretchSpacer();
            pBottomSizer->Add(&m_txtPageNo, UISizerFlags().Expand().Align(dkALIGN_RIGHT).Border(dkLEFT, elementSpacing));
			pBottomSizer->SetMinHeight(GetWindowMetrics(UINormalBtnHeightIndex));

            pMainSizer->Add(pTopSizer, UISizerFlags().Expand().Border(dkRIGHT | dkBOTTOM, horizontalMargin));
            pMainSizer->Add(&m_txtTranslate, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));
            pMainSizer->Add(pBottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT | dkBOTTOM, horizontalMargin));

            SetSizer(pMainSizer);

            m_txtLookupWord.SetTextBoxStyle(ENGLISH_NORMAL);
            m_txtLookupWord.SelectAll(TRUE);
            if (m_strWord.empty())
            {
                UIIME* pIME = UIIMEManager::GetIME(m_txtLookupWord.GetInputMode(), &m_txtLookupWord);
                if (pIME)
                {
                    pIME->SetShowDelay(true);
                }
            }
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(pTopSizer);
            SafeDeletePointer(pBottomSizer);
        }
    }
}

void UIDictionaryDialog::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
	switch(dwCmdId)
	{
	case IDSTATIC:
		{
			TranslateAndDisplay(); 
#ifndef KINDLE_FOR_TOUCH
			if(UIIM::GetInstance()->IsShow())
			{
				m_txtLookupWord.SetFocus(true);
				UIDialog::OnKeyPress(KEY_FONT);
				m_txtLookupWord.SetFocus(false);
			}
#endif
		}
	break;
	case ID_BTN_TOUCH_BACK:
		{
			EndDialog(1);
		}
		break;
	default:
		break;
	}
}

void UIDictionaryDialog::Clear()
{
	//进行清空操作
	m_strWord = "";
	m_iTotalPageNum = 0;
	m_iCurPageIndex = 0;
	m_iCurDictIndex = 0;
	m_vDictInfo.clear();
}

//应该从窗口获得单词，然后进行翻译
void UIDictionaryDialog::TranslateAndDisplay()
{
	Clear();
	m_strWord = m_txtLookupWord.GetTextUTF8();
	if(!m_strWord.empty())
	{
		int iDictCount = Dictionary::GetDictCount();
		if(iDictCount > 0)
		{	
			vector<Dict_Info> vec_dict_info = Dictionary::GetDictInfo();
			for(int i = 0; i < iDictCount; i++)
			{
				string strTmpMean = Dictionary::Translate(m_strWord.c_str(), i);
				if(!strTmpMean.empty())
				{
					if(vec_dict_info[i].dictFormart == DICT_HTML)
					{
						strTmpMean = GetTextFromHtml(strTmpMean);
					}
					DictInfo dict;
					dict.dictName = vec_dict_info[i].dictname;
					dict.dictMean = strTmpMean;
					m_txtTranslate.SetText(strTmpMean.c_str());
					int totalPage = m_txtTranslate.GetPageCount();
					dict.dictPageNum = totalPage;
					m_iTotalPageNum += totalPage;
					m_vDictInfo.push_back(dict);
				}
			}
			if(m_vDictInfo.empty())
			{
				m_txtTranslate.SetText(StringManager::GetPCSTRById(DICT_NO_RESULT));
			}
			else
			{
				m_txtTranslate.SetText(m_vDictInfo[0].dictMean.c_str());
			}
		}
		else
		{
			m_txtTranslate.SetText(StringManager::GetPCSTRById(NO_INSTALL_DICT_INFO));
		}
	}
	else
	{
		m_txtTranslate.SetText(StringManager::GetPCSTRById(DICT_TIPS));
	}
	m_txtLookupWord.SelectAll();
	UpdatePageNoBtn();
}

string UIDictionaryDialog::GetTextFromHtml(const string& strMean)
{
	string strConvert = "";
	DKE_SHOWCONTENT_HANDLE contentHandle;
	IDkStream* pDemoStream = DkStreamFactory::GetMemoryStream((DK_VOID*)strMean.c_str(), strMean.length(), strMean.length());
	contentHandle = DKE_OpenHTMLFragment(pDemoStream, L"", DKE_HTMLSTYLE_STANDARD, DKE_SHOWCONTENT_FIXEDPAGE);
	IDKESection* pSection = contentHandle.pSection;
	if(pSection)
	{
		pSection->SetFontSize(GetWindowFontSize(FontSize20Index));
		pSection->SetParaSpacing(1.2);
		pSection->SetTextColor(DK_ARGBCOLOR(255, 0, 0, 0));
		DK_BOX pageBox(0, 0, CDisplay::GetDisplay()->GetScreenWidth() - 2*GetWindowMetrics(UIHorizonMarginIndex)
			, GetWindowMetrics(UIDictionaryDialogTranslateImageHeightIndex));
		pSection->SetPageBox(pageBox);
		pSection->ParseSection(false);
		strConvert = EncodeUtil::ToString(pSection->GetSectionText());
	}
	DKE_CloseHTMLFragment(DKE_SHOWCONTENT_FIXEDPAGE, contentHandle);
	SafeDeletePointer(pDemoStream);
	return strConvert;
}

void UIDictionaryDialog::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    UIWindow::MoveWindow(left, top, width, height);
    Layout();
	TranslateAndDisplay();
}

void UIDictionaryDialog::Dispose()
{
	UIDialog::Dispose ();
} 

void UIDictionaryDialog::UpdatePageNoBtn()
{
	string strDictName = "";
	if(!m_vDictInfo.empty())
	{
		if(m_iCurPageIndex + 1 > m_vDictInfo[m_iCurDictIndex].dictPageNum)
		{
			m_iCurPageIndex = 0;
			m_iCurDictIndex++;
			if(m_iCurDictIndex + 1 > (int)m_vDictInfo.size())
			{
				m_iCurDictIndex = 0;
			}
			m_txtTranslate.SetText(m_vDictInfo[m_iCurDictIndex].dictMean.c_str());
		}
		else if(m_iCurPageIndex < 0)
		{
			m_iCurDictIndex--;
			m_iCurDictIndex = m_iCurDictIndex < 0 ? 0 : m_iCurDictIndex;
			m_iCurPageIndex = m_vDictInfo[m_iCurDictIndex].dictPageNum - 1;
			m_txtTranslate.SetText(m_vDictInfo[m_iCurDictIndex].dictMean.c_str());
		}
		strDictName = StringManager::GetPCSTRById(LEFT_TITLE_NUMBER) + m_vDictInfo[m_iCurDictIndex].dictName + StringManager::GetPCSTRById(RIGHT_TITLE_NUMBER);
	}
	else
	{
		m_iTotalPageNum = 1;
		m_iCurPageIndex = 0;
	}
	m_txtTranslate.SetDrawPageNo(m_iCurPageIndex);

	char str[128] = {0};
	int curPageIndex = m_iCurPageIndex;
	for(int i = 0; i < m_iCurDictIndex; i++)
	{
		curPageIndex += m_vDictInfo[i].dictPageNum;
	}
	
	snprintf(str, sizeof(str),"%d/%d %s", curPageIndex + 1, m_iTotalPageNum, StringManager::GetPCSTRById(BOOK_PAGE));
	m_txtPageNo.SetText(str);
	m_txtDictName.SetText(strDictName.c_str());
	UpdateWindow();
}

void UIDictionaryDialog::HandleDlgPageUpDown(bool bPageDown)
{
	bPageDown ? ++m_iCurPageIndex : --m_iCurPageIndex;
	UpdatePageNoBtn();
}

bool UIDictionaryDialog::OnTouchEvent(MoveEvent* moveEvent)
{
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}

bool UIDictionaryDialog::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    switch (direction)
    {
        case FD_UP:
        case FD_LEFT:
            HandleDlgPageUpDown(true);
            break;
        case FD_DOWN:
        case FD_RIGHT:
            HandleDlgPageUpDown(false);
            break;
        default:
            break;
    }
    return true;
}

bool UIDictionaryDialog::OnSingleTapUp(MoveEvent* moveEvent)
{
	double widthPercent  = 0.5;
    int x = moveEvent->GetX();

	if (x>=0 && x<= m_iWidth*widthPercent)
	{
		HandleDlgPageUpDown(false);
	}
	else
	{
		HandleDlgPageUpDown(true);
	}
    return true;
}
