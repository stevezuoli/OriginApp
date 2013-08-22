#include "QiushibaikeImpl.h"

#include "Common/SystemSetting_DK.h"
#include "../DK91Search/Curl_Post.h"
#include "Utility/ThreadHelper.h"
#include "I18n/StringManager.h"
#include "SQM.h"
#include "Framework/CNativeThread.h"
#include <sys/stat.h>
#include "unistd.h"
#include "Utility.h"
#include <stdlib.h>
#include "GUI/MessageEventArgs.h"

using namespace std;
using namespace DK91SearchAPI;
using namespace DkFormat;

#define         QIUSHIBAIKE_SERVERURL                "http://www.qiushibaike.com/new2/late/20/page/"
#define         PREFIXCONTENT                        "<div class=\"content\""
#define         PREFIXENDCONTENT                        "\">"
#define         SUFFIXCONTENT                        "</div>"
#define         MaxLenOneItem                        1000
#define         MinLenOneItem                        50


SINGLETON_CPP(QiushibaikeImpl);

const char* QiushibaikeImpl::ContentReadyEvent = "QiuShiBaiKeContentReady";
QiushibaikeImpl::QiushibaikeImpl()
{
    m_bIsWorking = FALSE;
    m_pThread = 0;
    sem_init(&m_sem,0,1);    
}

QiushibaikeImpl::~QiushibaikeImpl()
{
    m_pThread = 0;
    sem_destroy(&m_sem);
}

HRESULT QiushibaikeImpl::StartRetriveContent(UINT32 uSeqNo)
{
    DebugPrintf(DLC_LIUJT, "QiushibaikeImpl::StartRetriveContent entering!!!");
    ThreadParameter* pThreadParam = new ThreadParameter(uSeqNo, (void*)this);
    if(!pThreadParam)
    {
        return E_FAIL;
    }
    ThreadHelper::CreateThread(&m_pThread,RetriveContent,(void*)pThreadParam,"QiushibaikeImpl @ RetriveContent", true, 25600);
    return S_OK;
}

void* QiushibaikeImpl::RetriveContent(void *p)
{
    ThreadParameter* pThreadParam = (ThreadParameter*)p;
    if(!pThreadParam)
    {
        return NULL;
    }
    
    QiushibaikeImpl * pQSBKImpl = (QiushibaikeImpl*)(pThreadParam->GetOwner());
    DebugPrintf(DLC_LIUJT, "QiushibaikeImpl::RetriveContent begin to check new release!");
    if(!pQSBKImpl || pQSBKImpl->m_bIsWorking || !WifiManager::GetInstance()->IsConnected())
    {
        DebugPrintf(DLC_LIUJT, "QiushibaikeImpl::RetriveContent End ischecking=%d",pQSBKImpl->m_bIsWorking);
        ThreadHelper::ThreadExit(0);
        return NULL;
    }
    
    DebugPrintf(DLC_LIUJT, "QiushibaikeImpl::RetriveContent begin to set thread properties!");
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);    

    WAIT(pQSBKImpl->m_sem);
    pQSBKImpl->m_bIsWorking = TRUE;

    pQSBKImpl->SendContent(pThreadParam->GetSeqNo());
    
    pQSBKImpl->m_bIsWorking = FALSE;

    delete pThreadParam;
    pThreadParam = NULL;

    POST(pQSBKImpl->m_sem);

    ThreadHelper::ThreadExit(0);
    return NULL;
}


        
string QiushibaikeImpl::GetContent(int *piErrCode, UINT32 iPageNum)
{
    if(NULL == piErrCode)
    {
        return "";
    }

    DebugPrintf(DLC_LIUJT, "QiushibaikeImpl::GetContent Entering, url=%s, parameters=%d", QIUSHIBAIKE_SERVERURL, iPageNum);
    string url(QIUSHIBAIKE_SERVERURL);
    CHAR str[10];
    memset(str, 0, sizeof(str));
    sprintf(str, "%d", iPageNum);
    url.append(str);//.append("?slow");
    string szResultData = Get(url,"",piErrCode, "Windows Chrome 3");

    if(*piErrCode || szResultData.empty())
    {
        DebugPrintf(DLC_LIUJT, "QiushibaikeImpl::GetContent get failed result: %d, or get empty response!!!",*piErrCode);
    }

    DebugPrintf(DLC_LIUJT, "QiushibaikeImpl::GetContent End");

    return szResultData;
}

void QiushibaikeImpl::FilterContent(string szContent)
{
    if(szContent.empty())
    {
        return ;
    }
    m_vContentList.clear();
    int iPrefixLen = strlen(PREFIXENDCONTENT);
    string::size_type headIndex = 0, tailIndex = 0;
    do
    {
        headIndex = szContent.find(PREFIXCONTENT, headIndex);
        if(string::npos == headIndex)
        {
            break;
        }
		headIndex = szContent.find(PREFIXENDCONTENT,headIndex);
		if(string::npos == headIndex)
		{
			break;
		}
        tailIndex = szContent.find(SUFFIXCONTENT, headIndex);
        if(string::npos == tailIndex)
        {
            break;
        }
        
        string szResult = szContent.substr(headIndex+iPrefixLen, tailIndex-headIndex-iPrefixLen);
        headIndex = tailIndex;        
        szResult = removeBR(szResult);
        if(szResult.size() > MaxLenOneItem || szResult.size() < MinLenOneItem)
        {
            continue;
        }
        szResult = CUtility::TrimString(szResult);
        m_vContentList.push_back(szResult);
    }while(TRUE);
    
}

string QiushibaikeImpl::removeBR(string szContent)
{
    if(szContent.empty())
    {
       return szContent; 
    }
    string::size_type iIndex = 0, iTailIndex = 0;
    do{
        iIndex = szContent.find("<", iIndex);
        if(string::npos == iIndex)
        {
            break;
        }
        iTailIndex = szContent.find(">", iIndex);    
        if(string::npos == iTailIndex)
        {
            break;
        }
        string szTag = szContent.substr(iIndex, iTailIndex-iIndex+1);
        if(string::npos == szTag.find("br"))
        {
            break;
        }
        
        szContent = szContent.replace(iIndex, iTailIndex-iIndex+1, " ");
        
    }while(TRUE);
    
    return szContent;
}

void QiushibaikeImpl::SendContent(UINT32 uSeqNo)
{
    int iError = 0;
    FilterContent(GetContent(&iError, uSeqNo));

    //SNativeMessage msg;
    //MessageEventArgs args(KMessageQiuShiBaiKe, 0, ContentReady, uSeqNo);
    QiushibaikeContentReadyEventArgs args;
    args.uSeqNo = uSeqNo;
    FireEvent(ContentReadyEvent, args);
}



