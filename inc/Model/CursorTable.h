////////////////////////////////////////////////////////////////////////
// CBookItemModelImpl.h
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __INC_MODEL_CURSORTABLE__
#define __INC_MODEL_CURSORTABLE__

#include "dkBaseType.h"
#include <vector>
using std::vector;

struct ATOM;
struct DK_Selection
{
    ATOM begin;
    ATOM end;
}

class CursorTable
{
public:
    ATOM GetBeginPoint();
    ATOM GetEndPoint();

    vector<DK_RECT> GetSelectionRECTs(DK_Selection _selection);

private:

};

#endif  // __INC_MODEL_CURSORTABLE__



