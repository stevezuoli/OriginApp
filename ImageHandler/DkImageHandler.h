#ifndef __DKIMAGEHANDLERIMPL_INCLUDED__
#define __DKIMAGEHANDLERIMPL_INCLUDED__

#include "ImageHandler/ImageHandler.h"

namespace DkFormat
{
    class DkImageHandler : public ImageHandler
    {
    public:
        DkImageHandler();
        virtual ~DkImageHandler();

        virtual bool LoadFromFile(const char *pFileName);
        virtual bool LoadFromStream(IDkStream *pStream);
        virtual int GetWidth() { return m_iWidth; }
        virtual int GetHeight() { return m_iHeight; }
        virtual bool CropAndResize(DK_RECT srcRect, int iNewWidth, int iNewHeight, int iColorChannels = 4);
        virtual bool Rotate(int iAngle);
        virtual bool GetImage(DK_IMAGE *pImg);

    private:
        bool Initialize();
        bool ResetImage(DK_IMAGE *pImage, int iColorChannels = 0, int iWidth = 0, int iHeight = 0);
        void TransformImage();

    private:
        IDkStream * m_pStream;
        DK_IMAGE    m_image;
        int         m_iWidth;
        int         m_iHeight;
    };
}

#endif    // __DKIMAGEHANDLERIMPL_INCLUDED__
