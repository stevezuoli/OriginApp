////////////////////////////////////////////////////////////////////////
// ImageFullRepaintCalculation.h
// Contact: 
// Copyright (c) Duokan Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////

#ifndef __UIFULLREPAINT_H__
#define __UIFULLREPAINT_H__

#include "singleton.h"
#include "KernelBaseType.h"
#include "KernelType.h"
#include "dkBaseType.h"
#include <vector>

class ImageFullRepaintCalculation
{
    SINGLETON_H(ImageFullRepaintCalculation)
public:
    ImageFullRepaintCalculation();
    ~ImageFullRepaintCalculation();
    void InitEnvironment(int width, int height, int horinzontalNum, int verticalNum, int threshold);
    bool GetFullRepaintFlag();
    void ResetFullRepaintFlag();
    void SetBox(const DK_BOX& box);
    void SetRect(const DK_RECT& rect);
private:
    std::vector<DK_POS> m_posLst;
    std::vector<bool> m_posFlag;
    bool m_finalFlag;
    int m_threshold;
};
#endif //__UIFULLREPAINT_H__
