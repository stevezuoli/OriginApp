//===========================================================================
// Summary:
//		DkRenderDef.h
//      Render的一些公用定义
// Usage:
//	    Null
// Remarks:
//		Null
// Date:
//		2011-10-09
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_DKRENDERDEF_H__
#define	__KERNEL_COMMONLIBS_RENDER_DKRENDERDEF_H__

#include <map>

// 绘制引擎类型
enum DK_OUTPUTSYSTEM_TYPE
{
	DK_OUTPUTSYSTEM_SKIA,
};

struct DK_SLRGB
{
	DK_DOUBLE r, g, b;
    static const DK_SLRGB White;
    static const DK_SLRGB Black;
};

struct DKR_MEMORY_FONT_INFO
{
    DK_BYTE*  fontData;
    DK_UINT fontDataSize;
    DK_WSTRING fontFace;
    std::map<DK_WCHAR, DK_INT> charToGlyphIndexMap;
};

enum RM_DPK_BLENDMODE {
    // 	kClear_Mode,    //!< [0, 0]
    // 	kSrc_Mode,      //!< [Sa, Sc]
    // 	kDst_Mode,      //!< [Da, Dc]
    RM_DPK_BLENDMODE_SRCOVER,  //!< [Sa + Da - Sa*Da, Rc = Sc + (1 - Sa)*Dc]
    // 	kDstOver_Mode,  //!< [Sa + Da - Sa*Da, Rc = Dc + (1 - Da)*Sc]
    // 	kSrcIn_Mode,    //!< [Sa * Da, Sc * Da]
    // 	kDstIn_Mode,    //!< [Sa * Da, Sa * Dc]
    // 	kSrcOut_Mode,   //!< [Sa * (1 - Da), Sc * (1 - Da)]
    // 	kDstOut_Mode,   //!< [Da * (1 - Sa), Dc * (1 - Sa)]
    // 	kSrcATop_Mode,  //!< [Da, Sc * Da + (1 - Sa) * Dc]
    // 	kDstATop_Mode,  //!< [Sa, Sa * Dc + Sc * (1 - Da)]
    // 	kXor_Mode,      //!< [Sa + Da - 2 * Sa * Da, Sc * (1 - Da) + (1 - Sa) * Dc]

    // these modes are defined in the SVG Compositing standard
    // http://www.w3.org/TR/2009/WD-SVGCompositing-20090430/
    RM_DPK_BLENDMODE_PLUS,
    RM_DPK_BLENDMODE_MULTIPLY,
    RM_DPK_BLENDMODE_SCREEN,
    RM_DPK_BLENDMODE_OVERLAY,
    RM_DPK_BLENDMODE_DARKEN,
    RM_DPK_BLENDMODE_LIGHTEN,
    RM_DPK_BLENDMODE_COLORDODGE,
    RM_DPK_BLENDMODE_COLORBURN,
    RM_DPK_BLENDMODE_HARDLIGHT,
    RM_DPK_BLENDMODE_SOFTLIGHT,
    RM_DPK_BLENDMODE_DIFFERENCE,
    RM_DPK_BLENDMODE_EXCLUSION
    //	kLastMode = kExclusion_Mode
};

#endif
