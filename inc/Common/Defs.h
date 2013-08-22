#ifndef __COMMON_DEFS_H__
#define __COMMON_DEFS_H__

#include <stdint.h>
#include "KernelType.h"

#define CONNECT(Sender,Signal,Receiver,Slot)\
    EventListener::SubscribeEvent(\
Signal,\
Sender,\
std::tr1::bind(\
std::tr1::mem_fn(&Slot),\
Receiver,\
std::tr1::placeholders::_1));

typedef int64_t dkUpTime;;
typedef int dkColor;
/*  ---------------------------------------------------------------------------- */
/*  Possible SetSize flags */
/*  ---------------------------------------------------------------------------- */

/*  Use internally-calculated width if -1 */
#define dkSIZE_AUTO_WIDTH       0x0001
/*  Use internally-calculated height if -1 */
#define dkSIZE_AUTO_HEIGHT      0x0002
/*  Use internally-calculated width and height if each is -1 */
#define dkSIZE_AUTO             (dkSIZE_AUTO_WIDTH|dkSIZE_AUTO_HEIGHT)
/*  Ignore missing (-1) dimensions (use existing). */
/*  For readability only: test for dkSIZE_AUTO_WIDTH/HEIGHT in code. */
#define dkSIZE_USE_EXISTING     0x0000
/*  Allow -1 as a valid position */
#define dkSIZE_ALLOW_MINUS_ONE  0x0004
/*  Don't do parent client adjustments (for implementation only) */
#define dkSIZE_NO_ADJUSTMENTS   0x0008
/*  Change the window position even if it seems to be already correct */
#define dkSIZE_FORCE            0x0010
/*  Emit size event even if size didn't change */
#define dkSIZE_FORCE_EVENT      0x0020

enum {  dkDefaultCoord = -1 };

enum dkGeometryCentre
{
    dkCENTRE                  = 0x0001,
    dkCENTER                  = dkCENTRE
};
enum dkOrientation
{
    /* don't change the values of these elements, they are used elsewhere */
    dkHORIZONTAL              = 0x0004,
    dkVERTICAL                = 0x0008,

    dkBOTH                    = dkVERTICAL | dkHORIZONTAL
};

enum dkDirection
{
    dkLEFT                    = 0x0010,
    dkRIGHT                   = 0x0020,
    dkUP                      = 0x0040,
    dkDOWN                    = 0x0080,

    dkTOP                     = dkUP,
    dkBOTTOM                  = dkDOWN,

    dkNORTH                   = dkUP,
    dkSOUTH                   = dkDOWN,
    dkWEST                    = dkLEFT,
    dkEAST                    = dkRIGHT,

    dkALL                     = (dkUP | dkDOWN | dkRIGHT | dkLEFT)
};


enum dkAlignment
{
    dkALIGN_NOT               = 0x0000,
    dkALIGN_CENTER_HORIZONTAL = 0x0100,
    dkALIGN_CENTRE_HORIZONTAL = dkALIGN_CENTER_HORIZONTAL,
    dkALIGN_LEFT              = dkALIGN_NOT,
    dkALIGN_TOP               = dkALIGN_NOT,
    dkALIGN_RIGHT             = 0x0200,
    dkALIGN_BOTTOM            = 0x0400,
    dkALIGN_CENTER_VERTICAL   = 0x0800,
    dkALIGN_CENTRE_VERTICAL   = dkALIGN_CENTER_VERTICAL,

    dkALIGN_CENTER            = (dkALIGN_CENTER_HORIZONTAL | dkALIGN_CENTER_VERTICAL),
    dkALIGN_CENTRE            = dkALIGN_CENTER,

    /*  a mask to extract alignment from the combination of flags */
    dkALIGN_MASK              = 0x0f00
};

enum dkStretch
{
    dkSTRETCH_NOT             = 0x0000,
    dkSHRINK                  = 0x1000,
    dkGROW                    = 0x2000,
    dkEXPAND                  = dkGROW,
    dkSHAPED                  = 0x4000,
    dkFIXED_MINSIZE           = 0x8000,
    dkRESERVE_SPACE_EVEN_IF_HIDDEN = 0x0002,
    dkTILE                    = 0xc000,

    /* for compatibility only, default now, don't use explicitly any more */
    dkADJUST_MINSIZE          = 0
};

/*  border flags: the values are chosen for backwards compatibility */
enum dkBorder
{
    /*  this is different from dkBORDER_NONE as by default the controls do have */
    /*  border */
    dkBORDER_DEFAULT = 0,

    dkBORDER_NONE   = 0x00200000,
    dkBORDER_STATIC = 0x01000000,
    dkBORDER_SIMPLE = 0x02000000,
    dkBORDER_RAISED = 0x04000000,
    dkBORDER_SUNKEN = 0x08000000,
    dkBORDER_DOUBLE = 0x10000000, /* deprecated */
    dkBORDER_THEME =  0x10000000,

    /*  a mask to extract border style from the combination of flags */
    dkBORDER_MASK   = 0x1f200000
};

class dkSize
{
public:
    // members are public for compatibility, don't use them directly.
    int x, y;

    // constructors
    dkSize() : x(0), y(0) { }
    dkSize(int xx, int yy) : x(xx), y(yy) { }

    // no copy ctor or assignment operator - the defaults are ok

    bool operator==(const dkSize& sz) const { return x == sz.x && y == sz.y; }
    bool operator!=(const dkSize& sz) const { return x != sz.x || y != sz.y; }

    dkSize operator+(const dkSize& sz) const { return dkSize(x + sz.x, y + sz.y); }
    dkSize operator-(const dkSize& sz) const { return dkSize(x - sz.x, y - sz.y); }
    dkSize operator/(int i) const { return dkSize(x / i, y / i); }
    dkSize operator*(int i) const { return dkSize(x * i, y * i); }

    dkSize& operator+=(const dkSize& sz) { x += sz.x; y += sz.y; return *this; }
    dkSize& operator-=(const dkSize& sz) { x -= sz.x; y -= sz.y; return *this; }
    dkSize& operator/=(const int i) { x /= i; y /= i; return *this; }
    dkSize& operator*=(const int i) { x *= i; y *= i; return *this; }

    void IncTo(const dkSize& sz)
        { if ( sz.x > x ) x = sz.x; if ( sz.y > y ) y = sz.y; }
    void DecTo(const dkSize& sz)
        { if ( sz.x < x ) x = sz.x; if ( sz.y < y ) y = sz.y; }

    void IncBy(int dx, int dy) { x += dx; y += dy; }
    void IncBy(const dkSize& sz) { IncBy(sz.x, sz.y); }
    void IncBy(int d) { IncBy(d, d); }

    void DecBy(int dx, int dy) { IncBy(-dx, -dy); }
    void DecBy(const dkSize& sz) { DecBy(sz.x, sz.y); }
    void DecBy(int d) { DecBy(d, d); }


    dkSize& Scale(float xscale, float yscale)
        { x = (int)(x*xscale); y = (int)(y*yscale); return *this; }

    // accessors
    void Set(int xx, int yy) { x = xx; y = yy; }
    void SetWidth(int w) { x = w; }
    void SetHeight(int h) { y = h; }

    int GetWidth() const { return x; }
    int GetHeight() const { return y; }

    bool IsFullySpecified() const { return x != dkDefaultCoord && y != dkDefaultCoord; }

    // combine this size with the other one replacing the default (i.e. equal
    // to dkDefaultCoord) components of this object with those of the other
    void SetDefaults(const dkSize& size)
    {
        if ( x == dkDefaultCoord )
            x = size.x;
        if ( y == dkDefaultCoord )
            y = size.y;
    }

    // compatibility
    int GetX() const { return x; }
    int GetY() const { return y; }
};
const dkSize dkDefaultSize;

class dkPoint
{
public:
    int x, y;

    dkPoint() : x(0), y(0) { }
    dkPoint(int xx, int yy) : x(xx), y(yy) { }

    // no copy ctor or assignment operator - the defaults are ok

    // comparison
    bool operator==(const dkPoint& p) const { return x == p.x && y == p.y; }
    bool operator!=(const dkPoint& p) const { return !(*this == p); }

    // arithmetic operations (component wise)
    dkPoint operator+(const dkPoint& p) const { return dkPoint(x + p.x, y + p.y); }
    dkPoint operator-(const dkPoint& p) const { return dkPoint(x - p.x, y - p.y); }

    dkPoint& operator+=(const dkPoint& p) { x += p.x; y += p.y; return *this; }
    dkPoint& operator-=(const dkPoint& p) { x -= p.x; y -= p.y; return *this; }

    dkPoint& operator+=(const dkSize& s) { x += s.GetWidth(); y += s.GetHeight(); return *this; }
    dkPoint& operator-=(const dkSize& s) { x -= s.GetWidth(); y -= s.GetHeight(); return *this; }

    dkPoint operator+(const dkSize& s) const { return dkPoint(x + s.GetWidth(), y + s.GetHeight()); }
    dkPoint operator-(const dkSize& s) const { return dkPoint(x - s.GetWidth(), y - s.GetHeight()); }

    dkPoint operator-() const { return dkPoint(-x, -y); }
};

// ---------------------------------------------------------------------------
// dkRect
// ---------------------------------------------------------------------------

class  dkRect
{
public:
    dkRect()
        : x(0), y(0), width(0), height(0)
        { }
    dkRect(int xx, int yy, int ww, int hh)
        : x(xx), y(yy), width(ww), height(hh)
        { }
    dkRect(const dkPoint& topLeft, const dkPoint& bottomRight);
    dkRect(const dkPoint& pt, const dkSize& size)
        : x(pt.x), y(pt.y), width(size.x), height(size.y)
        { }
    dkRect(const dkSize& size)
        : x(0), y(0), width(size.x), height(size.y)
        { }

    // default copy ctor and assignment operators ok

    int GetX() const { return x; }
    void SetX(int xx) { x = xx; }

    int GetY() const { return y; }
    void SetY(int yy) { y = yy; }

    int GetWidth() const { return width; }
    void SetWidth(int w) { width = w; }

    int GetHeight() const { return height; }
    void SetHeight(int h) { height = h; }

    dkPoint GetPosition() const { return dkPoint(x, y); }
    void SetPosition( const dkPoint &p ) { x = p.x; y = p.y; }

    dkSize GetSize() const { return dkSize(width, height); }
    void SetSize( const dkSize &s ) { width = s.GetWidth(); height = s.GetHeight(); }

    bool IsEmpty() const { return (width <= 0) || (height <= 0); }

    int GetLeft()   const { return x; }
    int GetTop()    const { return y; }
    int GetBottom() const { return y + height - 1; }
    int GetRight()  const { return x + width - 1; }

    void SetLeft(int left) { x = left; }
    void SetRight(int right) { width = right - x + 1; }
    void SetTop(int top) { y = top; }
    void SetBottom(int bottom) { height = bottom - y + 1; }

    dkPoint GetTopLeft() const { return GetPosition(); }
    dkPoint GetLeftTop() const { return GetTopLeft(); }
    void SetTopLeft(const dkPoint &p) { SetPosition(p); }
    void SetLeftTop(const dkPoint &p) { SetTopLeft(p); }

    dkPoint GetBottomRight() const { return dkPoint(GetRight(), GetBottom()); }
    dkPoint GetRightBottom() const { return GetBottomRight(); }
    void SetBottomRight(const dkPoint &p) { SetRight(p.x); SetBottom(p.y); }
    void SetRightBottom(const dkPoint &p) { SetBottomRight(p); }

    dkPoint GetTopRight() const { return dkPoint(GetRight(), GetTop()); }
    dkPoint GetRightTop() const { return GetTopRight(); }
    void SetTopRight(const dkPoint &p) { SetRight(p.x); SetTop(p.y); }
    void SetRightTop(const dkPoint &p) { SetTopLeft(p); }

    dkPoint GetBottomLeft() const { return dkPoint(GetLeft(), GetBottom()); }
    dkPoint GetLeftBottom() const { return GetBottomLeft(); }
    void SetBottomLeft(const dkPoint &p) { SetLeft(p.x); SetBottom(p.y); }
    void SetLeftBottom(const dkPoint &p) { SetBottomLeft(p); }

    // operations with rect
    dkRect& Inflate(int dx, int dy);
    dkRect& Inflate(const dkSize& d) { return Inflate(d.x, d.y); }
    dkRect& Inflate(int d) { return Inflate(d, d); }
    dkRect Inflate(int dx, int dy) const
    {
        dkRect r = *this;
        r.Inflate(dx, dy);
        return r;
    }

    dkRect& Deflate(int dx, int dy) { return Inflate(-dx, -dy); }
    dkRect& Deflate(const dkSize& d) { return Inflate(-d.x, -d.y); }
    dkRect& Deflate(int d) { return Inflate(-d); }
    dkRect Deflate(int dx, int dy) const
    {
        dkRect r = *this;
        r.Deflate(dx, dy);
        return r;
    }

    void Offset(int dx, int dy) { x += dx; y += dy; }
    void Offset(const dkPoint& pt) { Offset(pt.x, pt.y); }

    dkRect& Intersect(const dkRect& rect);
    dkRect Intersect(const dkRect& rect) const
    {
        dkRect r = *this;
        r.Intersect(rect);
        return r;
    }

    dkRect& Union(const dkRect& rect);
    dkRect Union(const dkRect& rect) const
    {
        dkRect r = *this;
        r.Union(rect);
        return r;
    }

    // compare rectangles
    bool operator==(const dkRect& rect) const;
    bool operator!=(const dkRect& rect) const { return !(*this == rect); }

    // return true if the point is (not strcitly) inside the rect
    bool Contains(int x, int y) const;
    bool Contains(const dkPoint& pt) const { return Contains(pt.x, pt.y); }
    // return true if the rectangle is (not strcitly) inside the rect
    bool Contains(const dkRect& rect) const;

    // return true if the rectangles have a non empty intersection
    bool Intersects(const dkRect& rect) const;


    // these are like Union() but don't ignore empty rectangles
    dkRect operator+(const dkRect& rect) const;
    dkRect& operator+=(const dkRect& rect)
    {
        *this = *this + rect;
        return *this;
    }


    // centre this rectangle in the given (usually, but not necessarily,
    // larger) one
    dkRect CentreIn(const dkRect& r, int dir = dkBOTH) const
    {
        return dkRect(dir & dkHORIZONTAL ? r.x + (r.width - width)/2 : x,
                      dir & dkVERTICAL ? r.y + (r.height - height)/2 : y,
                      width, height);
    }

    dkRect CenterIn(const dkRect& r, int dir = dkBOTH) const
    {
        return CentreIn(r, dir);
    }

public:
    int x, y, width, height;
};

enum BookListMode
{
    BLM_ICON,
    BLM_LIST,
    BLM_MIN = BLM_ICON,
    BLM_MAX = BLM_LIST
};

enum BookListUsage
{
    BLU_BROWSE,
    BLU_SELECT
};

enum PageAncestorType
{
    PAT_BookShelf,
    PAT_BookStore,
    PAT_Personal,
    PAT_SystemSetting,
    PAT_Count
};

inline dkColor ArgbColorToDkColor(const DK_ARGBCOLOR& argbColor)
{
	return ( ~((argbColor.rgbRed<<16 & 0xFF0000) | (argbColor.rgbGreen<<8 & 0xFF00) | (argbColor.rgbBlue & 0xFF)));
}
#endif
