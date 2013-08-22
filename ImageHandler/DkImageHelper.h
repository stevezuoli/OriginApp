#ifndef __DKIMAGEHELPER_INCLUDE__
#define __DKIMAGEHELPER_INCLUDE__

#include "dkBaseType.h"
#include "ImageHandler/ImageHandler.h"

namespace DkFormat
{
    class DkImageHelper
    {
    public:
        // 内存由外部申请
        static bool DitherRGB32ToGray16(const DK_IMAGE &srcImage, DK_IMAGE *pDstImage);
        // 会替换内存
        static bool ConvertRGB32ToGray256(DK_IMAGE *pImage);
        // 不会替换内存
        static void ConvertGray256ToGray16(const DK_IMAGE &image);
        static void ConvertGray256ToBinary(const DK_IMAGE &image);
        static void DitherGray256ToGray16(const DK_IMAGE &image);
        static void DitherGray256ToBinary(const DK_IMAGE &image);
    public:
        static ImageHandler * CreateImageHandlerInstance(const char *pFilePath);
        static ImageHandler * CreateImageHandlerInstance(IDkStream *pImgStream);
    };
}

#endif
