#ifndef __CLOUD_CODER_H__
#define __CLOUD_CODER_H__

#include <map>
#include <string>
using namespace std;

namespace xiaomi
{
class CloudCoder
{
public:
    /* *
     * * Generate signature for the request.
     * 
     * @param method     http request method. GET or POST
     * @param requestUrl the full request url. e.g.: http://api.xiaomi.com/getUser?id=123321
     * @param params     request params. This should be a TreeMap because the
     *                   parameters are required to be in lexicographic order.
     * @param security   AES secret key. Must NOT be null.
     * @return hash value for the values provided
     */
    static string GenerateSignature(string method, string requestUrl, map<string, string> params, string security);

    static string Hash4SHA1(string plain);

    static string EncodeString(const string& security, const string& data);
    static string DecodeString(const string& security, const string& data);

private:
    //static QCryptographicHash* sha1_hash;
};//CloudCoder
};//xiaomi
#endif//__CLOUD_CODER_H__
