#include "Utility/EncodeUtil.h"
#include "Utility/CharUtil.h"
#include "DkFilterFactory.h"
#include "KernelEncoding.h"

extern int get_rsa_n_1024(unsigned char* buf);
namespace dk
{
namespace utility
{
bool EncodeUtil::HexToBin(const char* bin, std::vector<unsigned char>* result)
{
    while (*bin && *(bin + 1))
    {
        int v1 = CharUtil::HexCharToInt(*bin);
        int v2 = CharUtil::HexCharToInt(*(bin+1));
        if (v1 < 0 || v2 < 0)
        {
            return false;
        }
        result->push_back(16 * v1 + v2);
        bin += 2;
    }
    return true;
}

std::string EncodeUtil::BinToHex(const unsigned char* data, size_t dataLen)
{
    std::string result;
    for (size_t i = 0; i < dataLen; ++i)
    {
        char buf[3] = {0};
        buf[0] = CharUtil::IntToHexChar(data[i] / 16);
        buf[1] = CharUtil::IntToHexChar(data[i] % 16);
        result += buf;
    }
    return result;
}

std::vector<unsigned char> EncodeUtil::Md5(const void* data, size_t dataLen)
{
    std::vector<unsigned char> result(16);
    size_t md5Len = result.size();
    DkFilterFactory::EncodeBuffer(FILTER_MD5, NULL, (const unsigned char*)data, dataLen, &result[0] , &md5Len);
    return result;
}

bool EncodeUtil::RsaDecode(const unsigned char* data, size_t dataLen, std::vector<unsigned char>* result)
{
    unsigned char rsa_n[128];
    get_rsa_n_1024(rsa_n);
#if 0
        = {
    0xFF, 0x0D, 0xC1, 0x54, 0x04, 0x60, 0x5A, 0xF8, 0xC2, 0x1C, 0x7F, 0x73, 0x1B, 0x93, 0xC3, 0x25, 
    0x17, 0x63, 0xA1, 0x59, 0x8F, 0x82, 0xD7, 0x93, 0xC0, 0x33, 0x70, 0xCF, 0x4F, 0x51, 0xE8, 0x7F, 
    0xE0, 0x13, 0xE4, 0xA4, 0x2D, 0x5B, 0xD5, 0x3E, 0x0D, 0x61, 0x94, 0x03, 0xAD, 0xD5, 0x40, 0x65, 
    0xB5, 0x2F, 0x56, 0x90, 0xD1, 0x7A, 0xD8, 0xF8, 0x1B, 0xC9, 0xAE, 0xF1, 0xB2, 0x3D, 0xAE, 0xDE, 
    0x04, 0x00, 0xB9, 0xC1, 0x20, 0x28, 0xBC, 0x37, 0x0F, 0x46, 0xD3, 0xB7, 0x45, 0x0F, 0x4C, 0x6E, 
    0x4E, 0x8B, 0xC3, 0x99, 0x22, 0x39, 0xBC, 0xEC, 0xA3, 0x9A, 0x08, 0xDA, 0x5B, 0x57, 0x5B, 0x8B, 
    0x4A, 0x44, 0xB0, 0x45, 0x6F, 0x7B, 0x41, 0xE2, 0x8F, 0xA8, 0x3B, 0x8A, 0x7A, 0xE6, 0x8F, 0xEF, 
    0x24, 0xBE, 0x75, 0xB5, 0x63, 0xB8, 0x93, 0x85, 0x96, 0xEE, 0x8F, 0x88, 0x50, 0x94, 0xB3, 0xEF
    };
#endif
    FilterParam dkRsaParam;
    memset(&dkRsaParam, 0, sizeof (dkRsaParam));
    dkRsaParam.rsa.isPublicKey = DK_TRUE;
    dkRsaParam.rsa.n = rsa_n;
    dkRsaParam.rsa.nLen = sizeof (rsa_n);
    unsigned char rsa_e[] = {0x1, 0x0, 0x1 };
    dkRsaParam.rsa.e = rsa_e;
    dkRsaParam.rsa.eLen = sizeof (rsa_e);
    DK_UINT rsaDecodeLen = 0;
    if (DkFilterFactory::GetEnoughDecodeLength(FILTER_RSA, &dkRsaParam, dataLen, &rsaDecodeLen) != DKR_OK || rsaDecodeLen < 1)
    {
        return false;
    }
    result->resize(rsaDecodeLen);
    const size_t FIXED_RSA_RESULT_LEN = 16;
    if (DkFilterFactory::DecodeBuffer(FILTER_RSA, &dkRsaParam, data, dataLen, &(*result)[0], &rsaDecodeLen) == DKR_OK)
    {
        result->resize(rsaDecodeLen);
        if (rsaDecodeLen < FIXED_RSA_RESULT_LEN)
        {
            result->insert(result->begin(), FIXED_RSA_RESULT_LEN - rsaDecodeLen, 0);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool EncodeUtil::AesDecode(const void* key, size_t keyLen, const void* data, size_t dataLen, std::vector<unsigned char>* result)
{
    FilterParam dkAesParam;
    memset(&dkAesParam, 0, sizeof (dkAesParam));
    if (keyLen > 16)
    {
        return false;
    }
    memcpy(dkAesParam.aes.userKey + 16 - keyLen, key, keyLen);
    dkAesParam.aes.keyLen = 16;
    DK_UINT aesDecodeLen = 0;
    if (DkFilterFactory::GetEnoughDecodeLength(FILTER_AES_ECB, &dkAesParam, dataLen, &aesDecodeLen) != DKR_OK || aesDecodeLen < 1)
    {
        return false;
    }
    result->resize(aesDecodeLen);
    if (DkFilterFactory::DecodeBuffer(FILTER_AES_ECB, &dkAesParam, (const DK_BYTE*)data, dataLen, &(*result)[0], &aesDecodeLen) == DKR_OK)
    {
        result->resize(aesDecodeLen);
        return true;
    }
    return false;
}

std::wstring EncodeUtil::ToWString(const char* s)
{
    if (NULL == s)
    {
        return L"";
    }
    wchar_t* ws = EncodingUtil::CharToWChar(s);
    if (NULL == ws)
    {
        return L"";
    }
    std::wstring result(ws);
    DK_FREE(ws);
    return result;
}

std::wstring EncodeUtil::ToWString(const std::string& str)
{
    return ToWString(str.c_str());
}

std::string EncodeUtil::GBKToUTF8String(const char* gbkString)
{
    if (NULL == gbkString)
    {
        return "";
    }
    DK_SIZE_T gbkLen = strlen(gbkString);
    if (0 == gbkLen)
    {
        return "";
    }
    DK_SIZE_T utf8Len = gbkLen * EncodingUtil::MAX_BYTES_OF_UTF8CHAR + 1;
    std::vector<char> gbkBuffer(utf8Len, 0);
    if (EncodingUtil::Convert(
            EncodingUtil::GBK,
            EncodingUtil::UTF8,
            gbkString,
            &gbkLen,
            &gbkBuffer[0],
            &utf8Len) >= 0)
    {
        return std::string(&gbkBuffer[0]);
    }
    return "";
}

std::string EncodeUtil::GBKToUTF8String(const std::string& gbkString)
{
    return GBKToUTF8String(gbkString.c_str());
}

std::string EncodeUtil::UTF8ToGBKString(const char* utf8String)
{
    if (NULL == utf8String)
    {
        return "";
    }
    DK_SIZE_T utf8Len = strlen(utf8String);
    if (0 == utf8Len)
    {
        return "";
    }
    DK_SIZE_T gbkLen = utf8Len + 1;
    std::vector<char> gbkBuffer(gbkLen, 0);
    if (EncodingUtil::Convert(
            EncodingUtil::UTF8,
            EncodingUtil::GBK,
            utf8String,
            &utf8Len,
            &gbkBuffer[0],
            &gbkLen) >= 0)
    {
        return std::string(&gbkBuffer[0]);
    }
    return "";
}

std::string EncodeUtil::UTF8ToGBKString(const std::string& utf8String)
{
    return UTF8ToGBKString(utf8String.c_str());
}

std::string EncodeUtil::ToString(const wchar_t* ws)
{
    if (NULL == ws)
    {
        return "";
    }
    char* s = EncodingUtil::WCharToChar(ws);
    if (NULL == s)
    {
        return "";
    }
    std::string result(s);
    DK_FREE(s);
    return result;
}

std::string EncodeUtil::ToString(const std::wstring& wstr)
{
    return ToString(wstr.c_str());
}

std::string EncodeUtil::UTF8ToChineseTraditional(const char* src)
{
    std::wstring wsrc = ToWString(src);
    for (size_t i = 0; i < wsrc.size(); ++i)
    {
        wchar_t dst = 0;
        EncodingUtil::CHStoCHTChar(wsrc[i], &dst);
        wsrc[i] = dst;
    }
    return ToString(wsrc.c_str());
}

std::vector<unsigned char> EncodeUtil::SHA_512(const void* data, size_t dataLen)
{
    std::vector<unsigned char> result(64);
    size_t sha512Len = result.size();
    DkFilterFactory::EncodeBuffer(FILTER_SHA_512, NULL, (const unsigned char*)data, dataLen, &result[0] , &sha512Len);
    return result;
}

std::string EncodeUtil::EncodeToken(const std::string& str)
{
	const size_t sha512Len = 64;
	const size_t maxInputLen = 128;
	size_t strLen = str.size();
	std::vector<unsigned char> hash1 = EncodeUtil::SHA_512(str.c_str(), strLen);
	std::vector<unsigned char> hash2 = EncodeUtil::SHA_512(&hash1[0], sha512Len);
	std::vector<unsigned char> hash(256);
	std::vector<unsigned char> result(512);

	memcpy(&hash[0], &hash1[0], sha512Len);
	memcpy(&hash[sha512Len], &hash2[0], sha512Len);

	size_t codeLen = strLen < maxInputLen ? strLen : maxInputLen;
	for (size_t i = 0; i < codeLen; ++i)
	{
		result[i] = hash[i] ^ str[i] ^ 0xFF;
	}

	memcpy(&result[codeLen], &hash[0], codeLen);
	return EncodeUtil::BinToHex(&result[0], codeLen * 2);
}

std::string EncodeUtil::DecodeToken(const std::string& str)
{
	std::vector<unsigned char> buf;
	std::string result;
	
	if(!EncodeUtil::HexToBin(str.c_str(), &buf))
	{
		return result;
	}

	size_t codeLen = buf.size() / 2;
	for(size_t i = 0; i < codeLen; ++i)
	{
		unsigned char c = buf[i] ^ buf[i+codeLen] ^ 0xFF;
		result += c;
	}
	return result;
}

} // namespace utility
} // namespace dk
