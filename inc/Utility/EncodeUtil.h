#ifndef __UTILITY_ENCODEUTIL_H__
#define __UTILITY_ENCODEUTIL_H__

#include <vector>
#include <string>

namespace dk
{
namespace utility
{

class EncodeUtil
{
public:
    static bool HexToBin(const char* hex, std::vector<unsigned char>* result);
    static std::string BinToHex(const unsigned char* data, size_t dataLen);
    static std::vector<unsigned char> Md5(const void* data, size_t dataLen);
    static bool RsaDecode(const unsigned char* data, size_t dataLen, std::vector<unsigned char>* result);
    static bool AesDecode(const void* key, size_t keyLen, const void* data, size_t dataLen, std::vector<unsigned char>* result);
    static std::wstring ToWString(const char* s);
    static std::wstring ToWString(const std::string& str);
    static std::string ToString(const wchar_t* ws);
    static std::string ToString(const std::wstring& wstr);

    static std::string GBKToUTF8String(const char* gbkString);
    static std::string GBKToUTF8String(const std::string& gbkString);
    static std::string UTF8ToGBKString(const char* utf8String);
    static std::string UTF8ToGBKString(const std::string& utf8String);
    static std::string UTF8ToChineseTraditional(const char* src);

	static std::vector<unsigned char> SHA_512(const void* data, size_t dataLen);
    static std::string EncodeToken(const std::string& str);
	static std::string DecodeToken(const std::string& str);	

private:
    EncodeUtil();
};

} // namespace utility
} // namespace dk
#endif
