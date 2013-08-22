//===========================================================================
// Summary:
//		DkRender.h
//      Render的一些公用定义
// Usage:
//	    Null
// Remarks:
//		Null
// Date:
//		2011-09-17
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_DKRENDER_H__
#define	__KERNEL_COMMONLIBS_RENDER_DKRENDER_H__

#include "KernelType.h"


#include "IDKOutputSystem.h"
#include "IDKOutputPen.h"
#include "IDKOutputBrush.h"
#include "IDKOutputPath.h"
#include "IDKOutputRegion.h"
#include "IDKFontManager.h"
#include "IDKFontEngine.h"
#include "IDKOutputImage.h"
#include "DkStream.h"

class DkRenderFactory
{
public:
    // caller is own to delete the return object
    static IDKOutputSystem* CreateOutputSystem(DK_OUTPUTSYSTEM_TYPE type = DK_OUTPUTSYSTEM_SKIA);

    // caller is own to delete the return object
    static IDKOutputPath* CreatePath(DK_OUTPUTSYSTEM_TYPE type = DK_OUTPUTSYSTEM_SKIA);

    // caller is own to delete the return object
    static IDKFontManager* CreateFontManager();

    // caller is own to delete the return object
    static IDKFontEngine* CreateFontEngine(IDKFontManager* fontManager);

    // caller is own to delete the return object and stream
    static IDKOutputImage* CreateOutputImageFromStream(IDkStream* stream, DK_DOUBLE scale = 1.0);

    static IDKOutputImage* CreateOutputImageFromBitmap(const DK_BITMAPINFO* pBmpInfo, const DK_BYTE* pBmpData);

    static IDKFontEngineLite* CreateFontEngineLite(IDKFontEngineLite* pBaseFontEngineLite);
private:
    DkRenderFactory();
};
#endif
