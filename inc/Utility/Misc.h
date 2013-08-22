////////////////////////////////////////////////////////////////////////
// Misc.h
// Contact: chenm
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __MISC_H__
#define __MISC_H__

#include <string>
#include "curl/curl.h"

int Rand(int _max);
std::string GenerateUUID();
std::string UrlEncodeWithCurl(CURL *_pCurl,const std::string _strUrl,const std::string _strIgnoreArray);
void Asc2Bin(unsigned char *des, const unsigned char*src, int len);
inline int CalcDistanceSquare(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx * dx + dy * dy;
}

inline bool IsPointNeighbour(int x1, int y1, int x2, int y2, int distance)
{
    return CalcDistanceSquare(x1, y1, x2, y2) <= distance * distance;
}

inline int MakeInRange(int v, int minV, int maxV)
{
    if (v < minV)
    {
        v = minV;
    }
    if (v > maxV)
    {
        v = maxV;
    }
    return v;
}
template <class InputIterator, class OutputIterator, class Pred>
OutputIterator copy_if(InputIterator begin, InputIterator end, OutputIterator dst, Pred pred)
{
    while (begin != end)
    {
        if (pred(*begin))
        {
            *dst++=*begin;
        }
        ++begin;
    }
    return dst;
}
#endif
