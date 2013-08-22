#ifndef EBOOK_QIYITEST_CURL_POST_H
#define EBOOK_QIYITEST_CURL_POST_H
#include <iostream>
#include<string>
#include<vector>
#include<time.h>



namespace DK91SearchAPI{

enum HttpRequestType
{
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE
};

#ifdef USE_POST_CACHE
    class DKPostTableData
    {
    public:
        void SetRequestData(std::string szData);
        std::string GetRequestData();
        void SetRequestTime(time_t itime);
        time_t GetRequestTime();
    private:
        std::string szRequestData;
        time_t tRequestTime;
    };
#endif
    std::string SendRequest(std::string url, std::string params,int *errcode, HttpRequestType requestType = HTTP_GET,std::string userAgent = "", long timeout = 15);
    std::string Get(std::string url, std::string request,int *errcode,std::string userAgent = "", long timeout = 15);
    std::string Post(std::string url, std::string request,int *errcode,std::string userAgent = "", long timeout = 15);
    std::string Put(std::string url, std::string request,int *errcode,std::string userAgent = "", long timeout = 15);
    std::string Delete(std::string url, std::string request,int *errcode,std::string userAgent = "", long timeout = 15);
    std::string ReURL2AbURL(const std::string &ab_url,const std::string &re_url);

#ifdef USE_POST_CACHE
    class PostTable
    {
    public:
        std::string GetReturnData(std::string request);
        void InsertReturnData(std::string requestdata,std::string returndata);
    private:
        std::vector <DKPostTableData> RequestData;
        int SelectRequestData(std::string request);
        int CurReturnDataId;
        int OldestId;
        int NewestId;
    };
#endif
}
#endif
