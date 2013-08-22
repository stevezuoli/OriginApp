#include "DownloadManager/curlDownload.h"
#include <string.h>
#include "curl/curl.h"
#include <algorithm>
#include <sys/stat.h>
#include "interface.h"
#include "Utility/Misc.h"
#include "DownloadManager/IDownloadTask.h"
#include "Utility/SystemUtil.h"

using dk::utility::SystemUtil;

#ifdef _MSC_VER
#pragma comment(lib, "libcurl_imp.lib")
#define strcasecmp stricmp
#define strncasecmp  strnicmp
#else
//  #define FALSE false
//  #define TRUE true
#include <string>
#endif

using namespace std;

#define FIEL_SIZE_STR "Content-Range:"
#define FILE_LOC_STR "ocation: %s\r\n"
#define FILE_TRANS_CONT_STR "Accept-Ranges: %s\n"
#define FILE_NAME_REG "filename="
#define FILE_CONTENT_DISPOSITION "Content-Disposition:"
#define HTTP_RANGE_QUERY  "Range:bytes=2-3"
#define HTTP_HEADER "HTTP/"

namespace Download{


    DLResult downloadProgressFunc(void *progress_data,
        double t, /* dltotal */
        double d, /* dlnow */
        double ultotal,
        double ulnow)
    {
        //return non-zero to abort curl connection
        IDownloadTask * task = (IDownloadTask*)progress_data;
        if(NULL == task)
        {
            return DOWNLOAD_OK;
        }
        if (!task->TaskIsWorking())
        {
            return DOWNLOAD_ABORTED;
        }

        unsigned int utotal = (unsigned int)t;
        unsigned int udnow = (unsigned int)d;
        if(utotal > 0 && udnow > 0)
        {
            task->OnProgressUpdate(utotal, udnow);
        }
        return DOWNLOAD_OK;
    }

	// Parse http header information
    static size_t WriteHeaderCallback(void *ptr, size_t size, size_t nmemb, void *data)
    {
        if(NULL == ptr || NULL == data || size * nmemb <= 1)
        {
            return size *nmemb;
        }
        const char *pTmp = (const char *)ptr;
        const char *pHttp = NULL;
        HttpHeaderInfo *pHeaderInfo = (HttpHeaderInfo *)data;
		char szTmp[1024] = {0};
        if((pHttp = strstr(pTmp,HTTP_HEADER)) != NULL)
        {
            pHttp = strchr(pHttp,32);
            if(pHttp)
            {
                pHeaderInfo->m_clsHeaderState.uRequestCode = atoi(pHttp);
                int icode = (pHeaderInfo->m_clsHeaderState.uRequestCode) / 100;
                if((icode != 2) && (icode != 3))
                {
                    return 0;
                }
                else if(pHeaderInfo->m_clsHeaderState.uRequestCode != 206 && pHeaderInfo->m_clsHeaderState.bIsFromBreak && (icode != 3))
                {
                    return 0;
                }
            }
        }
		else if(sscanf(pTmp + 1,FILE_LOC_STR,szTmp))
		{
			pHeaderInfo->m_strLocation = szTmp;
		}
		else if((pHeaderInfo->m_clsHeaderState.uRequestCode) / 100 == 2)// 说明请求数据成功，开始记录http头信息
		{
			const char *pDisposition = NULL;
			if(sscanf(pTmp, FILE_TRANS_CONT_STR,szTmp))
			{
				pHeaderInfo->m_strAcceptRanges = szTmp;
			}
			else if((pDisposition = strstr(pTmp, FILE_CONTENT_DISPOSITION)))
			{
				const char *pFileName = strstr(pDisposition,FILE_NAME_REG);
				if(pFileName)
				{
					DebugPrintf(DLC_XU_KAI,"pFileName is %s",pFileName);
					pFileName += strlen(FILE_NAME_REG);
					const char *pMovePointer = pFileName;
					int iFileNameLen = 0;
					while(*pMovePointer)
					{
						if(*pMovePointer < 30 || *pMovePointer == '"')
						{
							break;
						}
						pMovePointer++;
						iFileNameLen++;
					}
					pHeaderInfo->m_strFileName = string(pFileName,iFileNameLen);
					DebugPrintf(DLC_XU_KAI,"pHeaderInfo->m_strFileName 2222222 is %s",pHeaderInfo->m_strFileName.c_str());
				}
			}
		}
        return (size_t)(size * nmemb);
    }

	DLResult CurlExecuteRequest(
            struct UrlFileInfo *_pclsInfo,
            IDownloadTask* task,
            size_t (*_pWriteFunction)(void *ptr, size_t size, size_t nmemb, void *data),
            void *_pWriteData,bool _bTimeOut)
	{
		DLResult nRet = DOWNLOAD_OK;

		if(NULL == _pclsInfo)
		{
            DebugPrintf(DLC_DIAGNOSTIC,"NULL == _pclsInfo ..............................");
			return DOWNLOAD_INIT_ERROR;
		}

		CURL *_pCurl = curl_easy_init();
		if(!_pCurl)
		{
            DebugPrintf(DLC_DIAGNOSTIC,"NULL == _pCurl ..............................");
			nRet = DOWNLOAD_INIT_ERROR;
			return nRet;
		}

		HttpHeaderInfo clsReturnHeaderInfo;
		clsReturnHeaderInfo.m_strLocation = _pclsInfo->szUrl;

        WriteDataInfo writeData = {&clsReturnHeaderInfo, (FILE *)_pWriteData};
		if(strlen(_pclsInfo->szRange))
		{
			curl_easy_setopt(_pCurl, CURLOPT_RANGE, _pclsInfo->szRange);
			clsReturnHeaderInfo.m_clsHeaderState.bIsFromBreak = 1;
		}

		if(strlen(_pclsInfo->szUesrAgent))
		{
			curl_easy_setopt(_pCurl, CURLOPT_USERAGENT, _pclsInfo->szUesrAgent);
		}

		curl_easy_setopt(_pCurl, CURLOPT_HEADERFUNCTION, WriteHeaderCallback);
		curl_easy_setopt(_pCurl, CURLOPT_WRITEHEADER, &clsReturnHeaderInfo);

		curl_easy_setopt(_pCurl, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(_pCurl, CURLOPT_PROGRESSFUNCTION, downloadProgressFunc);
		if(NULL != task)
		{
			curl_easy_setopt(_pCurl, CURLOPT_PROGRESSDATA, task);
		}


		curl_easy_setopt(_pCurl, CURLOPT_WRITEFUNCTION,_pWriteFunction);
		curl_easy_setopt(_pCurl, CURLOPT_WRITEDATA, &writeData);

		//curl_easy_setopt(_pCurl, CURLOPT_AUTOREFERER, 1);
		//curl_easy_setopt(_pCurl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(_pCurl, CURLOPT_FORBID_REUSE, 1);
		curl_easy_setopt(_pCurl, CURLOPT_NOSIGNAL, 1L);
#ifdef _DEBUG
		curl_easy_setopt(_pCurl, CURLOPT_VERBOSE, 1);
#endif

		if(_bTimeOut)
		{
			curl_easy_setopt(_pCurl, CURLOPT_CONNECTTIMEOUT , 30L);
		}
		CURLcode res = CURLE_OK;

		do 
		{
			string strUrlEncode = UrlEncodeWithCurl(_pCurl,clsReturnHeaderInfo.m_strLocation,"?&:/\\=+-\%");
			if(!strUrlEncode.empty())
			{
                //DebugPrintf(DLC_DIAGNOSTIC,"strUrlEncode = %s ..............................",strUrlEncode.c_str());
				curl_easy_setopt(_pCurl,CURLOPT_URL,strUrlEncode.c_str());
			}
			else
			{
                //DebugPrintf(DLC_DIAGNOSTIC,"clsReturnHeaderInfo.m_strLocation.c_str() = %s ..............................",clsReturnHeaderInfo.m_strLocation.c_str());
				curl_easy_setopt(_pCurl,CURLOPT_URL,clsReturnHeaderInfo.m_strLocation.c_str());
				curl_easy_setopt(_pCurl, CURLOPT_AUTOREFERER, 1);
				curl_easy_setopt(_pCurl, CURLOPT_FOLLOWLOCATION, 1);
			}
			res = curl_easy_perform(_pCurl);
		} while (res == CURLE_OK && (clsReturnHeaderInfo.m_clsHeaderState.uRequestCode / 100 == 3));

		if(res == CURLE_OK)
		{
			//DebugPrintf(DLC_DIAGNOSTIC,"res == CURLE_OK  ..............................");
			nRet = DOWNLOAD_OK;
		}
		else if(res == CURLE_WRITE_ERROR)
		{
			DebugPrintf(DLC_DIAGNOSTIC,"CURLE_WRITE_ERROR ..............................");
			if(clsReturnHeaderInfo.m_clsHeaderState.uRequestCode == 200 && clsReturnHeaderInfo.m_clsHeaderState.bIsFromBreak)
			{
				_pclsInfo->bSupportBreakpointContinuingly = false;
				nRet = DOWNLOAD_NOT_SUPPORT_BREAKPOINT_CONTINUINGLY;
			}
			else
			{
				nRet = DOWNLOAD_WRITE_ERROR;
			}
		}
		else if (res == CURLE_ABORTED_BY_CALLBACK)
		{
			//TODO: try to set the result as canceled
			//DebugPrintf(DLC_DIAGNOSTIC,"The DownloadPthread is canceled");
			nRet = DOWNLOAD_ABORTED;
		}
		else
		{
			DebugPrintf(DLC_DIAGNOSTIC,"download2File download network error nRet : %d",nRet);
			nRet = DOWNLOAD_NETWORK_ERROR;
		}

		curl_easy_cleanup(_pCurl);
		GetUrlFileInfo(clsReturnHeaderInfo,_pclsInfo);
		return nRet;
	}
    //参数1，url。
    //参数2，需要下载的内容range，字符串长度大于0时有效，否则表示全文件。如前1024个字节内容表示为0-1023
    //参数3，文件句柄，需要自己seek到正确位置，本函数只把range区间内容写到句柄传入时的位置。
    //参数4，下载进度，本进度只表示range内容的下载进度而非全文件。
    DLResult download2File(const std::string& strUrl,struct UrlFileInfo* _pclsInfo,FILE* fp, IDownloadTask* task)
    {
		DLResult nRet = DOWNLOAD_OK;
		//DebugPrintf(DLC_DIAGNOSTIC,"download2File used curl  CURLOPT_URL begin with url = %s", strUrl.c_str());
        if(strUrl.length() < 1 || !fp || !_pclsInfo)
        {
            DebugPrintf(DLC_DIAGNOSTIC,"download2File Error strUrl.length() < 1 || !fp || !_pclsInfo");
            nRet = DOWNLOAD_INVALID_ARGUMENT;
            return nRet;
        }
		snprintf(_pclsInfo->szUrl,MAX_URL_LENGTH,"%s",strUrl.c_str());

		if(task->GetLastOffSet() > 0)
		{
			sprintf(_pclsInfo->szRange,"%u-", task->GetLastOffSet());
			DebugPrintf(DLC_XU_KAI,"_pclsInfo->szRange : %s",_pclsInfo->szRange);
		}
		else
		{
			_pclsInfo->szRange[0] = 0;
		}



		nRet = CurlExecuteRequest(
                _pclsInfo,
                task,
                WriteDataCallBack,
                fp,
                false);

        //DebugPrintf(DLC_DIAGNOSTIC,"download2File used curl end nRet : %d",nRet);
        return nRet;
    }

	bool GetUrlFileInfo(const HttpHeaderInfo &_clsHeader,struct UrlFileInfo* _pInfo)
	{
		if(NULL == _pInfo || ((_clsHeader.m_clsHeaderState.uRequestCode / 100) != 2))
		{
			return false;
		}
		if (!_clsHeader.m_strLocation.empty())
		{
			snprintf(_pInfo->szUrl,MAX_URL_LENGTH,"%s",_clsHeader.m_strLocation.c_str());
		}

		if(!_clsHeader.m_strAcceptRanges.empty() && !strcasecmp(_clsHeader.m_strAcceptRanges.c_str(),"bytes"))
		{
			_pInfo->bSupportBreakpointContinuingly = true;
		}
		//else
		//{
		//	_pInfo->bSupportBreakpointContinuingly = false;
		//}
		
		if(!_clsHeader.m_strFileName.empty())
		{
			int iNameLen = _clsHeader.m_strFileName.length();
			if(iNameLen > MAX_NAME_LENGTH - 1)
				iNameLen = MAX_NAME_LENGTH - 1;
			strncpy(_pInfo->szFileName,_clsHeader.m_strFileName.c_str(),iNameLen);
			_pInfo->szFileName[iNameLen] = 0;
		}
		else
		{
			if (!_clsHeader.m_strLocation.empty())
			{
				const char *pTempName = strrchr(_clsHeader.m_strLocation.c_str(), '/');
				if(pTempName)
				{
					pTempName++;
					snprintf(_pInfo->szFileName, MAX_NAME_LENGTH - 1, "%s", pTempName);
					char* p1 = strpbrk(_pInfo->szFileName,"?&");
					if(p1)
					{
						*p1 = 0;
					}
				}

			}
		}
		return true;
	}

	size_t WriteCallBackForNobody(void *ptr, size_t size, size_t nmemb, void *data)
	{
		return 0;
	}

    size_t WriteDataCallBack(void *ptr, size_t size, size_t nmemb, void *data)
    {
        WriteDataInfo *pWriteData = (WriteDataInfo *)data;
        if(NULL == ptr 
            || NULL == pWriteData 
            || size * nmemb < 1 
            || NULL == pWriteData->headerInfo
            || (pWriteData->headerInfo->m_clsHeaderState.uRequestCode / 100 != 2)
            || NULL == pWriteData->fp)
        {
            return size *nmemb;
        }
        int writeMemb = fwrite(ptr, size, nmemb, pWriteData->fp);
        return size * writeMemb;
    }

    DLResult getFileInfoFromWeb(const std::string& strUrl, struct UrlFileInfo* info)
    {

        DLResult nRet = DOWNLOAD_OK;

        if(strUrl.length() <=0 || !info)
        {
            nRet = DOWNLOAD_INVALID_ARGUMENT;
            return nRet;
		}

		DebugPrintf(DLC_XU_KAI,"getFileInfoFromWeb used curl begin");
		snprintf(info->szUrl,MAX_URL_LENGTH,"%s",strUrl.c_str());

		FILE *fp = fopen("/dev/null","w+");

		nRet = CurlExecuteRequest(info,NULL,WriteCallBackForNobody,fp,true);

        DebugPrintf(DLC_XU_KAI,"getFileInfoFromWeb used curl end");
        return nRet;
    }

    DLResult download2Path(
        const std::string& strUrl,
        struct UrlFileInfo* _pclsInfo,
        const char* lpszPath,
        IDownloadTask * task
        )
    {

        if(!lpszPath || !_pclsInfo || !task)
        {
            DebugPrintf(DLC_DIAGNOSTIC,"download2Path Error something is NULL");
            return DOWNLOAD_FILE_ERROR;
        }
        DLResult nRet = DOWNLOAD_OK;

        FILE* fp = NULL;
        std::string strPathName = "";
        std::string strRange = "";

        struct stat aStat;
        int result = stat(lpszPath, &aStat );

        if(result || aStat.st_mode != S_IFDIR )
        {
            mkdir(lpszPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }

        DebugPrintf(DLC_XU_KAI,"lpszFileName is %s\n",_pclsInfo->szFileName);

        strPathName = lpszPath;
        if(strPathName.find_last_of('/')+1 != strPathName.length())
        {
            strPathName.append("/");
        }
        strPathName.append(_pclsInfo->szFileName);
        strPathName.append(".dat");

        //DebugPrintf(DLC_DIAGNOSTIC,"strPathName is %s\n",strPathName.c_str());
        //TODO: 可以将整个打开文件的操作放到download2File中，如果断点续传失败的话不用重新初始化curl 
        // check for existence 
        int iCheck = access(strPathName.c_str(),W_OK);

        //existence and support Breakpoint
        //DebugPrintf(DLC_DIAGNOSTIC,"iCheck : %d;bBreakpointContinuingly: %d; ptask->uDownloadedBytes : %d",
        //        iCheck,
        //        _pclsInfo->bSupportBreakpointContinuingly,
        //        task->GetLastOffSet());
        if((iCheck == 0) && _pclsInfo->bSupportBreakpointContinuingly && (task->GetLastOffSet() > 0))
        {
            //DebugPrintf(DLC_DIAGNOSTIC,"Open file for ab");
            fp = fopen(strPathName.c_str(),"ab");
        }
        else
        {
            //DebugPrintf(DLC_DIAGNOSTIC,"Open file for wb");
            //always open with write not append.
            fp = fopen(strPathName.c_str(),"wb");
        }
        if(fp)
        {
            //DebugPrintf(DLC_DIAGNOSTIC,"Open file ok");

            //always download from zero.
            //TODO: 该处可能要用ptask->uDownloadedBytes 和 当前文件大小进行比较，取较小的值作为已下载的字节数。现在是取文件的大小
            fseek(fp,0,SEEK_END);
            int iFileLen = ftell(fp);
            //DebugPrintf(DLC_DIAGNOSTIC,
            //        "ptask->uDownloadedBytes : %d; iFileLen : %d",
            //        task->GetLastOffSet(),
            //        iFileLen);
            if(iFileLen < 0)
            {
                iFileLen = 0;
            }
            task->SetLastOffSet(iFileLen);
            nRet = download2File(strUrl, _pclsInfo, fp, task);
            if(nRet == DOWNLOAD_NOT_SUPPORT_BREAKPOINT_CONTINUINGLY)
            {
                //DebugPrintf(DLC_DIAGNOSTIC,"nRet == DOWNLOAD_NOT_SUPPORT_BREAKPOINT_CONTINUINGLY");
                fclose(fp);
                fp = fopen(strPathName.c_str(),"wb");
                if(!fp)
                {
                    DebugPrintf(DLC_DIAGNOSTIC,"Open file err 1");
                    return DOWNLOAD_FILE_ERROR;
                }
                task->SetLastOffSet(0);
                nRet = download2File(strUrl, _pclsInfo, fp, task);
            }
            fclose(fp);
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC,"Open file err");
            nRet = DOWNLOAD_FILE_ERROR;
        }

        return nRet;
    }
}
