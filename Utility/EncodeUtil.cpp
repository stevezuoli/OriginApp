#include "Utility/EncodeUtil.h"
#include "Utility/CharUtil.h"
#include "Utility/MD5Checker.h"
#include "DkFilterFactory.h"
#include "KernelEncoding.h"
#include "Thirdparties/AES.h"
#include "Thirdparties/SHA1.h"
#include <stdio.h>
#include <errno.h>
#include "interface.h"

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


std::vector<unsigned char> EncodeUtil::SHA_1(const void* data, size_t dataLen)
{
    std::vector<unsigned char> result(20);
    size_t len = result.size();
    DkFilterFactory::EncodeBuffer(FILTER_SHA_1, NULL, (const unsigned char*)data, dataLen, &result[0] , &len);
    return result;
}

std::vector<unsigned char> EncodeUtil::Base64Encode(const void* data, size_t dataLen)
{
    std::vector<unsigned char> result;
    size_t len = 4 + dataLen + dataLen/3 ;    // bin to base64 len: n * 4/3  , add 4 bytes 
    result.resize(len);
    DkFilterFactory::EncodeBuffer(FILTER_BASE64, NULL, (const unsigned char*)data, dataLen, &result[0] , &len);
    return result;
}

std::vector<unsigned char> EncodeUtil::Base64Decode(const void* data, size_t dataLen)
{
    std::vector<unsigned char> result;
    if (dataLen < 4)
    {
        return result;
    }

    int equalCount = ((unsigned char*)data)[dataLen - 1] == '=' ? 1 : 0;
    equalCount = ((unsigned char*)data)[dataLen - 2] == '=' ? equalCount + 1 : equalCount;
    size_t len = dataLen*3/4 - equalCount; 
    result.resize(len);
    DkFilterFactory::DecodeBuffer(FILTER_BASE64, NULL, (const unsigned char*)data, dataLen, &result[0] , &len);
    return result;
}

std::string EncodeUtil::AESDecode(const char* key, const char* cipher, const unsigned int cipherlen, unsigned char* ivec)
{
    AES_KEY aes_key;
    char* plain = (char*)malloc((cipherlen + 8)*sizeof(char));
    int len = cipherlen;
    int bits = strlen(key) * 8;

    AES_set_decrypt_key((const unsigned char*)key, bits, &aes_key);
    if (ivec)
    {
        AES_cbc_encrypt((const unsigned char*)cipher, (unsigned char*)plain, len, &aes_key, ivec, 0);
    }
    else
    {
        AES_ecb_encrypt((const unsigned char*)cipher, (unsigned char*)plain, len, &aes_key, 0);
    }

//PKCS5Padding
    int rem = plain[len - 1];
    if (rem > 0 && rem <= 16)
    {
        len -= rem;
    }
    std::string decodeString(plain, plain + len);
    free(plain);
    return decodeString;
}

std::string EncodeUtil::AESEncode(const char* key, const char* plain, unsigned char* ivec)
{
    AES_KEY aes_key;
    char in[1024] = {0};
    char cipher[1024] = {0};

    int len = strlen(plain);
    int rem = 16 - len%16;
    int bits = strlen(key) * 8;

    memcpy(in, plain, len);
//PKCS5Padding
    for (int i = 0; i < rem; ++i)
    {
        in[len + i] = rem;
    }
    //memset(in + len, ' ', rem);
    len += rem;
    
    AES_set_encrypt_key((const unsigned char*)key, bits, &aes_key);
    if (ivec)
    {
        AES_cbc_encrypt((const unsigned char*)in, (unsigned char*)cipher, len, &aes_key, ivec, 1);
    }
    else
    {
        AES_ecb_encrypt((const unsigned char*)in, (unsigned char*)cipher, len, &aes_key, 1);
    }

    return std::string(cipher, cipher + len);
}

std::string EncodeUtil::SHA_160(const char* filePath)
{
    FILE* fp;
    if ((fp = fopen(filePath, "rb")) == NULL)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "error: %s", strerror(errno));
        return "";
    }

    unsigned long digest[HW] = {0};
    unsigned char data[1024] = {0};
    hash_context context;
    hash_initial(&context);
    size_t readSize = 0;
    //1024bytes once read
    while ((readSize = fread(data, 1, 1024, fp)))
    {
        hash_process(&context, data, readSize);
    }
    hash_final(&context, digest);
    fclose(fp);

    return LongArrayToString(digest, HW);
}

bool EncodeUtil::CalcFileBlockInfos(const char* filePath, 
                        const int blockSize,
                        std::vector<std::string>* shavec,
                        std::vector<std::string>* md5vec,
                        std::vector<int>* sizeVec)
{
    if (!shavec || !md5vec || !sizeVec)
    {
        return false;
    }

    FILE* fp;
    if ((fp = fopen(filePath, "rb")) == NULL)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "error: %s[%s]", strerror(errno), filePath);
        return false;
    }

    unsigned long shaDigest[HW] = {0};
    unsigned long md5Digest[4] = {0};
    unsigned char data[1024] = {0};
    unsigned char dataBak[1024] = {0};
    //sha init
    hash_context shaContext;
    hash_initial(&shaContext);
    //md5 init
    MD5_CTX md5Context;
    MD5Checker::GetInstance()->DK_MD5Init(&md5Context);
    size_t readSize = 0;
    int readBlockSize = 0;

    //1024bytes once read
    while ((readSize = fread(data, 1, 1024, fp)))
    {
        memcpy(dataBak, data, readSize);
        hash_process(&shaContext, data, readSize);
        MD5Checker::GetInstance()->DK_MD5Update(&md5Context, dataBak, readSize);
        //DebugPrintf(DLC_DIAGNOSTIC, "(%x, %x, %x, %x) (%x, %x) (%d)"
                //, md5Context.state[0]
                //, md5Context.state[1]
                //, md5Context.state[2]
                //, md5Context.state[3]
                //, md5Context.count[0]
                //, md5Context.count[1]
                //, readSize);
        readBlockSize += readSize;
        //block finished
        if (readBlockSize >= blockSize)
        {
            hash_final(&shaContext, shaDigest);
            shavec->push_back(LongArrayToString(shaDigest, HW));

            MD5Checker::GetInstance()->DK_MD5Final((unsigned char*)md5Digest, &md5Context);
            md5vec->push_back(BinToHex((const unsigned char*)md5Digest, 4 * sizeof(unsigned long)));
            //md5vec->push_back(LongArrayToString(md5Digest, 4));

            sizeVec->push_back(blockSize);

            //init for a new block
            readBlockSize = 0;
            hash_initial(&shaContext);
            MD5Checker::GetInstance()->DK_MD5Init(&md5Context);
        }
    }

    //the block remained
    if (readBlockSize > 0)
    {
        hash_final(&shaContext, shaDigest);
        shavec->push_back(LongArrayToString(shaDigest, HW));

        MD5Checker::GetInstance()->DK_MD5Final((unsigned char*)md5Digest, &md5Context);
        //md5vec->push_back(LongArrayToString(md5Digest, 4));
        md5vec->push_back(BinToHex((const unsigned char*)md5Digest, 4 * sizeof(unsigned long)));

        sizeVec->push_back(readBlockSize);
    }

    DebugPrintf(DLC_DIAGNOSTIC, "md5: %s", MD5Checker::GetInstance()->DK_MDFile(filePath));

    fclose(fp);
    return true;
}

std::string EncodeUtil::LongArrayToString(unsigned long* array, const int length)
{
    std::string result;
    for (int i = 0; i < length; ++i)
    {
        char data[16] = {0};
        snprintf(data, DK_DIM(data), "%08x", array[i]);
        result.append(data);
    }

    return result;
}
} // namespace utility
} // namespace dk
