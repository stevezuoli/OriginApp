//===========================================================================
// Summary:
//     Filter 参数
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-12-02
// Author:
//     Zhai Guanghe(zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_FILTER_FILTERPARAMETER_H__
#define __KERNEL_COMMONLIBS_FILTER_FILTERPARAMETER_H__

#include "KernelType.h"
#include "KernelBaseType.h"
#include "KernelRenderType.h"

enum FilterAlgorithm
{
    FILTER_NONE, 
    // 压缩部分
    FILTER_ZIP, FILTER_ZIP_EX, FILTER_FLATE, 
    FILTER_LZW, FILTER_RUN_LENGTH, FILTER_CCITTFAX, 
    FILTER_DCT, FILTER_JBIG2, FILTER_JPX, 
    // 编码部分
    FILTER_ASCIIHEX, FILTER_ASCII85, FILTER_BASE64, FILTER_URL, 
    // 加密部分
    FILTER_AES_CBC, FILTER_AES_CTR, FILTER_AES_ECB, FILTER_AES_KW, 
    FILTER_AES_CTR_EX, 
    FILTER_TWOFISH, 
    FILTER_RC4, FILTER_RSA_V15, FILTER_RSA, 
    // 散列部分
    FILTER_MD5, FILTER_SHA_1, FILTER_SHA_224, FILTER_SHA_256, FILTER_SHA_384, FILTER_SHA_512 
};

struct ZipParam
{
    DK_UINT uncompSize;
};

struct FlateParam
{
    DK_INT predictor;        // 标志
    DK_INT colors;           // 一个像素的颜色分量数
    DK_INT bitsPerComponent; // 每个颜色分量占的位数
    DK_INT columns;          // 列数（每行像素个数）
    DK_INT earlyChange;      // LZW独有
};

// 设为同一个，便于使用统一的差分函数
typedef FlateParam LzwParam;

struct CCITTFaxParam
{
    DK_INT  k;
    DK_BOOL endOfLine;
    DK_BOOL encodedByteAlign;
    DK_INT  columns;
    DK_INT  rows;
    DK_BOOL endOfBlock;
    DK_BOOL blackIs1;
    DK_INT  damagedRowsBeforeError;
};

struct DctParam
{
    DK_INT    colorTransform;
    DK_DOUBLE scale;  // 缩放比率，使用jpeg库的缩小输出功能，输出缩放比率为?/8的图像（提高速度并减少内存占用）
};

struct Jbig2Param
{
    DK_INT    globalDataLen;
    DK_BYTE * pJBIG2Globals;
    DK_UINT   length;        // 压缩数据的总长
};

struct JpxParam
{
    DK_UINT   length; // 压缩数据的总长
    DK_DOUBLE scale;  // 缩放比率，使用JPX库的缩小输出功能，输出缩放比率位于[scale, 1.0]的图像（提高速度并减少内存占用）
};

struct AesParam
{
    DK_BYTE userKey[32];    // 最多256位密钥
    DK_UINT keyLen;         // 密码长度
    DK_BYTE initVector[16]; // 分块长度128位
};

struct TwofishParam
{
    DK_BYTE userKey[32];    // 最多256位密钥
    DK_UINT keyLen;         // 密码长度
};

struct Rc4Param
{
    DK_BYTE userKey[32];    // 最多256位密钥
    DK_UINT keyLen;         // 密码长度
};

struct RsaParam
{
    DK_BOOL         isPublicKey;
    const DK_BYTE * n;
    DK_UINT         nLen;
    const DK_BYTE * e;
    DK_UINT         eLen;
    const DK_BYTE * d;
    DK_UINT         dLen;
    // 以下仅私钥使用，提高速度
    const DK_BYTE * p;
    DK_UINT         pLen;
    const DK_BYTE * q;
    DK_UINT         qLen;
    const DK_BYTE * dmp1;
    DK_UINT         dmp1Len;
    const DK_BYTE * dmq1;
    DK_UINT         dmq1Len;
    const DK_BYTE * iqmp;
    DK_UINT         iqmpLen;
};

union FilterParam
{
    // 压缩部分
    ZipParam      zip;
    FlateParam    flate;
    LzwParam      lzw;
    CCITTFaxParam ccittfax;
    DctParam      dct;
    Jbig2Param    jbig2;
    JpxParam      jpx;
    // 加密部分
    AesParam      aes;
    TwofishParam  twofish;
    Rc4Param      rc4;
    RsaParam      rsa;
};

enum FILTER_COLORSPACE_TYPE
{
    FILTER_COLORSPACE_UNDEFINED,
    FILTER_COLORSPACE_GRAY,
    FILTER_COLORSPACE_RGB
};

struct FilterInfo
{
    DK_UINT bound;                      // Decode / Encode的数据长度上界
    // 图像属性，当前只有JPX返回
    FILTER_COLORSPACE_TYPE colorSpace;
    DK_UINT width;
    DK_UINT height;

    FilterInfo()
    : bound(0)
    , colorSpace(FILTER_COLORSPACE_UNDEFINED)
    , width(0)
    , height(0)
    {
    }
};

#endif // __KERNEL_COMMONLIBS_FILTER_FILTERPARAMETER_H__
