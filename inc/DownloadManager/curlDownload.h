#include <iostream>
#include <string>
#include <pthread.h>
#include <vector>
#include <semaphore.h>

class IDownloadTask;
namespace Download
{

#define MAX_URL_LENGTH 1024
#define MAX_NAME_LENGTH 1024
#define RANGE_BUF_LENGTH 128

    struct UrlFileInfo
    {
        unsigned int    uLength;                            // 长度
        char            szFileName[MAX_NAME_LENGTH];        // 文件名(http header优先)
        char            szRange[RANGE_BUF_LENGTH];          // range，传入有效
        bool            bSupportBreakpointContinuingly;     // 是否支持断点续传(有待完善)
        char            szUrl[MAX_URL_LENGTH];				// 最终的有效URL
        char            origUrl[MAX_URL_LENGTH];			// 原始URL
        char            szUesrAgent[RANGE_BUF_LENGTH];      // userAgent
    };


    enum DLResult{
        DOWNLOAD_OK,
        DOWNLOAD_NOT_SUPPORT_BREAKPOINT_CONTINUINGLY,
        DOWNLOAD_INVALID_ARGUMENT,
        DOWNLOAD_INIT_ERROR,
        DOWNLOAD_EXECUTE_ERROR,
        DOWNLOAD_NETWORK_ERROR,
        DOWNLOAD_FILE_ERROR,
        DOWNLOAD_OUT_OF_MEMORY,
        DOWNLOAD_HIBERNATED,
        DOWNLOAD_ABORTED,
		DOWNLOAD_WRITE_ERROR
    };

    struct TaskState
    {
        bool *taskworking;                // 是否正在下载
        unsigned int *pDLPercentage;    // 当前的下载进度百分比,用于随时更新DownloadManager里TASK的下载进度
        unsigned int uDownloadedBytes;    // 已下载的字节数
        unsigned int uCurTransferBytes;    // 当前传输已传输的字节数
        unsigned int uTotalBytes;        // 总的字节数
    };

    struct HttpHeaderState
    {
	public:
		HttpHeaderState()
			: bIsFromBreak(false)
			, uRequestCode(0)
		{

		}
		~HttpHeaderState(){}
        bool bIsFromBreak;        // 表示正在断点续传
        unsigned int uRequestCode;    // http头返回的状态码
    };

	struct HttpHeaderInfo
	{
	public:
		HttpHeaderInfo()
			: m_clsHeaderState()
			, m_strLocation("")
			, m_strAcceptRanges("")
			, m_strFileName("")
			, m_strContentType("")
		{

		}
		~HttpHeaderInfo(){}
		HttpHeaderState m_clsHeaderState;
		std::string m_strLocation;			// 最终有效链接地址
		std::string m_strAcceptRanges;		// http头的Accept-Ranges字段
		std::string m_strFileName;			// http头的Content-Disposition:filename字段
		std::string m_strContentType;		// http头的Content-Type字段
	};
    
    struct WriteDataInfo
    {
        HttpHeaderInfo* headerInfo;
        FILE *fp;
    };

    DLResult getFileInfoFromWeb(
        const std::string& strUrl,  //参数1，搜索引擎提供的URL
        struct UrlFileInfo* info    //参数2，文件信息结构体，可以获取到：http header指定文件名，文件长度，断点续传支持(有待完善)。传入szRange有效
    );

    DLResult download2Path(
        const std::string& strUrl,			// 参数1，url。
		struct UrlFileInfo* _pclsInfo,		//参数2，文件信息结构体，可以获取到：http header指定文件名，文件长度，断点续传支持(有待完善)。传入szRange有效
        const char* lpszPath,				// 下载到路径，只在未指定文件名时有效。
        IDownloadTask* task					// 下载状态，用来控制下载的暂停，下载进度等
    );

	// 从HTTP头里获得文件的一些基本信息
	bool GetUrlFileInfo(const HttpHeaderInfo &_clsHeader,struct UrlFileInfo* _pInfo);

    size_t WriteDataCallBack(void *ptr, size_t size, size_t nmemb, void *data);

}
