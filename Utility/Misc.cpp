#include "Utility/Misc.h"
#include "interface.h"
#include "Utility/MD5Checker.h"
#include <time.h>
#include <sys/time.h> 
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>

//generate random value less than _max
using namespace std;

int Rand(int _max)
{
    static bool first_run = true;

    if (first_run)
    {
        first_run = false;
        srand((int)time(0));
    }

    return (int)(rand() % _max);
}

int ReadUrandom(char *pBuf,int iBufLen)
{
    int iReadLen = 0;
    ifstream ifs("/dev/urandom",ifstream::in | ifstream::binary);
    if(!ifs.is_open())
    {
        return 0;
    }
    ifs.read(pBuf,iBufLen);
    iReadLen = ifs.gcount();
    return iReadLen;
}

std::string CreateRandomContentByTime()
{
    struct timeval t_stamp;
    gettimeofday(&t_stamp, NULL );

    stringstream ss;
    ss    << (long)t_stamp.tv_sec << "##" << (long)t_stamp.tv_usec;
    string input = "";
    ss >> input;
    return input;
}
std::string GenerateUUID()
{
    char szRandomContent[36] = {0};
    char *pRandContent = szRandomContent;
    string strTmpContent = "";

    // 从/dev/urandom 文件读取32个字节作为随机内容
    int iLen = ReadUrandom(szRandomContent,32);
    szRandomContent[iLen] = 0;
    DebugPrintf(DLC_XU_KAI,"ReadUrandom iLen : %d",iLen);
    if(iLen == 0)
    {
        // 读取随机内容失败则通过时间创建随机内容
        string strTmpContent = CreateRandomContentByTime();
        pRandContent = (char *)strTmpContent.c_str();
    }
    char* pMd5String =MD5Checker::GetInstance()->DK_MDString(pRandContent);
    if(NULL == pMd5String)
    {
        DebugPrintf(DLC_XU_KAI,"pMd5String is NULL err");
        return "";
    }
    DebugPrintf(DLC_XU_KAI,"pMd5String is %s",pMd5String);
    std::string strMd5(pMd5String);

    // 将生成的MD5值中的小写字母转换为大写字母
    transform(strMd5.begin(), strMd5.end(), strMd5.begin(), ::toupper);
    DebugPrintf(DLC_XU_KAI,"strMd5.c_str() : %s",strMd5.c_str());
    return strMd5;
}



std::string UrlEncodeWithCurl(CURL *_pCurl,const std::string _strUrl,const std::string _strIgnoreArray)
{
	DebugPrintf(DLC_XU_KAI,"UrlEncodeWithCurl End _strUrl : %s\n",_strUrl.c_str());
	if(_strUrl.empty() || NULL == _pCurl)
	{
		return "";
	}
	size_t iBeginPos = 0;
	string strDestUrl = "";
	if(_strIgnoreArray.empty() || ((iBeginPos = _strUrl.find_first_of(_strIgnoreArray)) == string::npos))
	{
		char *pDest = curl_easy_escape(_pCurl,_strUrl.c_str(),_strUrl.length());
		if(pDest)
		{
			strDestUrl = pDest;
			curl_free(pDest);
			pDest = NULL;
		}
		return strDestUrl;
	}

	strDestUrl = _strUrl.substr(0,iBeginPos);
	size_t iEndPos = 0;
	do 
	{
		strDestUrl += _strUrl[iBeginPos];
		string strSrcUrl = "";
		iEndPos = _strUrl.find_first_of(_strIgnoreArray,iBeginPos + 1);
		strSrcUrl = _strUrl.substr(iBeginPos + 1,iEndPos - iBeginPos - 1);
		DebugPrintf(DLC_XU_KAI,"UrlEncodeWithCurl cccccccccccccccccccccccc strSrcUrl : %s\n",strSrcUrl.c_str());
		if(!strSrcUrl.empty())
		{
			char *pDest = curl_easy_escape(_pCurl,strSrcUrl.c_str(),strSrcUrl.length());
			if(pDest)
			{
				strDestUrl += pDest;
				curl_free(pDest);
				pDest = NULL;
			}
		}
		iBeginPos = iEndPos;

	} while (iBeginPos != string::npos);

	DebugPrintf(DLC_XU_KAI,"UrlEncodeWithCurl End strDestUrl : %s\n",strDestUrl.c_str());
	return strDestUrl;
}

unsigned char atox(const unsigned char *b)
{
	BYTE d = 0;
	BYTE e = *b;
	if(e >= 'a' && e<= 'f')
	{
		e += 'A' - 'a';// Convert the lowercase character to uppercase characters.
	}
	d |= ((e >= '0' && e <= '9') ? e-'0' : (e >= 'A' && e <= 'F') ? 10 + e - 'A' : 0); // 012~9ABCDEF
	d <<= 4;

	e = *(b+1);
	if(e >= 'a' && e<= 'f')
	{
		e += 'A' - 'a';// Convert the lowercase character to uppercase characters.
	}
	d |= ((e >= '0' && e <= '9') ? e-'0' : (e >= 'A' && e <= 'F') ? 10 + e - 'A' : 0); // 012~9ABCDEF
	printf("%x\n",d);
	return d;
}

void Asc2Bin(unsigned char *des, const unsigned char*src, int len)
{
	while (len--)
	{
		*des++ = atox(src);
		src += 2;
	}
}
