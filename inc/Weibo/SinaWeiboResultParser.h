/*
 * =====================================================================================
 *       Filename:  SinaWeiboResultParser.h
 *    Descri ption:  新浪微博返回结果分析器
 *
 *        Version:  1.0
 *        Created:  12/28/2012 10:26:34 AM
 * =====================================================================================
 */

#ifndef SINAWEIBORESULTPARSER_H 
#define SINAWEIBORESULTPARSER_H

#include "Weibo/ParsingDataStruct.h"

namespace dk
{
namespace weibo
{
class SinaWeiboResultParser
{
public:
    static ParsingOauthRetSPtr DoParseOauthRet(const char* oauthRetJsonString);
    static ParsingApiErrorSPtr DoParseApiError(const char* apiErrorJsonString);
    static ParsingUserSPtr DoParseUser(const char* userJsonString);
};          

};//weibo
}//dk
#endif//SINAWEIBORESULTPARSER_H
