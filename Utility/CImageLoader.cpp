////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/src/Utility/CImageLoader.cpp $
// $Revision: 12 $
// Contact: yuzhan
// Latest submission: $Date: 2008/08/25 20:50:27 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

//#include <eikenv.h>
#include "Utility/CImageLoader.h"
#include "GUI/CTpImage.h"
#include "interface.h"
SPtr<ITpImage> CImageLoader::GetStretechedImage(const CString& /*rstrImgSrc*/, INT32 /*iWidth*/, INT32 /*iHeight*/, INT32 /*iFlag*/)
{
    TRACE("not implement");
    TP_ASSERT(FALSE);
    return SPtr<ITpImage>();
}

SPtr<ITpImage> CImageLoader::GetImage(const char* rResourceImageFileName)
{
    DebugPrintf    (DLC_GUI, "Load source =====>>>image name:%s", rResourceImageFileName);
    return SPtr<ITpImage>(new CTpImage((char *)(rResourceImageFileName)));
}

SPtr<ITpImage> CImageLoader::GetImage(const UINT8* /*pBuf*/, INT32 /*iLen*/)
{
    TRACE("not implement");
    TP_ASSERT(FALSE);
    return SPtr<ITpImage>();
}
