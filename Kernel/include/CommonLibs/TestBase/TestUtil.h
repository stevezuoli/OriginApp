#ifndef __KERNEL_TESTS_TESTKERNELBASE_TESTUTIL_H__
#define __KERNEL_TESTS_TESTKERNELBASE_TESTUTIL_H__

#include "KernelType.h"

class TestUtil
{
public:
    static DK_BOOL IsStringInFile(const DK_CHAR* path, const DK_CHAR* str);
    static DK_BOOL InitFontConfig(const DK_CHAR* path = DK_NULL);
    static DK_BOOL SaveOutputDevAsBmp(const DK_BITMAPBUFFER_DEV* pDevice, const DK_CHAR* path);
    // 只用于DK_PIXELFORMAT_RGB32
    static DK_BOOL WriteImageToBmp(const DK_BYTE* pBuf, DK_INT w, DK_INT h, const DK_CHAR* path);
    // 只用于DK_PIXELFORMAT_RGB32
    static DK_BOOL IsBlankDev (DK_BITMAPBUFFER_DEV* pDev);
    static DK_VOID DumpProcessStatus();
private:
    TestUtil();
};

#define OUTPUT_LOCATION do{printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);}while(0)

#define ASSERT_WCSEQ(x, y)    ASSERT_TRUE(dk_wcscmp(x, y) == 0)
#define ASSERT_POSEQ(ex, ey, r)    ASSERT_TRUE(DkFloatEqual(ex, (r).X) \
                                            && DkFloatEqual(ey, (r).Y))
                    
std::ostream& operator<<(std::ostream& os, const DK_FLOWPOSITION& pos);
std::ostream& operator<<(std::ostream& os, const DK_BOX& box);

#endif
