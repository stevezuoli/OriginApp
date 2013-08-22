//===========================================================================
// Summary:
//		IDKOutputPath.h
//      Path的抽象
// Usage:
//	    Null
// Remarks:
//		Null
// Date:
//		2011-09-16
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTPATH_H__
#define	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTPATH_H__

#include "KernelType.h"
#include <vector>

class IDKOutputPen;

class IDKOutputPath
{
public:
	virtual ~IDKOutputPath() {}

public:
	// 获取路径当前的终点
	virtual DK_POS GetLastPoint() const = 0;

	// 移到某个点
	virtual DK_VOID MoveTo(DK_DOUBLE x, DK_DOUBLE y) = 0;

	// 提供一个点参数，该点与上一个点连接， 构成线
	virtual DK_VOID Line(DK_DOUBLE x, DK_DOUBLE y) = 0;

	// 提供一个控制点和一个终点，以上一个点为起点，构成二次Bezier曲线
	virtual DK_VOID QuadraticBezier(const DK_POS& ControlPoint, const DK_POS& EndPoint) = 0;

	// 提供三个点，前两个是控制点，最后一个是终点，以上一个点为起点，构成曲线
	virtual DK_VOID Bezier(const DK_POS& ControlPoint1, const DK_POS& ControlPoint2, const DK_POS& EndPoint) = 0;

	// 提供弧线所处矩形，以及起始角度和结束角度（角度单位，极坐标），构成弧线
	virtual DK_VOID Arch(const DK_BOX& rect, DK_DOUBLE dStartAngle, DK_DOUBLE dSweepAngle, DK_MATRIX* pTransform = DK_NULL) = 0;

	// 初始化路径，用于创建新路径
	virtual DK_VOID Reset() = 0;

	// 开始新的子路径，但不封闭当前子路径
	virtual DK_VOID StartSubPath() = 0;
	// 封闭并结束当前子路径
	virtual DK_VOID CloseSubPath() = 0;

	// 注：以下四个接口与GDI+的实现保持一致
	// 增加一条线（会与前一条线或贝塞尔曲线自动连接，如果起始点与前一次终点不一致则自动连线， 如不需要自动连线必须使用Move）
	virtual DK_VOID AddLine(DK_DOUBLE x1, DK_DOUBLE y1, DK_DOUBLE x2, DK_DOUBLE y2) = 0;
	// 增加贝塞尔曲线（会与前一条线或贝塞尔曲线自动连接，如果起始点与前一次终点不一致则自动连线， 如不需要自动连线必须使用Move）
	virtual DK_VOID AddBezier(DK_DOUBLE x1, DK_DOUBLE y1, DK_DOUBLE x2, DK_DOUBLE y2, DK_DOUBLE x3, DK_DOUBLE y3,
		DK_DOUBLE x4, DK_DOUBLE y4) = 0;
	// 增加一个椭圆（单独的封闭单元，不与前面或后面的图形部分连线）
	virtual DK_VOID AddEllipse(DK_DOUBLE dLeft, DK_DOUBLE dTop, DK_DOUBLE dWidth, DK_DOUBLE dHeight) = 0;
	// 增加一个矩形（单独的封闭子单元，不与前面或后面的图形部分连线）
	virtual DK_VOID AddRectangle(DK_DOUBLE x, DK_DOUBLE y, DK_DOUBLE dWidth, DK_DOUBLE dHeight) = 0;

	// 变换路径
	virtual DK_VOID Rotate(DK_FLOAT fAngle) = 0;

	// 添加一个路径
	// bConnect, DK_TRUE: pPath的第一个子路径只要可以，则追加到本路径的最后一个子路径; 
	//           DK_FALSE: pPath的第一个路径作为对立的子路径添加到改路径里。
	virtual DK_VOID AddPath(const IDKOutputPath* path, DK_BOOL bConnect) = 0;

	// 设置填充模式
	virtual DK_VOID SetFillMode(DK_FILLMODE_TYPE fillmode) = 0;
	// 得到路径外围矩形
	virtual DK_BOOL GetBounds(DK_BOX* pRect, DK_DOUBLE dLineWidth) = 0;

	// 克隆路径
	virtual IDKOutputPath* Clone() = 0;

	// 反转Path方向
	virtual DK_VOID Reverse() = 0;

	// 用指定的画笔绘制路径所得的结果替代该路径
	virtual DK_VOID Widen(IDKOutputPen* pPen) = 0;

	//-------------------------------------------
	// Summary:
	//		路径细分为线段
	// Parameters:
	//		[in] dMinDist		- 细分后的最大线段长度
	//		[out] vecPoint		- 细分后的线段顶点列表，每条子路径的顶点数由vecSubPathIndex决定
	//		[out] vecSubPathIndex	- 每条子路径的起点索引
	// Return:
	//		返回值表示是否成功
	//-------------------------------------------
	virtual DK_BOOL Subdivide(DK_DOUBLE dMaxDist, std::vector<DK_POS>& vecPoint, std::vector<DK_UINT>& vecSubPathIndex) = 0;

	// 把所有路径数据做坐标变换
	virtual DK_VOID Transform(const DK_MATRIX& matrix) = 0;

	//	如果是矩形，当前操作将矩形四个边对齐到整数
	virtual DK_BOOL AlignToIntForRectPath() = 0;

	// 指定的点是否落在路径填充的范围内
	virtual DK_BOOL IsVisible(DK_POS pt) = 0;

	// 指定的点是否落在用指定的画笔绘制路径所得的结果范围内
	virtual DK_BOOL IsOutlineVisible(DK_POS pt, IDKOutputPen* pPen) = 0;
};
#endif
