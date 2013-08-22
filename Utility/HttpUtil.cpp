#include "Utility/HttpUtil.h"
#include <string.h>
#include "Utility/CharUtil.h"


namespace dk
{
namespace utility
{

std::string HttpUtil::UrlEncode(const char* s)
{
    if (NULL == s)
    {
        return "";
    }
    const unsigned char* us = (const unsigned char*)s;
    std::string result;
    while (unsigned int c = *us++)
    {
        if (isalnum(c))
        {
            result.push_back(c);
        }
        else if (' ' == c)
        {
            result.push_back('+');
        }
        else
        {
            result.push_back('%');
            result.push_back(CharUtil::IntToHexChar(c / 16));
            result.push_back(CharUtil::IntToHexChar(c % 16));
        }
    }
    return result;
}

std::string HttpUtil::UrlDecode(const char* s)
{
    if (NULL == s)
    {
        return "";
    }
    const char* sEnd = s + strlen(s);
    std::string result;
    while (s != sEnd)
    {
        if ('+' == *s)
        {
            result.push_back(' ');
            ++s;
        }
        else if ('%' == *s && s + 2 <= sEnd)
        {
            int v1 = CharUtil::HexCharToInt(*(s+1));
            int v2 = CharUtil::HexCharToInt(*(s+2));
            if (v1 < 0 || v2 < 0)
            {
                return result;
            }
            result.push_back(16 * v1 + v2);
            s += 3;
        }
        else
        {
            result.push_back(*s);
            ++s;
        }
    }
    return result;
}

std::string HttpUtil::GetParaFromUrl(const char* url, const char* key)
{
    std::string s = key;
    s.push_back('=');
    const char* result = strcasestr(url, s.c_str());
    if (NULL != result)
    {
        if (result > url && (result[-1] == '?' || result[-1] == '&'))
        {
            const char* valueEnd =strchr(result, '&');
            if (NULL == valueEnd)
            {
                valueEnd = url + strlen(url);
            }
            return UrlDecode(std::string(result + s.size(),  valueEnd).c_str());
        }
    }
    return "";
}

} // namespace utility
} // namespace dk

