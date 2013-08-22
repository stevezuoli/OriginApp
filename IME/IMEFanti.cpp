#include "IME/IMEFcitx.h"
#include "IME/IMEFanti.h"


void IMEFanti::Init()
{
    IMEFcitx::GetInstance()->Init();
}

eProcessResult IMEFanti::ProcessKeys(char iASCIICode)
{
    return IMEFcitx::GetInstance()->ProcessKeys(iASCIICode);
}

eProcessResult IMEFanti::SelectIndex(int iIndex)
{
    return IMEFcitx::GetInstance()->SelectIndex(iIndex);
}

std::string IMEFanti::GetInputString()
{
    return IMEFcitx::GetInstance()->GetInputString();
}

std::string IMEFanti::GetStringByIndex(unsigned int Index)
{
    return IMEFcitx::GetInstance()->GetStringByIndex(Index);
}

unsigned int IMEFanti::SetInputIndex(unsigned int CurIndex)
{
    return IMEFcitx::GetInstance()->SetInputIndex(CurIndex);
}

unsigned int IMEFanti::GetInputIndex()
{
    return IMEFcitx::GetInstance()->GetInputIndex();
}

void IMEFanti::Reset()
{
    IMEFcitx::GetInstance()->Reset();
    IMEFcitx::GetInstance()->SetPinyinFanti();
}

