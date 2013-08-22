// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#include "dkBaseType.h"
#if !defined(AFX_STDAFX_H__983E12EB_300A_4DBA_BAEE_8150A9C48791__INCLUDED_)
#define AFX_STDAFX_H__983E12EB_300A_4DBA_BAEE_8150A9C48791__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct _XDisplay;        /* Forward declare before use for C++ */
typedef struct _XDisplay Display;

typedef char *XPointer;
typedef unsigned long Window;
typedef unsigned long CARD32;
typedef unsigned short CARD16;
typedef unsigned char  CARD8;
//typedef unsigned long uint;
//typedef signed char           INT8;
//typedef        signed short        INT16;
//typedef char *XPointer;

typedef unsigned long Time;
typedef unsigned long KeySym;
typedef unsigned long Drawable;
typedef void* XImage;
typedef unsigned long XIMStyle;
typedef unsigned long XEvent;
typedef unsigned long XSetWindowAttributes;
typedef unsigned long Atom;
typedef unsigned long XGCValues;
typedef unsigned long Colormap;

#define  XFontSet    int
#define  GC             int
#define Bool int

#define Status int
#define True 1
#define False 0

#define Status int
#define PY_PATH_MAX   512
#define PKGDATADIR

#define ShiftMask        (1<<0)
#define LockMask        (1<<1)
#define ControlMask        (1<<2)
#define Mod1Mask        (1<<3)
#define Mod2Mask        (1<<4)
#define Mod3Mask        (1<<5)
#define Mod4Mask        (1<<6)
#define Mod5Mask        (1<<7)


typedef struct {
    unsigned long pixel;
    unsigned short red, green, blue;
    char flags;  /* do_red, do_green, do_blue */
    char pad;
} XColor;
typedef struct
{
    CARD32    keysym;
    CARD32    modifier;
    CARD32    modifier_mask;
} XIMTriggerKey;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__983E12EB_300A_4DBA_BAEE_8150A9C48791__INCLUDED_)
