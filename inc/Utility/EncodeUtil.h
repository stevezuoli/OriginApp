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
    static std::string SHA_160(const char* filePath);
    static std::string EncodeToken(const std::string& str);
    static std::string DecodeToken(const std::string& str); 

    static std::vector<unsigned char> SHA_1(const void* data, size_t dataLen);
    static std::vector<unsigned char> Base64Encode(const void* data, size_t dataLen);
    static std::vector<unsigned char> Base64Decode(const void* data, size_t dataLen);

    //calc the sha&md5  once a time, avoid read file twice.
    static bool CalcFileBlockInfos(const char* filePath, 
                            const int blockSize,
                            std::vector<std::string>* shavec,
                            std::vector<std::string>* md5vec,
                            std::vector<int>* sizeVec);
//USE PKCS5Padding, mehtod in Thirdparties/AES.h, TODO: merge with AesDecode
    static std::string AESEncode(const char* key, const char* plain, unsigned char* ivec = NULL);
    static std::string AESDecode(const char* key, const char* cipher, const unsigned int cipherlen, unsigned char* ivec = NULL);

    //4bytes as a unit, get hex string
    static std::string LongArrayToString(unsigned long* array, const int length);
private:
    EncodeUtil();
};

} // namespace utility
} // namespace dk
#endif
