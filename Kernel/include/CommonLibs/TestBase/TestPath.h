#ifndef __KERNEL_TESTS_TESTBASE_TESTPATH_H__
#define __KERNEL_TESTS_TESTBASE_TESTPATH_H__
#include "UnicodeString.h"
class TestPath
{
public:
    static DK_VOID SetProjectName(const DK_CHAR* prjName);
    static DK_BOOL GetInFilePath(const DK_CHAR* fileName, DK_STRING* result);
    static DK_BOOL GetFontFilePath(const DK_CHAR* fileName, DK_STRING* result);
    static DK_BOOL GetOutFilePath(const DK_CHAR* fileName, DK_STRING* result);
};
#endif
