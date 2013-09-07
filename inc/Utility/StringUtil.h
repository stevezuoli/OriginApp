#ifndef __UTILITY_STRINGUTIL_H__
#define __UTILITY_STRINGUTIL_H__

#include <string>
#include <vector>

namespace dk
{
namespace utility
{

class StringUtil
{
public:
    static std::string ToLower(const char* s);
    static std::string ToLower(const std::string& s)
    {
        return ToLower(s.c_str());
    }
    static std::wstring ToLower(const wchar_t* s);
    static std::wstring ToLower(const std::wstring& s)
    {
        return ToLower(s.c_str());
    }
    static std::string ToUpper(const char* s);
    static std::string ToUpper(const std::string& s)
    {
        return ToUpper(s.c_str());
    }
    static std::wstring ToUpper(const wchar_t* s);
    static std::wstring ToUpper(const std::wstring& s)
    {
        return ToUpper(s.c_str());
    }
	static std::string ReplaceString(const std::string& source, const char c, const char* dst);
	static std::string ReplaceString(const std::string& source, const char* pat, const char* dst);
	static std::string EscapeHTML(const char* pConvertStr);
	static std::string Trim(const char* pstr);
    static bool StartWith(const char* src, const char* pat);
    static bool EndWith(const char* src, const char* pat);
    static bool IsNullOrEmpty(const char* s)
    {
        return 0 == s || 0 == s[0];
    }
    static int CountsOfChar(const std::string& str, const std::string& pat);
    static bool IsValidEmail(const char* email);
    static std::vector<std::string> Split(const char* s, const char* seps);
    static std::string Join(const std::vector<std::string>& ids, const char* seps);
	static bool MatchByFirstLetter(const std::wstring& src, const std::wstring& dst);
	static bool MatchCharByPinyin(wchar_t src, wchar_t dst);
    static void HideEmailInfo(std::string& src);
private:
    StringUtil();
}; // namespace StringUtil

} // bookstore
} // dk

#endif
