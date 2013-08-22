#ifndef __TOUCHAPPUI_UIPERSONALEXPERIENCEPAGE_H__
#define __TOUCHAPPUI_UIPERSONALEXPERIENCEPAGE_H__

#include "GUI/UITextSingleLine.h"
#include "GUI/UIPage.h"
#include "PersonalUI/UIPersonalExperienceLabel.h"
#include "PersonalUI/UISingleLine.h"
#include "PersonalUI/UIDistributionChart.h"
#include "GUI/UIImageTransparent.h"
#include "Common/ReadingHistoryStat.h"

#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#endif

class UIPersonalExperiencePage : public UIPage
{
public:
	virtual LPCSTR GetClassName() const
	{
		return "UIPersonalExperiencePage";
	}

	UIPersonalExperiencePage();
	~UIPersonalExperiencePage();
	virtual void MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height);
	void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

private:
	void Init();
	void InitUI();
	void UpdateExperience();
	bool LocalExperienceChanged(const EventArgs& args);
	void OnRefreshExperienceClick();
	void OnShareExperienceClick();
	std::string GetJpegScreenShot();

private:

#ifdef KINDLE_FOR_TOUCH
	UITouchBackButton m_btnBack;
#else
	UITitleBar m_titleBar;
#endif
	UITouchComplexButton m_btnShare;
	UITouchComplexButton m_btnRefresh;
	UITextSingleLine m_ExperienceTitle;
	UITextSingleLine m_newIndex;
	UITextSingleLine m_newIndexQuantifier;
	UITextSingleLine m_newDescription;
	
	UITextSingleLine m_newReadBookNum;
	UITextSingleLine m_newReadBook;

	UITextSingleLine m_readHourNum;
	UITextSingleLine m_readHour;
	
	UITextSingleLine m_newCompletedBookNum;
	UITextSingleLine m_newCompletedBook;
	
	UITextSingleLine m_usedDayNum;
	UITextSingleLine m_usedDay;

	UITextSingleLine m_timeSpan;
	UITextSingleLine m_favouriteReadingTime;
	UIImageTransparent	m_clockImage;

	UISingleLine m_readingdataLine1;
	UISingleLine m_readingdataLine2;
	UISingleLine m_readingdataLine3;
	UISingleLine m_readingdataLine4;
	UISingleLine m_line1;
	UISingleLine m_line2;

	UITextSingleLine m_hour0;
	UITextSingleLine m_hour6;
	UITextSingleLine m_hour12;
	UITextSingleLine m_hour18;
	UITextSingleLine m_hour24;
	UIDistributionChart m_distribution;
	UITextSingleLine m_lastUploaded;

	ReadingHistory m_readingData;
    UISizer* m_refreshSizer;
};
#endif

