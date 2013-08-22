#ifndef UISizer_H
#define UISizer_H

#include <stdio.h>
#include <assert.h>
#include <list>
#include "Common/Defs.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UIWindow.h"
using namespace WindowsMetrics;

// ----------------------------------------------------------------------------
// UISizerFlags: flags used for an item in the sizer
// ----------------------------------------------------------------------------
class UIWindow;
class UISizerItem;
class UISizer;
class UIBoxSizer;

typedef std::list<UISizerItem*> UISizerItemList;
typedef std::list<UISizerItem*>::iterator UISizerItemListIterator;
typedef std::list<UISizerItem*>::const_iterator UISizerItemListConstIterator;

class UISizerFlags
{
public:
    // construct the flags object initialized with the given proportion (0 by
    // default)
    UISizerFlags(int proportion = 0) : m_proportion(proportion)
    {
        m_reduceFactor = 0;
        m_flags = 0;
        m_leftBorderInPixels = 0;
        m_rightBorderInPixels = 0;
        m_topBorderInPixels = 0;
        m_bottomBorderInPixels = 0;
    }

    // setters for all sizer flags, they all return the object itself so that
    // calls to them can be chained

    UISizerFlags& Proportion(int proportion)
    {
        m_proportion = proportion;
        return *this;
    }

    UISizerFlags& Expand()
    {
        m_flags |= dkEXPAND;
        return *this;
    }

    UISizerFlags& Reduce(int reduceFactor)
    {
        m_reduceFactor = reduceFactor;
        return *this;
    }

    // notice that Align() replaces the current alignment flags, use specific
    // methods below such as Top(), Left() &c if you want to set just the
    // vertical or horizontal alignment
    UISizerFlags& Align(int alignment) // combination of dkAlignment values
    {
        m_flags &= ~dkALIGN_MASK;
        m_flags |= alignment;

        return *this;
    }

    // some shortcuts for Align()
    UISizerFlags& Centre() { return Align(dkALIGN_CENTRE); }
    UISizerFlags& Center() { return Centre(); }

    UISizerFlags& Top()
    {
        m_flags &= ~(dkALIGN_BOTTOM | dkALIGN_CENTRE_VERTICAL);
        return *this;
    }

    UISizerFlags& Left()
    {
        m_flags &= ~(dkALIGN_RIGHT | dkALIGN_CENTRE_HORIZONTAL);
        return *this;
    }

    UISizerFlags& Right()
    {
        m_flags = (m_flags & ~dkALIGN_CENTRE_HORIZONTAL) | dkALIGN_RIGHT;
        return *this;
    }

    UISizerFlags& Bottom()
    {
        m_flags = (m_flags & ~dkALIGN_CENTRE_VERTICAL) | dkALIGN_BOTTOM;
        return *this;
    }


    // default border size used by Border() below
    static int GetDefaultBorder()
    {
        return GetWindowMetrics(UIDefaultInternalBorderIndex);
    }

    UISizerFlags& Border(int direction, int borderInPixels)
    {
        m_flags |= direction;

        if (direction & dkLEFT)  m_leftBorderInPixels  = borderInPixels;
        if (direction & dkRIGHT) m_rightBorderInPixels = borderInPixels;
        if (direction & dkTOP)   m_topBorderInPixels = borderInPixels;
        if (direction & dkBOTTOM)m_bottomBorderInPixels = borderInPixels;

        return *this;
    }

    UISizerFlags& Border(int direction = dkALL)
    {
        return Border(direction, GetDefaultBorder());
    }

    UISizerFlags& DoubleBorder(int direction = dkALL)
    {
        return Border(direction, 2*GetDefaultBorder());
    }

    UISizerFlags& TripleBorder(int direction = dkALL)
    {
        return Border(direction, 3*GetDefaultBorder());
    }

    UISizerFlags& HorzBorder()
    {
        return Border(dkLEFT | dkRIGHT, GetDefaultBorder());
    }

    UISizerFlags& DoubleHorzBorder()
    {
        return Border(dkLEFT | dkRIGHT, 2*GetDefaultBorder());
    }

    UISizerFlags& VertBorder()
    {
        return Border(dkTOP | dkBOTTOM, GetDefaultBorder());
    }

    UISizerFlags& DoubleVertBorder()
    {
        return Border(dkTOP | dkBOTTOM, 2*GetDefaultBorder());
    }

    // setters for the others flags
    UISizerFlags& Shaped()
    {
        m_flags |= dkSHAPED;

        return *this;
    }

    UISizerFlags& FixedMinSize()
    {
        m_flags |= dkFIXED_MINSIZE;

        return *this;
    }

    // makes the item ignore window's visibility status
    UISizerFlags& ReserveSpaceEvenIfHidden()
    {
        m_flags |= dkRESERVE_SPACE_EVEN_IF_HIDDEN;
        return *this;
    }

    // accessors for UISizer only
    int GetProportion() const { return m_proportion; }
    int GetFlags() const { return m_flags; }
    //int GetBorderInPixels() const { return m_borderInPixels; }
    int GetBorderInPixels(int direction) const
    {
        if (direction & dkLEFT)  return m_leftBorderInPixels;
        if (direction & dkRIGHT) return m_rightBorderInPixels;
        if (direction & dkTOP)   return m_topBorderInPixels;
        if (direction & dkBOTTOM) return m_bottomBorderInPixels;
        return 0;//error direction
    }

    int GetReduceFactor() const { return m_reduceFactor; }
private:
    int m_proportion;
    int m_flags;
    //int m_borderInPixels;
    int m_leftBorderInPixels;
    int m_rightBorderInPixels;
    int m_topBorderInPixels;
    int m_bottomBorderInPixels;
    int m_reduceFactor;
};


// ----------------------------------------------------------------------------
// UISizerSpacer: used by UISizerItem to represent a spacer
// ----------------------------------------------------------------------------

class UISizerSpacer
{
public:
    UISizerSpacer(const dkSize& size) : m_size(size), m_isShown(true) { }

    void SetSize(const dkSize& size) { m_size = size; }
    const dkSize& GetSize() const { return m_size; }

    void Show(bool show) { m_isShown = show; }
    bool IsShown() const { return m_isShown; }

private:
    // the size, in pixel
    dkSize m_size;

    // is the spacer currently shown?
    bool m_isShown;
};

// ----------------------------------------------------------------------------
// UISizerItem
// ----------------------------------------------------------------------------

class UISizerItem
{
public:
    // window
    UISizerItem( UIWindow *window,
                 int proportion=0,
                 int flag=0,
                 int border=0,
                 int reduceFactor=0 );

    // window with flags
    UISizerItem(UIWindow *window, const UISizerFlags& flags)
    {
        Init(flags);

        DoSetWindow(window);
    }

    // subsizer
    UISizerItem( UISizer *sizer,
                 int proportion=0,
                 int flag=0,
                 int border=0,
                 int reduceFactor=0 );

    // sizer with flags
    UISizerItem(UISizer *sizer, const UISizerFlags& flags)
    {
        Init(flags);

        DoSetSizer(sizer);
    }

    // spacer
    UISizerItem( int width,
                 int height,
                 int proportion=0,
                 int flag=0,
                 int border=0,
                 int reduceFactor=0 );

    // spacer with flags
    UISizerItem(int width, int height, const UISizerFlags& flags)
    {
        Init(flags);

        DoSetSpacer(dkSize(width, height));
    }

    UISizerItem();
    virtual ~UISizerItem();

    virtual void DeleteWindows();

    // Enable deleting the SizerItem without destroying the contained sizer.
    void DetachSizer() { m_sizer = NULL; }

    virtual dkSize GetSize() const;
    virtual dkSize CalcMin();
    virtual void SetDimension( const dkPoint& pos, const dkSize& size );

    dkSize GetMinSize() const
        { return m_minSize; }
    dkSize GetMinSizeWithBorder() const;

    void SetMinSize(const dkSize& size)
    {
        if ( IsWindow() )
            m_window->SetMinSize(size);
        m_minSize = size;
    }
    void SetMinWidth(int width)
    {
        if ( IsWindow() )
            m_window->SetMinWidth(width);
        m_minSize.SetWidth(width);
    }
    void SetMinHeight(int height)
    {
        if (IsWindow())
            m_window->SetMinHeight(height);
        m_minSize.SetHeight(height);
    }

    void SetMinSize( int x, int y )
        { SetMinSize(dkSize(x, y)); }
    void SetInitSize( int x, int y )
        { SetMinSize(dkSize(x, y)); }

    // if either of dimensions is zero, ratio is assumed to be 1
    // to avoid "divide by zero" errors
    void SetRatio(int width, int height)
        { m_ratio = (width && height) ? ((float) width / (float) height) : 1; }
    void SetRatio(const dkSize& size)
        { SetRatio(size.x, size.y); }
    void SetRatio(float ratio)
        { m_ratio = ratio; }
    float GetRatio() const
        { return m_ratio; }

    virtual dkRect GetRect() { return m_rect; }

    // set a sizer item id (different from a window id, all sizer items,
    // including spacers, can have an associated id)
    void SetId(int id) { m_id = id; }
    int GetId() const { return m_id; }

    bool IsWindow() const { return m_kind == Item_Window; }
    bool IsSizer() const { return m_kind == Item_Sizer; }
    bool IsSpacer() const { return m_kind == Item_Spacer; }

    void SetProportion( int proportion )
        { m_proportion = proportion; }
    int GetProportion() const
        { return m_proportion; }
    void SetReduceFactor(int reduceFactor )
    { m_reduceFactor = reduceFactor; }
    int GetReduceFactor() const
    { return m_reduceFactor; }
    void SetFlag( int flag )
        { m_flag = flag; }
    int GetFlag() const
        { return m_flag; }
    void SetBorder( int border )
        { 
            m_leftBorder = border; 
            m_rightBorder = border; 
            m_topBorder = border; 
            m_bottomBorder = border; 
        }
    void SetBorder(int direction, int border)
    {
        if (direction & dkLEFT)  m_leftBorder  = border;
        if (direction & dkRIGHT) m_rightBorder = border;
        if (direction & dkTOP)   m_topBorder = border;
        if (direction & dkBOTTOM)m_bottomBorder = border;
    }
    int GetBorder(int direction) const
        {
            if (direction & dkLEFT) return m_leftBorder;
            if (direction & dkRIGHT) return m_rightBorder;
            if (direction & dkTOP) return m_topBorder;
            if (direction & dkBOTTOM) return m_bottomBorder;
        }

    UIWindow *GetWindow() const
        { return m_kind == Item_Window ? m_window : NULL; }
    UISizer *GetSizer() const
        { return m_kind == Item_Sizer ? m_sizer : NULL; }
    dkSize GetSpacer() const;

    // This function behaves obviously for the windows and spacers but for the
    // sizers it returns true if any sizer element is shown and only returns
    // false if all of them are hidden. Also, it always returns true if
    // dkRESERVE_SPACE_EVEN_IF_HIDDEN flag was used.
    bool IsShown() const;

    void Show(bool show);

    dkPoint GetPosition() const
        { return m_pos; }

    // Called once the first component of an item has been decided. This is
    // used in algorithms that depend on knowing the size in one direction
    // before the min size in the other direction can be known.
    // Returns true if it made use of the information (and min size was changed).
    bool InformFirstDirection( int direction, int size, int availableOtherDir=-1 );

    // these functions delete the current contents of the item if it's a sizer
    // or a spacer but not if it is a window
    void AssignWindow(UIWindow *window)
    {
        Free();
        DoSetWindow(window);
    }

    void AssignSizer(UISizer *sizer)
    {
        Free();
        DoSetSizer(sizer);
    }

    void AssignSpacer(const dkSize& size)
    {
        Free();
        DoSetSpacer(size);
    }

    void AssignSpacer(int w, int h) { AssignSpacer(dkSize(w, h)); }
protected:
    // common part of several ctors
    void Init() { m_kind = Item_None; }

    // common part of ctors taking UISizerFlags
    void Init(const UISizerFlags& flags);

    // free current contents
    void Free();

    // common parts of Set/AssignXXX()
    void DoSetWindow(UIWindow *window);
    void DoSetSizer(UISizer *sizer);
    void DoSetSpacer(const dkSize& size);

    // discriminated union: depending on m_kind one of the fields is valid
    enum
    {
        Item_None,
        Item_Window,
        Item_Sizer,
        Item_Spacer,
        Item_Max
    } m_kind;
    union
    {
        UIWindow      *m_window;
        UISizer       *m_sizer;
        UISizerSpacer *m_spacer;
    };

    dkPoint      m_pos;
    dkSize       m_minSize;
    int          m_proportion;
    int          m_leftBorder;
    int          m_rightBorder;
    int          m_topBorder;
    int          m_bottomBorder;
    int          m_flag;
    int          m_id;
    int          m_reduceFactor;

    // on screen rectangle of this item (not including borders)
    dkRect       m_rect;

    // Aspect ratio can always be calculated from m_size,
    // but this would cause precision loss when the window
    // is shrunk.  It is safer to preserve the initial value.
    float        m_ratio;


private:
    UISizerItem(const UISizerItem&);
    UISizerItem& operator=(const UISizerItem&);
};

//---------------------------------------------------------------------------
// UISizer
//---------------------------------------------------------------------------

class UISizer
{
public:

    UISizer() { m_containingWindow = NULL; }
    virtual ~UISizer();
    void SetName(const char* name)
    {
#ifdef _DEBUG
        m_name = name;
#endif
    }
    const char* GetName() const
    {
#ifdef _DEBUG
        return m_name.c_str();
#else
        return NULL;
#endif
    }

    // methods for adding elements to the sizer: there are Add/Insert/Prepend
    // overloads for each of window/sizer/spacer/UISizerItem
    UISizerItem* Add(UIWindow *window,
                     int proportion = 0,
                     int flag = 0,
                     int border = 0 );
    UISizerItem* Add(UISizer *sizer,
                     int proportion = 0,
                     int flag = 0,
                     int border = 0 );
    UISizerItem* Add(int width,
                     int height,
                     int proportion = 0,
                     int flag = 0,
                     int border = 0 );
    UISizerItem* Add( UIWindow *window, const UISizerFlags& flags);
    UISizerItem* Add( UISizer *sizer, const UISizerFlags& flags);
    UISizerItem* Add( int width, int height, const UISizerFlags& flags);
    UISizerItem* Add( UISizerItem *item);

    virtual UISizerItem *AddSpacer(int size);
    UISizerItem* AddStretchSpacer(int prop = 1);

    UISizerItem* Insert(size_t index,
                        UIWindow *window,
                        int proportion = 0,
                        int flag = 0,
                        int border = 0 );
    UISizerItem* Insert(size_t index,
                        UISizer *sizer,
                        int proportion = 0,
                        int flag = 0,
                        int border = 0 );
    UISizerItem* Insert(size_t index,
                        int width,
                        int height,
                        int proportion = 0,
                        int flag = 0,
                        int border = 0 );
    UISizerItem* Insert(size_t index,
                        UIWindow *window,
                        const UISizerFlags& flags);
    UISizerItem* Insert(size_t index,
                        UISizer *sizer,
                        const UISizerFlags& flags);
    UISizerItem* Insert(size_t index,
                        int width,
                        int height,
                        const UISizerFlags& flags);

    // NB: do _not_ override this function in the derived classes, this one is
    //     virtual for compatibility reasons only to allow old code overriding
    //     it to continue to work, override DoInsert() instead in the new code
    virtual UISizerItem* Insert(size_t index, UISizerItem *item);

    UISizerItem* InsertSpacer(size_t index, int size);
    UISizerItem* InsertStretchSpacer(size_t index, int prop = 1);

    UISizerItem* Prepend(UIWindow *window,
                         int proportion = 0,
                         int flag = 0,
                         int border = 0 );
    UISizerItem* Prepend(UISizer *sizer,
                         int proportion = 0,
                         int flag = 0,
                         int border = 0 );
    UISizerItem* Prepend(int width,
                         int height,
                         int proportion = 0,
                         int flag = 0,
                         int border = 0 );
    UISizerItem* Prepend(UIWindow *window, const UISizerFlags& flags);
    UISizerItem* Prepend(UISizer *sizer, const UISizerFlags& flags);
    UISizerItem* Prepend(int width, int height, const UISizerFlags& flags);
    UISizerItem* Prepend(UISizerItem *item);

    UISizerItem* PrependSpacer(int size);
    UISizerItem* PrependStretchSpacer(int prop = 1);

    // set (or possibly unset if window is NULL) or get the window this sizer
    // is used in
    void SetContainingWindow(UIWindow *window);
    UIWindow *GetContainingWindow() const { return m_containingWindow; }

    virtual bool Remove( UISizer *sizer );
    virtual bool Remove( int index );

    virtual bool Detach( UIWindow *window );
    virtual bool Detach( UISizer *sizer );
    virtual bool Detach( int index );

    virtual bool Replace( UIWindow *oldwin, UIWindow *newwin, bool recursive = false );
    virtual bool Replace( UISizer *oldsz, UISizer *newsz, bool recursive = false );
    virtual bool Replace( size_t index, UISizerItem *newitem );

    virtual void Clear( bool delete_windows = false );
    virtual void DeleteWindows();

    // Inform sizer about the first direction that has been decided (by parent item)
    // Returns true if it made use of the informtion (and recalculated min size)
    virtual bool InformFirstDirection( int direction, int size, int availableOtherDir)
        { 
            (void)(direction);
            (void)(size);
            (void)(availableOtherDir);
            return false; 
        }

    void SetMinSize( int width, int height )
        { DoSetMinSize( width, height ); }
    void SetMinHeight(int height)
        { m_minSize.y = height; }
    void SetMinWidth(int width)
        { m_minSize.x = width; }
    void SetMinSize( const dkSize& size )
        { DoSetMinSize( size.x, size.y ); }

    // Searches recursively
    bool SetItemMinSize( UIWindow *window, int width, int height )
        { return DoSetItemMinSize( window, width, height ); }
    bool SetItemMinSize( UIWindow *window, const dkSize& size )
        { return DoSetItemMinSize( window, size.x, size.y ); }

    // Searches recursively
    bool SetItemMinSize( UISizer *sizer, int width, int height )
        { return DoSetItemMinSize( sizer, width, height ); }
    bool SetItemMinSize( UISizer *sizer, const dkSize& size )
        { return DoSetItemMinSize( sizer, size.x, size.y ); }

    bool SetItemMinSize( size_t index, int width, int height )
        { return DoSetItemMinSize( index, width, height ); }
    bool SetItemMinSize( size_t index, const dkSize& size )
        { return DoSetItemMinSize( index, size.x, size.y ); }

    dkSize GetSize() const
        { return m_size; }
    dkPoint GetPosition() const
        { return m_position; }

    // Calculate the minimal size or return m_minSize if bigger.
    dkSize GetMinSize();

    // These virtual functions are used by the layout algorithm: first
    // CalcMin() is called to calculate the minimal size of the sizer and
    // prepare for laying it out and then RecalcSizes() is called to really
    // update all the sizer items
    virtual dkSize CalcMin() = 0;
    virtual void RecalcSizes() = 0;

    virtual void Layout();

    dkSize ComputeFittingClientSize(UIWindow *window);
    dkSize ComputeFittingWindowSize(UIWindow *window);

    dkSize Fit( UIWindow *window );
    void SetSizeHints( UIWindow *window );

    UISizerItemList& GetChildren()
        { return m_children; }
    const UISizerItemList& GetChildren() const
        { return m_children; }

    void SetDimension(const dkPoint& pos, const dkSize& size);
    void SetDimension(int x, int y, int width, int height)
        { SetDimension(dkPoint(x, y), dkSize(width, height)); }

    size_t GetItemCount() const { return m_children.size(); }
    bool IsEmpty() const { return m_children.empty(); }

    int IndexOf(UIWindow* window) const;
    int IndexOf(UISizer* UISizer) const;
    int IndexOf(UISizerItem* item) const;

    UISizerItem* GetItem( UIWindow *window, bool recursive = false );
    UISizerItem* GetItem( UISizer *sizer, bool recursive = false );
    UISizerItem* GetItem( size_t index );
    UISizerItem* GetItemById( int id, bool recursive = false );

    // Manage whether individual scene items are considered
    // in the layout calculations or not.
    bool Show( UIWindow *window, bool show = true, bool recursive = false );
    bool Show( UISizer *sizer, bool show = true, bool recursive = false );
    bool Show( size_t index, bool show = true );

    bool Hide( UISizer *sizer, bool recursive = false )
        { return Show( sizer, false, recursive ); }
    bool Hide( UIWindow *window, bool recursive = false )
        { return Show( window, false, recursive ); }
    bool Hide( size_t index )
        { return Show( index, false ); }

    bool IsShown( UIWindow *window ) const;
    bool IsShown( UISizer *sizer ) const;
    bool IsShown( size_t index ) const;

    // Recursively call UIWindow::Show () on all sizer items.
    virtual void ShowItems (bool show);

    void Show(bool show) { ShowItems(show); }

protected:
    dkSize              m_size;
    dkSize              m_minSize;
    dkPoint             m_position;
    UISizerItemList     m_children;

    // the window this sizer is used in, can be NULL
    UIWindow *m_containingWindow;

    dkSize GetMaxClientSize( UIWindow *window ) const;
    dkSize GetMinClientSize( UIWindow *window );
    //dkSize VirtualFitSize( UIWindow *window );

    virtual void DoSetMinSize( int width, int height );
    virtual bool DoSetItemMinSize( UIWindow *window, int width, int height );
    virtual bool DoSetItemMinSize( UISizer *sizer, int width, int height );
    virtual bool DoSetItemMinSize( size_t index, int width, int height );

    // insert a new item into m_children at given index and return the item
    // itself
    virtual UISizerItem* DoInsert(size_t index, UISizerItem *item);
    
    UISizerItemListConstIterator ItemIteratorAt(size_t index) const;
    UISizerItemListIterator ItemIteratorAt(size_t index);
private:
#ifdef _DEBUG
    std::string m_name;
#endif
};

//---------------------------------------------------------------------------
// dkGridSizer
//---------------------------------------------------------------------------
class UIGridSizer: public UISizer
{
public:
    // ctors specifying the number of columns only: number of rows will be
    // deduced automatically depending on the number of sizer elements
    UIGridSizer( int cols, int vgap, int hgap );
    UIGridSizer( int cols, const dkSize& gap = dkSize(0, 0) );

    // ctors specifying the number of rows and columns
    UIGridSizer( int rows, int cols, int vgap, int hgap );
    UIGridSizer( int rows, int cols, const dkSize& gap );

    virtual void RecalcSizes();
    virtual dkSize CalcMin();

    void SetCols( int cols )
    {
        assert(cols >= 0);
        m_cols = cols;
    }

    void SetRows( int rows )
    {
        assert(rows >= 0);
        m_rows = rows;
    }

    void SetVGap( int gap )     { m_vgap = gap; }
    void SetHGap( int gap )     { m_hgap = gap; }
    int GetCols() const         { return m_cols; }
    int GetRows() const         { return m_rows; }
    int GetVGap() const         { return m_vgap; }
    int GetHGap() const         { return m_hgap; }

    int GetEffectiveColsCount() const   { return m_cols ? m_cols : CalcCols(); }
    int GetEffectiveRowsCount() const   { return m_rows ? m_rows : CalcRows(); }

    // return the number of total items and the number of columns and rows
    // (for internal use only)
    int CalcRowsCols(int& rows, int& cols) const;

protected:
    // the number of rows/columns in the sizer, if 0 then it is determined
    // dynamically depending on the total number of items
    int    m_rows;
    int    m_cols;

    // gaps between rows and columns
    int    m_vgap;
    int    m_hgap;

    virtual UISizerItem *DoInsert(size_t index, UISizerItem *item);

    void SetItemBounds( UISizerItem *item, int x, int y, int w, int h );

    // returns the number of columns/rows needed for the current total number
    // of children (and the fixed number of rows/columns)
    int CalcCols() const
    {
        if (m_rows <= 0)
        {
            //printf("can't calculate number of cols if number of rows is not specified");
            return -1;
        }
        return (m_children.size() + m_rows - 1) / m_rows;
    }

    int CalcRows() const
    {
        if (m_cols <= 0)
        {
            //printf("Can't calculate number of cols if number of rows is not specified");
            return -1;
        }
        return (m_children.size() + m_cols - 1) / m_cols;
    }
};

//---------------------------------------------------------------------------
// UIBoxSizer
//---------------------------------------------------------------------------
class UIBoxSizer: public UISizer
{
public:
    UIBoxSizer(int orient)
    {
        m_orient = orient;
        m_totalProportion = 0;

        assert( m_orient == dkHORIZONTAL || m_orient == dkVERTICAL);
    }

    virtual UISizerItem *AddSpacer(int size);

    int GetOrientation() const { return m_orient; }

    bool IsVertical() const { return m_orient == dkVERTICAL; }

    void SetOrientation(int orient) { m_orient = orient; }

    // implementation of our resizing logic
    virtual dkSize CalcMin();
    virtual void RecalcSizes();

protected:
    // helpers for our code: this returns the component of the given dkSize in
    // the direction of the sizer and in the other direction, respectively
    int GetSizeInMajorDir(const dkSize& sz) const
    {
        return m_orient == dkHORIZONTAL ? sz.x : sz.y;
    }

    int& SizeInMajorDir(dkSize& sz)
    {
        return m_orient == dkHORIZONTAL ? sz.x : sz.y;
    }

    int& PosInMajorDir(dkPoint& pt)
    {
        return m_orient == dkHORIZONTAL ? pt.x : pt.y;
    }

    int GetSizeInMinorDir(const dkSize& sz) const
    {
        return m_orient == dkHORIZONTAL ? sz.y : sz.x;
    }

    int& SizeInMinorDir(dkSize& sz)
    {
        return m_orient == dkHORIZONTAL ? sz.y : sz.x;
    }

    int& PosInMinorDir(dkPoint& pt)
    {
        return m_orient == dkHORIZONTAL ? pt.y : pt.x;
    }

    // another helper: creates dkSize from major and minor components
    dkSize SizeFromMajorMinor(int major, int minor) const
    {
        if ( m_orient == dkHORIZONTAL )
        {
            return dkSize(major, minor);
        }
        else // dkVERTICAL
        {
            return dkSize(minor, major);
        }
    }


    // either dkHORIZONTAL or dkVERTICAL
    int m_orient;

    // the sum of proportion of all of our elements
    int m_totalProportion;

    // the sum of reducefactor of all of our elements
    int m_totalReduceFactor;

    // the minimal size needed for this sizer as calculated by the last call to
    // our CalcMin()
    dkSize m_minSize;

private:
};

inline UISizerItem*
UISizer::Insert(size_t index, UISizerItem *item)
{
    return DoInsert(index, item);
}


inline UISizerItem*
UISizer::Add( UISizerItem *item )
{
    return Insert( m_children.size(), item );
}

inline UISizerItem*
UISizer::Add( UIWindow *window, int proportion, int flag, int border )
{
    return Add( new UISizerItem( window, proportion, flag, border ) );
}

inline UISizerItem*
UISizer::Add( UISizer *sizer, int proportion, int flag, int border )
{
    return Add( new UISizerItem( sizer, proportion, flag, border ) );
}

inline UISizerItem*
UISizer::Add( int width, int height, int proportion, int flag, int border )
{
    return Add( new UISizerItem( width, height, proportion, flag, border ) );
}

inline UISizerItem*
UISizer::Add( UIWindow *window, const UISizerFlags& flags )
{
    return Add( new UISizerItem(window, flags) );
}

inline UISizerItem*
UISizer::Add( UISizer *sizer, const UISizerFlags& flags )
{
    return Add( new UISizerItem(sizer, flags) );
}

inline UISizerItem*
UISizer::Add( int width, int height, const UISizerFlags& flags )
{
    return Add( new UISizerItem(width, height, flags) );
}

inline UISizerItem*
UISizer::AddSpacer(int size)
{
    return Add(size, size);
}

inline UISizerItem*
UISizer::AddStretchSpacer(int prop)
{
    return Add(0, 0, prop);
}

inline UISizerItem*
UISizer::Prepend( UISizerItem *item )
{
    return Insert( 0, item );
}

inline UISizerItem*
UISizer::Prepend( UIWindow *window, int proportion, int flag, int border )
{
    return Prepend( new UISizerItem( window, proportion, flag, border ) );
}

inline UISizerItem*
UISizer::Prepend( UISizer *sizer, int proportion, int flag, int border )
{
    return Prepend( new UISizerItem( sizer, proportion, flag, border ) );
}

inline UISizerItem*
UISizer::Prepend( int width, int height, int proportion, int flag, int border )
{
    return Prepend( new UISizerItem( width, height, proportion, flag, border ) );
}

inline UISizerItem*
UISizer::PrependSpacer(int size)
{
    return Prepend(size, size);
}

inline UISizerItem*
UISizer::PrependStretchSpacer(int prop)
{
    return Prepend(0, 0, prop);
}

inline UISizerItem*
UISizer::Prepend( UIWindow *window, const UISizerFlags& flags )
{
    return Prepend( new UISizerItem(window, flags) );
}

inline UISizerItem*
UISizer::Prepend( UISizer *sizer, const UISizerFlags& flags )
{
    return Prepend( new UISizerItem(sizer, flags) );
}

inline UISizerItem*
UISizer::Prepend( int width, int height, const UISizerFlags& flags )
{
    return Prepend( new UISizerItem(width, height, flags) );
}

inline UISizerItem*
UISizer::Insert( size_t index,
                 UIWindow *window,
                 int proportion,
                 int flag,
                 int border)
{
    return Insert( index, new UISizerItem( window, proportion, flag, border ) );
}

inline UISizerItem*
UISizer::Insert( size_t index,
                 UISizer *sizer,
                 int proportion,
                 int flag,
                 int border)
{
    return Insert( index, new UISizerItem( sizer, proportion, flag, border ) );
}

inline UISizerItem*
UISizer::Insert( size_t index,
                 int width,
                 int height,
                 int proportion,
                 int flag,
                 int border)
{
    return Insert( index, new UISizerItem( width, height, proportion, flag, border ) );
}

inline UISizerItem*
UISizer::Insert( size_t index, UIWindow *window, const UISizerFlags& flags )
{
    return Insert( index, new UISizerItem(window, flags) );
}

inline UISizerItem*
UISizer::Insert( size_t index, UISizer *sizer, const UISizerFlags& flags )
{
    return Insert( index, new UISizerItem(sizer, flags) );
}

inline UISizerItem*
UISizer::Insert( size_t index, int width, int height, const UISizerFlags& flags )
{
    return Insert( index, new UISizerItem(width, height, flags) );
}

inline UISizerItem*
UISizer::InsertSpacer(size_t index, int size)
{
    return Insert(index, size, size);
}

inline UISizerItem*
UISizer::InsertStretchSpacer(size_t index, int prop)
{
    return Insert(index, 0, 0, prop);
}

#endif //UISizer_H
