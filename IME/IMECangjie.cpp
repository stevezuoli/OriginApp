#include "IME/IMEFcitx.h"
#include "IME/IMECangjie.h"


void IMECangjie::Init()
{
    IMEFcitx::GetInstance()->Init();
}
eProcessResult IMECangjie::ProcessKeys(char iASCIICode)
{
    return IMEFcitx::GetInstance()->ProcessKeys(iASCIICode);
}
eProcessResult IMECangjie::SelectIndex(int iIndex)
{
    return IMEFcitx::GetInstance()->SelectIndex(iIndex);
}
std::string IMECangjie::GetInputString()
{
    return IMEFcitx::GetInstance()->GetInputString();
}
std::string IMECangjie::GetStringByIndex(unsigned int Index)
{
    return IMEFcitx::GetInstance()->GetStringByIndex(Index);
}
unsigned int IMECangjie::SetInputIndex(unsigned int CurIndex)
{
    return IMEFcitx::GetInstance()->SetInputIndex(CurIndex);
}
unsigned int IMECangjie::GetInputIndex()
{
    return IMEFcitx::GetInstance()->GetInputIndex();
}
void IMECangjie::Reset()
{
    IMEFcitx::GetInstance()->Reset();
    IMEFcitx::GetInstance()->SetCangjie();
}

