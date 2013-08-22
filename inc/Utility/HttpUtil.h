#ifndef __UTILITY_HTTPUTIL_H__
#define __UTILITY_HTTPUTIL_H__

#include <string>

namespace dk
{
namespace utility
{

class HttpUtil
{
public:
    static std::string UrlEncode(const char* s);
    static std::string UrlDecode(const char* s);
    static std::string GetParaFromUrl(const char* url, const char* key);
private:
    HttpUtil();
};

} // namespace utility
} // namespace dk
#endif
