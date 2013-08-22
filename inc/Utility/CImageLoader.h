////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/porting/inc/Utility/CImageLoader.h $ 
// $Revision: 7 $
// Contact: yuzhan
// Latest submission: $Date: 2008/08/25 20:50:27 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////

#ifndef CRESOURCEIMAGELOADER_H_
#define CRESOURCEIMAGELOADER_H_

#include "GUI/ITpImage.h"
#include "DkSPtr.h"
#include "Utility/CString.h"

//using namespace DkFormat;
using DkFormat::SPtr;

class CImageLoader
{

    ////////////////////Constructor Section/////////////////////////
private:
    CImageLoader();

public:
    ~CImageLoader();
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    static SPtr<ITpImage> GetStretechedImage(const CString& rResourceImageFileName, INT32 iWidth, INT32 iHeight, INT32 iFlag);
    static SPtr<ITpImage> GetImage(const char *pResourceImageFileName);
    static SPtr<ITpImage> GetImage(const UINT8* pBuf, INT32 iLen);

    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////
private:
    ////////////////////Field Section/////////////////////////

};

#endif /*CRESOURCEIMAGELOADER_H_*/
