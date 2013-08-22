#include "TouchAppUI/LayoutReader.h"
#include "BookReader/TextBookReader.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"

const float	   LayoutReader::s_emboldenTable[] = {1.0f, 1.5f, 2.0f};

//////////////////   LayoutReader  ///////////////////////////////////////
LayoutReader::LayoutReader(UIReaderSettingDlg* pReaderSettingDlg) :
	  m_pDemoReader(NULL)
	, m_pReaderSettingdlg(pReaderSettingDlg)
{
}

LayoutReader::~LayoutReader()
{
	ClearReader();
}

void LayoutReader::ClearReader()
{
	if(m_pDemoReader)
	{
		m_pDemoReader->CloseDocument();
		SafeDeletePointer(m_pDemoReader);
	}
}

void LayoutReader::InitReader()
{
	ClearReader();
	m_pDemoReader = new TextBookReader();
	if (m_pDemoReader)
	{
		IBMPProcessor* pProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
		if (NULL == pProcessor)
		{
			return;
		}
		m_pDemoReader->AddBMPProcessor(pProcessor);
		m_pDemoReader->SetFontChange(true);
		string strDemoTxt = StringManager::GetDemoString();
		m_pDemoReader->OpenDocumentFromString(strDemoTxt);
	}
	return;
}

DK_IMAGE* LayoutReader::GetDemoImage()
{
	InitReader();
	if (m_pDemoReader && m_pReaderSettingdlg)
	{
		LayoutStyle eLayoutIndex = m_pReaderSettingdlg->GetCurLayoutStyle();

		// 设置版式
		UINT uLineSpacing = 0;
		UINT uParaSpacing = 0;
		UINT uIndent = 0;
		UINT uTopMargin = 0;
		UINT uLeftMargin = 0;
		if (!LayoutHelper::LayoutParaFromStyle(eLayoutIndex,&uLineSpacing,&uParaSpacing,&uIndent,&uTopMargin,&uLeftMargin)) {
		//由于设计修改,暂时去掉对自定义格式的调整
			//uLineSpacing = m_pReaderSettingDlg->GetCurLineGap();
			//uParaSpacing = m_pReaderSettingDlg->GetCurPageMargin();
			//uIndent = m_pReaderSettingDlg->GetCurIndent();
		}

		DK_LAYOUTSETTING    clslayout;
		clslayout.dFontSize    = m_pReaderSettingdlg->GetCurFontSize();
		clslayout.dLineGap     = uLineSpacing * 0.01f;
		clslayout.dParaSpacing = uParaSpacing * 0.01f;
		clslayout.dIndent      = uIndent;
		clslayout.dTabStop     = SystemSettingInfo::GetInstance()->GetTabStop();
		clslayout.bOriginalLayout = (eLayoutIndex==LAYOUT_SYSLE_NORMAL);
		m_pDemoReader->SetFontChange(true);

		bool bIsInstant = false;
		//bIsInstant = IsInstantTurnPageModel();
		m_pDemoReader->SetInstantTurnPageModel(bIsInstant);
		m_pDemoReader->SetLayoutSettings (clslayout);
		//m_pDemoReader->ReParseContent();

		// 设置页面大小
		int readingLayoutMode = m_pReaderSettingdlg->GetCurReadingLayoutMode();
		int iTopMargin = SystemSettingInfo::GetInstance()->GetPageTopMargin(readingLayoutMode);
		int iBottomMargin  = SystemSettingInfo::GetInstance()->GetPageBottomMargin(readingLayoutMode);
		int iHorizontalMargin = SystemSettingInfo::GetInstance()->GetPageHorizontalMargin(readingLayoutMode);
		int iBookWidth = m_pReaderSettingdlg->GetScreenWidth() - 2 * iHorizontalMargin;
		int iBookHeigth = m_pReaderSettingdlg->GetScreenHeight() - iTopMargin - iBottomMargin;
		m_pDemoReader->SetPageSize(iHorizontalMargin, iTopMargin, iBookWidth,iBookHeigth);

		//设置字体渲染
		DK_FONT_SMOOTH_TYPE fontSmoothType = DK_FONT_SMOOTH_NORMAL;
		if(SystemSettingInfo::GetInstance()->GetFontRender() == 0)
		{
			fontSmoothType = DK_FONT_SMOOTH_SHARP;
		}
	    m_pDemoReader->SetFontSmoothType(fontSmoothType);

        // 设置简繁转换
        bool familiarToTraditional = (bool)m_pReaderSettingdlg->GetCurFontStyle();
        m_pDemoReader->SetFamiliarToTraditional(familiarToTraditional);

		// 设置加黑
		m_pDemoReader->SetTextGrayScaleLevel(s_emboldenTable[m_pReaderSettingdlg->GetCurEmBoldenLevel()]);
		m_pDemoReader->MoveToPageDirect(0);
		return m_pDemoReader->GetPageBMP();
	}
	return NULL;
}


bool LayoutReader::IsInstantTurnPageModel()
{
    return SystemSettingInfo::GetInstance()->IsInstantTurnPageModel();
}
