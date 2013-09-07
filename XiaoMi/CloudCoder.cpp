#include "interface.h"
#include "XiaoMi/CloudCoder.h"
#include "Utility/StringUtil.h"
#include "Utility/EncodeUtil.h"
#include "KernelDef.h"
#include "Common/Url.h"
#include <stdio.h>
#include <vector>

using namespace dk::utility;
using namespace dk::common;
namespace xiaomi
{
string CloudCoder::GenerateSignature(string method, string requestUrl, map<string, string> params, string security)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "method: %s, requestUrl: %s, security: %s",
    //        method.c_str(), requestUrl.c_str(), security.c_str());
    if (security.empty())
    {
        DebugPrintf(DLC_ERROR, "security is empty.");
        return "";
    }
    vector<string> exps;
    if (!method.empty())
    {
        exps.push_back(StringUtil::ToUpper(method.c_str()));
    }

    if (!requestUrl.empty())
    {
        //from java
        //Uri uri = Uri.parse(requestUrl);
        //exps.add(uri.getEncodedPath());
        Url url(requestUrl.c_str());
        exps.push_back(url.GetPath());
    }

    if (!params.empty())
    {
        string param;
        map<string, string>::const_iterator i = params.begin();
        while (i != params.end())
        {
            param.clear();
            param.append(i->first).append(1, '=').append(i->second);
            exps.push_back(param);
            ++i;
        }
    }

    exps.push_back(security);

    bool first = true;
    string text;
    for (vector<string>::iterator i = exps.begin(); i != exps.end(); ++i)
    {
        if (!first)
            text.append(1, '&');

        text.append(i->c_str());
        first = false;
    }

    //DebugPrintf(DLC_DIAGNOSTIC, "text: %s", text.c_str());
    //DebugPrintf(DLC_DIAGNOSTIC, "for test: %s", Hash4SHA1("0123456789abcdefghijklmnopqrstuvwxyz").c_str());
    return Hash4SHA1(text);
}

string CloudCoder::Hash4SHA1(string plain)
{
    std::vector<unsigned char> digest = EncodeUtil::SHA_1((const void*)plain.c_str(), plain.size());
    //for (int i = 0; i < digest.size(); ++i)
    //{
        //DebugPrintf(DLC_DIAGNOSTIC, "\tdigest: %x", digest[i]);
    //}
    if (!digest.empty())
    {
        std::vector<unsigned char> result = EncodeUtil::Base64Encode(&digest[0], digest.size());
        return string(result.begin(), result.end());
    }
    //sha1_hash->reset();
    //if (sha1_hash)
    //{
        //sha1_hash->addData(plain.toAscii());
        //return sha1_hash->result().toBase64();
    //}

    return "";
}

string CloudCoder::EncodeString(const string& security, const string& data)
{
    std::vector<unsigned char> key = EncodeUtil::Base64Decode(security.c_str(), security.length());
    std::string strKey(key.begin(), key.end());

    if (!key.empty())
    {
        unsigned char iv[] = "0102030405060708";
        string cipherText = EncodeUtil::AESEncode(strKey.c_str(), data.c_str(), iv);
        std::vector<unsigned char> result = EncodeUtil::Base64Encode(cipherText.c_str(), cipherText.length());
        return string(result.begin(), result.end());
    }
    
    return "";
}

string CloudCoder::DecodeString(const string& security, const string& data)
{
    std::vector<unsigned char> key = EncodeUtil::Base64Decode(security.c_str(), security.length());
    std::string strKey(key.begin(), key.end());

    if (!key.empty())
    {
        unsigned char iv[] = "0102030405060708";
        string plainText = EncodeUtil::AESDecode(strKey.c_str(), data.c_str(), data.size(), iv);
        return plainText;
        //std::vector<unsigned char> result = EncodeUtil::Base64Encode(plainText.c_str(), plainText.length());
        //return string(result.begin(), result.end());
    }

    return "";
}
}
