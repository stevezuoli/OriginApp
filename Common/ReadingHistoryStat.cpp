#include <tr1/functional>
#include <fstream>
#include <iostream>
#include <time.h>

#include "BookStore/BookStoreInfoManager.h"
#include "Common/CAccountManager.h"
#include "Common/ReadingHistoryStat.h"
#include "CommonUI/UIUtility.h"
#include "FileManager_DK.h"
#include "Mutex.h"
#include "Utility/StringUtil.h"
#include "Utility/SystemUtil.h"
#include "Wifi/WifiManager.h"
#include "Utility/PathManager.h"

using namespace std;
using namespace dk::bookstore;
using namespace dk::utility;
using namespace dk::account;

#define BOOKSTATUS				"bookStatus"
#define BOOK_TITLE				"bookTitle"
#define BOOK_TYPE				"bookType"
#define BOOK_COMPLETE			"bookCompleted"

#define NEWDATA					"newData"
#define NEWDATA_TITLE			"title"
#define NEWDATA_TYPE			"type"
#define UTC_TIMESTAMP			"utc_timestamp"
#define NEW_BOOKS				"new_books"
#define NEW_READ_SECONDS		"new_read_seconds"
#define NEW_READ_PAGES			"new_read_pages"
#define NEW_COMPLETED_BOOKS		"new_completed_books"
#define DISTRIBUTION			"distribution"

#define LASTSERVERDATA			"lastServerData"
#define TOTAL_READING_BOOK		"total_reading_books"
#define TOTAL_BOOKS 			"total_books"
#define TITAL_SECONDS 			"total_seconds"
#define TOTAL_COMPLETE_BOOKS 	"total_completed_books"
#define TOTAL_DAYS				"total_days"
#define RANKING_RATIO 			"ranking_ratio"
#define SERVER_UTC_TIMESTAMP 	"server_utc_timestamp"
#define READING_DISTRIBUTION	"reading_distribution"

#define LASTUPLOADTIME			"lastUploadTime"

const char* ReadingHistoryStat::EventPersonalReaingDataUpdated = "EventPersonalReaingDataUpdated";

SINGLETON_CPP(ReadingHistoryStat)
ReadingHistoryStat::ReadingHistoryStat()
	: m_startTime(0)
	, m_endTime(0)
	, m_lastUploadTime(0)
{
	pthread_mutex_init(&m_lock, 0);
	SubscribeMessageEvent(BookStoreInfoManager::EventGetPersonalExperiencesUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&ReadingHistoryStat::OnGetUpdateServerReadingData),
        this,
        std::tr1::placeholders::_1));

	SubscribeMessageEvent(BookStoreInfoManager::EventPostPersonalExperiencesUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&ReadingHistoryStat::OnPostUpdateServerReadingData),
        this,
        std::tr1::placeholders::_1));

	SubscribeMessageEvent(CAccountManager::EventAccount, *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&ReadingHistoryStat::OnWifiAvailableEvent),
        this,
        std::tr1::placeholders::_1));
	ImportOldData();
}

ReadingHistoryStat::~ReadingHistoryStat()
{
	pthread_mutex_destroy(&m_lock);
}

JsonObjectSPtr ReadingHistoryStat::GetOrCreateSubObject(JsonObjectSPtr jsonObj, const char* field)
{
	JsonObjectSPtr fieldJson = JsonObjectSPtr();
	if(jsonObj.get() && field)
	{
		fieldJson = jsonObj->GetSubObject(field);
		if(!fieldJson.get())
		{
			fieldJson = JsonObject::CreateJsonObject();
			jsonObj->AddJsonObject(field, fieldJson);
		}
	}
	return fieldJson;
}

JsonObjectSPtr ReadingHistoryStat::CreateUserObject(const char* fileName)
{
	JsonObjectSPtr jsonReadingData = JsonObjectSPtr();
	if(fileName)
	{
		MigrateReadingHistory(fileName);
		const string& filePath = PathManager::GetReadingHistoryPath() + fileName;
		string strJson = SystemUtil::ReadStringFromFile(filePath.c_str(), false);
		if(!strJson.empty())
		{
			jsonReadingData = JsonObject::CreateFromString(strJson.c_str());
			if(!jsonReadingData.get())
			{
				unlink(filePath.c_str());
			}
		} 
		else
		{
			jsonReadingData = JsonObject::CreateJsonObject();
		}
	}
	return jsonReadingData;
}

bool ReadingHistoryStat::AddCompletePageNum(int num)
{
	AutoLock lock(&m_lock);
	string email= CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get())
	{
		JsonObjectSPtr jsonNewData = GetOrCreateSubObject(jsonReadingData, NEWDATA);
		if(jsonNewData.get())
		{
			int totalPages = 0;
			jsonNewData->GetIntValue(NEW_READ_PAGES, &totalPages);
			totalPages += num;
			jsonNewData->DeleteJsonObject(NEW_READ_PAGES);
			jsonNewData->AddIntValue(NEW_READ_PAGES, totalPages);
			UpdateDataToFile(email, jsonReadingData);
			return true;
		}
	}
	return false;
}

void ReadingHistoryStat::SetBookOpenTime(int64_t startTime)
{
	m_startTime = startTime;
}

void ReadingHistoryStat::SetBookCloseTime(int64_t endTime)
{
	time_t readingEndTime = time(NULL);
	time_t readingStartTime = readingEndTime - (endTime - m_startTime);

	//一次阅读时间最大间隔为12 小时，超过视为错误数据
	if(readingEndTime > readingStartTime && readingEndTime < (readingStartTime + 3600*12))
	{
		AutoLock lock(&m_lock);
		struct tm fullStartTime = *gmtime(&readingStartTime);
		int totalSecond = readingEndTime - readingStartTime;
		AddNewReadSeconds(totalSecond);
		
		int readingSecondHour[24] = {0};
		int startHour = fullStartTime.tm_hour;
		int startsecond = fullStartTime.tm_min*60 + fullStartTime.tm_sec;

		//判断是否跨时
		if(totalSecond + startsecond > 3600)
		{
			int firstHourReadSecond = 3600 - startsecond;
			readingSecondHour[startHour] += firstHourReadSecond;
			totalSecond -= firstHourReadSecond;
			startHour++;
			while(totalSecond > 3600)
			{
				readingSecondHour[startHour] += 3600;
				startHour++;
				startHour = startHour%24;
				totalSecond -= 3600;
			}
		}
		readingSecondHour[startHour] += totalSecond;
		UpdateDistribution(readingSecondHour);
	}
	//关闭书时默认上传数据，防止用户在一直开wifi 的情况下不能上传阅读数据的问题
	PostReadingData();
}

bool ReadingHistoryStat::AddNewReadSeconds(int seconds)
{
	string email= CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get())
	{
		JsonObjectSPtr jsonNewData = GetOrCreateSubObject(jsonReadingData, NEWDATA);
		if(jsonNewData.get())
		{
			double totalSecond = 0;
			jsonNewData->GetDoubleValue(NEW_READ_SECONDS, &totalSecond);
			totalSecond += seconds;
			jsonNewData->DeleteJsonObject(NEW_READ_SECONDS);
			jsonNewData->AddDoubleValue(NEW_READ_SECONDS, totalSecond);
			UpdateDataToFile(email, jsonReadingData);
			return true;
		}
	}
	return false;
}

bool ReadingHistoryStat::UpdateDistribution(const int* distribution)
{
	const string& email= CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get())
	{
		int readingSecondHour[24] = {0};
		JsonObjectSPtr jsonNewData =  GetOrCreateSubObject(jsonReadingData, NEWDATA);
		JsonObjectSPtr jsonDistribution = GetOrCreateSubObject(jsonNewData, DISTRIBUTION);
		if(jsonDistribution.get())
		{
			for(int i = 0; i < 24; i++)
			{
				char hourInTime[3];
				int secondInHour = 0;
				snprintf(hourInTime, DK_DIM(hourInTime), "%d", i);
				if(jsonDistribution->GetIntValue(hourInTime, &secondInHour))
				{
					readingSecondHour[i] = secondInHour;
				}
				readingSecondHour[i] += distribution[i];
				if(readingSecondHour[i] > 0)
				{
					jsonDistribution->DeleteJsonObject(hourInTime);
					jsonDistribution->AddIntValue(hourInTime, readingSecondHour[i]);
				}
			}
			UpdateDataToFile(email, jsonReadingData);
			return true;
		}
	}
	return false;
}

bool ReadingHistoryStat::GetReadingHistory(ReadingHistory* readingData)
{
	AutoLock lock(&m_lock);
	const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get() && readingData)
	{
        JsonObjectSPtr jsonNewData = jsonReadingData->GetSubObject(NEWDATA);
        double newReadSeconds = 0.0;
        std::vector<int> newDistribution;
        newDistribution.clear();
        if (jsonNewData.get())
        {
            jsonNewData->GetDoubleValue(NEW_READ_SECONDS, &newReadSeconds);
            JsonObjectSPtr distributionJson = jsonNewData->GetSubObject(DISTRIBUTION);
            if(distributionJson)
            {
                for(int i = 0; i < 24; i++)
                {
                    char hourInTime[3];
                    int secondInHour = 0;
                    snprintf(hourInTime, DK_DIM(hourInTime), "%d", i);
                    distributionJson->GetIntValue(hourInTime, &secondInHour);
                    newDistribution.push_back(secondInHour);
                }
            }
        }
        JsonObjectSPtr jsonReadingHistory = jsonReadingData->GetSubObject(LASTSERVERDATA);
        double historyReadSeconds = 0.0;
        std::vector<int> historyDistribution;
        historyDistribution.clear();
		if(jsonReadingHistory.get())
		{
			jsonReadingHistory->GetIntValue(TOTAL_READING_BOOK, &readingData->totalReadingBooks);
			jsonReadingHistory->GetIntValue(TOTAL_BOOKS, &readingData->totalBooks);
			jsonReadingHistory->GetDoubleValue(TITAL_SECONDS, &historyReadSeconds);
			jsonReadingHistory->GetIntValue(TOTAL_COMPLETE_BOOKS, &readingData->totalCompletedBooks);
			jsonReadingHistory->GetIntValue(TOTAL_DAYS, &readingData->totalDay);
			jsonReadingHistory->GetDoubleValue(RANKING_RATIO, &readingData->rankingRatio);
			jsonReadingData->GetIntValue(LASTUPLOADTIME, &readingData->serverUtcTimeStamp);
			JsonObjectSPtr distributionJson = jsonReadingHistory->GetSubObject(READING_DISTRIBUTION);
			if(distributionJson)
			{
				for(int i = 0; i < 24; i++)
				{
					char hourInTime[3];
					int secondInHour = 0;
					snprintf(hourInTime, DK_DIM(hourInTime), "%d", i);
					distributionJson->GetIntValue(hourInTime, &secondInHour);
					historyDistribution.push_back(secondInHour);
				}
			}
        }
        readingData->totalSeconds = historyReadSeconds + newReadSeconds;
        readingData->distribution.clear();
        for(int i = 0; i < 24; i++)
        {
            int secondInHour = 0;
            secondInHour += i < newDistribution.size() ? newDistribution[i] : 0;
            secondInHour += i < historyDistribution.size() ? historyDistribution[i] : 0;
            readingData->distribution.push_back(secondInHour);
        }
        return true;
	}
	return false;
}

bool ReadingHistoryStat::UpdateLastServerData(const string& lastServerData, bool isGet)
{
	JsonObjectSPtr jsonObj = JsonObject::CreateFromString(lastServerData.c_str());
	if(jsonObj.get())
	{
		const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
		JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
		jsonReadingData->DeleteJsonObject(LASTSERVERDATA);
		jsonReadingData->AddJsonObject(LASTSERVERDATA, jsonObj);

		//上传成功后删除已上传的数据
		if(!isGet)
		{
			JsonObjectSPtr jsonNewData = JsonObject::CreateJsonObject();
			jsonReadingData->DeleteJsonObject(NEWDATA);
			jsonReadingData->AddJsonObject(NEWDATA, jsonNewData);

			time_t timeNow = time(NULL);
			jsonReadingData->DeleteJsonObject(LASTUPLOADTIME);
			jsonReadingData->AddIntValue(LASTUPLOADTIME, (int)timeNow);
		}
		UpdateDataToFile(email, jsonReadingData);

		PersonalReadingDataArgs personalReadingDataArgs;
		FireEvent(EventPersonalReaingDataUpdated, personalReadingDataArgs);
		return true;
	}
	return false;
}

bool ReadingHistoryStat::AddNewBookForServer(const char* bookId, const char* title, const DkFileFormat& type)
{
	if(bookId && title)
	{
		const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
		JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
		if(jsonReadingData.get())
		{
			JsonObjectSPtr jsonNewData = GetOrCreateSubObject(jsonReadingData, NEWDATA);
			if(jsonNewData.get())
			{
				JsonObjectSPtr jsonNewBook = GetOrCreateSubObject(jsonNewData, NEW_BOOKS);
				if(jsonNewBook.get())
				{
					JsonObjectSPtr jsonBookInfo = JsonObject::CreateJsonObject();
					jsonBookInfo->AddStringValue(NEWDATA_TITLE, title);
					jsonBookInfo->AddStringValue(NEWDATA_TYPE, GetBookStringFormat(type));
					jsonNewBook->AddJsonObject(bookId, jsonBookInfo);
					UpdateDataToFile(email, jsonReadingData);
					return true;
				}
			}
		}
	}
	return false;
}
	
bool ReadingHistoryStat::AddCompletedBookForServer(const char* bookId, const char* title, const DkFileFormat& type)
{
	if(bookId && title)
	{
		const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
		JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
		if(jsonReadingData.get())
		{
			JsonObjectSPtr jsonNewData = GetOrCreateSubObject(jsonReadingData, NEWDATA);
			if(jsonNewData.get())
			{
				JsonObjectSPtr jsonNewCompleteBook = GetOrCreateSubObject(jsonNewData, NEW_COMPLETED_BOOKS);
				if(jsonNewCompleteBook.get())
				{
					JsonObjectSPtr jsonBookInfo = JsonObject::CreateJsonObject();
					jsonBookInfo->AddStringValue(NEWDATA_TITLE, title);
					jsonBookInfo->AddStringValue(NEWDATA_TYPE, GetBookStringFormat(type));
					jsonNewCompleteBook->AddJsonObject(bookId, jsonBookInfo);
					UpdateDataToFile(email, jsonReadingData);
					return true;
				}
			}
		}
	}
	return false;
}

bool ReadingHistoryStat::AddNewBook(const char* bookID, const char* bookName, const DkFileFormat& bookType)
{
	AutoLock lock(&m_lock);
	const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get())
	{
		JsonObjectSPtr jsonBooksData = GetOrCreateSubObject(jsonReadingData, BOOKSTATUS);
		if(jsonBooksData.get())
		{
			//只有当bookStatus 中没有该书时才加入，避免重复加入
			JsonObjectSPtr jsonBookStatus = jsonBooksData->GetSubObject(bookID);
			if(!jsonBookStatus.get())
			{
				jsonBookStatus = JsonObject::CreateJsonObject();
				jsonBookStatus->AddStringValue(BOOK_TITLE, bookName);
				jsonBookStatus->AddStringValue(BOOK_TYPE, GetBookStringFormat(bookType));
				jsonBookStatus->AddIntValue(BOOK_COMPLETE, 0);
				jsonBooksData->AddJsonObject(bookID, jsonBookStatus);
				UpdateDataToFile(email, jsonReadingData);
			
				//向newData 中存入，供其上传给服务器
				AddNewBookForServer(bookID, bookName, bookType);
				return true;
			}
		}
	}
	return false;
}

bool ReadingHistoryStat::SetBookCompleted(const char* bookID, const char* bookName, const DkFileFormat& bookType, bool isTrialBook)
{
	if(isTrialBook)
	{
		return true;
	}
	AutoLock lock(&m_lock);

	const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get())
	{
		JsonObjectSPtr jsonBooksData = GetOrCreateSubObject(jsonReadingData, BOOKSTATUS);
		if(jsonBooksData.get())
		{
			JsonObjectSPtr jsonBookStatus = jsonBooksData->GetSubObject(bookID);
			if(jsonBookStatus.get())
			{
				int isCompleted = 0;
				//只有当该书的状态为没有完成时才去设置该值，并将该书信息存入newData
				if(jsonBookStatus->GetIntValue(BOOK_COMPLETE, &isCompleted) && isCompleted == 0)
				{
					jsonBookStatus->DeleteJsonObject(BOOK_COMPLETE);
					jsonBookStatus->AddIntValue(BOOK_COMPLETE, 1);
					UpdateDataToFile(email, jsonReadingData);
				
					//向newData 中存入，供其上传给服务器
					AddCompletedBookForServer(bookID, bookName, bookType);
					return true;
				}
			}
		}
	}
	return false;
}

void ReadingHistoryStat::UpdateDataToFile(const string& fileName, JsonObjectSPtr jsonReadingData)
{
	if(!fileName.empty() && jsonReadingData.get())
	{
		string strJson = jsonReadingData->GetJsonString();
		string filePath = PathManager::GetReadingHistoryPath();
		if(!PathManager::MakeDirectoryRecursive(filePath.c_str()))
	    {
	    	return;
	    }
		filePath += fileName;
		FILE* fp = fopen(filePath.c_str(), "wb+");
		fwrite(strJson.c_str(), strJson.length(), 1, fp);
		fclose(fp);
	}
}

void ReadingHistoryStat::FetchPersonalReadingData()
{
	if(WifiManager::GetInstance()->IsConnected() && CAccountManager::GetInstance()->IsLoggedIn())
	{
		BookStoreInfoManager* pManager = BookStoreInfoManager::GetInstance();
	    if (pManager)
	    {
	        pManager->FetchPersonalExperiences();
	    }
	}
}

void ReadingHistoryStat::SplitReadingDataByUser()
{
	if(access(NewReadingHistoryFile, F_OK) != -1)
	{
		string strJson = SystemUtil::ReadStringFromFile(NewReadingHistoryFile, false);
		JsonObjectSPtr jsonReadingData = JsonObject::CreateFromString(strJson.c_str());
		if(jsonReadingData.get())
		{
			vector<string> keys = jsonReadingData->GetAllKeys();
			for(vector<string>::iterator itr = keys.begin(); itr != keys.end(); itr++)
			{
				JsonObjectSPtr jsonUserdata = jsonReadingData->GetSubObject((*itr).c_str());
				UpdateDataToFile(*itr, jsonUserdata);
			}
		}
		unlink(NewReadingHistoryFile);
	}
}

void ReadingHistoryStat::ImportOldData()
{
	SplitReadingDataByUser();
	CDKFileManager* fileManager = CDKFileManager::GetFileManager();
	if(!fileManager)
	{
		return;
	}
	
	ifstream readingBookFile(ReadingBooksInFile, ios::in);
	if(readingBookFile)
	{
		vector<HistortyBookInfo> bookInfo;
		string bookPath;
		while(getline(readingBookFile, bookPath))
		{
			//只有在当前书架中有该书时才导入
			PCDKFile fileInfo = fileManager->GetFileByPath(bookPath);
			if(fileInfo)
			{
				HistortyBookInfo infoTmp;
				infoTmp.bookID = fileInfo->GetBookID();
				infoTmp.bookName = fileInfo->GetFileName();
				infoTmp.bookType = GetBookStringFormat(fileInfo->GetFileFormat());
				//只有当数据正确时才导入
				if(infoTmp.bookID.size() == 32 && !infoTmp.bookType.empty())
				{
					bookInfo.push_back(infoTmp);
				}
			}
		}
		ImportNewReadBook(bookInfo);
		readingBookFile.close();
		unlink(ReadingBooksInFile);
		return;
	}

	ifstream completeBookFile(CompletedBooksInFile,ios::in);
	if(completeBookFile)
	{
		vector<HistortyBookInfo> bookInfo;
		string bookPath;
		while(getline(completeBookFile, bookPath))
		{
			//只有在当前书架中有该书时才导入
			PCDKFile fileInfo = fileManager->GetFileByPath(bookPath);
			if(fileInfo)
			{
				HistortyBookInfo infoTmp;
				infoTmp.bookID = fileInfo->GetBookID();
				infoTmp.bookName = fileInfo->GetFileName();
				infoTmp.bookType = GetBookStringFormat(fileInfo->GetFileFormat());
				//只有当数据正确时才导读
				if(infoTmp.bookID.size() == 32 && !infoTmp.bookType.empty())
				{
					bookInfo.push_back(infoTmp);
				}
			}
		}
		ImportCompleteBook(bookInfo);
		readingBookFile.close();
		unlink(CompletedBooksInFile);
		return;
	}
}

string ReadingHistoryStat::GetBookStringFormat(DkFileFormat bookFormat)
{
	switch (bookFormat)
    {
    case DFF_Text:
        return "txt";
    case DFF_ElectronicPublishing:
		return "epub";
    case DFF_MobiPocket:
        return "mobi";
    case DFF_PortableDocumentFormat:
        return "pdf";
    default:
        break;
    }
	return "";
}

void ReadingHistoryStat::ImportCompleteBook(const vector<HistortyBookInfo>& bookInfo)
{
	const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get())
	{
		JsonObjectSPtr jsonBooksData = GetOrCreateSubObject(jsonReadingData, BOOKSTATUS);
		JsonObjectSPtr jsonNewData = GetOrCreateSubObject(jsonReadingData, NEWDATA);
		if(jsonBooksData.get() && jsonNewData.get())
		{
			for(size_t i = 0; i < bookInfo.size(); i++)
			{
				JsonObjectSPtr jsonBookStatus = jsonBooksData->GetSubObject(bookInfo[i].bookID.c_str());
				if(jsonBookStatus.get())
				{
					int isCompleted = 0;
					//只有当该书的状态为没有完成时才去设置该值，并将该书信息存入newData
					if(jsonBookStatus->GetIntValue(BOOK_COMPLETE, &isCompleted) && isCompleted == 0)
					{
						jsonBookStatus->DeleteJsonObject(BOOK_COMPLETE);
						jsonBookStatus->AddIntValue(BOOK_COMPLETE, 1);
					}
					
					JsonObjectSPtr jsonNewCompleteBook = GetOrCreateSubObject(jsonNewData, NEW_COMPLETED_BOOKS);
					if(jsonNewCompleteBook.get())
					{
						JsonObjectSPtr jsonBookInfo = JsonObject::CreateJsonObject();
						jsonBookInfo->AddStringValue(NEWDATA_TITLE, bookInfo[i].bookName);
						jsonBookInfo->AddStringValue(NEWDATA_TYPE, bookInfo[i].bookType);
						jsonNewCompleteBook->AddJsonObject(bookInfo[i].bookID.c_str(), jsonBookInfo);
					}
				}
			}
			UpdateDataToFile(email, jsonReadingData);
		}
	}
}

void ReadingHistoryStat::ImportNewReadBook(const vector<HistortyBookInfo>& bookInfo)
{
	const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get())
	{
		JsonObjectSPtr jsonBooksData = GetOrCreateSubObject(jsonReadingData, BOOKSTATUS);
		JsonObjectSPtr jsonNewData = GetOrCreateSubObject(jsonReadingData, NEWDATA);
		if(jsonBooksData.get() && jsonNewData.get())
		{
			for(size_t i = 0; i < bookInfo.size(); i++)
			{
				JsonObjectSPtr jsonBookStatus = jsonBooksData->GetSubObject(bookInfo[i].bookID.c_str());
				if(!jsonBookStatus.get())
				{
					jsonBookStatus = JsonObject::CreateJsonObject();
					jsonBookStatus->AddStringValue(BOOK_TITLE, bookInfo[i].bookName);
					jsonBookStatus->AddStringValue(BOOK_TYPE, bookInfo[i].bookType);
					jsonBookStatus->AddIntValue(BOOK_COMPLETE, 0);
					jsonBooksData->AddJsonObject(bookInfo[i].bookID.c_str(), jsonBookStatus);

					JsonObjectSPtr jsonNewBook = GetOrCreateSubObject(jsonNewData, NEW_BOOKS);
					if(jsonNewBook.get())
					{
						JsonObjectSPtr jsonBookInfo = JsonObject::CreateJsonObject();
						jsonBookInfo->AddStringValue(NEWDATA_TITLE, bookInfo[i].bookName);
						jsonBookInfo->AddStringValue(NEWDATA_TYPE, bookInfo[i].bookType);
						jsonNewBook->AddJsonObject(bookInfo[i].bookID.c_str(), jsonBookInfo);
					}
				}
			}
			UpdateDataToFile(email, jsonReadingData);
		}
	}
}

PersonalPostResult ReadingHistoryStat::PostReadingData()
{
	if(!WifiManager::GetInstance()->IsConnected() || !CAccountManager::GetInstance()->IsLoggedIn())
	{
		return PersonalPostResult_NotLogin;
	}
	AutoLock lock(&m_lock);
	const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get())
	{
		int64_t timeNow = SystemUtil::GetUpdateTimeInMs()/1000;
		//如果上传时间间隔小于1个小时，则不上传
		//重启后第一次直接上传
		if(timeNow > (m_lastUploadTime + 3600) || m_lastUploadTime == 0)
		{
			JsonObjectSPtr jsonNewData = jsonReadingData->GetSubObject(NEWDATA);
			//如果数据为空，则不上传数据
			if(jsonNewData.get() && IsNeedToPostReadingHistory(jsonNewData))
			{
				jsonNewData->AddDoubleValue(UTC_TIMESTAMP, time(NULL)*1.0*1000);
				string strJson = jsonNewData->GetJsonString();
				BookStoreInfoManager* pManager = BookStoreInfoManager::GetInstance();
				if (pManager)
				{
					pManager->PostPersonalExperiences(strJson.c_str());
					return PersonalPostResult_OK;
				}
			}
			return PersonalPostResult_DataError;
		}
		return PersonalPostResult_TimeLimit;
	}
	return PersonalPostResult_Error;
}

bool ReadingHistoryStat::OnGetUpdateServerReadingData(const EventArgs& args)
{
	const FetchPersonalExperiencesArgs& readingDataArgs = (const FetchPersonalExperiencesArgs&)args;
	if(readingDataArgs.succeeded)
	{
		UpdateLastServerData(readingDataArgs.jsonReadingData, true);
	}
	return true;
}

bool ReadingHistoryStat::OnPostUpdateServerReadingData(const EventArgs& args)
{
	const FetchPersonalExperiencesArgs& readingDataArgs = (const FetchPersonalExperiencesArgs&)args;
	if(readingDataArgs.succeeded)
	{
        UpdateLastServerData(readingDataArgs.jsonReadingData, false);
        m_lastUploadTime = SystemUtil::GetUpdateTimeInMs()/1000;
	}
	else if(readingDataArgs.errorCode == 8)
	{
        DeleteErrorNewData();
	}
	return true;
}

bool ReadingHistoryStat::OnWifiAvailableEvent(const EventArgs& args)
{
    const AccountEventArgs& loginEvent = (const AccountEventArgs&)args;
    if (loginEvent.IsLoggedIn())
    {
        m_lastUploadTime = 0;
        PostReadingData();
    }
    return true;
}

void ReadingHistoryStat::DeleteErrorNewData()
{
	AutoLock lock(&m_lock);
	const string& email = CAccountManager::GetInstance()->GetEmailFromFile();
	JsonObjectSPtr jsonReadingData = CreateUserObject(email.c_str());
	if(jsonReadingData.get())
	{
		JsonObjectSPtr jsonNewData = jsonReadingData->GetSubObject(NEWDATA);
		if(jsonNewData.get())
		{
			jsonNewData->DeleteJsonObject(DISTRIBUTION);
			jsonNewData->DeleteJsonObject(NEW_READ_SECONDS);
			UpdateDataToFile(email, jsonReadingData);
		}
	}
	return;
}

void ReadingHistoryStat::MigrateReadingHistory(const char* account)
{
	if(account)
	{
		std::vector<std::string> duokanAccount;
		if(CAccountManager::GetInstance()->GetDuokanByXiaomiAccount(account, &duokanAccount))
		{
			for(std::vector<std::string>::iterator itr = duokanAccount.begin(); itr != duokanAccount.end(); itr++)
			{
				string tmpPath = PathManager::GetReadingHistoryPath() + *itr;
				if(access(tmpPath.c_str(), F_OK) != -1)
				{
					string filePath = PathManager::GetReadingHistoryPath() + account;
					rename(tmpPath.c_str(), filePath.c_str());
					break;
				}
			}
		}
		
	}
}

bool ReadingHistoryStat::IsNeedToPostReadingHistory(JsonObjectSPtr jsonNewData)
{
	if(!jsonNewData.get())
	{
		return false;
	}
	double totalSecond = 0;
	if(!jsonNewData->GetDoubleValue(NEW_READ_SECONDS, &totalSecond) || totalSecond <= 0)
	{
		return false;
	}

	int readPages = 0;
	if(!jsonNewData->GetIntValue(NEW_READ_PAGES, &readPages) || readPages <= 0)
	{
		return false;
	}
	return true;
}


bool ReadingHistoryStat::OnBookOpen(
        const char* bookID,
        const char* bookName,
        const DkFileFormat& bookType,
        bool isTrialBook)
{
    if (StringUtil::IsNullOrEmpty(bookID))
    {
        return false;
    }

    SetBookOpenTime(SystemUtil::GetUpdateTimeInMs()/1000);
	if(!isTrialBook)
	{
		return AddNewBook(bookID, bookName, bookType);
	}
    return true;
}

bool ReadingHistoryStat::OnBookClose(int pagesRead)
{
    SetBookCloseTime(SystemUtil::GetUpdateTimeInMs()/1000);
    return AddCompletePageNum(pagesRead);
}
