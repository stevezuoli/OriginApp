//===========================================================================
// Summary:
//		IDKOutputBrush.h
//      Brush的抽象
// Usage:
//	    Null
// Remarks:
//		Null
// Date:
//		2011-09-11
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTBRUSH_H__
#define	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTBRUSH_H__

#include "KernelType.h"

class IDKOutputBrush
{
public:
	virtual ~IDKOutputBrush() {}

public:
	virtual DK_BRUSH_TYPE GetType() const = 0;
	virtual DK_RGBQUAD GetBrushColor() const = 0;
	virtual DK_VOID SetBrushColor(DK_RGBQUAD clr) = 0;
};

#endif // #ifndef	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTBRUSH_H__
