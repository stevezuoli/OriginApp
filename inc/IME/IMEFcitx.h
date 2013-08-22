#ifndef __IMEFCITX_H__
#define __IMEFCITX_H__

#include "IIMEBase.h"
#include "singleton.h"  

class IMEFcitx
    :public IIMEBase 
{
SINGLETON_H(IMEFcitx)
    
public:
    virtual eProcessResult ProcessKeys(char iASCIICode);
    virtual eProcessResult SelectIndex(int iIndex);
    virtual std::string GetInputString();
    virtual std::string GetStringByIndex(unsigned int Index);
    virtual unsigned int SetInputIndex(unsigned int CurIndex);
    virtual unsigned int GetInputIndex();
    virtual void Reset();
    virtual void Init();
    void SetPinyin();
    void SetWubi();
    void SetPinyinFanti();
    void SetCangjie();
private:
    IMEFcitx();
    ~IMEFcitx();
    int ChangePageAndGetCurIndex(int Index);
};
#endif
