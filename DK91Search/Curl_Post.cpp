#include "Curl_Post.h"
#include "interface.h"
#include <malloc.h>
#include <fstream>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
using namespace DK91SearchAPI;

#define MAX_OUTTIME_CACHE   60*60
#define MAX_CACHE_COUNT     100

#ifdef USE_POST_CACHE

void DKPostTableData::SetRequestData(string szData)
{
    this->szRequestData = szData;
}
string DKPostTableData::GetRequestData()
{
    return this->szRequestData;
}

void DKPostTableData::SetRequestTime(time_t itime)
{
    this->tRequestTime = itime;
}

time_t DKPostTableData::GetRequestTime()
{
    return this->tRequestTime;
}


int PostTable::SelectRequestData(string request)
{
    unsigned int i = 0;
    int oldesttime = 0;
    int newesttime = 0;
    for(;i<this->RequestData.size();i++)
    {
        if(RequestData[i].GetRequestData() == request)
        {
            break;
        }
        if(RequestData[i].GetRequestTime() > newesttime)
        {
            this->NewestId = i;
            newesttime = RequestData[i].GetRequestTime();
        }
        if(RequestData[i].GetRequestTime() < oldesttime)
        {
            this->OldestId = i;
            oldesttime = RequestData[i].GetRequestTime();
        }
    }
    return i;
}

string PostTable::GetReturnData(string request)
{
    string szReturnData;
    this->CurReturnDataId = this->SelectRequestData(request);
    if(this->CurReturnDataId < (int) this->RequestData.size())
    {
        time_t curtime = time(NULL);
        time_t requesttime = this->RequestData[this->CurReturnDataId].GetRequestTime();
        if((curtime > requesttime) && (curtime - requesttime) < MAX_OUTTIME_CACHE)
        {
            char szXmlName[64];
            sprintf(szXmlName,"%d.xml",this->CurReturnDataId);
            fstream dict(szXmlName,ios::in|ios::binary);
            if(dict)
            {
                string st1;
                while(dict >> st1)
                {
                    szReturnData+=st1;
                }
                return szReturnData;
            }

        }
    }
    else
    {
        this->CurReturnDataId = -1;
    }
    return szReturnData;
}

void PostTable::InsertReturnData(string requestdata,string returndata)
{
    char szXmlName[64];
    int iWriteID = -1;
    time_t t1 = time(NULL);
    if((this->CurReturnDataId >= 0) && (this->CurReturnDataId < (int)this->RequestData.size()) && (this->RequestData[this->CurReturnDataId].GetRequestData() == requestdata))
    {
        iWriteID = this->CurReturnDataId;
    }
    else
    {
        iWriteID = this->SelectRequestData(requestdata);
        if(iWriteID == (int)this->RequestData.size())
        {
            if(iWriteID >= MAX_CACHE_COUNT)
            {
                if(this->RequestData[this->NewestId].GetRequestTime() > t1)
                {
                    this->RequestData[this->NewestId].SetRequestData(requestdata);
                    iWriteID = this->NewestId;
                }
                else
                {
                    this->RequestData[this->OldestId].SetRequestData(requestdata);
                    iWriteID = this->OldestId;
                }
            }
            else
            {
                DKPostTableData  data;
                data.SetRequestData(requestdata);
                this->RequestData.push_back(data);
            }
        }
    }
    this->RequestData[iWriteID].SetRequestTime(t1);
    sprintf(szXmlName,"%d.xml",iWriteID);
    fstream dict(szXmlName,ios::out|ios::binary);
    if(dict)
    {
        dict.write(returndata.c_str(),returndata.length());
    }
}
#endif

///////////////////////////////////////////////////////////////////////
// Namespace: DK91SearchAPI
// Method: PickDomain
// Author: Xuk
// Written:  2011.8.18
// Reviewed: N/A
// Reviewer: N/A
// Description:  Pick Domain from url,and must contain http(s)://
// Returns: succeed return http(s):// and faild return ""
/////////////////////////////////////////////////////////////////////
string PickDomain(const string& url)
{
    string strTmp = "://";
    size_t pos = url.find(strTmp);
    if(pos == string::npos)
    {
        return "";
    }
    size_t pos1 = url.find('/',pos + 3);
    if(pos1 != string::npos)
    {
        string strSub = "";
        strSub = url.substr(0,pos1 - 0);
        return strSub;
    }
    return url;
}

///////////////////////////////////////////////////////////////////////
// Namespace: DK91SearchAPI
// Method: ReURL2AbURL
// Author: Xuk
// Written:  2011.8.18
// Reviewed: N/A
// Reviewer: N/A
// Description: transform relative url to absolute url
// Returns: succeed return http(s)://xxx and faild return ""
/////////////////////////////////////////////////////////////////////

string DK91SearchAPI::ReURL2AbURL(
const string& ab_url, // 绝对url
const string& re_url  // 相对url
)  
{
    string domain = "", root = ""; //域名 ， 当前目录  

    size_t pos = 0;
    if(re_url.find("://") != string::npos)//这不是一个相对url，直接返回  
    {
        return re_url;
    }

    domain = PickDomain(ab_url);//返回格式：http(s)://test.com/  
    if(domain.empty())
    {
        return "";
    }

    //root 格式：http(s)://test.com/test  
    if('/' == ab_url[ab_url.size()-1])
    {
        root = ab_url.substr(0,ab_url.size()-1);
    }
    else
    {
        root = ab_url;  
        size_t iPos1 = ab_url.find("://");
        size_t iPos = ab_url.rfind('/');
        if(iPos != std::string::npos && iPos1 != std::string::npos)
        {
            if(iPos - iPos1 > 3)
            {
                string strSubEx = ab_url.substr(iPos);
                if(strSubEx.find('.') != string::npos)
                {
                    root = ab_url.substr(0,iPos);
                }
            }
        }
    }

    string strDesURL = re_url;
    if (re_url[0] == '/')  
    {
        //全局相对路径其实这一段的所谓相对路径，仅仅是指不含有主机名的路径而已。  
        //主要特征就是在相对URL的最前面是含有"/"的  
        strDesURL = domain + re_url;  
    }  
    else 
    {  
        while ((pos = strDesURL.find("../")) == 0)  
        {  
            root = root.substr(0, root.rfind('/'));  
            //如果相对url和当前目录层次不配对，会导致root == "http(s?):/"  
            if(root.find("://") == string::npos)
            {
                return "";
            }
            strDesURL = strDesURL.substr(pos + 3);  
        }  
        strDesURL = root + '/' + strDesURL;  
    }  
    return strDesURL;  

} 


//TODO:最后一个参数改为一个结构体指针，结构体里有一个字符串指针及字符串的长度
size_t write91ResultData(void *buffer, size_t size, size_t nmemb, void **szdata)
{
    DebugPrintf(DLC_XU_KAI,"size is %d nmemb is %d",size , nmemb);
    if(!szdata || !buffer)
    {
        DebugPrintf(DLC_XU_KAI, "szdata is NULL\n");
        return 0;
    }
    char *sztmp = (char *)(*szdata);
    int _ioldlen = 0;
    if(sztmp)
    {
        _ioldlen = strlen(sztmp);
    }
    sztmp = (char *)realloc(sztmp,_ioldlen + size * nmemb + 1);
    DebugPrintf(DLC_XU_KAI,"_ioldlen is %d size * nmemb is %d",_ioldlen,size * nmemb);
    if(sztmp)
    {
        memcpy(sztmp + _ioldlen,buffer,size * nmemb);
        sztmp[_ioldlen + size * nmemb] = 0;
    }
    *szdata = sztmp;
    return size*nmemb;
}

string DK91SearchAPI::Post(string url,string parameters,int *errcode,string userAgent, long timeout)
{
    return SendRequest(url, parameters, errcode, HTTP_POST,userAgent, timeout);
}

string DK91SearchAPI::Get(string url,string parameters,int *errcode,string userAgent, long timeout)
{
    return SendRequest(url, parameters, errcode, HTTP_GET,userAgent, timeout);
}


string DK91SearchAPI::Put(string url,string parameters,int *errcode,string userAgent, long timeout)
{
    return SendRequest(url, parameters, errcode, HTTP_PUT,userAgent, timeout);
}


string DK91SearchAPI::Delete(string url,string parameters,int *errcode,string userAgent, long timeout)
{
    return SendRequest(url, parameters, errcode, HTTP_DELETE,userAgent, timeout);
}

string DK91SearchAPI::SendRequest(string url,string params,int *errcode, HttpRequestType requestType,string userAgent, long timeout)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "DK91SearchAPI::SendRequest url=%s, params=%s, request type=%d", url.c_str(), params.c_str(), requestType);
    if(errcode == NULL || url.length() == 0)
    {
        return "";
    }
    
    *errcode = 0;
    CURL *curl = NULL;
    char *_szresultdata = NULL;
    char errbuf[CURL_ERROR_SIZE] = {0};
    string st = "";

    curl = curl_easy_init();
    string tmp = url + (params.empty() ? "":"?")+params;
    switch (requestType)
    {
        case HTTP_GET:
            {
                curl_easy_setopt(curl, CURLOPT_URL,tmp.c_str()); 
                curl_easy_setopt(curl, CURLOPT_HTTPGET,1L);
            }
            break;
       case HTTP_POST:
            {
                curl_easy_setopt(curl, CURLOPT_URL,url.c_str());  
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());                
            }
            break;
        case HTTP_PUT:
            {
                curl_easy_setopt(curl, CURLOPT_URL,tmp.c_str()); 
                curl_easy_setopt(curl, CURLOPT_PUT,1L);
                curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, 0);
                curl_easy_setopt(curl, CURLOPT_INFILESIZE, 0);
            }
            break;
        case HTTP_DELETE:
            {
                curl_easy_setopt(curl, CURLOPT_URL,tmp.c_str()); 
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,"DELETE");
            }
            break;
        default:
            {
                curl_easy_setopt(curl, CURLOPT_URL,tmp.c_str()); 
                curl_easy_setopt(curl, CURLOPT_HTTPGET,1L);
            }
            break;
    }

    if(!userAgent.empty())
    {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,&_szresultdata);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write91ResultData);
    curl_easy_setopt(curl, CURLOPT_AUTOREFERER ,1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT , timeout);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT , timeout*2);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,1);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    CURLcode ret_err = curl_easy_perform(curl);
    if(ret_err)
    {
        *errcode = ret_err;
        DebugPrintf(DLC_DIAGNOSTIC,"errbuf is %s",errbuf);
    }
    curl_easy_cleanup(curl);
    //DebugPrintf(DLC_DIAGNOSTIC,"DK91SearchAPI::SendRequest used curl end");
    if(_szresultdata)
    {
        DebugPrintf(DLC_DIAGNOSTIC,"_szresultdata : %s",_szresultdata);
        st=_szresultdata;
        free(_szresultdata);
        _szresultdata = NULL;
    }
    return st;
}
