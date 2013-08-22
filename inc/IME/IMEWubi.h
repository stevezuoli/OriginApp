#ifndef __IMEWUBI_H__
#define __IMEWUBI_H__
#include "IME/IIMEBase.h"

class IMEWubi:public IIMEBase
{
public:
    virtual void Init();
    virtual eProcessResult ProcessKeys(char iASCIICode);
    virtual eProcessResult SelectIndex(int iIndex) ;
    virtual std::string GetInputString();
    virtual std::string GetStringByIndex(unsigned int Index);
    virtual unsigned int SetInputIndex(unsigned int CurIndex) ;
    virtual unsigned int GetInputIndex();
    virtual void Reset();
};
#endif

