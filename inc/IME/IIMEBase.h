#ifndef __IIMEBASE_H__
#define __IIMEBASE_H__

#ifndef WORD16
    #define WORD16 unsigned short
#endif
#include <string>
typedef enum
{
    CAN_NOT_PROCESS    ,
    PROCESSED_NO_RESULT    ,
    PROCESSED_WITH_RESULT
}eProcessResult;

class IIMEBase
{
public:
    /******************************
    *    
    *    函数名:Init()
    *    功能:进行必要的初始化
    *
    *******************************/
    virtual void Init() = 0;
    /******************************
    *    
    *    函数名:ProcessKeys
    *    功能:将输入的字符传入输入法 并返回处理状态
    *    参数: 
    *        iASCIICode - ASCII 码值可能出现的有 
    *                        'a'-'z'
    *                        'A'-'Z'
    *                        8(回格)
    *                        13(回车)
    *    返回值:
    *        CAN_NOT_PROCESS            - 五笔状态下输入Z ， 光标在第一位时输入8(回格) 都无法处理 返回该值
    *        PROCESSED_NO_RESULT     - 选词状态没有最终结果   例:输入 'a' 得到 "啊 阿 吖"等字符串 并不能确定最终结果 
    *        PROCESSED_WITH_RESULT     - 已经确定结果,GetStringByIndex(0)作为最终结果.  例:五笔4个字符后会确定一个字
    *
    *******************************/
    virtual eProcessResult ProcessKeys(char iASCIICode) = 0;

    /******************************
    *    
    *    函数名:SelectIndex
    *    功能:选择了指定索引的候选词
    *    参数:0开始的索引值 
    *    返回值:
    *        CAN_NOT_PROCESS         - 一共只有4个候选词  如果申请index = 5 则无法处理
    *        PROCESSED_NO_RESULT     - 选词状态没有最终结果     例:输入 'nh' 得到 "你好=0 你=1 您=2 "等字符串 在使用SelectIndex(1) 得到 "你h" 并不能确定最终结果 
    *        PROCESSED_WITH_RESULT    - 已经确定结果,GetStringByIndex(0)作为最终结果 。 
                                        例:输入 'nh' 得到 "你好=0 你=1 您=2 "等字符串 在使用SelectIndex(0) 得到 "你好" 并可以使用 GetStringByIndex(0)得到你好字符串
    *
    *******************************/
    virtual eProcessResult SelectIndex(int iIndex) = 0;

    /******************************
    *    
    *    函数名:GetInputString
    *    功能:获取当前的拼音输入字符串
    *    返回值:小端unicode编码的字符串
    *    举例:
    *        通过ProcessKey依次输入"dk"2个字符,得到例如"贷款 打开 端口 多 的"等候选词。此时调用函数ProcessKey 3(对应"多"字) 再调用GetInputString 则返回"多k"字符串。
    *
    *******************************/
    virtual std::string GetInputString()= 0;

    /******************************
    *    
    *    函数名:GetStringByIndex
    *    功能:获取指定索引的候选词
    *    参数:0开始的索引值
    *    返回值:返回00结尾的 小端unicode
    *
    *******************************/
    virtual std::string GetStringByIndex(unsigned int Index)= 0;

    /******************************
    *    
    *    函数名:GetStringByIndex
    *
    *******************************/
    virtual std::string GetResultString(){return this->GetStringByIndex(0);};
    
    /******************************
    *    
    *    函数名:SetInputIndex
    *    功能:指定光标位于单词的位置 非强制性 
    *    参数:
    *    CurIndex - 需要设置的位置索引值
    *    返回值:
    *        实际设置的位置索引
    *    注意:
    *        返回的索引值应以字符个数个准
    *        例如unicode的"hello" 占用10个字节 但是只有5个字 所以index值应该取0-5 0在h之前 5在o之后
    *******************************/
    virtual unsigned int SetInputIndex(unsigned int CurIndex) = 0;

    /******************************
    *    
    *    函数名:GetInputIndex
    *    功能:取得当前光标位置
    *    返回值:
    *        光标位置值
    *******************************/
    virtual unsigned int GetInputIndex() = 0;

    /******************************
    *    
    *    函数名:Reset
    *    功能:清除所有缓存数据 开始录入新单词 在切换到该输入法时 以及关闭该输入法时都会自动调用到该
    *******************************/
    virtual void Reset() = 0;

    virtual ~IIMEBase(){};
};

#endif
