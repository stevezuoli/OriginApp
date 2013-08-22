#ifndef __IMENUMBER_H__
#define __IMENUMBER_H__

#include "dkBaseType.h"
#include "IIMEBase.h"

class IMENumber : public IIMEBase
{
public:
    IMENumber();
    virtual void Init() ;
    virtual eProcessResult ProcessKeys(char iASCIICode) ;
    virtual eProcessResult SelectIndex(int iIndex) ;
    virtual std::string GetInputString();
    virtual std::string GetStringByIndex(unsigned int Index);
    virtual unsigned int SetInputIndex(unsigned int CurIndex);
    virtual unsigned int GetInputIndex();
    virtual void Reset();
    virtual std::string GetUpTip();
    virtual std::string GetDownTip();
private:
    std::string m_result;
};


#endif
