#include "Common/Defs.h"
#include "KernelDef.h"

dkRect::dkRect(const dkPoint& point1, const dkPoint& point2)
{
    x = point1.x;
    y = point1.y;
    width = point2.x - point1.x;
    height = point2.y - point1.y;

    if (width < 0)
    {
        width = -width;
        x = point2.x;
    }
    width++;

    if (height < 0)
    {
        height = -height;
        y = point2.y;
    }
    height++;
}

bool dkRect::operator==(const dkRect& rect) const
{
    return ((x == rect.x) &&
            (y == rect.y) &&
            (width == rect.width) &&
            (height == rect.height));
}

dkRect dkRect::operator+(const dkRect& rect) const
{
    int x1 = dk_min(this->x, rect.x);
    int y1 = dk_min(this->y, rect.y);
    int y2 = dk_max(y+height, rect.height+rect.y);
    int x2 = dk_max(x+width, rect.width+rect.x);
    return dkRect(x1, y1, x2-x1, y2-y1);
}

dkRect& dkRect::Union(const dkRect& rect)
{
    // ignore empty rectangles: union with an empty rectangle shouldn't extend
    // this one to (0, 0)
    if ( !width || !height )
    {
        *this = rect;
    }
    else if ( rect.width && rect.height )
    {
        int x1 = dk_min(x, rect.x);
        int y1 = dk_min(y, rect.y);
        int y2 = dk_max(y + height, rect.height + rect.y);
        int x2 = dk_max(x + width, rect.width + rect.x);

        x = x1;
        y = y1;
        width = x2 - x1;
        height = y2 - y1;
    }
    //else: we're not empty and rect is empty

    return *this;
}

dkRect& dkRect::Inflate(int dx, int dy)
{
     if (-2*dx>width)
     {
         // Don't allow deflate to eat more width than we have,
         // a well-defined rectangle cannot have negative width.
         x+=width/2;
         width=0;
     }
     else
     {
         // The inflate is valid.
         x-=dx;
         width+=2*dx;
     }

     if (-2*dy>height)
     {
         // Don't allow deflate to eat more height than we have,
         // a well-defined rectangle cannot have negative height.
         y+=height/2;
         height=0;
     }
     else
     {
         // The inflate is valid.
         y-=dy;
         height+=2*dy;
     }

    return *this;
}

bool dkRect::Contains(int cx, int cy) const
{
    return ( (cx >= x) && (cy >= y)
          && ((cy - y) < height)
          && ((cx - x) < width)
          );
}

bool dkRect::Contains(const dkRect& rect) const
{
    return Contains(rect.GetTopLeft()) && Contains(rect.GetBottomRight());
}

dkRect& dkRect::Intersect(const dkRect& rect)
{
    int x2 = GetRight(),
        y2 = GetBottom();

    if ( x < rect.x )
        x = rect.x;
    if ( y < rect.y )
        y = rect.y;
    if ( x2 > rect.GetRight() )
        x2 = rect.GetRight();
    if ( y2 > rect.GetBottom() )
        y2 = rect.GetBottom();

    width = x2 - x + 1;
    height = y2 - y + 1;

    if ( width <= 0 || height <= 0 )
    {
        width =
        height = 0;
    }

    return *this;
}

bool dkRect::Intersects(const dkRect& rect) const
{
    dkRect r = Intersect(rect);

    // if there is no intersection, both width and height are 0
    return r.width != 0;
}
