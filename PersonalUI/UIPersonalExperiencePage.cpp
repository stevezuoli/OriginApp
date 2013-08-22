#include "PersonalUI/UIPersonalExperiencePage.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/CPageNavigator.h"
#include "Utility/ColorManager.h"
#include "GUI/UISizer.h"
#include "Common/CAccountManager.h"
#include <tr1/functional>
#include "Common/SystemSetting_DK.h"
#include "CommonUI/UIUtility.h"
#include "Framework/CDisplay.h"
#include "Utility/PathManager.h"
#include "DkFilterFactory.h"
#include "Utility/RenderUtil.h"
#include "Utility/TimerThread.h"
#include "Utility/SystemUtil.h"
#include "CommonUI/UIWeiboSharePage.h"
#include "drivers/DeviceInfo.h"

using dk::utility::TimerThread;
using namespace dk::utility;
using dk::utility::PathManager;
using namespace WindowsMetrics;
using namespace dk::account;

UIPersonalExperiencePage::UIPersonalExperiencePage()
#ifdef KINDLE_FOR_TOUCH
    :UIPage()
    , m_btnBack(this, ID_BTN_TOUCH_BACK)
#else
    :UIBookStorePage()
#endif
	, m_clockImage(this)
	, m_readingdataLine1(this)
	, m_readingdataLine2(this)
	, m_readingdataLine3(this)
	, m_readingdataLine4(this)
	, m_line1(this)
	, m_line2(this)
	, m_distribution(this)
	, m_readingData()
    , m_refreshSizer(NULL)
{
	Init();
	SubscribeMessageEvent(ReadingHistoryStat::EventPersonalReaingDataUpdated, *ReadingHistoryStat::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalExperiencePage::LocalExperienceChanged),
        this,
        std::tr1::placeholders::_1));
	ReadingHistoryStat::GetInstance()->FetchPersonalReadingData();
}
UIPersonalExperiencePage::~UIPersonalExperiencePage()
{
}

void UIPersonalExperiencePage::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    UIPage::MoveWindow(left, top, width, height);
	Layout();
}

void UIPersonalExperiencePage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
#ifdef KINDLE_FOR_TOUCH
    case ID_BTN_TOUCH_BACK:
		CPageNavigator::BackToPrePage();
        break;
#endif
	case ID_BTN_PERSONAL_EXPERIENCE_SHARE:
		OnShareExperienceClick();
		break;
	case ID_BTN_PERSONAL_EXPERIENCE_REFRESH:
		OnRefreshExperienceClick();
		break;
    default:
        break;
    }
#ifndef KINDLE_FOR_TOUCH
	UIBookStorePage::OnCommand(dwCmdId, pSender, dwParam);
#endif
}

void UIPersonalExperiencePage::OnRefreshExperienceClick()
{
    ReadingHistoryStat* readingHistory = ReadingHistoryStat::GetInstance();
	if(UIUtility::CheckAndReLoginSync() && readingHistory)
	{
		m_btnRefresh.SetEnable(false);
        m_btnRefresh.SetText(StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_REFRESHING));
        if (m_refreshSizer)
        {
            m_refreshSizer->Layout();
        }
        CDisplay::CacheDisabler cache;
        m_btnRefresh.UpdateWindow();
        readingHistory->FetchPersonalReadingData();
	}
}

void UIPersonalExperiencePage::OnShareExperienceClick()
{
	double everageValue = m_readingData.totalSeconds*1.0 / 60;
	if(m_readingData.totalDay > 0)
	{
		everageValue /= m_readingData.totalDay;
	}
	
	char everageString[15];
	if(everageValue > 60)
	{
		everageValue /= 60;
		snprintf(everageString, DK_DIM(everageString), "%.1lf %s", everageValue, StringManager::GetPCSTRById(QUANTIFUER_HOUR));
	}
	else
	{
		snprintf(everageString, DK_DIM(everageString), "%d %s", (int)everageValue, StringManager::GetPCSTRById(QUANTIFUER_MINUTE));
	}

	char shaerText[200];
	snprintf(shaerText, DK_DIM(shaerText), StringManager::GetPCSTRById(SHAREWEIBO_PERSONALEXPERIENCE), 
			m_readingData.rankingRatio*100, 
			int(m_readingData.totalSeconds/3600),
			everageString);
	string imagepath = GetJpegScreenShot();
	UIWeiboSharePage* pPage = new UIWeiboSharePage(shaerText);
	pPage->SetSharedPicture(imagepath.c_str(), true);
	if (pPage)
	{
	    CPageNavigator::Goto(pPage);
	}
}

string UIPersonalExperiencePage::GetJpegScreenShot()
{
	DK_IMAGE imageScreenShort = CDisplay::GetDisplay()->GetMemDC();
	string picturePath("");
	if(imageScreenShort.pbData)
	{
		int titleBarHeight = GetWindowMetrics(UITitleBarHeightIndex);
		int imageHeight = imageScreenShort.iHeight - titleBarHeight;
		
		time_t rawtime = time(NULL);
		struct tm fullTime = *gmtime(&rawtime);
		char jpegName[256] = {0};
	    strftime(jpegName, 256, "20%y%m%d%H%M%S", &fullTime);
		char fileName[512] = {0};
	    snprintf(fileName, DK_DIM(fileName), "%s/%s.jpeg", PathManager::GetTempPath().c_str(), jpegName);

		int length = imageScreenShort.iWidth * imageHeight;
		DK_BYTE *data = new DK_BYTE[length];
	    if(data)
	    {
	    	int basePosition = titleBarHeight*imageScreenShort.iWidth;
	    	for(int i = 0; i < length; i++)
			{
				data[i] = ~imageScreenShort.pbData[i + basePosition];
			}
	    }
			
		DK_BITMAPBUFFER_DEV dev;
		dev.lWidth = imageScreenShort.iWidth;
	    dev.lHeight = imageHeight;
	    dev.nPixelFormat = DK_PIXELFORMAT_GRAY;
	    dev.lStride = imageScreenShort.iStrideWidth;
	    dev.nDPI = RenderConst::SCREEN_DPI;
	    dev.pbyData = data;
		
		if(DKR_OK == DkFilterFactory::SaveBitmapToJpeg(dev, 70, fileName))
		{
			picturePath = fileName;
		}
		delete []data;
	    data = NULL;
		return fileName;
	}
	return picturePath;
}

void UIPersonalExperiencePage::UpdateExperience()
{
	ReadingHistoryStat* readingHistory = ReadingHistoryStat::GetInstance();
	readingHistory->GetReadingHistory(&m_readingData);

	char readingMinuteAverage[10];
	int minuteAverage = (int)(m_readingData.totalSeconds / 60);
	if(m_readingData.totalDay > 1)
	{
		minuteAverage = minuteAverage / m_readingData.totalDay;
	}
	snprintf(readingMinuteAverage, DK_DIM(readingMinuteAverage), "%d", minuteAverage);

	char rankingRatio[10];
	snprintf(rankingRatio, DK_DIM(rankingRatio), "%g", m_readingData.rankingRatio*100);
	m_newIndex.SetText(rankingRatio);

	char description[100];
	snprintf(description, DK_DIM(description), StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_INDEX), m_readingData.rankingRatio*100);
	m_newDescription.SetText(description);

	char totalBooks[10];
    snprintf(totalBooks, DK_DIM(totalBooks), "%d", m_readingData.totalReadingBooks);
	m_newReadBookNum.SetText(totalBooks);

	char totalHours[10];
	int totalReadingHour = (int)(m_readingData.totalSeconds / 3600);
	((int)m_readingData.totalSeconds % 3600 == 0) || ++totalReadingHour;
	snprintf(totalHours, DK_DIM(totalHours), "%d", totalReadingHour);
	m_readHourNum.SetText(totalHours);

	char totalCompletedBooks[10];
	snprintf(totalCompletedBooks, DK_DIM(totalCompletedBooks), "%d", m_readingData.totalCompletedBooks);
	m_newCompletedBookNum.SetText(totalCompletedBooks);

	char totalDay[10];
	snprintf(totalDay, DK_DIM(totalDay), "%d", m_readingData.totalDay);
	m_usedDayNum.SetText(totalDay);

	DKTime localTime;
	int localUTCHour = localTime.GetTimeZoneSeconds()/3600;
	//localUTCHour%24 是为防止localUTCHour 出现错误数据
	//因为localUTCHour可能为负值，所以先加24 然后取余
	localUTCHour = (localUTCHour%24 + 24)%24;
	vector<int> UTCDistribution = m_readingData.distribution;
	vector<int> localTimeDistribution;

	//将UTC 时间转换为本地时间
	for(size_t i = 0; i < UTCDistribution.size(); i++)
	{
		localTimeDistribution.push_back(UTCDistribution[((24 - localUTCHour) + i)%UTCDistribution.size()]);
	}

	//计算最大值，方便计算画图
	int distributionSize = localTimeDistribution.size();
	char readingTime[100] = {0};
	int startTime = 0;
	int maxSum = 0;
	for(int i = 0; i < distributionSize; i++)
	{
		if(maxSum < localTimeDistribution[i] + localTimeDistribution[(i + 1)%distributionSize])
		{
			maxSum = localTimeDistribution[i] + localTimeDistribution[(i + 1)%distributionSize];
			startTime = i;
		}
	}

	int maxSecond = 0;
	for(std::vector<int>::iterator iter = localTimeDistribution.begin(); iter != localTimeDistribution.end(); iter++)
	{
		maxSecond = (maxSecond > (*iter) ? maxSecond : (*iter));
	}

	//计算平均值
	int everagePosition = 0;
	for(std::vector<int>::iterator iter = localTimeDistribution.begin(); iter != localTimeDistribution.end(); iter++)
	{
		everagePosition += *iter;
	}
	if(localTimeDistribution.size() > 0)
	{
		everagePosition /= localTimeDistribution.size();
	}
	else
	{
		everagePosition = 0;
	}

	//设置分布图要显示的字
	double everageValue = m_readingData.totalSeconds*1.0 / 60;
	if(m_readingData.totalDay > 0)
	{
		everageValue /= m_readingData.totalDay;
	}
	
	char everageString[15];
	if(everageValue > 60)
	{
		everageValue /= 60;
		snprintf(everageString, DK_DIM(everageString), "%.1lf %s", everageValue, StringManager::GetPCSTRById(QUANTIFUER_HOUR));
	}
	else
	{
		snprintf(everageString, DK_DIM(everageString), "%d %s", (int)everageValue, StringManager::GetPCSTRById(QUANTIFUER_MINUTE));
	}
	
	char strEverage[30];
	snprintf(strEverage, DK_DIM(strEverage), StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_EVERAGE), everageString);
	m_distribution.SetDistribution(localTimeDistribution);
	m_distribution.SetMaxHeight(maxSecond);
	m_distribution.SetDescription(strEverage, everagePosition);
	
	snprintf(readingTime, DK_DIM(readingTime), StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_READING_TIME), startTime, (startTime + 2)%24);
	m_favouriteReadingTime.SetText(readingTime);

	if(m_readingData.serverUtcTimeStamp != 0)
	{
		time_t uploadTime = (time_t)(m_readingData.serverUtcTimeStamp + localTime.GetTimeZoneSeconds());
		struct tm fullUploadTime = *gmtime(&uploadTime);
		char lastUploadTime[50];
		snprintf(lastUploadTime, DK_DIM(lastUploadTime), StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_SYNC), 
			fullUploadTime.tm_year + 1900, fullUploadTime.tm_mon + 1, fullUploadTime.tm_mday, fullUploadTime.tm_hour, fullUploadTime.tm_min);
		m_lastUploaded.SetText(lastUploadTime);
	}
	else
	{
		m_lastUploaded.SetText(StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_SYNC_NOT));
    }
#ifndef KINDLE_FOR_TOUCH
    m_btnRefresh.SetFocus(true);
#endif
	Layout();
	CDisplay::GetDisplay()->SetFullRepaint(true);
	Repaint();
}

void UIPersonalExperiencePage::Init()
{
	const int fontSize16 = GetWindowFontSize(FontSize16Index);
	const int fontSize20 = GetWindowFontSize(FontSize20Index);
	const int fontSize32 = GetWindowFontSize(FontSize32Index);

#ifdef KINDLE_FOR_TOUCH
    const int btnHorizontalMargin = GetWindowMetrics(UIBtnDefaultHorizontalMarginIndex);
    const int btnVerticalMargin = GetWindowMetrics(UIBtnDefaultVerticalMarginIndex);
    m_btnShare.Initialize(ID_BTN_PERSONAL_EXPERIENCE_SHARE, "", fontSize20);
    m_btnShare.SetBackground(ImageManager::GetImage(IMAGE_ICON_TRANSMIT));
    m_btnShare.SetLeftMargin(0);
    m_btnShare.SetVAlign(VALIGN_CENTER);
    m_btnRefresh.Initialize(ID_BTN_PERSONAL_EXPERIENCE_REFRESH, StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_REFRESH), fontSize20);
#else
    const int btnHorizontalMargin = 0;
    const int btnVerticalMargin = 0;
    m_btnShare.Initialize(ID_BTN_PERSONAL_EXPERIENCE_SHARE, StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_SHARE)
        , 'S', fontSize20, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnRefresh.Initialize(ID_BTN_PERSONAL_EXPERIENCE_REFRESH, StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_REFRESH)
        , 'R', fontSize20, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnBack.SetEnable(false);
    m_btnBack.SetMinWidth(GetWindowMetrics(UINormalSmallBtnWidthIndex));
#endif
    m_btnRefresh.SetLeftMargin(btnHorizontalMargin);
    m_btnRefresh.SetTopMargin(btnVerticalMargin);

	m_ExperienceTitle.SetFontSize(GetWindowFontSize(FontSize28Index));
    m_ExperienceTitle.SetText(StringManager::GetPCSTRById(PERSONAL_EXPERIENCE));
	m_ExperienceTitle.SetAlign(ALIGN_CENTER);

	m_newIndex.SetFontSize(GetWindowFontSize(FontSize60Index));
	m_newIndex.SetEnglishGothic();
	m_newIndex.SetVAlign(VALIGN_TOP);

	string IndexQuantifier = "%";
	m_newIndexQuantifier.SetText(IndexQuantifier);
	m_newIndexQuantifier.SetEnglishGothic();
	m_newIndexQuantifier.SetFontSize(GetWindowFontSize(FontSize30Index));
	m_newIndexQuantifier.SetVAlign(VALIGN_TOP);
	
	m_newDescription.SetFontSize(fontSize16);
	m_newDescription.SetAlign(ALIGN_CENTER);

	m_newReadBookNum.SetFontSize(fontSize32);
	m_newReadBookNum.SetEnglishGothic();
	m_newReadBook.SetText(StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_BOOKNUM));
	m_newReadBook.SetFontSize(fontSize16);
	
	m_readHourNum.SetFontSize(fontSize32);
	m_readHourNum.SetEnglishGothic();
	m_readHour.SetText(StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_TOTAL_TIME));
	m_readHour.SetFontSize(fontSize16);

	m_newCompletedBookNum.SetFontSize(fontSize32);
	m_newCompletedBookNum.SetEnglishGothic();
	m_newCompletedBook.SetText(StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_COMPELTE_BOOKNUM));
	m_newCompletedBook.SetFontSize(fontSize16);

	m_usedDayNum.SetFontSize(fontSize32);
	m_usedDayNum.SetEnglishGothic();
	m_usedDay.SetText(StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_TOTAL_DAY));
	m_usedDay.SetFontSize(fontSize16);

	m_clockImage.SetImageFile(ImageManager::GetImagePath(IMAGE_ICON_CLOCK));
	m_clockImage.SetMinSize(dkSize(m_clockImage.GetImageWidth(), m_clockImage.GetImageHeight()));
	m_timeSpan.SetText(StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_TIME_SPAN));
	m_timeSpan.SetFontSize(fontSize20);
	
	m_favouriteReadingTime.SetFontSize(fontSize16);
	m_lastUploaded.SetFontSize(fontSize20);

	m_hour0.SetText("0:00");
	m_hour0.SetFontSize(fontSize16);
	m_hour6.SetText("6:00");
	m_hour6.SetFontSize(fontSize16);
	m_hour12.SetText("12:00");
	m_hour12.SetFontSize(fontSize16);
	m_hour18.SetText("18:00");
	m_hour18.SetFontSize(fontSize16);
	m_hour24.SetText("24:00");
	m_hour24.SetFontSize(fontSize16);

	AppendChild(&m_btnBack);
	AppendChild(&m_btnShare);
	AppendChild(&m_btnRefresh);
	AppendChild(&m_ExperienceTitle);
	AppendChild(&m_newIndex);
	AppendChild(&m_newIndexQuantifier);
	AppendChild(&m_newDescription);
	AppendChild(&m_newReadBookNum);
	AppendChild(&m_newReadBook);
	AppendChild(&m_readHourNum);
	AppendChild(&m_readHour);
	AppendChild(&m_newCompletedBookNum);
	AppendChild(&m_newCompletedBook);
	AppendChild(&m_usedDayNum);
	AppendChild(&m_usedDay);
	AppendChild(&m_timeSpan);
	AppendChild(&m_favouriteReadingTime);
	AppendChild(&m_clockImage);
	AppendChild(&m_titleBar);
	AppendChild(&m_line1);
	AppendChild(&m_line2);
	AppendChild(&m_readingdataLine1);
	AppendChild(&m_readingdataLine2);
	AppendChild(&m_readingdataLine3);
	AppendChild(&m_readingdataLine4);
	AppendChild(&m_distribution);
	AppendChild(&m_hour0);
	AppendChild(&m_hour6);
	AppendChild(&m_hour12);
	AppendChild(&m_hour18);
	AppendChild(&m_hour24);
	AppendChild(&m_lastUploaded);
#ifndef KINDLE_FOR_TOUCH
	m_titleBar.MoveWindow(0, 0, 600, GetWindowMetrics(UITitleBarHeightIndex));
#endif
	InitUI();
	UpdateExperience();
}

void UIPersonalExperiencePage::InitUI()
{
	const int elementSpacing = GetWindowMetrics(UIPersonalExperienceElementSpacingIndex);
	const int lineHeight = GetWindowMetrics(UIPersonalExperienceLineMinHeightIndex);
	m_line1.SetMinHeight(lineHeight);
	m_line2.SetMinHeight(lineHeight);
	m_readingdataLine1.SetMinWidth(lineHeight);
	m_readingdataLine2.SetMinWidth(lineHeight);
	m_readingdataLine3.SetMinWidth(lineHeight);
	m_readingdataLine4.SetMinWidth(lineHeight);
	m_distribution.SetMinHeight(GetWindowMetrics(UIPersonalExperienceDistributionHieghtIndex));
	if (!m_windowSizer)
	{
		UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);

		UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);
#ifdef KINDLE_FOR_TOUCH
		topSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_LEFT).Center());
#else
		topSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_LEFT));
#endif
        topSizer->Add(&m_ExperienceTitle, UISizerFlags(1).Expand().Align(dkALIGN_CENTER));
        topSizer->Add(&m_btnShare, UISizerFlags().Centre());
        topSizer->SetMinHeight(GetWindowMetrics(UIPixelValue61Index));

		UISizer* indexSizer = new UIBoxSizer(dkHORIZONTAL);
		indexSizer->AddStretchSpacer();
		indexSizer->Add(&m_newIndex, UISizerFlags().Align(dkALIGN_TOP));
		indexSizer->Add(&m_newIndexQuantifier, UISizerFlags().Align(dkALIGN_TOP));
		indexSizer->AddStretchSpacer();

		UISizer* readingDataSizer = new UIBoxSizer(dkHORIZONTAL);
		UISizer* readBookSizer = new UIBoxSizer(dkVERTICAL);
		readBookSizer->Add(&m_newReadBookNum, UISizerFlags().Expand());
		readBookSizer->Add(&m_newReadBook, UISizerFlags().Expand());

		UISizer* readHourSizer = new UIBoxSizer(dkVERTICAL);
		readHourSizer->Add(&m_readHourNum, UISizerFlags().Expand());
		readHourSizer->Add(&m_readHour, UISizerFlags().Expand());

		UISizer* completedBookSizer = new UIBoxSizer(dkVERTICAL);
		completedBookSizer->Add(&m_newCompletedBookNum, UISizerFlags().Expand());
		completedBookSizer->Add(&m_newCompletedBook, UISizerFlags().Expand());
		
		UISizer* usedDaySizer = new UIBoxSizer(dkVERTICAL);
		usedDaySizer->Add(&m_usedDayNum, UISizerFlags().Expand());
		usedDaySizer->Add(&m_usedDay, UISizerFlags().Expand());

		UISizer* timeSpanSizer = new UIBoxSizer(dkHORIZONTAL);
		UISizer* readingTimeSizer = new UIBoxSizer(dkVERTICAL);
		readingTimeSizer->Add(&m_timeSpan);
		readingTimeSizer->Add(&m_favouriteReadingTime);
		timeSpanSizer->Add(&m_clockImage, UISizerFlags().Border(dkRIGHT, 2*elementSpacing));
		timeSpanSizer->Add(readingTimeSizer);

		
		UISizer* timescale = new UIBoxSizer(dkHORIZONTAL);
		timescale->Add(&m_hour0);
		timescale->AddStretchSpacer();
		timescale->Add(&m_hour6);
		timescale->AddStretchSpacer();
		timescale->Add(&m_hour12);
		timescale->AddStretchSpacer();
		timescale->Add(&m_hour18);
		timescale->AddStretchSpacer();
		timescale->Add(&m_hour24);

        SafeDeletePointer(m_refreshSizer);
		m_refreshSizer = new UIBoxSizer(dkHORIZONTAL);
		m_refreshSizer->Add(&m_lastUploaded, UISizerFlags(1).Expand().Align(dkALIGN_CENTER_VERTICAL));
#ifdef KINDLE_FOR_TOUCH
		m_refreshSizer->Add(&m_btnRefresh, UISizerFlags().Expand().Align(dkALIGN_CENTER_VERTICAL));
#else
		m_refreshSizer->Add(&m_btnRefresh, UISizerFlags().Expand().Align(dkALIGN_RIGHT));
#endif
		readingDataSizer->Add(&m_readingdataLine1, UISizerFlags().Expand().Border(dkRIGHT, elementSpacing));
		readingDataSizer->Add(readBookSizer, UISizerFlags().Expand());
		readingDataSizer->AddStretchSpacer();
		readingDataSizer->Add(&m_readingdataLine2, UISizerFlags().Expand().Border(dkRIGHT, elementSpacing));
		readingDataSizer->Add(readHourSizer, UISizerFlags().Expand());
		readingDataSizer->AddStretchSpacer();
		readingDataSizer->Add(&m_readingdataLine3, UISizerFlags().Expand().Border(dkRIGHT, elementSpacing));
		readingDataSizer->Add(completedBookSizer, UISizerFlags().Expand());
		readingDataSizer->AddStretchSpacer();
		readingDataSizer->Add(&m_readingdataLine4, UISizerFlags().Expand().Border(dkRIGHT, elementSpacing));
		readingDataSizer->Add(usedDaySizer, UISizerFlags().Expand());
	
        const int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);
		mainSizer->Add(&m_titleBar, UISizerFlags().Expand());
#ifdef KINDLE_FOR_TOUCH
		mainSizer->Add(topSizer, UISizerFlags().Expand().Border(dkRIGHT | dkBOTTOM, horizonMargin));
#else
		mainSizer->Add(topSizer, UISizerFlags().Expand().Border(dkRIGHT | dkLEFT | dkBOTTOM, horizonMargin));
#endif
		mainSizer->Add(indexSizer, UISizerFlags().Expand());
		mainSizer->Add(&m_newDescription, UISizerFlags().Expand().Align(dkALIGN_CENTRE_HORIZONTAL));
		mainSizer->Add(&m_line1, UISizerFlags().Expand().Border(dkTOP, horizonMargin));
		mainSizer->AddSpacer(elementSpacing*3);
		mainSizer->Add(readingDataSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin));
		mainSizer->AddSpacer(elementSpacing*3);
		mainSizer->Add(&m_line2, UISizerFlags().Expand().Border(dkBOTTOM, horizonMargin));
		mainSizer->Add(timeSpanSizer, UISizerFlags().Expand().Border(dkLEFT, horizonMargin));
		mainSizer->AddSpacer(elementSpacing*2);
		mainSizer->Add(&m_distribution, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin));
		mainSizer->AddSpacer(elementSpacing);
		mainSizer->Add(timescale, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin));
		mainSizer->AddStretchSpacer();
		mainSizer->Add(m_refreshSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin).Border(dkBOTTOM, GetWindowMetrics(UIPixelValue15Index)));
		SetSizer(mainSizer);
	}
}

bool UIPersonalExperiencePage::LocalExperienceChanged(const EventArgs& args)
{
	m_btnRefresh.SetText(StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_REFRESH));
	m_btnRefresh.SetEnable(true);
	UpdateExperience();
	return true;
}

