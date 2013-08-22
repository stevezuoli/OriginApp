//===========================================================================
// Summary:
//        IBookReader接口定义
// Version:
//    0.1 draft - creating.
//    
// Usage:
//        Null
// Remarks:
//        Null
// Date:
//        2011-11-21
// Author:
//        Chen Mao (chenm@duokan.com)
//===========================================================================

#ifndef __KINDLEAPP_INC_BMPPROCESSOR_H__
#define __KINDLEAPP_INC_BMPPROCESSOR_H__

#define _LINUX 1
#include "KernelType.h"
#include "GUI/dkImage.h"

/*
// 测试用，dkImage.h里面的应该需要修改
typedef struct __tagDK_IMAGE{
      int   iColorChannels; // 色彩通道数，目前可为1（8位灰度图）或4（32位彩色图）
      DK_LONG  lWidth;         // 图象实际宽度(以象素点为单位, 每象素点所占用的字节数由"图象类型"决定 )
      DK_LONG  lHeight;         // 图象高度(以象素点为单位, 每象素点所占用的字节数由"图象类型"决定)
      DK_LONG  lStride;         // 图象存储宽度(以字节为单位)
      unsigned char* pbData;         // 图象数据
      __tagDK_IMAGE()
        : iColorChannels(4)
        , lWidth(600)
        , lHeight(800)
        , lStride(600*4)
        , pbData(DK_NULL)
    {
    };
} DK_IMAGE;
*/

using DkFormat::DK_IMAGE;

// class defined in this header
class IBMPProcessor;
class BMPProcessFactory;
class RotateProcessor;
class SharpenProcessor;
class DarkenProcessor;
class GrayScaleProcessor;

class IBMPProcessor
{
public:
    enum TYPE
    {
        DK_BMPPROCESSOR_ROTATE,
        DK_BMPPROCESSOR_SHARPEN,
        DK_BMPPROCESSOR_DARKEN,
        DK_BMPPROCESSOR_GRAYSCALE
    };

    virtual ~IBMPProcessor(){};
    virtual bool Process(DK_IMAGE* pImage) = 0;
    virtual IBMPProcessor::TYPE GetType () = 0;
};

class BMPProcessFactory
{
public:
    static IBMPProcessor* CreateInstance(IBMPProcessor::TYPE eType, int iPara = 0);
};

//-------------------------------------------
// Summary:
//        图像旋转处理器
//-------------------------------------------
class RotateProcessor: public IBMPProcessor
{
public:
    RotateProcessor(int rotate);
    virtual bool Process(DK_IMAGE* pImage);
    virtual IBMPProcessor::TYPE GetType () { return IBMPProcessor::DK_BMPPROCESSOR_ROTATE; }
};

//-------------------------------------------
// Summary:
//        图像锐化处理器
//-------------------------------------------
class SharpenProcessor: public IBMPProcessor
{
public:
    SharpenProcessor ();

    static  IBMPProcessor* GetInstance();

    void SetSharpenLevel (int iLevel) { m_iSharpenLevel = iLevel; }
    virtual bool Process(DK_IMAGE* pImage);
    virtual IBMPProcessor::TYPE GetType () { return IBMPProcessor::DK_BMPPROCESSOR_SHARPEN; }

private:
    bool SharpenImage(unsigned char* pBuf, unsigned int uWidth, unsigned int uHeight, unsigned int uN, int iLevel);
    void SharpenImage3x3(unsigned char* pBuf, unsigned int uWidth, unsigned int uHeight, unsigned int uN, int iLevel, unsigned char* pResult);

private:
    static IBMPProcessor* m_pInstance;
    int m_iSharpenLevel;
};

//-------------------------------------------
// Summary:
//        图像加黑处理器
//-------------------------------------------
class DarkenProcessor: public IBMPProcessor
{
public:
    DarkenProcessor();

    static  IBMPProcessor* GetInstance();
    
    void SetDarkenLevel (int iLevel) { m_iDarkenLevel = iLevel; }
    virtual bool Process(DK_IMAGE* pImage);
    virtual IBMPProcessor::TYPE GetType () { return IBMPProcessor::DK_BMPPROCESSOR_DARKEN; }

private:
    bool EmboldenImage(unsigned char* pBuf, unsigned int uWidth, unsigned int uHeight, unsigned int uN, int iLevel);

private:
    static IBMPProcessor* m_pInstance;
    int m_iDarkenLevel;
};

//-------------------------------------------
// Summary:
//        彩色变换会灰度处理器
//-------------------------------------------
class GrayScaleProcessor: public IBMPProcessor
{
public:
    static  IBMPProcessor* GetInstance();
    virtual bool Process(DK_IMAGE* pImage);
    virtual IBMPProcessor::TYPE GetType () { return IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE; }
    
private:
    unsigned char ConvertRGBToGray(unsigned char blue, unsigned char green, unsigned char red);
    bool RGBToGray(DK_IMAGE* pImage);
    
    static IBMPProcessor* m_pInstance;
};

#endif //__KINDLEAPP_INC_BMPPROCESSOR_H__
