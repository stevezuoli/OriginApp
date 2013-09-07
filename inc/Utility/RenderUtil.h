#ifndef __UTILITY_RENDERUTIL_H__
#define __UTILITY_RENDERUTIL_H__

#include "GUI/dkImage.h"
#include "KernelRenderType.h"
#include <stdint.h>

using DkFormat::DK_IMAGE;

namespace dk
{
namespace utility
{

class RenderUtil
{
public:
    static bool CreateRenderImageAndDevice32(
            int width,
            int height,
            const DK_ARGBCOLOR& backgroundColor,
            DK_IMAGE* image,
            DK_BITMAPBUFFER_DEV* dev);
    static bool SaveBitmapToJpeg(const DkFormat::DK_IMAGE& bitmap, const char* jpgFilePath);
    static bool SaveDKImage(DkFormat::DK_IMAGE image, const char* filename);
private:
    RenderUtil();
}; // class RenderUtil

} // namespace utility
} // namespace dk

#endif

