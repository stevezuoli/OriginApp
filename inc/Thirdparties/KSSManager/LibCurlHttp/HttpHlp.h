/* -------------------------------------------------------------------------
//	File Name	:	HttpHlp.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-20 16:15:30
//	Description	:
//
// -----------------------------------------------------------------------*/

#ifndef __HTTPHLP_H__
#define __HTTPHLP_H__

#include <map>
#include <string>
#include <cstring>
#include <cstdio>
#include "../Misc/ErrorCheck.h"

// -------------------------------------------------------------------------

namespace HttpHlp {

    struct TraitsHttpHeaders {
        static char separator1() {
            return '\n';
        }
        static char separator2() {
            return ':';
        }
    };

    struct TraitsHttpParams {
        static char separator1() {
            return '&';
        }
        static char separator2() {
            return '=';
        }
    };

    struct TraitsNoEncoding {
        static char separator1() {
            return '|';
        }
        static char separator2() {
            return '=';
        }
    };

    struct UrlEncoding {

        static std::string& encode(const char* psrc, std::string& str)	{
            KSS_ASSERT(psrc != NULL && *psrc != 0);

            str.resize(::strlen(psrc) * 3 + 1, '\0');

            char* pdst = &(*str.begin());

            for (; *psrc != 0; ++psrc) {

                unsigned char* p = (unsigned char*)psrc;
                if (need_encoding(*p)) {

                    char a[3];
                    sprintf(a, "%02x", *p);
                    *pdst = '%';
                    *(pdst + 1) = a[0];
                    *(pdst + 2) = a[1];
                    pdst += 3;
                }
                else {
                    *pdst++ = *p;
                }
            }

            KSS_ASSERT(str.size() >= pdst - &(*str.begin()));
            str.resize(pdst - &(*str.begin()));

            return str;
        }

        static void decode(const char* psrc, char* pdst) {

            KSS_ASSERT(psrc != NULL && *psrc != 0);
            KSS_ASSERT(pdst != NULL);

            for (; *psrc != 0; ++psrc) {

                if (*psrc == '%') {

                    int code;
                    if (sscanf(psrc+1, "%02x", &code) != 1) 
                        code = '?';
                    *pdst++ = code;
                    psrc += 2;
                }
                else {
                    *pdst++ = *psrc;
                }
            }
        }

        static bool need_encoding(const char* pcsz) {
            KSS_ASSERT(pcsz != NULL);
            for (; *pcsz != 0; ++pcsz) {
                if (need_encoding(*pcsz))
                    return true;
            }
            return false;
        }

    private:
        static bool need_encoding(const char ch) {
            const unsigned char *p = (const unsigned char*)&ch;
            return (*p >  126 || *p == '&' || *p == ' ' || *p == '=' || *p == '%' || 
                *p == '.' || *p == '/' || *p == '+' || 
                *p == '`' || *p == '{' || *p == '}' || *p == '|' || *p == '[' ||
                *p == ']' || *p == '\"' || *p == '<' || *p == '>' || *p == '\\' ||
                *p == '^' || *p == ';' || *p == ':');
        }
    };

    template <class _Traits>
    class KeyValueParams {
    public:
        void add(const char* pcszKey, const char* pcszValue, bool fNeedCoding = true) {
            KSS_ASSERT(pcszKey != NULL && *pcszKey != 0);

            if (!m_str.empty())
                m_str += _Traits::separator1();
            m_str += pcszKey;
            m_str += _Traits::separator2();
            if (pcszValue != NULL && *pcszValue != 0) {
                if (fNeedCoding) {
                    std::string str;
                    m_str += UrlEncoding::encode(pcszValue, str);
                }
                else {
                    m_str += pcszValue;
                }
            }
        }

        void add(const char* pcszKeyValuePairs) {
            KSS_ASSERT(pcszKeyValuePairs != NULL && *pcszKeyValuePairs != 0);
            if (!m_str.empty())
                m_str += _Traits::separator1();
            m_str += pcszKeyValuePairs;
        }

        const char* get() const {
            if (!m_str.empty())
                return m_str.c_str();
            else
                return NULL;
        }

        void clear() {
            m_str.assign("");
        }

    private:
        std::string		m_str;
    };

    typedef KeyValueParams<TraitsHttpHeaders>	Headers;
    typedef KeyValueParams<TraitsHttpParams>	Params;
}

//--------------------------------------------------------------------------
#endif /* __HTTPHLP_H__ */