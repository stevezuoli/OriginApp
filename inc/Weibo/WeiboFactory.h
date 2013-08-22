#ifndef WEIBOFACTORY_H
#define WEIBOFACTORY_H

#include "Weibo/SinaWeiboManager.h"
#include "Weibo/SinaWeiboMethodImpl.h"
#include "Weibo/ParsingDataStruct.h"

namespace dk
{
namespace weibo
{
class WeiboFactory
{
public:
    static SinaWeiboManager* GetSinaWeibo()
    {
        static SinaWeiboManager sinaWeiboManager;
        return &sinaWeiboManager;
    }
    //
};//WeiboFactory
}//weibo
}//dk
#endif//WEIBOFACTORY_H
