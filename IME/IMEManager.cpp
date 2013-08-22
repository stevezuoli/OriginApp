#include "IME/IMEManager.h"
#include "pthread.h"
#include "Common/ConfigInfo.h"
#include "Utility/ThreadHelper.h"

#define IME_NEW(CLASSNAME,SHROTNAME)        \
    Imes[IME_NAME_##CLASSNAME] = new CLASSNAME(); \
    ImesShortNames[IME_NAME_##CLASSNAME] = SHROTNAME;\


#define NO_IME -1

IIMEBase* IMEManager::Imes[IME_COUNT];
INT32 IMEManager::iIndex = NO_IME;
BOOL IMEManager::fIsInit= FALSE;
LPCSTR IMEManager::ImesShortNames[IME_COUNT]={0};
extern INT8 iIMIndex;
pthread_t pThreadIme;

void* IMEManager::_init_ime_data(void*)
{
    IME_LIST(IME_NEW);
    iIndex = NO_IME;
    for(int i=0;i<IME_COUNT;i++)
    {
        if(Imes[i])
        {
            Imes[i]->Init();
        }
    }

    fIsInit = TRUE;
    return NULL;
}

void IMEManager::Init()
{
    static BOOL fFirst = TRUE;
    if (fFirst)
    {
        fFirst = FALSE;
        // pthread_create(&pThreadIme, NULL, _init_ime_data, NULL);
        ThreadHelper::CreateThread(&pThreadIme, _init_ime_data, NULL, "IMEManager @ _init_ime_data");
    }
    else
    {
        if(fIsInit == FALSE)
        {
            ThreadHelper::JoinThread(pThreadIme, NULL);
        }
    }

};
IIMEBase* IMEManager::GetNext()
{
    while(!fIsInit)
    {
        sleep(1);
    }
    if(iIndex != -1)
    {
        Imes[iIndex]->Reset();
    }
    iIndex++;
    if(iIndex == 0 && SystemSettingInfo::GetInstance()->m_IME.GetOptionIME() / 2 % 2 == 0)
    {
        iIndex ++;
    }
    if(iIndex == 1 && SystemSettingInfo::GetInstance()->m_IME.GetOptionIME() / 4 % 2 == 0)
    {
        iIndex ++;
    }
    if(iIndex == 2 && SystemSettingInfo::GetInstance()->m_IME.GetOptionIME() / 8 % 2 == 0)
    {
        iIndex ++;
    }
    if(iIndex == 3 && SystemSettingInfo::GetInstance()->m_IME.GetOptionIME() / 16 % 2 == 0)
    {
        iIndex ++;
    }
    if(iIndex == 4 && SystemSettingInfo::GetInstance()->m_IME.GetOptionIME() / 32 % 2 == 0)
    {
        iIndex ++;
    }
    if(iIndex >=IME_COUNT)
    {
        iIndex = NO_IME;
        return NULL;
    }
    else
    {
        Imes[iIndex]->Reset();
        return Imes[iIndex];
    }

}
INT32 IMEManager::GetImeCount()
{
    return IME_COUNT;
}
INT32 IMEManager::GetCurImeIndex()
{
    // TODO:
    return iIndex;
}
LPCSTR IMEManager::GetCurImeShortName()
{
    if(iIndex>=0 && iIndex< IME_COUNT)
    {
        return ImesShortNames[iIndex];
    }
    else
    {
        return NULL;
    }
}

void IMEManager::Clear()
{
    iIndex = NO_IME;
}

IIMEBase* IMEManager::GetActiveIME()
{
	if(!fIsInit) 
	{
		Init();
	}

	while(!fIsInit)
	{
		sleep(1);
	}

    int ime = SystemSettingInfo::GetInstance()->m_IME.GetOptionIME();

    DebugPrintf(DLC_LUOXP, "IMEManager::GetActiveIME() ime = %d", ime);

    int index = 0;
    while ( ime >= 1 && (ime & 2) == 0)
    {
        ime /= 2;
        index++;
    }

    DebugPrintf(DLC_LUOXP, "IMEManager::GetActiveIME() index = %d", index);

	return Imes[index];
}


