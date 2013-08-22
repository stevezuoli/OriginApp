#ifndef __UTILITY_CHARUTIL_H__
#define __UTILITY_CHARUTIL_H__

namespace dk
{
namespace utility
{
class CharUtil
{
public:
    static int HexCharToInt(char c)
    {
        if ('0' <= c && c <= '9')
        {
            return c - '0';
        }
        else if ('a' <= c && c <= 'f')
        {
            return 10 + c - 'a';
        }
        else if ('A' <= c && c <= 'F')
        {
            return 10 + c - 'A';
        }
        return -1;
    }

    static char IntToHexChar(int i)
    {
        if (0 <= i && i <= 9)
        {
            return i + '0';
        }
        else if (i < 16)
        {
            return i - 10 + 'a';
        }
        else
        {
            return 0;
        }
    }

    // 按照中文习惯的顺序比较，特殊字符在前，数字与字母在后，然后是中文
    inline static int CharCompareByDisplay(int a, int b)
    {
        if (a < 128 && b < 128)
        {
            if (0 == a || 0 == b)
            {
                return a - b;
            }
            else if (isalnum(a) && !isalnum(b))
            {
                return 1;
            }
            else if (!isalnum(a) && isalnum(b))
            {
                return -1;
            }
            else
            {
                return toupper(a) - toupper(b);
            }
        }
        else
        {
            return a - b;
        }
    }
    inline static int StringCompareByDisplay(const char* a, const char* b)
    {
        const unsigned char* ua = (const unsigned char*)a;
        const unsigned char* ub = (const unsigned char*)b;

        while (*ua && *ub)
        {
            int t = CharCompareByDisplay(*ua, *ub);
            if (t != 0)
            {
                return t;
            }
            ++ua;
            ++ub;
        }
        if (*ua == 0)
        {
            if (*ub == 0)
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return 1;
        }
    }



private:
    CharUtil();
};
} // namespace utility
} // namespace dk
#endif
