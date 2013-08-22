#include "IME/IMENumber.h"

void IMENumber::Init() 
{

}
IMENumber::IMENumber()
{
}

eProcessResult IMENumber::ProcessKeys(char iASCIICode)
{
    std::string s = "PQWERTYUIO";
    char buf[2] = {0, 0};
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (iASCIICode == s[i] || iASCIICode == s[i] - 'A' + 'a')
        {
            buf[0] = '0' + i;
            break;
        }
    }
    if (!buf[0])
    {
        m_result.clear();
        return CAN_NOT_PROCESS;
    }
    else
    {
        m_result = buf;
        return PROCESSED_WITH_RESULT;
    }
}

eProcessResult IMENumber::SelectIndex(int iIndex)
{
    if(iIndex == 1)
    {
        return PROCESSED_WITH_RESULT;
    }
    else
    {
        return CAN_NOT_PROCESS;
    }
}
std::string IMENumber::GetInputString()
{
    return "";
}
std::string IMENumber::GetStringByIndex(unsigned int Index)
{
    return m_result;
}
unsigned int IMENumber::SetInputIndex(unsigned int CurIndex)
{
    return 0;
}
unsigned int IMENumber::GetInputIndex()
{
    return 0;
}
void IMENumber::Reset()
{
    m_result.clear();
}
std::string IMENumber::GetUpTip()
{
    return "1\t2\t3\t4\t5\t6\t7\t8\t9\t0\t";
}
std::string IMENumber::GetDownTip()
{
    return "Q\tW\tE\tR\tT\tY\tU\tI\tO\tP\t";
}

