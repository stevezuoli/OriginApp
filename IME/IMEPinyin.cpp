#include "IME/IMEPinyin.h"
#include "IME/IMEFcitx.h"

void IMEPinyin::Init()
{
    IMEFcitx::GetInstance()->Init();
}

eProcessResult IMEPinyin::ProcessKeys(char iASCIICode)
{
    return IMEFcitx::GetInstance()->ProcessKeys(iASCIICode);
}

eProcessResult IMEPinyin::SelectIndex(int iIndex)
{
    return IMEFcitx::GetInstance()->SelectIndex(iIndex);
}

std::string IMEPinyin::GetInputString()
{
    return IMEFcitx::GetInstance()->GetInputString();
}

std::string IMEPinyin::GetStringByIndex(unsigned int Index)
{
    return IMEFcitx::GetInstance()->GetStringByIndex(Index);
}

unsigned int IMEPinyin::SetInputIndex(unsigned int CurIndex)
{
    return IMEFcitx::GetInstance()->SetInputIndex(CurIndex);
}

unsigned int IMEPinyin::GetInputIndex()
{
    return IMEFcitx::GetInstance()->GetInputIndex();
}

void IMEPinyin::Reset()
{
    IMEFcitx::GetInstance()->Reset();
    IMEFcitx::GetInstance()->SetPinyin();
}


