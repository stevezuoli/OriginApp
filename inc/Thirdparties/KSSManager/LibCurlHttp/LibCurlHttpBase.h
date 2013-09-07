/* -------------------------------------------------------------------------
//	File Name	:	LibCurlHttpBase.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-19 17:10:44
//	Description	:
//
// -----------------------------------------------------------------------*/

#ifndef __LIBCURLHTTPBASE_H__
#define __LIBCURLHTTPBASE_H__

#include "../LibKss/CrossPlatformConfig.h"
#include "../LibKss/LibKssEnum.h"
#include "../LibKss/LibKssError.h"
#include "../LibKss/LibKssStruct.h"

#include "curl/curl.h"
#include "../FastDelegate/FastDelegate.h"

#include <string>
#include <vector>

// -------------------------------------------------------------------------

namespace LibcURL
{
    typedef fastdelegate::FastDelegate0<bool> CancelDelegate;

    class CHttpBase
    {
    protected:
        CURL*                       m_pLibCURL;
        std::string                 m_url;
        curl_slist*                 m_pSendHeader;
        Int32                       m_timeOut;
        KSS::ProxyInfo              m_proxySetting;

        std::string                 m_recvData;
        std::string                 m_recvHeader;
        Int32                       m_httpCode;
        std::vector<char>           m_errorMessage;
        

        CancelDelegate              m_cancelDelegate;

        // progress info
        double                      m_uploadTotal;
        double                      m_uploadNow;
        double                      m_downloadTotal;
        double                      m_downloadNow;
        double                      m_uploadSpeed;
        double                      m_downloadSpeed;

    public:
        static ErrorNo initGlobal();
        static void  uninitGlobal();

    public:
        CHttpBase();
        virtual ~CHttpBase();
        
        void setTimeout(Int32 timeout);
        void setURL(const char*url);
        void setSendHeader(const char* headers);
        void setProxyInfo(const KSS::ProxyInfo &proxySetting);
        virtual void setCancelDelegate(CancelDelegate cancel);

        ErrorNo perform();

        std::string getErrorMessage();
        ErrorNo     getHttpCode();
        const std::string& getRecvData();
        const std::string& getRecvHeader();

    protected:
        static size_t   defaultWriteStreamFunc(void* ptr, size_t msize, size_t n, std::string* stream);
        static Int32    progressCallback(void* userData, double dlTotal, double dlNow, double ulTotal, double ulNow);
        static const Int32 CURL_HTTP_DEFAULT_TIMEOUT = (20 * 60 * 1000);

        void _curlSetUploadSpeed(UInt32 limit);                                   // CURLOPT_MAX_SEND_SPEED_LARGE
        void _curlSetDownloadSpeed(UInt32 limit);                                 // CURLOPT_MAX_RECV_SPEED_LARGE
        
    private:
        virtual ErrorNo _initCurl();
        virtual void    _cleanUpCurl();
        virtual bool    _isUsingHttps(); 
        
        void _curlSetUrlAddress();                                    // CURLOPT_URL
        void _curlSetTimeOutMS();                                     // CURLOPT_TIMEOUT_MS
        void _curlSetSendHeader();                                    // CURLOPT_HTTPHEADER
        void _curlSetProgressFunction();                              // CURLOPT_NOPROGRESS
        void _curlSetProxy();                                         // CURLOPT_PROXYTYPE...
        void _curlSetErrorBuffer();                                   // CURLOPT_ERRORBUFFER

        virtual void _curlSetRecvDataFunction();                      // CURLOPT_WRITEFUNCTION,  CURLOPT_WRITEDATA
        virtual void _curlSetRecvHeaderFunction();                    // CURLOPT_HEADERFUNCTION, CURLOPT_HEADERDATA
        virtual void _curlSetProtocols();                             // CURLOPT_PROTOCOLS, CURLOPT_SSL_VERIFYPEER
        virtual void _curlSetDIY();                                   // do it for yourself :-)
    };
}

//--------------------------------------------------------------------------
#endif /* __LIBCURLHTTPBASE_H__ */
