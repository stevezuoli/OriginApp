#include "ScreenSaverUpdate/ScreenSaverItem.h"

using namespace std;
ScreenSaverItem::ScreenSaverItem()
	: m_strUrl("")
	, m_strPathName("")
	, m_iExpireDate(0)
	, m_iPriority(0)
{

}

ScreenSaverItem::~ScreenSaverItem()
{

}

string ScreenSaverItem::GetUrl()const
{
	return m_strUrl;
}

void ScreenSaverItem::SetUrl(const std::string &_strUrl)
{
	m_strUrl = _strUrl;
}
void ScreenSaverItem::SetPathName(const std::string &_strPathName)
{
	m_strPathName = _strPathName;
}
std::string ScreenSaverItem::GetPathName()const
{
	return m_strPathName;
}
void ScreenSaverItem::SetExpireDate(int _iExpireDate)
{
	m_iExpireDate = _iExpireDate;
}
int ScreenSaverItem::GetExpireDate() const
{
	return m_iExpireDate;
}
void ScreenSaverItem::SetPriority(int _iPriority)
{
	m_iPriority = _iPriority;
}
int ScreenSaverItem::GetPriority()const
{
	return m_iPriority;
}
