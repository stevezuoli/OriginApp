#ifndef __SCREENSAVERUPDATE_SCREENSAVERUPDATEINFO_H
#define __SCREENSAVERUPDATE_SCREENSAVERUPDATEINFO_H

#include "ScreenSaverItem.h"
#include "../Common/Mutex.h"
#include "drivers/DeviceInfo.h"
#include "singleton.h"
#include <string>
#include <vector>

class ScreenSaverUpdateInfo : public MutexBase
{
	SINGLETON_H(ScreenSaverUpdateInfo)
public:
	ScreenSaverUpdateInfo();
	~ScreenSaverUpdateInfo();
	bool CheckForUpdate();
	bool AddWaitListToDownloader();
	bool AddDownloadedToCurrentList(const std::string &_strUrl,const std::string &_strFileName);
private:
	bool ParseReturnJsonData(const std::string &_strContent);
	bool SaveCurrentScreenSaverToConfige();
	bool LoadCurrentScreenSaverFromConfige();
private:
	std::vector <ScreenSaverItem> m_vctCurrentList;
	std::vector <ScreenSaverItem> m_vctWaitDownloadList;
};

#endif//__SCREENSAVERUPDATE_SCREENSAVERUPDATEINFO_H