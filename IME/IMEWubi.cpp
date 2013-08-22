#include "IME/IMEWubi.h"
#include "IME/IMEFcitx.h"

void IMEWubi::Init()
{
    IMEFcitx::GetInstance()->Init();
}
eProcessResult IMEWubi::ProcessKeys(char iASCIICode)
{
    if('z' == iASCIICode || 'Z' == iASCIICode)
    {
        return CAN_NOT_PROCESS;
    }
    return IMEFcitx::GetInstance()->ProcessKeys(iASCIICode);
}
eProcessResult IMEWubi::SelectIndex(int iIndex)
{
    return IMEFcitx::GetInstance()->SelectIndex(iIndex);
}
std::string IMEWubi::GetInputString()
{
    return IMEFcitx::GetInstance()->GetInputString();
}
std::string IMEWubi::GetStringByIndex(unsigned int Index)
{
    return IMEFcitx::GetInstance()->GetStringByIndex(Index);
}
unsigned int IMEWubi::SetInputIndex(unsigned int CurIndex)
{
    return IMEFcitx::GetInstance()->SetInputIndex(CurIndex);
}
unsigned int IMEWubi::GetInputIndex()
{
    return IMEFcitx::GetInstance()->GetInputIndex();
}
void IMEWubi::Reset()
{
    IMEFcitx::GetInstance()->Reset();
    IMEFcitx::GetInstance()->SetWubi();
}

