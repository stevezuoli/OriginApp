#ifndef __SCREENSAVERUPDATE_SCREENSAVERITEM_H_
#define __SCREENSAVERUPDATE_SCREENSAVERITEM_H_

#include <string>
class ScreenSaverItem
{
public:
	ScreenSaverItem();
	~ScreenSaverItem();
	void SetUrl(const std::string &_strUrl);
	std::string GetUrl()const;
	void SetPathName(const std::string &_strPathName);
	std::string GetPathName()const;
	void SetExpireDate(int _iExpireDate);
	int GetExpireDate() const;
	void SetPriority(int _iPriority);
	int GetPriority()const;
	bool operator==(const ScreenSaverItem &_TmpItem)
	{
		return this->m_strUrl == _TmpItem.m_strUrl;
	}
private:
	std::string m_strUrl;
	std::string m_strPathName;
	int			m_iExpireDate;
	int			m_iPriority;
};

#endif//__SCREENSAVERUPDATE_SCREENSAVERITEM_H_