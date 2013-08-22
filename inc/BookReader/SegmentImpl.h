#ifndef _SEGMENTIMPL_H_
#define _SEGMENTIMPL_H_

#include "dkObjBase.h"
#include "GUI/dkImage.h"
#include "dkWinError.h"

typedef DK_RECT RECT;

HRESULT GetPageEdge(BYTE *pbData, int iWidth, int iHeight, int iStrideWidth, int iStepW, RECT *prcCutPos);
#endif // _SEGMENTIMPL_H_

