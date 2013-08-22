#ifndef __IMEMANAGER_H__
#define __IMEMANAGER_H__

#include "dkBaseType.h"
#include "IIMEBase.h"
#include "IME/IMENumber.h"
#include "IME/IMEPinyin.h"
#include "IME/IMEWubi.h"
#include "IME/IMEFanti.h"
#include "IME/IMECangjie.h"
#include "IME/IMELatin.h"
#include "Common/SystemSetting_DK.h"

// TODO: Use StringManager to get the abbr IME name
#define IME_LIST(MACRO_NAME)                \
    MACRO_NAME(IMEPinyin,"拼")              \
    MACRO_NAME(IMEWubi,"五")                \
    MACRO_NAME(IMEFanti,"繁")               \
    MACRO_NAME(IMECangjie,"倉")             \
    MACRO_NAME(IMELatin,"LT")                \

#define IME_NAMES(CLASSNAME,SHROTNAME)      \
    IME_NAME_##CLASSNAME,                   \

enum DK_ALL_IME
{
    IME_LIST(IME_NAMES)
    IME_COUNT
};



class IMEManager
{
public:
	static IIMEBase* GetActiveIME();
    static IIMEBase* GetNext();
    static INT32 GetImeCount();
    static INT32 GetCurImeIndex();
    static void Clear();
    static LPCSTR GetCurImeShortName();
    static void Init();
    static void* _init_ime_data(void*);
private:
    
    static IIMEBase *Imes[IME_COUNT];
    static LPCSTR ImesShortNames[IME_COUNT];
    static INT32 iIndex;
    static BOOL fIsInit;
    
};

#endif
