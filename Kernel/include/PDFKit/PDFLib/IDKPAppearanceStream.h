#ifndef __KERNEL_PDFKIT_PDFLIB_IDKPAPPEARANCESTREAM_H__
#define __KERNEL_PDFKIT_PDFLIB_IDKPAPPEARANCESTREAM_H__

#include "DKPTypeDef.h"

class IDKPAppearanceStream
{
public:
    //-------------------------------------------
    // Summary:
    //    设置线宽
    // Parameters:
    //    [in] dLineWidth - 线宽
    // Remarks:
    //    默认值1.0。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetLineWidth(DK_DOUBLE dLineWidth) = 0;

    //-------------------------------------------
    // Summary:
    //    设置线帽风格
    // Parameters:
    //    [in] lineCapType - 线帽风格
    // Remarks:
    //    默认值DPK_LINE_CAP_BUTT。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetLineCap(DKP_LINE_CAP_TYPE lineCapType) = 0;

    //-------------------------------------------
    // Summary:
    //    设置线联接风格
    // Parameters:
    //    [in] lineJoinType - 线联接风格
    // Remarks:
    //    默认值DPK_LINE_JOIN_MITER。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetLineJoin(DKP_LINE_JOIN_TYPE lineJoinType) = 0;

    //-------------------------------------------
    // Summary:
    //    设置尖角限量
    // Parameters:
    //    [in] dMiterLimit - 尖角限量
    // Remarks:
    //    默认值10.0。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetMiterLimit(DK_DOUBLE dMiterLimit) = 0;

    //-------------------------------------------
    // Summary:
    //    设置虚线风格
    // Parameters:
    //    [in] dDashPhase - 虚线相位
    //    [in] pDashArray - 虚线数组指针
    //    [in] nCount - 虚线数组元素个数
    // Remarks:
    //    默认值实线"[] 0"。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetLineDash(DK_DOUBLE dDashPhase, const DK_DOUBLE* pDashArray, DK_UINT nCount) = 0;

    //-------------------------------------------
    // Summary:
    //    设置平整度误差
    // Parameters:
    //    [in] nFlatTole - 平整度误差，取值范围[0, 100]。
    // Remarks:
    //    默认值0。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetFlatnessTolerance(DK_UINT nFlatTole) = 0;

    //-------------------------------------------
    // Summary:
    //    设置不透明度
    // Parameters:
    //    [in] dOpacity - 不透明度，0表示完全透明，1表示完全不透明。
    // Remarks:
    //    默认值1.0。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetOpacity(DK_DOUBLE dOpacity) = 0;

    //-------------------------------------------
    // Summary:
    //    设置混合模式
    // Parameters:
    //    [in] blendModeType - 混合模式
    // Remarks:
    //    默认值DPK_BLEND_MODE_NORMAL。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetBlendMode(DKP_BLEND_MODE_TYPE blendModeType) = 0;

    //-------------------------------------------
    // Summary:
    //    保存当前图形状态至图形状态栈
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SaveState() = 0;

    //-------------------------------------------
    // Summary:
    //    恢复最近保存的图形状态
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID RestoreState() = 0;

    //-------------------------------------------
    // Summary:
    //    将当前矩阵与指定矩阵做连接操作
    // Parameters:
    //    [in] dMatrixArray - 指定矩阵数组
    // Remarks:
    //    默认值为CTM，将默认用户坐标系转换为设备坐标系的矩阵。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID ConcatenateMatrix(DK_DOUBLE dMatrixArray[6]) = 0;

    //-------------------------------------------
    // Summary:
    //    移动坐标点并新起一个子路径
    // Parameters:
    //    [in] x - 横坐标
    //    [in] y - 纵坐标
    // Remarks:
    //    使用用户空间坐标系
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID MoveTo(DK_DOUBLE x, DK_DOUBLE y) = 0;

    //-------------------------------------------
    // Summary:
    //    从当前坐标点连直线至指定坐标点
    // Parameters:
    //    [in] x - 横坐标
    //    [in] y - 纵坐标
    // Remarks:
    //    使用用户空间坐标系
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID AddLineTo(DK_DOUBLE x, DK_DOUBLE y) = 0;

    //-------------------------------------------
    // Summary:
    //    添加一个由连续直线构成的子路径
    // Parameters:
    //    [in] dPoints - 坐标点数组
    //    [in] nCount - 坐标点个数
    // Remarks:
    //    使用用户空间坐标系
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID AddLines(const DKP_DPOINT* dPoints, DK_UINT nCount) = 0;

    //-------------------------------------------
    // Summary:
    //    从当前坐标点连贝瑟尔三次曲线至指定坐标点
    // Parameters:
    //    [in] cp1x - 控制点1横坐标
    //    [in] cp1y - 控制点1纵坐标
    //    [in] cp2x - 控制点1横坐标
    //    [in] cp2y - 控制点1纵坐标
    //    [in] x - 终点横坐标
    //    [in] y - 终点纵坐标
    // Remarks:
    //    使用用户空间坐标系
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID AddCurveTo(DK_DOUBLE cp1x, DK_DOUBLE cp1y, DK_DOUBLE cp2x, DK_DOUBLE cp2y, DK_DOUBLE x, DK_DOUBLE y) = 0;
    
    //-------------------------------------------
    // Summary:
    //    从当前点连直线至子路径起始点并关闭子路径
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID ClosePath() = 0;

    //-------------------------------------------
    // Summary:
    //    添加一个矩形子路径
    // Parameters:
    //    [in] dRect - 坐标点数组
    //    [in] nCount - 坐标点个数
    // Remarks:
    //    使用用户空间坐标系
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID AddRect(const DKP_DRECT& dRect) = 0;

    //-------------------------------------------
    // Summary:
    //    描画路径
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID StrokePath() = 0;

    //-------------------------------------------
    // Summary:
    //    填充路径
    // Parameters:
    //    [in] bNonZero - 填充方式，TRUE表示非零填充，FALSE表示奇偶填充。
    // Remarks:
    //    填充前所有开放的子路径都将被关闭
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID FillPath(DK_BOOL bNonZero = DK_TRUE) = 0;

    //-------------------------------------------
    // Summary:
    //    填充并描画路径
    // Parameters:
    //    [in] bNonZero - 填充方式，TRUE表示非零填充，FALSE表示奇偶填充。
    // Remarks:
    //    相当于构造两个相同的路径，并依次进行填充和描画
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID FillAndStrokePath(DK_BOOL bNonZero = DK_TRUE) = 0;

    //-------------------------------------------
    // Summary:
    //    结束路径，不进行填充或描画
    //-------------------------------------------
    virtual DK_VOID EndPath() = 0;

    //-------------------------------------------
    // Summary:
    //    修改当前裁剪路径
    // Parameters:
    //    [in] bNonZero - 填充方式，TRUE表示非零填充，FALSE表示奇偶填充。
    // Remarks:
    //    将当前路径与当前裁剪路径相交作为新的裁剪路径
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID ModifyClipPath(DK_BOOL bNonZero = DK_TRUE) = 0;
        
    //-------------------------------------------
    // Summary:
    //    设置描画颜色
    // Parameters:
    //    [in] pColorArray - 颜色数组指针
    //    [in] nCount - 坐标点个数，1为DeviceGray，3为DeviceRGB，4为DeviceCMYK。
    // Remarks:
    //    默认值：黑色
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetStrokeColor(const DK_DOUBLE* pColorArray, DK_UINT nCount) = 0;

    //-------------------------------------------
    // Summary:
    //    设置填充颜色
    // Parameters:
    //    [in] pColorArray - 颜色数组指针
    //    [in] nCount - 坐标点个数，1为DeviceGray，3为DeviceRGB，4为DeviceCMYK。
    // Remarks:
    //    默认值：黑色
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetFillColor(const DK_DOUBLE* pColorArray, DK_UINT nCount) = 0;

    //-------------------------------------------
    // Summary:
    //    新起一个文本对象。
    // Remarks:
    //    初始化文本矩阵和文本行矩阵为恒等矩阵。文本对象不能嵌套。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID BeginText() = 0;

    //-------------------------------------------
    // Summary:
    //    结束一个文本对象。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID EndText() = 0;

    //-------------------------------------------
    // Summary:
    //    设置文本对象的当前字体、字号
    // Parameters:
    //    [in] bChinese - 字体类型，TRUE表示中文字体，FALSE表示英文字体
    //    [in] nFontSize - 字体字号，单位：磅
    // Remarks:
    //    中文字体采用宋体，英文字体采用Helvetica。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetFont(DK_BOOL bChinese, DK_UINT nFontSize) = 0;

    //-------------------------------------------
    // Summary:
    //    设置下一段文本将要显示的位置
    // Parameters:
    //    [in] x - 横坐标
    //    [in] y - 纵坐标
    // Remarks:
    //    使用用户空间坐标系
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID SetTextPosition(DK_DOUBLE x, DK_DOUBLE y) = 0;

    //-------------------------------------------
    // Summary:
    //    显示一段文本内容
    // Parameters:
    //    [in] szText - 指向文本内容的指针
    // Remarks:
    //    unicode字符串，以'\0'结尾
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID ShowText(const DK_WCHAR* szText) = 0;

    //-------------------------------------------
    // Summary:
    //    在指定位置显示一段文本内容
    // Parameters:
    //    [in] szText - 指向文本内容的指针
    //    [in] x - 横坐标
    //    [in] y - 纵坐标
    // Remarks:
    //    unicode字符串，以'\0'结尾。使用用户空间坐标系。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID ShowTextAtPosition(const DK_WCHAR* szText, DK_DOUBLE x, DK_DOUBLE y) = 0;

    //-------------------------------------------
    // Summary:
    //    清除此外观流中的所有的设置、绘制操作
    // Remarks:
    //    用于清除新创建的外观流内容以及相关资源（字体，透明度等）
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_VOID Clear() = 0;

public:
    virtual ~IDKPAppearanceStream() {}
};

#endif // #ifndef __KERNEL_PDFKIT_PDFLIB_IDKPAPPEARANCESTREAM_H__
