#ifndef __IMEFANTI_H__
#define __IMEFANTI_H__
#include "IME/IIMEBase.h"

class IMEFanti : public IIMEBase
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

