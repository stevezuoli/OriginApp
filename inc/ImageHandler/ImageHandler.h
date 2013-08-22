#ifndef __IMAGEHANDLER_INCLUDE__
#define __IMAGEHANDLER_INCLUDE__

#include "dkObjBase.h"
#include "DkSPtr.h"
#include "GUI/dkImage.h"
#include "dkBuffer.h"
#include "SequentialStream.h"

class IDkStream;

namespace DkFormat
{
    class ImageHandler
    {
    public:
        virtual ~ImageHandler() {};

        virtual bool LoadFromFile(const char *pFileName) = 0;
        virtual bool LoadFromStream(IDkStream *pStream) = 0;
        // 返回原始宽度
        virtual int GetWidth() = 0;
        // 返回原始高度
        virtual int GetHeight() = 0;
        // 将图片指定区域裁剪出并缩放到指定大小
        // 如果等比例缩放的话，上层应保证传入的高宽比一致
        virtual bool CropAndResize(DK_RECT srcRect, int iNewWidth, int iNewHeight, int iColorChannels = 4) = 0;
        // 设置旋转角度，要在CropAndResize后调用，传入0/90/180/270
        virtual bool Rotate(int iAngle) = 0;
        // 获取DK_IMAGE，应在CropAndResize和RotateImage（如果需要）后调用
        virtual bool GetImage(DK_IMAGE *pImg) = 0;
    };
}
#endif
