#include "ScreenSaverUpdate/ScreenSaverUpdateInfo.h"
#include "../DK91Search/Curl_Post.h"
#include "DownloadManager/DownloadTaskFactory.h"
#include "DownloadManager/DownloaderImpl.h"
#include "Utility/PathManager.h"
#include "Utility/JsonObject.h"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace DK91SearchAPI;
using dk::utility::PathManager;

#define SCREENSAVER_SERVERURL "http://picture.duokan.com:2002/pictures"

SINGLETON_CPP(ScreenSaverUpdateInfo)

ScreenSaverUpdateInfo::ScreenSaverUpdateInfo()
{
	Mutex lock(this);
	m_vctCurrentList.clear();
	LoadCurrentScreenSaverFromConfige();
	m_vctWaitDownloadList.clear();
}

ScreenSaverUpdateInfo::~ScreenSaverUpdateInfo()
{
	Mutex lock(this);
	m_vctCurrentList.clear();
	m_vctWaitDownloadList.clear();
}

bool ScreenSaverUpdateInfo::CheckForUpdate()
{
	Mutex lock(this);
	DeviceInfo clsDeviceInfo;
	string strDevicesId = clsDeviceInfo.GetDeviceID();
	string strRequest = "";
	strRequest = "device_id=" + strDevicesId + "&app_id=" + "KindleUser" + "&build=" + "2012051401";
	int iErrorCode = 0;
	string strReturn = Get(SCREENSAVER_SERVERURL,strRequest,&iErrorCode);
	if(strReturn.empty() || iErrorCode)
	{
		return false;
	}
	if(false == ParseReturnJsonData(strReturn))
	{
		return false;
	}
	for(vector<ScreenSaverItem>::iterator itr = m_vctCurrentList.begin();itr != m_vctCurrentList.end();)
	{
		vector<ScreenSaverItem>::iterator itrCur = std::find(m_vctWaitDownloadList.begin(),m_vctWaitDownloadList.end(),*itr);
		if (m_vctWaitDownloadList.end() != itrCur)
		{
			m_vctWaitDownloadList.erase(itrCur);
			itr++;
		}
		else
		{
			itr = m_vctCurrentList.erase(itr);
		}
	}
	if(m_vctWaitDownloadList.empty())
	{
		return false;
	}
	return true;
}

bool ScreenSaverUpdateInfo::AddWaitListToDownloader()
{
	Mutex lock(this);
	IDownloader *pDownloadManager = DownloaderImpl::GetInstance();
	if (!pDownloadManager)
	{
		return false;
	}
	for(vector<ScreenSaverItem>::iterator itr = m_vctWaitDownloadList.begin();itr != m_vctWaitDownloadList.end();itr++)
	{
		string strFileName = "TmpFile.jpg";
		string strUrl = itr->GetUrl();
		if(strUrl.empty())
		{
			continue;
		}
		size_t iPos = strUrl.rfind('/');
		if(string::npos != iPos)
		{
			strFileName = strUrl.substr(iPos + 1);
		}
		IDownloadTask *ScreenSaverTask = DownloadTaskFactory::CreateScreenSaverDownloadTask(strUrl,strUrl,strFileName,0);
		pDownloadManager->AddTask(ScreenSaverTask);
	}
	return true;
}


bool ScreenSaverUpdateInfo::AddDownloadedToCurrentList(const std::string &_strUrl,const std::string &_strFileName)
{
	if(_strUrl.empty() || _strFileName.empty())
	{
		return false;
	}
	Mutex lock(this);
	ScreenSaverItem clsTmpItem;
	clsTmpItem.SetUrl(_strUrl);
	vector<ScreenSaverItem>::iterator itr = find(m_vctWaitDownloadList.begin(),m_vctWaitDownloadList.end(),clsTmpItem);
	if(m_vctWaitDownloadList.end() == itr)
	{
		return false;
	}
	itr->SetPathName(_strFileName);
	m_vctCurrentList.push_back(*itr);
	m_vctWaitDownloadList.erase(itr);
	string strOldPath = DOWNLOADPATH + _strFileName;
	string strNewPath = PathManager::GetScreenSaverDir()+ _strFileName;
	rename(strOldPath.c_str(),strNewPath.c_str());
	SaveCurrentScreenSaverToConfige();
	return true;
}

#define PARSEOBJECT_CODE		"code"
#define PARSEOBJECT_MESSAGE		"message"
#define PARSEOBJECT_COUNT		"count"
#define PARSEOBJECT_CONTENT		"content"
#define PARSEOBJECT_EXPIREDATE	"effective_date"
#define PARSEOBJECT_STARTUPPIC	"startup_pic"
#define PARSEOBJECT_PRIORITY	"priority"
#define PARSEOBJECT_FILENAME	"filename"

bool ScreenSaverUpdateInfo::ParseReturnJsonData(const std::string &_strContent)
{
	if(_strContent.empty())
	{
		return false;
	}
	JsonObjectSPtr rootObj = JsonObject::CreateFromString(_strContent.c_str());
    if (rootObj.get())
    {
		int iCode;
		JsonObjectSPtr codeObj = rootObj->GetSubObject(PARSEOBJECT_CODE);
		if(codeObj.get() && codeObj->GetIntValue(&iCode) && iCode == 0)
		{
			m_vctWaitDownloadList.clear();
			JsonObjectSPtr contentObj = rootObj->GetSubObject(PARSEOBJECT_CONTENT);
			if(contentObj.get())
			{
				int iLen = contentObj->GetArrayLength();
				for(int i = 0; i < iLen; i++)
				{
					JsonObjectSPtr tmpObj = contentObj->GetElementByIndex(i);
					if(tmpObj.get())
					{
						ScreenSaverItem clsScreenSaverItem;
						int iExpiredate;
						JsonObjectSPtr expiredateObj = tmpObj->GetSubObject(PARSEOBJECT_EXPIREDATE);
						if(expiredateObj.get() && expiredateObj->GetIntValue(&iExpiredate))
						{
							clsScreenSaverItem.SetExpireDate(iExpiredate);
						}

						string strUrl;
						JsonObjectSPtr urlObj = tmpObj->GetSubObject(PARSEOBJECT_STARTUPPIC);
						if(urlObj.get() && urlObj->GetStringValue(&strUrl))
						{
							clsScreenSaverItem.SetUrl(strUrl.c_str());
						}

						int priority;
						JsonObjectSPtr priorityObj = tmpObj->GetSubObject(PARSEOBJECT_PRIORITY);
						if(priorityObj.get() && priorityObj->GetIntValue(&priority))
						{
							 clsScreenSaverItem.SetPriority(priority);
						}

						string strFileName;
						JsonObjectSPtr fileNameObj = tmpObj->GetSubObject(PARSEOBJECT_FILENAME);
						if(fileNameObj.get() && fileNameObj->GetStringValue(&strFileName))
						{
							clsScreenSaverItem.SetPathName(strFileName.c_str());
						}
						m_vctWaitDownloadList.push_back(clsScreenSaverItem);
					}
				}
			}
			return true;
		}
    }
	return false;
}
bool ScreenSaverUpdateInfo::SaveCurrentScreenSaverToConfige()
{
	FILE *pCfg = fopen(PathManager::GetScreenSaverCfgPath().c_str(),"w");
	if(!pCfg)
	{
		return false;
	}

	JsonObjectSPtr rootObj = JsonObject::CreateJsonObject();
	JsonObjectSPtr screenSaverArrayObj = JsonObject::CreateJsonArrayObject();
	for(vector <ScreenSaverItem>::iterator itr = m_vctCurrentList.begin();itr != m_vctCurrentList.end();itr++)
	{
		JsonObjectSPtr subObj = JsonObject::CreateJsonObject();
		string strFileName = itr->GetPathName();
		if(!strFileName.empty())
		{
			JsonObjectSPtr fileNameObj = JsonObject::CreateJsonStringObject(strFileName.c_str());
			subObj->AddJsonObject(PARSEOBJECT_FILENAME, fileNameObj);
		}

		int iExpireDate = itr->GetExpireDate();
		JsonObjectSPtr expireDateObj = JsonObject::CreateJsonIntObject(iExpireDate);
		subObj->AddJsonObject(PARSEOBJECT_EXPIREDATE, expireDateObj);

		int iPriority = itr->GetPriority();
		JsonObjectSPtr priorityObj = JsonObject::CreateJsonIntObject(iPriority);
		subObj->AddJsonObject(PARSEOBJECT_PRIORITY, priorityObj);

		string strUrl = itr->GetUrl();
		if(!strUrl.empty())
		{
			JsonObjectSPtr urlObj = JsonObject::CreateJsonStringObject(strUrl.c_str());
			subObj->AddJsonObject(PARSEOBJECT_STARTUPPIC, urlObj);
		}
		screenSaverArrayObj->AddJsonObjectArray(subObj);
	}
	rootObj->AddJsonObject(PARSEOBJECT_CONTENT, screenSaverArrayObj);

	JsonObjectSPtr codeObj = JsonObject::CreateJsonIntObject(0);
	rootObj->AddJsonObject(PARSEOBJECT_CODE, codeObj);

	const char *pContent = rootObj->GetJsonString();
	int iContentLen = strlen(pContent);
	fwrite(pContent, 1, iContentLen, pCfg);
	fclose(pCfg);
	pCfg = NULL;
	return true;
}

bool ScreenSaverUpdateInfo::LoadCurrentScreenSaverFromConfige()
{
	FILE *fpCfg = fopen(PathManager::GetScreenSaverCfgPath().c_str(),"r");
	if(NULL == fpCfg)
	{
		return false;
	}
	if(fseek(fpCfg,0,SEEK_END) != 0)
	{
		fclose(fpCfg);
		return false;
	}
	long lSize = ftell(fpCfg);
	if(lSize < 0)
	{
		fclose(fpCfg);
		return false;
	}

	char *pContent = new char[lSize + 1];
	if(NULL == pContent)
	{
		fclose(fpCfg);
		return false;
	}
	fseek(fpCfg,0,SEEK_SET);
	size_t iReadLen = fread(pContent,1,lSize,fpCfg);
	if((long)iReadLen == lSize)
	{
		pContent[lSize] = 0;
		ParseReturnJsonData(pContent);
		m_vctCurrentList = m_vctWaitDownloadList;
	}
	delete []pContent;
	pContent = NULL;

	fclose(fpCfg);
	fpCfg = NULL;
	return true;
}

