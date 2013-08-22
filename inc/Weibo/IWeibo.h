#ifndef IWEIBO_H
#define IWEIBO_H

#include <string>

namespace dk
{
namespace weibo
{
class IWeiboMethod;

class IWeibo
{
public:
    IWeibo() { m_weiboMethod = NULL; }
    virtual ~IWeibo(){}
    virtual IWeiboMethod* GetMethod() const { return m_weiboMethod; }

protected:
    IWeiboMethod* m_weiboMethod;
};//IWeibo

}//weibo
}//dk
#endif//IWEIBO_H
