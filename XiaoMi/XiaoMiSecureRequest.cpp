#include "XiaoMi/XiaomiSecureRequest.h"
#include "XiaoMi/CloudCoder.h"
#include "Utility/StringUtil.h"
#include "Utility/HttpUtil.h"
#include "Utility/EncodeUtil.h"
#include "KernelDef.h"

namespace xiaomi
{
string SecureRequest::s_methodName[DownloadTask::METHOD_COUNT] = {"GET", "POST", "POSTFORM", "PUT", "DELETE"};

DownloadTaskSPtr SecureRequest::BuildDownloadTask(const string& url, const map<string ,string>& params,
            map<string, string> cookies, const string& security, DownloadTask::HTTP_METHOD method)
{
    if (method == DownloadTask::GET || method == DownloadTask::POST)
    {
        map<string, string> requestParams =
            EncryptParams(s_methodName[method], url, params, security);

        switch (method)
        {
            case DownloadTask::GET:
                return BuildGetTask(url, requestParams, cookies);
            case DownloadTask::POST:
                return BuildPostTask(url, requestParams, cookies);
            default:
                break;
        }
    }

    return DownloadTaskSPtr();
}

string SecureRequest::ParseStringResponse(const string& response, const string& security)
{
    return DecryptResponse(response, security);
}

map<string, string> SecureRequest::EncryptParams(DownloadTask::HTTP_METHOD method, const string& url,
            map<string, string> params, const string& security)
{
    if (method < 0 || method >= DownloadTask::METHOD_COUNT)
    {
        return map<string, string>();
    }

    return EncryptParams(s_methodName[method], url, params, security);
}

map<string, string> SecureRequest::EncryptParams(const string& method, const string& url,
            map<string, string> params, const string& security)
{
    map<string, string> requestParams;
    for (map<string, string>::const_iterator iter = params.begin(); iter != params.end(); ++iter)
    {
        //do NOT encrypt params whose name starts with underscore (_)
        if (!utility::StringUtil::StartWith(iter->first.c_str(), "_"))
        {
            requestParams[iter->first] = CloudCoder::EncodeString(security, iter->second);
        }
    }

    string signature = CloudCoder::GenerateSignature(method, url, requestParams, security);
    requestParams["signature"] = signature;

    return requestParams;
}

string SecureRequest::DecryptResponse(const string& data, const string& security)
{
    std::vector<unsigned char> response = utility::EncodeUtil::Base64Decode(data.c_str(), data.length());
    string responseData(response.begin(), response.end());
    return CloudCoder::DecodeString(security, responseData);
}

string SecureRequest::DecryptValue(const string& value, const string& security)
{
    //string decodedValue = utility::HttpUtil::UrlDecode(value.c_str());
    std::vector<unsigned char> baseDecodedValue = 
        utility::EncodeUtil::Base64Decode(value.c_str(), value.length());
    string data(baseDecodedValue.begin(), baseDecodedValue.end());
    return CloudCoder::DecodeString(security, data);
}

string SecureRequest::AppendUrl(const string& origin, map<string, string> queryValues)
{
    string url(origin);
    bool first = origin.find('?') == string::npos;
    string query;
    for (map<string, string>::const_iterator iter = queryValues.begin(); iter != queryValues.end(); ++iter)
    {
        query.clear();
        url.append(1, first ? '?' : '&');
        query.append(iter->first).append(1, '=').append(utility::HttpUtil::UrlEncode(iter->second.c_str()));
        url.append(query);
        first = false;
    }

    return url;
}

DownloadTaskSPtr SecureRequest::BuildGetTask(const string& url, const map<string, string>& params, 
            const map<string, string> cookies)
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    if (!task)
    {
        return DownloadTaskSPtr();
    }

    for (map<string, string>::const_iterator iter = cookies.begin(); iter != cookies.end(); ++iter)
    {
        task->AddCookie(iter->first.c_str(), iter->second.c_str());
    }

    task->SetUrl(AppendUrl(url, params).c_str());

    return task;
}

DownloadTaskSPtr SecureRequest::BuildPostTask(const string& url, const map<string, string>& params, 
            const map<string, string> cookies)
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    if (!task)
    {
        return DownloadTaskSPtr();
    }

    task->SetMethod(DownloadTask::POST);
    for (map<string, string>::const_iterator iter = cookies.begin(); iter != cookies.end(); ++iter)
    {
        task->AddCookie(iter->first.c_str(), iter->second.c_str());
    }

    task->SetUrl(url.c_str());

    for (map<string, string>::const_iterator iter = params.begin(); iter != params.end(); ++iter)
    {
        task->AddPostData(iter->first.c_str(), iter->second.c_str());
    }

    return task;
}

}
