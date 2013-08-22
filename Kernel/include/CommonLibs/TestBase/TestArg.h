#ifndef __KERNEL_TESTS_TESTBASE_TESTARG_H__
#define __KERNEL_TESTS_TESTBASE_TESTARG_H__

#include "KernelType.h"

class TestArg
{
public:
    // 如果arg以"key:"开头则返回:之后的字符串位置,否则返回DK_NULL
    static const DK_CHAR* MatchArgWithPara(const DK_CHAR* arg, const DK_CHAR* key);
    static DK_BOOL MatchArg(const DK_CHAR* arg, const DK_CHAR* key);
private:
    TestArg();
};
#endif
