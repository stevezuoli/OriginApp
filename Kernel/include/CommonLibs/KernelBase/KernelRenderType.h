//===========================================================================
// Summary:
//     KernelRenderType.h
// Usage:
//     KernelBase库与render相关的定义
//            ...
// Remarks:
//     Null
// Date:
//     2011-09-16
// Author:
//     Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_KERNELBASE_KERNELRENDERTYPE_H__
#define __KERNEL_COMMONLIBS_KERNELBASE_KERNELRENDERTYPE_H__

//===========================================================================

#include "KernelDef.h"
#include <algorithm>

//===========================================================================
// Enum definition

class RenderConst
{
public:
    enum
    {
        SCREEN_DPI = 96
    };
};

// 度量单位
enum DK_MEASURE_TYPE
{
    DK_MEASURE_PIXEL,             // 像素点
    DK_MEASURE_POINT,             // 磅，定义为1/72英寸
    DK_MEASURE_CM,                // 厘米
    DK_MEASURE_MM,                // 毫米
    DK_MEASURE_INCH,              // 英寸
    DK_MEASURE_CUSTOM             // 用户自定义
};

// 绘制输出设备类型
enum DK_OUTPUTDEV_TYPE
{
    DK_OUTPUTDEV_UNKNOWN,         // 未知类型
    DK_OUTPUTDEV_BITMAPBUFFER,    // 位图缓冲区
};

enum DK_COLORSPACE_TYPE
{
    DK_COLORSPACE_UNDEFINED       = 0,
    DK_COLORSPACE_GRAY            = 1,
    DK_COLORSPACE_RGB             = 2,
    DK_COLORSPACE_ARGB            = 3,
    DK_COLORSPACE_SCRGB           = 4,
    DK_COLORSPACE_CMYK            = 5,
    DK_COLORSPACE_ACMYK           = 6
};

// 路径填充模式
enum DK_FILLMODE_TYPE
{
    DK_FILLMODE_Alternate,
    DK_FILLMODE_Winding
};

enum DK_IMAGEPATTERN_TILING_TYPE
{
    DK_IMAGEPATTERN_TILING_CONSTANTSPACING  = 1,
    DK_IMAGEPATTERN_TILING_NODISTORTION     = 2,
    DK_IMAGEPATTERN_TILING_CONSTANTANDFAST  = 3
};

// 刷子类型
enum DK_BRUSH_TYPE
{
    DK_BRUSH_SOLID            = 0,
    DK_BRUSH_GRAPHICSPATTERN  = 1,
    DK_BRUSH_IMAGEPATTERN     = 2,
    DK_BRUSH_PATHGRADIENT     = 3,
    DK_BRUSH_LINEARGRADIENT   = 4,
    DK_BRUSH_NONE             = 5,
};

// 线型枚举
enum DK_LINEJOIN_TYPE
{
    DK_LINEJOIN_MITER         = 0,
    DK_LINEJOIN_BEVEL         = 1,
    DK_LINEJOIN_ROUND         = 2,
    DK_LINEJOIN_MITERCLIPPED  = 3
};

enum DK_LINECAP_TYPE
{
    DK_LINECAP_FLAT           = 0,
    DK_LINECAP_SQUARE         = 1,
    DK_LINECAP_ROUND          = 2,
    DK_LINECAP_TRIANGLE       = 3,

    DK_LINECAP_NOANCHRO       = 0x10, // corresponds to flat cap
    DK_LINECAP_SQUARE_ANCHOR  = 0x11, // corresponds to square cap
    DK_LINECAP_ROUND_ANCHOR   = 0x12, // corresponds to round cap
    DK_LINECAP_DIAMON_ANCHOR  = 0x13, // corresponds to triangle cap
    DK_LINECAP_ARROR_ANCHOR   = 0x14, // no correspondence

    DK_LINECAP_CUSTOM         = 0xff, // custom cap
    DK_LINECAP_ANCHORMASK     = 0xf0  // mask to check for anchor or not.
};

enum DK_DASHCAP_TYPE
{
    DK_DASHCAP_FLAT           = 0,
    DK_DASHCAP_ROUND          = 2,
    DK_DASHCAP_TRIANGLE       = 3
};

enum DK_DASHSTYLE_TYPE
{
    DK_DASHSTYLE_NONE,
    DK_DASHSTYLE_SOLOD,
    DK_DASHSTYLE_DASH,
    DK_DASHSTYLE_CUSTOM,
};

enum DK_PRINTER_TYPE
{    
    DK_PRINTER_UnKnown = 0,
    DK_PRINTER_PS = 1,
    DK_PRINTER_PCL6 = 2,
    DK_PRINTER_PCL5 = 3
};

enum DK_PRINTEROPTIMIZE_TYPE
{
    DK_PRINTEROPTIMIZE_PPU = 0,
    DK_PRINTEROPTIMIZE_FASTEST = 1,
    DK_PRINTEROPTIMIZE_ACCURATE = 2
};

enum DK_PRINTERIMGOPTIMIZE_TYPE
{
    DK_PRINTEROPTIMIZE_Split    = 0x00000001,
    DK_PRINTEROPTIMIZE_Compress = 0x00000010,
    DK_PRINTEROPTIMIZE_Rough    = 0x00000100
};

// Clip mode
enum DK_COMBINEMODE_TYPE
{
    DK_COMBINEMODE_REPLACE,     // 0
    DK_COMBINEMODE_INTERSECT,   // 1
    DK_COMBINEMODE_UNION,       // 2
    DK_COMBINEMODE_XOR,         // 3
    DK_COMBINEMODE_EXCLUDE,     // 4
    DK_COMBINEMODE_COMPLEMENT   // 5 (Exclude From)
};

// 输出质量
enum DK_QUALITYMODE_TYPE
{
    DK_QUALITYMODE_INVALID   = -1,
    DK_QUALITYMODE_DEFAULT   = 0,
    DK_QUALITYMODE_LOW       = 1, // Best performance
    DK_QUALITYMODE_HIGH      = 2  // Best rendering quality
};

// 图形平滑模式
enum DK_SMOOTHMODE_TYPE
{
    DK_SMOOTHMODE_INVALID        = DK_QUALITYMODE_INVALID,
    DK_SMOOTHMODE_DEFAULT        = DK_QUALITYMODE_DEFAULT,
    DK_SMOOTHMODE_HIGHSPEED      = DK_QUALITYMODE_LOW,
    DK_SMOOTHMODE_HIGHQUALITY    = DK_QUALITYMODE_HIGH,
    DK_SMOOTHMODE_NONE,
    DK_SMOOTHMODE_ANTIALIAS
};

// 使用LCD优化输出文字时，位图数据中像素排列方式
enum DK_LCDMODE_TYPE
{
    DK_LCDMODE_NONE,    // 无LCD优化
    DK_LCDMODE_RGB,     // RGB排列
    DK_LCDMODE_BGR,     // BGR排列
    DK_LCDMODE_VRGB,    // 竖向RGB排列
    DK_LCDMODE_VBGR,    // 竖向BGR排列
};

// 位图像素格式
enum DK_PIXELFORMAT
{
    DK_PIXELFORMAT_UNKNOWN, // 未知格式

    DK_PIXELFORMAT_RGB32,   // 不透明的32位RGB颜色，内存中连续的4字节表示一个像素，依次是BGR，第4个字节不使用
                            // Remark(zhaigh): 在SkiaBasic中，没有区分CPU是BE还是LE，上面的结果只在LE的平台下正确

    DK_PIXELFORMAT_RGB565,  // 不透明的16位RGB颜色，内存中连续的2字节表示一个像素，不依赖BE/LE

    DK_PIXELFORMAT_GRAY,    // 8位灰阶，每个字节表示一个像素不支持，暂不支持

    DK_PIXELFORMAT_ARGB32,  // 保留，暂不支持
};

enum DK_FONT_STYLE
{
    DK_FontStyleRegular           = 0,
    DK_FontStyleBold              = 1,
    DK_FontStyleItalic            = 2,
    DK_FontStyleBoldItalic        = 3,
    DK_FontStyleBoldPlus          = 16,
    DK_FontStyleBoldPlusItalic    = 18
};

// 绘制平滑参数，可以为多个值的组合
enum DK_SMOOTHTAG
{
    DK_SMOOTH_NONE            = 0x0,             // 无平滑
    DK_SMOOTH_IMAGE           = 0x1 << 1,        // 平滑图像
    DK_SMOOTH_GRAPH           = 0x1 << 2,        // 平滑图形
    DK_SMOOTH_TEXT            = 0x1 << 3,        // 平滑文字

    // 针对LCD屏幕优化，只有在设置DK_SMOOTH_TEXT时有效
    DK_SMOOTH_LCDTEXT_RGB     = 0x1 << 4,        // 显示设备像素为横向RGB排列
    DK_SMOOTH_LCDTEXT_BGR     = 0x1 << 5,        // 显示设备像素为横向BGR排列
};

/////////////////////////////////////////////////////////////////////////////
// Bitmap related
/////////////////////////////////////////////////////////////////////////////

#pragma pack(push,2)
struct DK_BITMAPFILEHEADER {
    DK_WORD    bfType;
    DK_DWORD   bfSize;
    DK_WORD    bfReserved1;
    DK_WORD    bfReserved2;
    DK_DWORD   bfOffBits;
};
#pragma pack(pop)

// BMP类型文件头信息
struct DK_BITMAPINFOHEADER
{
    DK_DWORD      biSize;
    DK_INT32       biWidth;
    DK_INT32       biHeight;
    DK_WORD       biPlanes;
    DK_WORD       biBitCount;
    DK_DWORD      biCompression;
    DK_DWORD      biSizeImage;
    DK_INT32       biXPelsPerMeter;
    DK_INT32       biYPelsPerMeter;
    DK_DWORD      biClrUsed;
    DK_DWORD      biClrImportant;
};

// RGB颜色
struct DK_RGBQUAD
{
    DK_BYTE    rgbBlue;
    DK_BYTE    rgbGreen;
    DK_BYTE    rgbRed;
    DK_BYTE    rgbReserved;

    DK_RGBQUAD()
        : rgbBlue((DK_BYTE)0)
          , rgbGreen((DK_BYTE)0)
          , rgbRed((DK_BYTE)0)
          , rgbReserved((DK_BYTE)0)
    {}

    DK_RGBQUAD(DK_BYTE rgbBlue, DK_BYTE rgbGreen, DK_BYTE rgbRed, DK_BYTE rgbReserved)
        : rgbBlue(rgbBlue)
          , rgbGreen(rgbGreen)
          , rgbRed(rgbRed)
          , rgbReserved(rgbReserved)
    {}

    DK_BOOL operator==(const DK_RGBQUAD& color) const
    {
        return rgbBlue == color.rgbBlue 
            && rgbGreen == color.rgbGreen 
            && rgbRed == color.rgbRed 
            && rgbReserved == color.rgbReserved;
    }

    DK_BOOL operator!=(const DK_RGBQUAD& color) const
    {
        return !(*this == color);
    }
    static const DK_RGBQUAD White;
    static const DK_RGBQUAD Black;
};

// BMP数据信息
struct DK_BITMAPINFO
{
    DK_BITMAPINFOHEADER    bmiHeader;
    DK_RGBQUAD             bmiColors[2];
};

inline DK_SIZE_T DkBmpTableNum(const DK_BITMAPINFOHEADER& bmih)
{
    DK_SIZE_T colorTableEntryCount = 0;
    if (bmih.biBitCount < 16)
    {
        colorTableEntryCount = bmih.biClrUsed ? bmih.biClrUsed : (1 << bmih.biBitCount);
    }
    return colorTableEntryCount;
}

inline DK_SIZE_T DkBmpTableSize(const DK_BITMAPINFOHEADER& bmih)
{
    return DkBmpTableNum(bmih) * 4;
}

inline DK_SIZE_T DkBmpInfoSize(const DK_BITMAPINFO& bmi)
{
    return DkBmpTableSize(bmi.bmiHeader) + sizeof(bmi.bmiHeader);
}

// return image data length in bytes(each row align in 4 bytes)
inline DK_SIZE_T DkBmpDataSize(const DK_BITMAPINFOHEADER& bmiHeader)
{
    return DkBitAlign(bmiHeader.biBitCount * bmiHeader.biWidth, 4) * abs(bmiHeader.biHeight);
}

/////////////////////////////////////////////////////////////////////////////
// Shape definition, like box, point, rect
/////////////////////////////////////////////////////////////////////////////

struct DK_POS 
{
    DK_DOUBLE X;
    DK_DOUBLE Y;

    DK_POS()
        : X(0.0), Y(0.0)
    {}

    DK_POS(DK_DOUBLE x, DK_DOUBLE y)
        : X(x), Y(y)
    {}

    DK_BOOL operator==(const DK_POS& pos) const
    {
        return X == pos.X && Y == pos.Y;
    }

    DK_BOOL operator!=(const DK_POS& pos) const
    {
        return !(*this == pos);
    }

    DK_BOOL IsEmpty() const { return X == 0.0 && Y == 0.0; }

    // 偏移指定距离
    DK_VOID Offset(DK_DOUBLE dx, DK_DOUBLE dy) { X += dx; Y += dy; }

    // the square of length
    DK_DOUBLE LenSquare(const DK_POS& pos) const { return DK_SQUARE(X - pos.X) + DK_SQUARE(Y - pos.Y); }
};

struct DK_BOX 
{
    DK_DOUBLE X0;
    DK_DOUBLE Y0;    // 左上角坐标
    DK_DOUBLE X1;
    DK_DOUBLE Y1;    // 右下角坐标

    DK_BOX()
        :X0(0.0), Y0(0.0), X1(0.0), Y1(0.0)
    {}

    DK_BOX(DK_DOUBLE x0, DK_DOUBLE y0, DK_DOUBLE x1, DK_DOUBLE y1)
        : X0(x0), Y0(y0), X1(x1), Y1(y1)
    {}

    DK_BOOL operator==(const DK_BOX& box) const
    {
        if (this == &box)
        {
            return DK_TRUE;
        }
        return X0 == box.X0
            && Y0 == box.Y0
            && X1 == box.X1
            && Y1 == box.Y1;
    }

    DK_BOOL operator!=(const DK_BOX& box) const
    {
        return !(*this == box);
    }

    DK_DOUBLE Width() const
    {
        return fabs(X1 - X0);
    }

    DK_DOUBLE Height() const
    {
        return fabs(Y1 - Y0);
    }

    // 判断是否为空区域
    DK_BOOL IsEmpty() const
    {
        return (X0 == X1 && Y0 == Y1);
    }

    // 判断点是否位于BOX内
    DK_BOOL PosInBox(const DK_POS& pos) const
    {
        return (pos.X >= X0 && pos.X <= X1 && pos.Y >= Y0 && pos.Y <= Y1);
    }

    // 判断是否包含目标BOX
    DK_BOOL ContainsBox(const DK_BOX& box) const
    {
        return (X0 <= box.X0 && X1 >= box.X1 && Y0 <= box.Y0 && Y1 >= box.Y1);
    }

    // 判断两个BOX是否有交集
    DK_BOOL IsIntersect(const DK_BOX& box) const
    {
        return (X0 <= box.X1 && X1 >= box.X0 && Y0 <= box.Y1 && Y1 >= box.Y0);
    }

    // 偏移指定距离
    DK_VOID Offset(DK_DOUBLE dx, DK_DOUBLE dy) { X0 += dx;  X1 += dx; Y0 += dy; Y1 += dy; }
    static DK_BOX Intersect(const DK_BOX& lhs, const DK_BOX& rhs)
    {
        if (lhs.IsIntersect(rhs))
        {
            return DK_BOX(
                    dk_max(lhs.X0, rhs.X0), 
                    dk_max(lhs.Y0, rhs.Y0), 
                    dk_min(lhs.X1, rhs.X1), 
                    dk_min(lhs.Y1, rhs.Y1));
        }
        else
        {
            return DK_BOX();
        }
    }

    DK_VOID Normalize()
    {
        if (X0 > X1)
            std::swap(X0, X1);

        if (Y0 > Y1)
            std::swap(Y0, Y1);
    }

    DK_BOX MergeBox(const DK_BOX& hs)
    {
        return DK_BOX(
                dk_min(X0, hs.X0),
                dk_min(Y0, hs.Y0),
                dk_max(X1, hs.X1),
                dk_max(Y1, hs.Y1)
                );
    }

};

struct DK_POINT 
{
    DK_LONG X;
    DK_LONG Y;

    DK_POINT()
        : X(0), Y(0)
    {}

    DK_POINT(DK_LONG x, DK_LONG y)
        : X(x), Y(y)
    {}

    DK_BOOL operator==(const DK_POINT& pos) const
    {
        return (X == pos.X) && (Y == pos.Y);
    }

    DK_BOOL operator!=(const DK_POINT& pos) const
    {
        return !(*this == pos);
    }
};

/////////////////////////////////////////////////////////////////////////////
// Color definition
/////////////////////////////////////////////////////////////////////////////

struct DK_RGBCOLOR
{
    DK_INT    rgbRed;
    DK_INT    rgbGreen;
    DK_INT    rgbBlue;

    DK_RGBCOLOR()
        : rgbRed(0), rgbGreen(0), rgbBlue(0)
    {}

    DK_RGBCOLOR(DK_INT rgbRed, DK_INT rgbGreen, DK_INT rgbBlue)
        : rgbRed(rgbRed), rgbGreen(rgbGreen), rgbBlue(rgbBlue)
    {}

    DK_BOOL operator==(const DK_RGBCOLOR& color) const
    {
        return rgbRed == color.rgbRed && rgbGreen == color.rgbGreen && rgbBlue == color.rgbBlue;
    }

    DK_BOOL operator!=(const DK_RGBCOLOR& color) const
    {
        return !(*this == color);
    }
};

struct DK_ARGBCOLOR
{
    DK_INT rgbAlpha;
    DK_INT rgbRed;
    DK_INT rgbGreen;
    DK_INT rgbBlue;

    DK_ARGBCOLOR()
        : rgbAlpha(255), rgbRed(0), rgbGreen(0), rgbBlue(0)
    {}

    DK_ARGBCOLOR(DK_INT rgbAlpha, DK_INT rgbRed, DK_INT rgbGreen, DK_INT rgbBlue)
        : rgbAlpha(rgbAlpha), rgbRed(rgbRed), rgbGreen(rgbGreen), rgbBlue(rgbBlue)
    {}

    DK_BOOL operator==(const DK_ARGBCOLOR& color) const
    {
        return rgbAlpha == color.rgbAlpha && rgbRed == color.rgbRed && rgbGreen == color.rgbGreen && rgbBlue == color.rgbBlue;
    }

    DK_BOOL operator!=(const DK_ARGBCOLOR& color) const
    {
        return !(*this == color);
    }

    // NOTE(pengf): For comare in hashset only, no other means
    DK_BOOL FieldLessCompare(const DK_ARGBCOLOR& rhs) const
    {
        FIELD_LESS_OPERATOR_COMPARE(rgbRed);
        FIELD_LESS_OPERATOR_COMPARE(rgbGreen);
        FIELD_LESS_OPERATOR_COMPARE(rgbBlue);
        FIELD_LESS_OPERATOR_COMPARE(rgbAlpha);
        return DK_FALSE;
    }

    DK_BOOL FieldMoreCompare(const DK_ARGBCOLOR& rhs) const
    {
        FIELD_MORE_OPERATOR_COMPARE(rgbRed);
        FIELD_MORE_OPERATOR_COMPARE(rgbGreen);
        FIELD_MORE_OPERATOR_COMPARE(rgbBlue);
        FIELD_MORE_OPERATOR_COMPARE(rgbAlpha);
        return DK_FALSE;
    }
};

struct DK_CMYKCOLOR
{
    DK_DOUBLE C;
    DK_DOUBLE M;
    DK_DOUBLE Y;
    DK_DOUBLE K;

    DK_CMYKCOLOR()
        : C(0.0), M(0.0), Y(0.0), K(1.0)
    {}

    DK_CMYKCOLOR(DK_DOUBLE inC, DK_DOUBLE inM, DK_DOUBLE inY, DK_DOUBLE inK)
        : C(inC), M(inM), Y(inY), K(inK)
    {}

    DK_BOOL operator==(const DK_CMYKCOLOR& color) const
    {
        return C == color.C && M == color.M && Y == color.Y && K == color.K;
    }

    DK_BOOL operator!=(const DK_CMYKCOLOR& color) const
    {
        return !(*this == color);
    }
};

struct DK_ACMYKCOLOR
{
    DK_DOUBLE A;
    DK_DOUBLE C;
    DK_DOUBLE M;
    DK_DOUBLE Y;
    DK_DOUBLE K;

    DK_ACMYKCOLOR()
        : A(0.0), C(0.0), M(0.0), Y(0.0), K(1.0)
    {}

    DK_ACMYKCOLOR(DK_DOUBLE inA, DK_DOUBLE inC, DK_DOUBLE inM, DK_DOUBLE inY, DK_DOUBLE inK)
        : A(inA), C(inC), M(inM), Y(inY), K(inK)
    {}

    DK_BOOL operator==(const DK_ACMYKCOLOR& color) const
    {
        return A == color.A && C == color.C && M == color.M && Y == color.Y && K == color.K;
    }

    DK_BOOL operator!=(const DK_ACMYKCOLOR& color) const
    {
        return !(*this == color);
    }
};

/////////////////////////////////////////////////////////////////////////////
// Enum definition
/////////////////////////////////////////////////////////////////////////////

// 变换矩阵
// 点p是行向量(x,y,1)
// 变换是右乘一个矩阵p*CTM
struct DK_MATRIX
{
    DK_REAL m11;
    DK_REAL m12;
    DK_REAL m21; 
    DK_REAL m22;
    DK_REAL dx; 
    DK_REAL dy;

    DK_MATRIX()
        : m11(1.0), m12(0.0), m21(0.0), m22(1.0), dx(0.0), dy(0.0)
    {}

    DK_MATRIX(DK_REAL m11, DK_REAL m12, DK_REAL m21, DK_REAL m22, DK_REAL dx,    DK_REAL dy)
        : m11(m11), m12(m12), m21(m21), m22(m22), dx(dx), dy(dy)
    {}

    DK_MATRIX(DK_DOUBLE m11, DK_DOUBLE m12, DK_DOUBLE m21, DK_DOUBLE m22, DK_DOUBLE dx, DK_DOUBLE dy)
        : m11((DK_REAL)m11), m12((DK_REAL)m12), m21((DK_REAL)m21), m22((DK_REAL)m22), dx((DK_REAL)dx), dy((DK_REAL)dy)
    {}

    // pCTM至少要有6个元素，而且按 m11, m12, m21, m22, dx, dy 顺序排列。
    DK_MATRIX(const DK_DOUBLE* pCTM)
        : m11((DK_REAL)pCTM[0]), m12((DK_REAL)pCTM[1]), m21((DK_REAL)pCTM[2]), m22((DK_REAL)pCTM[3]), dx((DK_REAL)pCTM[4]), dy((DK_REAL)pCTM[5])
    {}

    // pCTM至少要有6个元素，而且按 m11, m12, m21, m22, dx, dy 顺序排列。
    DK_MATRIX(const DK_REAL* pCTM)
        : m11((DK_REAL)pCTM[0]), m12((DK_REAL)pCTM[1]), m21((DK_REAL)pCTM[2]), m22((DK_REAL)pCTM[3]), dx((DK_REAL)pCTM[4]), dy((DK_REAL)pCTM[5])
    {}

    DK_BOOL operator==(const DK_MATRIX& matrix) const
    {
        if (this == &matrix)
            return DK_TRUE;
        return DkFloatEqual(m11, matrix.m11) &&
            DkFloatEqual(m12, matrix.m12) &&
            DkFloatEqual(m21, matrix.m21) &&
            DkFloatEqual(m22, matrix.m22) &&
            DkFloatEqual(dx, matrix.dx) &&
            DkFloatEqual(dy, matrix.dy);
    }

    DK_BOOL operator!=(const DK_MATRIX& matrix) const
    {
        return !(*this == matrix);
    }

    DK_BOOL IsDefaultMatrix() const
    {
        return DkFloatEqual(m11, 1.0f) &&
            DkFloatEqualToZero(m12) &&
            DkFloatEqualToZero(m21) &&
            DkFloatEqual(m22, 1.0f) &&
            DkFloatEqualToZero(dx) &&
            DkFloatEqualToZero(dy);
    }

    DK_BOOL IsRotateOrSkewMatrix() const
    {
        // 不需要分解，快速判断是否有旋转或拉伸
        return (!DkFloatEqualToZero(m12) || !DkFloatEqualToZero(m21) || m11 < 0.0 || m22 < 0.0);
    }

    // 点的矩阵变换
    DK_VOID TransformPos(DK_POS* pt) const
    {
        DK_ASSERT(DK_NULL != pt);
        if (DK_NULL == pt)
        {
            return;
        }
        DK_POS result;
        DK_DOUBLE x = pt->X;
        DK_DOUBLE y = pt->Y;
        pt->X = m11 * x + m21 * y + dx;
        pt->Y = m12 * x + m22 * y + dy;
    }

    // 矩形经矩阵变换后，求外接矩形
    DK_BOX TransformBox2Bounding(const DK_BOX& box) const
    {
        DK_POS points[4] =
        {
            DK_POS(box.X0, box.Y0),
            DK_POS(box.X1, box.Y0),
            DK_POS(box.X1, box.Y1),
            DK_POS(box.X0, box.Y1)
        };

        for (DK_INT i = 0; i < 4; i++)
        {
            TransformPos(&points[i]);
        }

        DK_DOUBLE dXMin = points[0].X;
        DK_DOUBLE dXMax = dXMin;
        DK_DOUBLE dYMin = points[0].Y;
        DK_DOUBLE dYMax = dYMin;

        for (DK_INT i = 1; i < 4; i++)
        {
            DK_POS& pt = points[i];
            if (pt.X < dXMin)
                dXMin = pt.X;
            else if (pt.X > dXMax)
                dXMax = pt.X;
            if (pt.Y < dYMin)
                dYMin = pt.Y;
            else if (pt.Y > dYMax)
                dYMax = pt.Y;
        }

        return DK_BOX(dXMin, dYMin, dXMax, dYMax);
    }
    static DK_VOID CalcMatrix(DK_DOUBLE dXScale,
            DK_DOUBLE dRotate,
            DK_DOUBLE dSkew,
            DK_DOUBLE dYScale,
            DK_DOUBLE dXMove,
            DK_DOUBLE dYMove,
            DK_MATRIX* mMatrix);
    static DK_BOOL InvertMatrix(const DK_MATRIX& matrixSrc, DK_MATRIX* matrixDst);

    static DK_MATRIX ScaleMatrix(DK_DOUBLE scaleX, DK_DOUBLE scaleY)
    {
        return DK_MATRIX(scaleX, 0, 0, scaleY, 0, 0);
    }

    static DK_MATRIX SkewMatrix(DK_DOUBLE angle)
    {
        return DK_MATRIX(1, 0, tan(angle), 1, 0, 0);
    }

    static DK_MATRIX RotateMatrix(DK_DOUBLE angle)
    {
        DK_DOUBLE sina = sin(angle);
        DK_DOUBLE cosa = cos(angle);
        return DK_MATRIX(cosa, sina, -sina, cosa, 0, 0);
    }
};

DK_MATRIX operator*(const DK_MATRIX& lhs, const DK_MATRIX& rhs);

/////////////////////////////////////////////////////////////////////////////
// Misc definition
/////////////////////////////////////////////////////////////////////////////

// 图像灰度值处理回调函数
typedef struct DK_GRAYMSKFUNCS 
{
    DK_UINT (*pFunc)(DK_UINT, DK_VOID*);
    DK_VOID* pUserArg;

    DK_GRAYMSKFUNCS() : pFunc(DK_NULL), pUserArg(DK_NULL) {}

} *DK_LPGRAYMSKFUNCS;

// 8bpp的灰度位图，以左下角作为坐标起点
struct DK_BITMAPGRAY8
{
    DK_LCDMODE_TYPE lcdMode;           // LCD模式，为DK_LCDMODE_NONE表示是普通灰度图
    DK_LONG         lWidth;            // 位图宽度
    DK_LONG         lHeight;           // 位图高度
    DK_LONG         lStride;           // 每行位图数据的宽度
    DK_BYTE*        pbyData;           // 位图数据

    DK_VOID*        pReserved1;        // 保留
    DK_VOID*        pReserved2;        // 保留

    DK_BITMAPGRAY8()
        : lcdMode(DK_LCDMODE_NONE), lWidth(0), lHeight(0), lStride(0), pbyData(DK_NULL), pReserved1(DK_NULL), pReserved2(DK_NULL)
    {}
};

// DK_OUTPUTDEV_BITMAPBUFFER使用的输出设备对象类型
struct DK_BITMAPBUFFER_DEV
{
    DK_LONG           lWidth;          // 位图宽度
    DK_LONG           lHeight;         // 位图高度
    DK_PIXELFORMAT    nPixelFormat;    // 像素格式
    DK_LONG           lStride;         // 每行位图数据的宽度，字节单位
    DK_INT            nDPI;            // DPI
    DK_BYTE*          pbyData;         // 位图数据

    DK_BITMAPBUFFER_DEV()
        : lWidth(0), lHeight(0), nPixelFormat(DK_PIXELFORMAT_UNKNOWN), lStride(0), nDPI(RenderConst::SCREEN_DPI), pbyData(DK_NULL)
    {}
};

// 绘制Gamma参数，数值范围在0.1-10.0之间
typedef struct DK_RENDERGAMMA
{
    DK_DOUBLE        dTextGamma;            // 文本绘制Gamma参数

    DK_RENDERGAMMA()
        : dTextGamma(1.0)
    {}
} DK_RENDERGAMMA;

// 文字渲染平滑选项
enum DK_FONT_SMOOTH_TYPE
{
    DK_FONT_SMOOTH_NORMAL, // 平滑（默认）
    DK_FONT_SMOOTH_SHARP,  // 锐利（AUTO_HINT）
    DK_FONT_SMOOTH_BINARY  // 二值化（for kindle）
};

// 页面绘制参数
typedef struct DK_RENDERINFO
{
    DK_UINT             nPageNum;       // 页码（起始页为1）
    DK_OUTPUTDEV_TYPE   nDeviceType;    // 输出设备类型
    DK_VOID*            pDevice;        // 输出设备对象
    DK_BOX              rcPage;         // 待输出的页面矩形（页面坐标系），只有该区域内的页面内容会被输出
    DK_DOUBLE           dXScale;        // X方向页面缩放比（>0）
    DK_DOUBLE           dYScale;        // Y方向页面缩放比（>0）
    DK_DOUBLE           dRotate;        // 输出旋转角度（0~360）***注：沿输出设备坐标（0，0）点旋转（当前用户坐标系）
    DK_DWORD            dwSmoothTag;    // 平滑参数，见DK_SMOOTHTAG枚举
    DK_RENDERGAMMA      gamma;          // Gamma参数
    DK_FONT_SMOOTH_TYPE fontSmoothType; // 文字的平滑参数，默认为平滑

    DK_RENDERINFO()
        : nPageNum(0), nDeviceType(DK_OUTPUTDEV_UNKNOWN), pDevice(DK_NULL), dXScale(1.0), dYScale(1.0), dRotate(0.0), dwSmoothTag(DK_SMOOTH_NONE), fontSmoothType(DK_FONT_SMOOTH_NORMAL)
    {}
} DK_RENDERINFO;

/////////////////////////////////////////////////////////////////////////////
// Reflow infomation definition
/////////////////////////////////////////////////////////////////////////////

// 流式坐标位置，用于唯一确定流式文档（如ePub）中的一个位置。
struct DK_FLOWPOSITION
{
    DK_UINT nChapterIndex;     // 章节索引
    DK_UINT nParaIndex;        // 段索引
    DK_UINT nElemIndex;        // 元素索引

    DK_FLOWPOSITION()
        :nChapterIndex(0), nParaIndex(0), nElemIndex(0)
    {}

    DK_FLOWPOSITION(DK_UINT nChapter, DK_UINT nPara, DK_UINT nElem)
        : nChapterIndex(nChapter), nParaIndex(nPara), nElemIndex(nElem)
    {}

    DK_BOOL operator<(const DK_FLOWPOSITION& pos) const
    {
        return nChapterIndex < pos.nChapterIndex 
            || (nChapterIndex == pos.nChapterIndex && nParaIndex < pos.nParaIndex)
            || (nChapterIndex == pos.nChapterIndex && nParaIndex == pos.nParaIndex && nElemIndex < pos.nElemIndex);
    }

    DK_BOOL operator>(const DK_FLOWPOSITION& pos) const
    {
        return nChapterIndex > pos.nChapterIndex 
            || (nChapterIndex == pos.nChapterIndex && nParaIndex > pos.nParaIndex)
            || (nChapterIndex == pos.nChapterIndex && nParaIndex == pos.nParaIndex && nElemIndex > pos.nElemIndex);
    }

    DK_BOOL operator>=(const DK_FLOWPOSITION& pos) const 
    {
        return !(*this < pos); 
    }
    DK_BOOL operator<=(const DK_FLOWPOSITION& pos) const 
    { 
        return !(*this > pos); 
    }
    DK_BOOL operator==(const DK_FLOWPOSITION& pos) const 
    {
        return nChapterIndex == pos.nChapterIndex && nParaIndex == pos.nParaIndex && nElemIndex == pos.nElemIndex; 
    }
    DK_BOOL operator!=(const DK_FLOWPOSITION& pos) const 
    {
        return !(*this == pos); 
    }

    static DK_FLOWPOSITION DocBegin() { return DK_FLOWPOSITION(0, 0, 0); }
    static DK_FLOWPOSITION DocEnd() { return DK_FLOWPOSITION(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF); }
};

enum DK_RENDER_COLORMODE_TYPE
{
    DK_RENDER_COLORMODE_NORMAL,
    DK_RENDER_COLORMODE_NIGHT,
    DK_RENDER_COLORMODE_GIVENCOLOR
};

// 流式绘制选项
struct DK_FLOWRENDEROPTION
{
    DK_OUTPUTDEV_TYPE           nDeviceType;    // 输出设备类型
    DK_VOID*                    pDevice;        // 输出设备对象

    DK_DOUBLE                   dBaseYPos;      // 基准流式位置在设备空间上的输出Y坐标
    DK_DOUBLE                   dMinPartRow;    // 只有当可见行高大于等于此值时，才显示不完整的行

    DK_DOUBLE                   dScale;         // 缩放比（>0）
    DK_DWORD                    dwSmoothTag;    // 平滑参数，见DK_SMOOTHTAG枚举
    DK_RENDERGAMMA              gamma;          // Gamma参数
    DK_FONT_SMOOTH_TYPE         fontSmoothType; // 文字的平滑参数，默认为平滑
    DK_BOOL                     bMeasureOnly;   // 是否仅作测量，不做绘制

    DK_RENDER_COLORMODE_TYPE    colorMode;      // 黑夜模式下文字和路径颜色反色
    DK_ARGBCOLOR                colorNightMode; // 黑夜模式某些图片需要填纯色底
    DK_DOUBLE                   percent;        // 黑夜模式反色后alpha 0.0~1.0
    DK_DOUBLE                   textAlpha;      // 非黑夜模式下文字透明度 0.0~1.0
    DK_ARGBCOLOR                colorGiven;     // 指定颜色模式下文字颜色

    DK_FLOWRENDEROPTION()
        : nDeviceType(DK_OUTPUTDEV_UNKNOWN), pDevice(DK_NULL)
        , dBaseYPos(0.0), dMinPartRow(65536.0)
        , dScale(1.0), dwSmoothTag(DK_SMOOTH_NONE), fontSmoothType(DK_FONT_SMOOTH_NORMAL), bMeasureOnly(DK_FALSE)
        , colorMode(DK_RENDER_COLORMODE_NORMAL), percent(1.0), textAlpha(1.0)
    {}
};

// 流式绘制结果
struct DK_FLOWRENDERRESULT
{
    DK_UINT                nRowCount;       // 完全绘制的行数
    DK_FLOWPOSITION        posStart;        // 实际绘制的起始流式位置，即绘制结果第一行第一个位置
    DK_FLOWPOSITION        posEnd;          // 实际绘制的结束流式位置，即首个不能完全显示的行的第一个位置

    DK_BOOL                bPageBreak;      // 由于强制分页符而结束绘制
    DK_BOOL                bDocBegin;       // 到达文档开始，以文档开始作为绘制起始流式位置
    DK_BOOL                bDocEnd;         // 到达文档结尾而结束绘制

    DK_FLOWRENDERRESULT()
        : nRowCount(0), bPageBreak(DK_FALSE), bDocBegin(DK_FALSE), bDocEnd(DK_FALSE)
    {}
};

// PDF重排页面绘制参数
struct DK_RENDERINFOEX
{
    // 绘制位置
    // pos.nChapterIndex 表示页码（起始页为1）;
    // pos.nParaIndex 表示通过调用 IDKPPage 函数 GetPageTextContentStream 获得的 PDKPTEXTINFONODE 链表下标;
    // pos.nElemIndex 表示 PDKPTEXTINFONODE.DKPTEXTINFO.strContent 下标;
    DK_FLOWPOSITION      pos;            

    DK_OUTPUTDEV_TYPE    nDeviceType;    // 输出设备类型
    DK_VOID*             pDevice;        // 输出设备对象
    DK_BOX               rcPage;         // 待输出的页面矩形（页面坐标系），只有该区域内的页面内容会被输出
    DK_DOUBLE            dScale;         // 页面缩放比（>0）
    DK_DWORD             dwSmoothTag;    // 平滑参数，见DK_SMOOTHTAG枚举
    DK_RENDERGAMMA       gamma;          // Gamma参数
    DK_BOOL              bBinaryText;    // 是否要求二值化输出文字内容，二值化时平滑参数失效

    DK_RENDERINFOEX()
        : nDeviceType(DK_OUTPUTDEV_UNKNOWN), pDevice(DK_NULL), dScale(1.0), dwSmoothTag(DK_SMOOTH_NONE), bBinaryText(DK_FALSE)
    {}
}; 

enum DK_FONTDEFINE_TYPE
{
    DK_FONTDEFINE_UNKNOWN,
    DK_FONTDEFINE_FACENAME,
    DK_FONTDEFINE_FILENAME,
    DK_FONTDEFINE_HANDLE
};

enum DK_FONTSTYLE_TYPE
{
    DK_FONTSTYLE_NORMAL,
    DK_FONTSTYLE_ITALIC
};

enum DK_FONTWEIGHT_TYPE
{
    DK_FONTWEIGHT_NORMAL,
    DK_FONTWEIGHT_BOLD
};

struct DK_FONT
{
    DK_FONTDEFINE_TYPE fontDefineType;      // 字体定义方式
    union
    {
        const DK_WCHAR* pFontFaceName;      // 字体名字，在fontDefineType为DK_FONTDEFINE_FACENAME时有效
        const DK_WCHAR* pFontFilePath;      // 字体文件全路径，在fontDefineType为DK_FONTDEFINE_FILENAME时有效
        DK_VOID* pFontHandle;               // 字体句柄，在fontDefineType为DK_FONTDEFINE_HANDLE时有效
    };

    DK_DOUBLE fontSize;
    DK_FONTSTYLE_TYPE fontStyle;
    DK_FONTWEIGHT_TYPE fontWeight;

    DK_FONT()
        : fontDefineType(DK_FONTDEFINE_UNKNOWN)
        , fontSize(0.0)
        , fontStyle(DK_FONTSTYLE_NORMAL)
        , fontWeight(DK_FONTWEIGHT_NORMAL)
    {
        pFontFaceName = DK_NULL;
        pFontFilePath = DK_NULL;
        pFontHandle = DK_NULL;
    }
};

struct DK_FONTFAMILY
{
    DK_WCHAR* pszFaceName;
    DK_WCHAR* pszFontFile;
    DK_FONTSTYLE_TYPE fontStyle;
    DK_FONTWEIGHT_TYPE fontWeight;
};

enum DK_TEXTDECORATION_TYPE
{
    DK_TEXTDECORATION_NONE,
    DK_TEXTDECORATION_UNDERLINE,
    DK_TEXTDECORATION_OVERLINE,
    DK_TEXTDECORATION_THROUGHLINE
};

struct DK_RICHTEXT
{
    const DK_WCHAR* pText;
    DK_FONT font;
    DK_TEXTDECORATION_TYPE textDecoration;
    DK_ARGBCOLOR color;
    DK_ARGBCOLOR bgColor;
    DK_DOUBLE tabStop;

    DK_RICHTEXT()
        : pText(DK_NULL)
        , textDecoration(DK_TEXTDECORATION_NONE)
        , tabStop(0.0)
    {
        // do nothing;
    }

    DK_BOOL IsEmpty() const
    {
        return DK_NULL == pText || 0 == *pText;
    }
};

#endif
