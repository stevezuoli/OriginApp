#ifndef __KERNEL_TEST_TESTEPUBLIB_DKETESTUTIL_H__
#define __KERNEL_TEST_TESTEPUBLIB_DKETESTUTIL_H__

#include "DKEAPI.h"

class IDkStream;

class DKETestScope
{
public:
    DKETestScope();

    ~DKETestScope();
private:
    DISALLOW_COPY_AND_ASSIGN(DKETestScope);
};
class UnicodeString;

DK_VOID DKE_RegisterUtf8Font(const DK_CHAR* faceName, const DK_CHAR* fontFile);
DK_VOID DKE_SetUtf8DefaultFont(const DK_CHAR* faceName, DK_CHARSET_TYPE type);
DK_ReturnCode GetFullPathInFs(const UnicodeString& refPath, const UnicodeString& relPath, UnicodeString* pFullPath);
DK_ReturnCode GetFsFileStreamByPath(const UnicodeString& fullPath, IDkStream** ppStream);
DK_BOOL IsFileExistInFS(const UnicodeString& fullPath);

#endif
