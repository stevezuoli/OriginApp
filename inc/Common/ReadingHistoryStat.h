#ifndef __READINGHISTORYSTAT_H__
#define __READINGHISTORYSTAT_H__

#include "singleton.h"
#include "Utility/JsonObject.h"
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include "GUI/EventListener.h"
#include "Common/File_DK.h"

//旧阅读数据
#define ReadingHistoryFile "/mnt/us/system/reading.dk"
#define ReadingBooksInFile "/mnt/us/system/readingBooks.dk"
#define CompletedBooksInFile "/mnt/us/system/completedBooks.dk"

//新版的阅读数据
#define NewReadingHistoryFile "/mnt/us/system/readingHistory.dk"

struct ReadingHistory
{
	ReadingHistory()
	{
		serverUtcTimeStamp = 0;
		totalReadingBooks = 0;
		totalSeconds = 0.0;
		totalBooks = 0;
		totalCompletedBooks = 0;
		totalDay = 0;
		rankingRatio = 0.0;
	}
	int totalReadingBooks;
	double totalSeconds;
	int totalBooks;
	int totalCompletedBooks;
	int totalDay;
	double rankingRatio;
	int serverUtcTimeStamp;
	std::vector<int> distribution;
};

struct HistortyBookInfo
{
	std::string bookID;
	std::string bookName;
	std::string bookType;
};

enum PersonalPostResult
{
	PersonalPostResult_OK,
	PersonalPostResult_Error,
	PersonalPostResult_NotLogin,
	PersonalPostResult_TimeLimit,
	PersonalPostResult_DataError
};

class PersonalReadingDataArgs: public EventArgs
{
public:
	PersonalReadingDataArgs()
	{
	}
	
	virtual EventArgs* Clone() const
	{
		return new PersonalReadingDataArgs(*this);
	}
};


class ReadingHistoryStat : public EventSet, EventListener
{
        SINGLETON_H(ReadingHistoryStat)
public:
	static const char* EventPersonalReaingDataUpdated;

public:
	


	//获得个人阅历(该数据为服务器端数据)
	bool GetReadingHistory(ReadingHistory* readingData);

	//上传数据到服务器
	PersonalPostResult PostReadingData();

	//下载服务器端数据
	void FetchPersonalReadingData();

	//当某本书看完时调用此函数
	bool SetBookCompleted(const char* bookID, const char* bookName, const DkFileFormat& bookType, bool isTrialBook);


	bool OnBookOpen(const char* bookID, const char* bookName, const DkFileFormat& bookType, bool isTrialBook);
	bool OnBookClose(int pagesRead);
private:
	ReadingHistoryStat();
	virtual ~ReadingHistoryStat();
    //
	//当阅读新书是调用此函数，如果数据重复则不添加
	bool AddNewBook(const char* bookID, const char* bookName, const DkFileFormat& bookType);
    //
	//图书打开时设置此值
    void SetBookOpenTime(int64_t);

	//阅读图书结束时设置此值
	//并根据m_endTime  m_startTime 计算此次阅读的秒数和时间分布
	void SetBookCloseTime(int64_t);

	//设置阅读的页数
	bool AddCompletePageNum(int num);
	JsonObjectSPtr GetOrCreateSubObject(JsonObjectSPtr jsonObj, const char* field);
	JsonObjectSPtr CreateUserObject(const char* fileName);

	//添加新增阅读秒数
	bool AddNewReadSeconds(int seconds);

	//添加时间分布
	bool UpdateDistribution(const int* distribution);

	//添加需要向服务器端提交的数据
	bool AddNewBookForServer(const char* bookId, const char* title, const DkFileFormat& type);
	bool AddCompletedBookForServer(const char* bookId, const char* title, const DkFileFormat& type);

	//更新最后一次从服务器端获取的个人阅读数据
	bool UpdateLastServerData(const std::string& lastServerData, bool isGet);

	//更新文件
	void UpdateDataToFile(const string& fileName, JsonObjectSPtr jsonReadingData);

	//导入旧数据
	void ImportOldData();

	// 将NewReadingHistoryFile拆分为多个文件，每个文件以用户名命名
	void SplitReadingDataByUser();

	//判断有阅读历史，如果有的话可以上传，否则不可以
	bool IsNeedToPostReadingHistory(JsonObjectSPtr jsonNewData);

	//如果上传后返回errorcode 为8, 则删除本地错误数据
	void DeleteErrorNewData();

	// 迁移本地个人阅历相关数据
	void MigrateReadingHistory(const char* account);

	void ImportNewReadBook(const std::vector<HistortyBookInfo>& bookInfo);
	void ImportCompleteBook(const std::vector<HistortyBookInfo>& bookInfo);

	std::string GetBookStringFormat(DkFileFormat bookFormat);

	bool OnGetUpdateServerReadingData(const EventArgs& args);
	bool OnPostUpdateServerReadingData(const EventArgs& args);
	bool OnWifiAvailableEvent(const EventArgs& args);

private:
	//非UTC 时间，而是系统启动到现在的时间，以秒为单位
	int64_t m_startTime;
	int64_t m_endTime;
	int64_t m_lastUploadTime;
	pthread_mutex_t m_lock;
};

#endif

