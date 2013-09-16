#ifndef __XIAOMI_SECURE_REQUEST_H__
#define __XIAOMI_SECURE_REQUEST_H__

#include "DownloadManager/DownloadTask.h"

#include <string>
#include <map>

using namespace dk;
using namespace std;

namespace xiaomi
{
class SecureRequest
{
//encode, sign method notes:
//http://wiki.n.miliao.com/xmg/%E5%B8%90%E5%8F%B7%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E#.E5.AF.B9API.E5.8A.A0.E5.AF.86.E5.92.8C.E7.AD.BE.E5.90.8D
public:
    static DownloadTaskSPtr BuildDownloadTask(const string& url, const map<string ,string>& params,
            map<string, string> cookies, const string& security, DownloadTask::HTTP_METHOD method);
    static string ParseStringResponse(const string& response, const string& security);

    //Decrypt value of Get params or Post datas
    static string DecryptValue(const string& value, const string& security);

    static map<string, string> EncryptParams(DownloadTask::HTTP_METHOD method, const string& url,
            map<string, string> params, const string& security);
private:
    static map<string, string> EncryptParams(const string& method, const string& url,
            map<string, string> params, const string& security);
    static string DecryptResponse(const string& data, const string& security);

    static string AppendUrl(const string& origin, map<string, string> queryPairs);
    static string s_methodName[DownloadTask::METHOD_COUNT];

    static DownloadTaskSPtr BuildGetTask(const string& url, const map<string, string>& params, 
            const map<string, string> cookies);
    static DownloadTaskSPtr BuildPostTask(const string& url, const map<string, string>& params, 
            const map<string, string> cookies);
};//SecureRequest
}
#endif//__XIAOMI_SECURE_REQUEST_H__
