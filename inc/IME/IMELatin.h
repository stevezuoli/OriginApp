#ifndef __IMELATIN_H__
#define __IMELATIN_H__
#include "IME/IIMEBase.h"

typedef enum
{
    Latin_dutch = 0,
    Latin_finnish,
    Latin_french,
    Latin_german,
    Latin_italy,
    Latin_norwegian,
    Latin_portuguese,
    Latin_spanish,
    Latin_swedish,
    Latin_turkish,
    Latin_All
}LatinType;




class IMELatin:public IIMEBase
{
public:
    IMELatin();
    virtual void Init();
    virtual eProcessResult ProcessKeys(char iASCIICode);
    virtual eProcessResult SelectIndex(int iIndex) ;
    virtual std::string GetInputString();
    virtual std::string GetStringByIndex(unsigned int Index);
    virtual std::string GetResultString();
    virtual unsigned int SetInputIndex(unsigned int CurIndex);
    virtual unsigned int GetInputIndex();
    virtual void Reset();
    
    void SetLatinType(LatinType eLatinType);
protected:
    std::string m_strResult;
    std::string m_cTmpChar;
    std::string m_strInputString;
    std::string m_rgcLatinChar;
    unsigned int  m_iCharCount;
    LatinType m_eLatinType;
    char m_fIsLocking;
};




#endif
