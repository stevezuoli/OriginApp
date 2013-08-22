#include "stdio.h"
#include <vector>
#include "GUI/UIWindow.h"
#include "GUI/UISizer.h"
#include "CommandID.h"

//check for flags conflicts

static const int SIZER_FLAGS_MASK = dkCENTER | dkHORIZONTAL | dkVERTICAL |
    dkLEFT | dkRIGHT | dkUP | dkDOWN |
    dkALIGN_NOT | dkALIGN_CENTER_HORIZONTAL | dkALIGN_RIGHT | dkALIGN_BOTTOM | dkALIGN_CENTER_VERTICAL |
    dkFIXED_MINSIZE | dkRESERVE_SPACE_EVEN_IF_HIDDEN | dkSTRETCH_NOT | dkSHRINK | dkGROW | dkSHAPED;

#define dkASSERT_VALID_FLAGS(f, mask)   \
        assert((f & mask) == f)

#define ASSERT_VALID_SIZER_FLAGS(f) dkASSERT_VALID_FLAGS(f, SIZER_FLAGS_MASK)

void UISizerItem::Init(const UISizerFlags& flags)
{
    Init();

    m_proportion = flags.GetProportion();
    m_flag = flags.GetFlags();
    m_leftBorder = flags.GetBorderInPixels(dkLEFT);
    m_rightBorder = flags.GetBorderInPixels(dkRIGHT);
    m_topBorder = flags.GetBorderInPixels(dkTOP);
    m_bottomBorder = flags.GetBorderInPixels(dkBOTTOM);
    m_reduceFactor = flags.GetReduceFactor();

    ASSERT_VALID_SIZER_FLAGS( m_flag );
}

UISizerItem::UISizerItem()
{
    Init();

    m_proportion = 0;
    m_leftBorder = 0;
    m_rightBorder = 0;
    m_topBorder = 0;
    m_bottomBorder = 0;
    m_flag = 0;
    m_id = IDSTATIC;
    m_reduceFactor = 0;
}

// window item
void UISizerItem::DoSetWindow(UIWindow *window)
{
    if (NULL == window)
    {
        //printf("NULL window in UISizerItem::SetWindow()");
        return;
    }

    m_kind = Item_Window;
    m_window = window;

    // window doesn't become smaller than its initial size, whatever happens
    m_minSize = window->GetSize();

    //if ( m_flag & dkFIXED_MINSIZE )
    //    window->SetMinSize(m_minSize);

    // aspect ratio calculated from initial size
    SetRatio(m_minSize);
}

UISizerItem::UISizerItem(UIWindow *window,
                         int proportion,
                         int flag,
                         int border,
                         int reduceFactor )
           : m_kind(Item_None),
             m_proportion(proportion),
             m_flag(flag),
             m_id(IDSTATIC),
             m_reduceFactor(reduceFactor)
{
    ASSERT_VALID_SIZER_FLAGS( m_flag );
    SetBorder(0);
    SetBorder(flag, border);
    DoSetWindow(window);
}

// sizer item
void UISizerItem::DoSetSizer(UISizer *sizer)
{
    m_kind = Item_Sizer;
    m_sizer = sizer;
}

UISizerItem::UISizerItem(UISizer *sizer,
                         int proportion,
                         int flag,
                         int border,
                         int reduceFactor )
           : m_kind(Item_None),
             m_sizer(NULL),
             m_proportion(proportion),
             m_flag(flag),
             m_id(IDSTATIC),
             m_reduceFactor(0),
             m_ratio(0.0)
{
    ASSERT_VALID_SIZER_FLAGS( m_flag );
    SetBorder(0);
    SetBorder(flag, border);
    DoSetSizer(sizer);

    // m_minSize is set later
}

// spacer item
void UISizerItem::DoSetSpacer(const dkSize& size)
{
    m_kind = Item_Spacer;
    m_spacer = new UISizerSpacer(size);
    m_minSize = size;
    SetRatio(size);
}

UISizerItem::UISizerItem(int width,
                         int height,
                         int proportion,
                         int flag,
                         int border,
                         int reduceFactor )
           : m_kind(Item_None),
             m_sizer(NULL),
             m_minSize(width, height), // minimal size is the initial size
             m_proportion(proportion),
             m_flag(flag),
             m_id(IDSTATIC),
             m_reduceFactor(reduceFactor)
{
    ASSERT_VALID_SIZER_FLAGS( m_flag );
    SetBorder(0);
    SetBorder(flag, border);
    DoSetSpacer(dkSize(width, height));
}

UISizerItem::~UISizerItem()
{
    Free();
}

void UISizerItem::Free()
{
    switch ( m_kind )
    {
        case Item_None:
            break;

        case Item_Window:
            m_window->SetContainingSizer(NULL);
            break;

        case Item_Sizer:
            delete m_sizer;
            break;

        case Item_Spacer:
            delete m_spacer;
            break;

        case Item_Max:
        default:
            //printf("unexpected UISizerItem::m_kind");
            break;
    }

    m_kind = Item_None;
}

dkSize UISizerItem::GetSpacer() const
{
    dkSize size;
    if ( m_kind == Item_Spacer )
        size = m_spacer->GetSize();

    return size;
}


dkSize UISizerItem::GetSize() const
{
    dkSize ret;
    switch ( m_kind )
    {
        case Item_None:
            break;

        case Item_Window:
            ret = m_window->GetSize();
            break;

        case Item_Sizer:
            ret = m_sizer->GetSize();
            break;

        case Item_Spacer:
            ret = m_spacer->GetSize();
            break;

        case Item_Max:
        default:
            //printf("unexpected UISizerItem::m_kind");
            break;
    }

    if (m_flag & dkWEST)
        ret.x += m_leftBorder;
    if (m_flag & dkEAST)
        ret.x += m_rightBorder;
    if (m_flag & dkNORTH)
        ret.y += m_topBorder;
    if (m_flag & dkSOUTH)
        ret.y += m_bottomBorder;

    return ret;
}

bool UISizerItem::InformFirstDirection(int direction, int size, int availableOtherDir)
{
    // The size that come here will be including borders. Child items should get it
    // without borders.
    if( size>0 )
    {
        if( direction==dkHORIZONTAL )
        {
            if (m_flag & dkWEST)
                size -= m_leftBorder;
            if (m_flag & dkEAST)
                size -= m_rightBorder;
        }
        else if( direction==dkVERTICAL )
        {
            if (m_flag & dkNORTH)
                size -= m_topBorder;
            if (m_flag & dkSOUTH)
                size -= m_bottomBorder;
        }
    }

    bool didUse = false;
    // Pass the information along to the held object
    if (IsSizer())
    {
        didUse = GetSizer()->InformFirstDirection(direction,size,availableOtherDir);
        if (didUse)
            m_minSize = GetSizer()->CalcMin();

        DebugPrintf(DLC_GUI_VERBOSE, "Get min size for %08X (%s): (%d, %d)",
                GetSizer(),
                GetSizer()->GetName(),
                m_minSize.x,
                m_minSize.y);

    }
    else if (IsWindow())
    {
        didUse =  GetWindow()->InformFirstDirection(direction,size,availableOtherDir);
        if (didUse)
        {
            m_minSize = m_window->GetMinSize();
            DebugPrintf(DLC_GUI_VERBOSE, "Get min size for %08X (%s, %s): (%d, %d)",
                m_window,
                m_window->GetClassName(),
                m_window->GetText(),
                m_minSize.x,
                m_minSize.y);
        }
            //m_minSize = m_window->GetEffectiveMinSize();

        // This information is useful for items with dkSHAPED flag, since
        // we can request an optimal min size for such an item. Even if
        // we overwrite the m_minSize member here, we can read it back from
        // the owned window (happens automatically).
        if( (m_flag & dkSHAPED) && (m_flag & dkEXPAND) && direction )
        {
            if( (m_ratio-0.) > 1e-6 )
            {
                if (m_proportion == 0)
                {
                    //printf("Shaped item, non-zero proportion in UISizerItem::InformFirstDirection()");
                    return false;
                }

                if( direction==dkHORIZONTAL && (m_ratio-0.) > 1e-6)
                {
                    // Clip size so that we don't take too much
                    if( availableOtherDir>=0 && int(size/m_ratio)-m_minSize.y>availableOtherDir )
                        size = int((availableOtherDir+m_minSize.y)*m_ratio);
                    m_minSize = dkSize(size,int(size/m_ratio));
                }
                else if( direction==dkVERTICAL )
                {
                    // Clip size so that we don't take too much
                    if( availableOtherDir>=0 && int(size*m_ratio)-m_minSize.x>availableOtherDir )
                        size = int((availableOtherDir+m_minSize.x)/m_ratio);
                    m_minSize = dkSize(int(size*m_ratio),size);
                }
                didUse = true;
            }
        }
    }

    return didUse;
}

dkSize UISizerItem::CalcMin()
{
    if (IsSizer())
    {
        m_minSize = m_sizer->GetMinSize();
        DebugPrintf(DLC_GUI_VERBOSE, "Calc min size for %08X (%s): (%d, %d)",
            m_sizer,
            m_sizer->GetName(),
            m_minSize.x,
            m_minSize.y);

        // if we have to preserve aspect ratio _AND_ this is
        // the first-time calculation, consider ret to be initial size
        if ( (m_flag & dkSHAPED) && ((m_ratio - 0.)<1e-6) )
            SetRatio(m_minSize);
    }
    else if ( IsWindow() )
    {
        // Since the size of the window may change during runtime, we
        // should use the current minimal/best size.
        //m_minSize = m_window->GetEffectiveMinSize();
        m_minSize = m_window->GetMinSize();
        DebugPrintf(DLC_GUI_VERBOSE, "Calc min size for %08X (%s, %s): (%d, %d)",
            m_window,
            m_window->GetClassName(),
            m_window->GetText(),
            m_minSize.x,
            m_minSize.y);
    }

    return GetMinSizeWithBorder();
}

dkSize UISizerItem::GetMinSizeWithBorder() const
{
    dkSize ret = m_minSize;

    if (m_flag & dkWEST)
        ret.x += m_leftBorder;
    if (m_flag & dkEAST)
        ret.x += m_rightBorder;
    if (m_flag & dkNORTH)
        ret.y += m_topBorder;
    if (m_flag & dkSOUTH)
        ret.y += m_bottomBorder;

    return ret;
}


void UISizerItem::SetDimension( const dkPoint& pos_, const dkSize& size_ )
{
    DebugPrintf(DLC_GUI_VERBOSE,
            "UISizerItem::SetDimension (%d,%d) (%d,%d)",
             pos_.x, pos_.y, size_.GetWidth(), size_.GetHeight());
    dkPoint pos = pos_;
    dkSize size = size_;
    if (m_flag & dkSHAPED)
    {
        // adjust aspect ratio
        int rwidth = (int) (size.y * m_ratio);
        if (rwidth > size.x)
        {
            // fit horizontally
            int rheight = (int) (size.x / m_ratio);
            // add vertical space
            if (m_flag & dkALIGN_CENTER_VERTICAL)
                pos.y += (size.y - rheight) / 2;
            else if (m_flag & dkALIGN_BOTTOM)
                pos.y += (size.y - rheight);
            // use reduced dimensions
            size.y =rheight;
        }
        else if (rwidth < size.x)
        {
            // add horizontal space
            if (m_flag & dkALIGN_CENTER_HORIZONTAL)
                pos.x += (size.x - rwidth) / 2;
            else if (m_flag & dkALIGN_RIGHT)
                pos.x += (size.x - rwidth);
            size.x = rwidth;
        }
    }

    // This is what GetPosition() returns. Since we calculate
    // borders afterwards, GetPosition() will be the left/top
    // corner of the surrounding border.
    m_pos = pos;

    if (m_flag & dkWEST)
    {
        pos.x += m_leftBorder;
        size.x -= m_leftBorder;
    }
    if (m_flag & dkEAST)
    {
        size.x -= m_rightBorder;
    }
    if (m_flag & dkNORTH)
    {
        pos.y += m_topBorder;
        size.y -= m_topBorder;
    }
    if (m_flag & dkSOUTH)
    {
        size.y -= m_bottomBorder;
    }

    if (size.x < 0)
        size.x = 0;
    if (size.y < 0)
        size.y = 0;

    m_rect = dkRect(pos, size);

    switch ( m_kind )
    {
        case Item_None:
            //printf("can't set size of uninitialized sizer item");
            break;

        case Item_Window:
        {
            DebugPrintf(DLC_GUI_VERBOSE, "SetDimension for window: %s, %s",
                    m_window->GetClassName(), m_window->GetText());
            // Use dkSIZE_FORCE_EVENT here since a sizer item might
            // have changed alignment or some other property which would
            // not change the size of the window. In such a case, no
            // dkSizeEvent would normally be generated and thus the
            // control wouldn't get laid out correctly here.
#if 0
            m_window->SetSize(pos.x, pos.y, size.x, size.y,
                              dkSIZE_ALLOW_MINUS_ONE|dkSIZE_FORCE_EVENT );
#else
            m_window->SetSize(pos.x, pos.y, size.x, size.y,
                              dkSIZE_ALLOW_MINUS_ONE );
#endif
            break;
        }
        case Item_Sizer:
            DebugPrintf(DLC_GUI_VERBOSE, "SetDimension for sizer");
            m_sizer->SetDimension(pos, size);
            break;

        case Item_Spacer:
            DebugPrintf(DLC_GUI_VERBOSE, "SetDimension for spacer");
            m_spacer->SetSize(size);
            break;

        case Item_Max:
        default:
            //printf("unexpected UISizerItem::m_kind");
            break;
    }
}

void UISizerItem::DeleteWindows()
{
    switch ( m_kind )
    {
        case Item_None:
        case Item_Spacer:
            break;

        case Item_Window:
            //We are deleting the window from this sizer - normally
            //the window destroys the sizer associated with it,
            //which might destroy this, which we don't want
            m_window->SetContainingSizer(NULL);
            //m_window->Destroy();//by zhy
            //Putting this after the switch will result in a spacer
            //not being deleted properly on destruction
            m_kind = Item_None;
            break;

        case Item_Sizer:
            m_sizer->DeleteWindows();
            break;

        case Item_Max:
        default:
            //printf("unexpected UISizerItem::m_kind");
            break;
    }

}

void UISizerItem::Show( bool show )
{
    switch ( m_kind )
    {
        case Item_None:
            //printf("can't show uninitialized sizer item");
            break;

        case Item_Window:
            m_window->SetVisible(show);
            break;

        case Item_Sizer:
            m_sizer->Show(show);
            break;

        case Item_Spacer:
            m_spacer->Show(show);
            break;

        case Item_Max:
        default:
            //printf("unexpected UISizerItem::m_kind");
            break;
    }
}

bool UISizerItem::IsShown() const
{
    if ( m_flag & dkRESERVE_SPACE_EVEN_IF_HIDDEN )
        return true;

    switch ( m_kind )
    {
        case Item_None:
            // we may be called from CalcMin(), just return false so that we're
            // not used
            break;

        case Item_Window:
            return m_window->IsShown();

        case Item_Sizer:
        {
            // arbitrarily decide that if at least one of our elements is
            // shown, so are we (this arbitrariness is the reason for
            // deprecating this function)
            for (UISizerItemListIterator node = m_sizer->GetChildren().begin();node != m_sizer->GetChildren().end();++node)
            {
                if ( (*node)->IsShown() )
                    return true;
            }
            return false;
        }

        case Item_Spacer:
            return m_spacer->IsShown();

        case Item_Max:
        default:
            //printf("unexpected UISizerItem::m_kind");
            break;
    }

    return false;
}

//---------------------------------------------------------------------------
// UISizer
//---------------------------------------------------------------------------

UISizer::~UISizer()
{
    UISizerItemListIterator it, en;
    for (it=m_children.begin(), en=m_children.end(); it!=en; ++it)
        delete *it;
    m_children.clear();
}

UISizerItem* UISizer::DoInsert( size_t index, UISizerItem *item )
{
    m_children.insert(ItemIteratorAt(index), item);

    if ( item->GetWindow() )
        item->GetWindow()->SetContainingSizer( this );

    if ( item->GetSizer() )
        item->GetSizer()->SetContainingWindow( m_containingWindow );

    return item;
}

void UISizer::SetContainingWindow(UIWindow *win)
{
    if ( win == m_containingWindow )
        return;

    m_containingWindow = win;

    // set the same window for all nested sizers as well, they also are in the
    // same window
    for (UISizerItemListIterator node = m_children.begin();node != m_children.end();++node)
    {
        UISizerItem *const item = *node;
        UISizer *const sizer = item->GetSizer();

        if ( sizer )
        {
            sizer->SetContainingWindow(win);
        }
    }
}

bool UISizer::Remove( UISizer *sizer )
{
    if (sizer == NULL)
    {
        //printf("Removing NULL sizer");
        return false;
    }

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetSizer() == sizer)
        {
            delete item;
            m_children.erase( node );
            return true;
        }

        ++node;
    }

    return false;
}

bool UISizer::Remove( int index )
{
    if (!(index >= 0 && (size_t)index < m_children.size()))
    {
        //printf("Remove index is out of range");
        return false;
    }

    UISizerItemListIterator node = ItemIteratorAt(index);

    if (node == m_children.end())
    {
        //printf("Failed to find child node");
        return false;
    }

    delete *node;
    m_children.erase(node);

    return true;
}

bool UISizer::Detach( UISizer *sizer )
{
    if (NULL == sizer)
    {
        //printf("Detaching NULL sizer");
        return false;
    }

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem  *item = *node;

        if (item->GetSizer() == sizer)
        {
            item->DetachSizer();
            delete item;
            m_children.erase( node );
            return true;
        }
        ++node;
    }

    return false;
}

bool UISizer::Detach( UIWindow *window )
{
    if (NULL == window)
    {
        //printf("Detaching NULL window");
        return false;
    }

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetWindow() == window)
        {
            delete item;
            m_children.erase( node );
            return true;
        }
        ++node;
    }

    return false;
}

bool UISizer::Detach( int index )
{
    if (!(index >= 0 && (size_t)index < m_children.size()))
    {
        //printf("Detach index is out of range");
        return false;
    }

    UISizerItemListIterator node = ItemIteratorAt(index);
    if (node == m_children.end())
    {
        //printf("Failed to find child node");
        return false;
    }

    UISizerItem *item = *node;

    if ( item->IsSizer() )
        item->DetachSizer();

    delete item;
    m_children.erase( node );
    return true;
}

bool UISizer::Replace( UIWindow *oldwin, UIWindow *newwin, bool recursive )
{
    if (NULL == oldwin || NULL == newwin)
    {
        //printf("Replacing NULL window/Replacing with NULL window");
        return false;
    }

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetWindow() == oldwin)
        {
            item->AssignWindow(newwin);
            newwin->SetContainingSizer( this );
            return true;
        }
        else if (recursive && item->IsSizer())
        {
            if (item->GetSizer()->Replace( oldwin, newwin, true ))
                return true;
        }
        ++node;
    }

    return false;
}

bool UISizer::Replace( UISizer *oldsz, UISizer *newsz, bool recursive )
{
    if (NULL == oldsz|| NULL == newsz)
    {
        //printf("Replacing NULL sizer/Replacing with NULL sizer");
        return false;
    }

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetSizer() == oldsz)
        {
            item->AssignSizer(newsz);
            return true;
        }
        else if (recursive && item->IsSizer())
        {
            if (item->GetSizer()->Replace( oldsz, newsz, true ))
                return true;
        }

        ++node;
    }

    return false;
}

bool UISizer::Replace( size_t old, UISizerItem *newitem )
{
    if (old >= m_children.size())
    {
        //printf("Replace index is out of range");
        return false;
    }
    if (NULL == newitem)
    {
        //printf("Replacing with NULL item");
        return false;
    }

    UISizerItemListIterator node = ItemIteratorAt(old);
    if (node == m_children.end())
    {
        //printf("Failed to find child node");
        return false;
    }

    UISizerItem *item = *node;
    *node = newitem;

    if (item->IsWindow() && item->GetWindow())
        item->GetWindow()->SetContainingSizer(NULL);

    delete item;

    return true;
}

void UISizer::Clear( bool delete_windows )
{
    // First clear the ContainingSizer pointers
    for (UISizerItemListIterator node = m_children.begin();node != m_children.end();++node)
    {
        UISizerItem *item = *node;
        if (item->IsWindow())
            item->GetWindow()->SetContainingSizer( NULL );
    }

    // Destroy the windows if needed
    if (delete_windows)
        DeleteWindows();

    // Now empty the list
    UISizerItemListIterator it, en;
    for (it=m_children.begin(), en=m_children.end(); it!=en; ++it)
        delete *it;
    m_children.clear();
}

void UISizer::DeleteWindows()
{
    for (UISizerItemListIterator node = m_children.begin();node != m_children.end();++node)
    {
        (*node)->DeleteWindows();
    }
}

dkSize UISizer::ComputeFittingClientSize(UIWindow *window)
{
    if (NULL == window)
    {
        //printf("window can't be NULL");
        return dkSize();
    }

    // take the min size by default and limit it by max size
    dkSize size = GetMinClientSize(window);
    //commented by zhy
    //currently we support for two sizetypes:minSize,maxSize only
    dkSize sizeMax = GetMaxClientSize(window);

    if ( sizeMax.x != dkDefaultCoord && size.x > sizeMax.x )
            size.x = sizeMax.x;
    if ( sizeMax.y != dkDefaultCoord && size.y > sizeMax.y )
            size.y = sizeMax.y;

    return size;
}

dkSize UISizer::ComputeFittingWindowSize(UIWindow *window)
{
    if (NULL == window)
    {
        //printf("window can't be NULL");
        return dkDefaultSize;
    }

    return ComputeFittingClientSize(window);
    //return window->ClientToWindowSize(ComputeFittingClientSize(window));
}

dkSize UISizer::Fit( UIWindow *window )
{
    if (NULL == window)
    {
        //printf("window can't be NULL");
        return dkDefaultSize;
    }

    // set client size
    window->SetClientSize(ComputeFittingClientSize(window));

    // return entire size
    return window->GetSize();
}

void UISizer::Layout()
{
    // (re)calculates minimums needed for each item and other preparations
    // for layout
    CalcMin();

    // Applies the layout and repositions/resizes the items
    RecalcSizes();
}

void UISizer::SetSizeHints( UIWindow *window )
{
    // Preserve the window's max size hints, but set the
    // lower bound according to the sizer calculations.

    // This is equivalent to calling Fit(), except that we need to set
    // the size hints _in between_ the two steps performed by Fit
    // (1. ComputeFittingClientSize, 2. SetClientSize). That's because
    // otherwise SetClientSize() could have no effect if there already are
    // size hints in effect that forbid requested client size.

    const dkSize clientSize = ComputeFittingClientSize(window);

    /*
     * by zhy, we use SetSize(...)
     *window->SetMinClientSize(clientSize);
     *window->SetClientSize(clientSize);
     */
    window->SetSize(clientSize);
    DebugPrintf(DLC_ZHY, "%s::SetSize (%d, %d)", window->GetClassName(), clientSize.GetWidth(), clientSize.GetHeight());
}

void UISizer::SetDimension(const dkPoint& pos, const dkSize& size)
{
    DebugPrintf(DLC_GUI_VERBOSE,
            "UISizer::SetDimension(), (%d, %d)~(%d, %d),%s",
            pos.x, pos.y, size.x, size.y, GetName());

    m_position = pos;
    m_size = size;
    Layout();

    // This call is required for dkWrapSizer to be able to calculate its
    // minimal size correctly.
    InformFirstDirection(dkHORIZONTAL, size.x, size.y);
}
// TODO on mac we need a function that determines how much free space this
// min size contains, in order to make sure that we have 20 pixels of free
// space around the controls
dkSize UISizer::GetMaxClientSize( UIWindow *window ) const
{
    return window->GetMaxSize();
    /*
     *return window->WindowToClientSize(window->GetMaxSize());
     */
}

dkSize UISizer::GetMinClientSize( UIWindow *window )
{
    (void)(window);
    return GetMinSize();  // Already returns client size.
}

//dkSize UISizer::VirtualFitSize( UIWindow *window )
//{
//    dkSize size     = GetMinClientSize( window );
//    dkSize sizeMax  = GetMaxClientSize( window );
//
//    // Limit the size if sizeMax != dkDefaultSize
//
//    if ( size.x > sizeMax.x && sizeMax.x != dkDefaultCoord )
//        size.x = sizeMax.x;
//    if ( size.y > sizeMax.y && sizeMax.y != dkDefaultCoord )
//        size.y = sizeMax.y;
//
//    return size;
//}

dkSize UISizer::GetMinSize()
{
    dkSize ret( CalcMin() );
    if (ret.x < m_minSize.x) ret.x = m_minSize.x;
    if (ret.y < m_minSize.y) ret.y = m_minSize.y;
    if (m_minSize.x > 0) ret.x = m_minSize.x;
    if (m_minSize.y > 0) ret.y = m_minSize.y;
    return ret;
}

void UISizer::DoSetMinSize( int width, int height )
{
    m_minSize.x = width;
    m_minSize.y = height;
}

bool UISizer::DoSetItemMinSize( UIWindow *window, int width, int height )
{
    if (NULL == window)
    {
        //printf("SetMinSize for NULL window");
        return false;
    }

    // Is it our immediate child?

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetWindow() == window)
        {
            item->SetMinSize( width, height );
            return true;
        }
        ++node;
    }

    // No?  Search any subsizers we own then

    node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if ( item->GetSizer() &&
             item->GetSizer()->DoSetItemMinSize( window, width, height ) )
        {
            // A child sizer found the requested windw, exit.
            return true;
        }
        ++node;
    }

    return false;
}

bool UISizer::DoSetItemMinSize( UISizer *sizer, int width, int height )
{
    if (NULL == sizer)
    {
        //printf("SetMinSize for NULL sizer");
        return false;
    }

    // Is it our immediate child?

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetSizer() == sizer)
        {
            item->GetSizer()->DoSetMinSize( width, height );
            return true;
        }
        ++node;
    }

    // No?  Search any subsizers we own then

    node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if ( item->GetSizer() &&
             item->GetSizer()->DoSetItemMinSize( sizer, width, height ) )
        {
            // A child found the requested sizer, exit.
            return true;
        }
        ++node;
    }

    return false;
}

bool UISizer::DoSetItemMinSize( size_t index, int width, int height )
{
    UISizerItemListIterator node = ItemIteratorAt(index);
    if (node == m_children.end())
    {
        //printf("Failed to find child node");
        return false;
    }

    UISizerItem *item = *node;

    if (item->GetSizer())
    {
        // Sizers contains the minimal size in them, if not calculated ...
        item->GetSizer()->DoSetMinSize( width, height );
    }
    else
    {
        // ... but the minimal size of spacers and windows is stored via the item
        item->SetMinSize( width, height );
    }

    return true;
}

int UISizer::IndexOf(UIWindow* window) const
{
    if (NULL == window)
    {
        //printf("IndexOf for NULL window");
        return -1;
    }

    UISizerItemListConstIterator node = m_children.begin();
    int i = 0;
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetWindow() == window)
        {
            return i;
        }

        ++node;
        ++i;
    }

    return -1;
}

int UISizer::IndexOf(UISizer* sizer) const
{
    if (NULL == sizer)
    {
        //printf("IndexOf for NULL sizer");
        return -1;
    }

    UISizerItemListConstIterator node = m_children.begin();
    int i = 0;
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetSizer() == sizer)
        {
            return i;
        }

        ++node;
        ++i;
    }

    return -1;
}

int UISizer::IndexOf(UISizerItem* item) const
{
    if (item == NULL)
    {
        //printf("IndexOf for NULL item");
        return -1;
    }

    UISizerItemListConstIterator node = m_children.begin();
    int i = 0;
    while (node != m_children.end())
    {
        if (*node == item)
        {
            return i;
        }
        ++node;
        ++i;
    }

    return -1;
}

UISizerItem* UISizer::GetItem( UIWindow *window, bool recursive )
{
    if (NULL == window)
    {
        //printf("GetItem for NULL window");
        return NULL;
    }

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetWindow() == window)
        {
            return item;
        }
        else if (recursive && item->IsSizer())
        {
            UISizerItem *subitem = item->GetSizer()->GetItem( window, true );
            if (subitem)
                return subitem;
        }

        ++node;
    }

    return NULL;
}

UISizerItem* UISizer::GetItem( UISizer *sizer, bool recursive )
{
    if (NULL == sizer)
    {
        //printf("GetItem for NULL sizer");
        return NULL;
    }

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetSizer() == sizer)
        {
            return item;
        }
        else if (recursive && item->IsSizer())
        {
            UISizerItem *subitem = item->GetSizer()->GetItem( sizer, true );
            if (subitem)
                return subitem;
        }

        ++node;
    }

    return NULL;
}

UISizerItem* UISizer::GetItem( size_t index )
{
    if (index >= m_children.size())
    {
        //printf("GetItem index is out of range");
        return NULL;
    }

    return (*(ItemIteratorAt(index)));
}

UISizerItem* UISizer::GetItemById( int id, bool recursive )
{
    // This gets a sizer item by the id of the sizer item
    // and NOT the id of a window if the item is a window.
    UISizerItemListIterator node = m_children.begin();

    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetId() == id)
        {
            return item;
        }
        else if (recursive && item->IsSizer())
        {
            UISizerItem *subitem = item->GetSizer()->GetItemById( id, true );
            if (subitem)
                return subitem;
        }

        ++node;
    }

    return NULL;
}

bool UISizer::Show( UIWindow *window, bool show, bool recursive )
{
    UISizerItem *item = GetItem( window, recursive );

    if ( item )
    {
         item->Show( show );
         return true;
    }

    return false;
}

bool UISizer::Show( UISizer *sizer, bool show, bool recursive )
{
    UISizerItem *item = GetItem( sizer, recursive );

    if ( item )
    {
         item->Show( show );
         return true;
    }

    return false;
}

bool UISizer::Show( size_t index, bool show)
{
    UISizerItem *item = GetItem( index );

    if ( item )
    {
         item->Show( show );
         return true;
    }

    return false;
}

void UISizer::ShowItems( bool show )
{
    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        (*node)->Show( show );
        ++node;
    }
}

bool UISizer::IsShown( UIWindow *window ) const
{
    UISizerItemListConstIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetWindow() == window)
        {
            return item->IsShown();
        }
        ++node;
    }

    //printf("IsShown failed to find sizer item");

    return false;
}

bool UISizer::IsShown( UISizer *sizer ) const
{
    UISizerItemListConstIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem *item = *node;

        if (item->GetSizer() == sizer)
        {
            return item->IsShown();
        }
        ++node;
    }

    //printf("IsShown failed to find sizer item");

    return false;
}

bool UISizer::IsShown( size_t index ) const
{
    if (index >= m_children.size())
    {
        //printf("IsShown index is out of range");
        return false;
    }

    UISizerItemListConstIterator node = ItemIteratorAt(index);
    return (*node)->IsShown();
}

UISizerItemListConstIterator UISizer::ItemIteratorAt(size_t index) const
{
    UISizerItemListConstIterator node = m_children.begin();
    UISizerItemListConstIterator en = m_children.end();
    for (size_t i = 0;node != en&& i < index; ++i,++node)
        ;
    return node;
}

UISizerItemListIterator UISizer::ItemIteratorAt(size_t index)
{
    UISizerItemListIterator node = m_children.begin();
    UISizerItemListIterator en = m_children.end();
    for (size_t i = 0;node != en&& i < index; ++i,++node)
        ;
    return node;
}

//---------------------------------------------------------------------------
// UIBoxSizer
//---------------------------------------------------------------------------

UISizerItem *UIBoxSizer::AddSpacer(int size)
{
    return IsVertical() ? Add(0, size) : Add(size, 0);
}

namespace
{

/*
    Helper of RecalcSizes(): checks if there is enough remaining space for the
    min size of the given item and returns its min size or the entire remaining
    space depending on which one is greater.

    This function updates the remaining space parameter to account for the size
    effectively allocated to the item.
 */
int
GetMinOrRemainingSize(int orient, const UISizerItem *item, int *remainingSpace_)
{
    int& remainingSpace = *remainingSpace_;

    int size;
    if ( remainingSpace > 0 )
    {
        const dkSize sizeMin = item->GetMinSizeWithBorder();
        size = orient == dkHORIZONTAL ? sizeMin.x : sizeMin.y;

        if ( size >= remainingSpace )
        {
            // truncate the item to fit in the remaining space, this is better
            // than showing it only partially in general, even if both choices
            // are bad -- but there is nothing else we can do
            size = remainingSpace;
        }

        remainingSpace -= size;
    }
    else // no remaining space
    {
        // no space at all left, no need to even query the item for its min
        // size as we can't give it to it anyhow
        size = 0;
    }

    return size;
}

} // anonymous namespace

void UIBoxSizer::RecalcSizes()
{
    if ( m_children.empty() )
        return;

    const int totalMinorSize = GetSizeInMinorDir(m_size);
    const int totalMajorSize = GetSizeInMajorDir(m_size);

    // the amount of free space which we should redistribute among the
    // stretchable items (i.e. those with non zero proportion)
    int delta = totalMajorSize - GetSizeInMajorDir(m_minSize);

    // declare loop variables used below:
    UISizerItemListConstIterator i;  // iterator in m_children list
    unsigned n = 0;                     // item index in majorSizes array


    // First, inform item about the available size in minor direction as this
    // can change their size in the major direction. Also compute the number of
    // visible items and sum of their min sizes in major direction.

    int minMajorSize = 0;
    for ( i = m_children.begin(); i != m_children.end(); ++i )
    {
        UISizerItem * const item = *i;

        if ( !item->IsShown() )
            continue;

        dkSize szMinPrev = item->GetMinSizeWithBorder();
        item->InformFirstDirection(m_orient^dkBOTH,totalMinorSize,delta);
        dkSize szMin = item->GetMinSizeWithBorder();
        int deltaChange = GetSizeInMajorDir(szMin-szMinPrev);
        if( deltaChange )
        {
            // Since we passed available space along to the item, it should not
            // take too much, so delta should not become negative.
            delta -= deltaChange;
        }
        minMajorSize += GetSizeInMajorDir(item->GetMinSizeWithBorder());
    }

    // update our min size and delta which may have changed
    SizeInMajorDir(m_minSize) = minMajorSize;
    delta = totalMajorSize - minMajorSize;


    // space and sum of proportions for the remaining items, both may change
    // below
    int remaining = totalMajorSize;
    int totalProportion = m_totalProportion;

    // size of the (visible) items in major direction, -1 means "not fixed yet"
    std::vector<int> majorSizes(GetItemCount(), dkDefaultCoord);


    // Check for the degenerated case when we don't have enough space for even
    // the min sizes of all the items: in this case we really can't do much
    // more than to allocate the min size to as many of fixed size items as
    // possible (on the assumption that variable size items such as text zones
    // or list boxes may use scrollbars to show their content even if their
    // size is less than min size but that fixed size items such as buttons
    // will suffer even more if we don't give them their min size)
    if ( totalMajorSize < minMajorSize )
    {
        // Second degenerated case pass: allocate min size to all fixed size
        // items.
        for ( i = m_children.begin(), n = 0; i != m_children.end(); ++i, ++n )
        {
            UISizerItem * const item = *i;

            if ( !item->IsShown() )
                continue;

            if ( m_totalReduceFactor > 0)
            {
                // deal with fixed size items only during this pass
                if ( item->GetReduceFactor() )
                    continue;
            }
            else
            {
                if ( item->GetProportion() )
                    continue;
            }

            majorSizes[n] = GetMinOrRemainingSize(m_orient, item, &remaining);
        }


        // Third degenerated case pass: allocate min size to all the remaining,
        // i.e. non-fixed size, items.
        // rule changed:assign the remaining space by the reducefactor of the item averagely.
        int averageForRemaining = remaining/(double)m_totalReduceFactor;
        for ( i= m_children.begin(), n = 0; i != m_children.end(); ++i, ++n)
        {
            UISizerItem * const item = *i;
            if ( !item->IsShown() )
                continue;

            // we've already dealt with fixed size items above
            if (m_totalReduceFactor > 0)
            {
                if ( item->GetReduceFactor() )
                {
                    majorSizes[n] = averageForRemaining * item->GetReduceFactor(); 
                }
            }
            else
            {
                if ( !item->GetProportion() )
                    continue;

                majorSizes[n] = GetMinOrRemainingSize(m_orient, item, &remaining);
            }
        }
    }
    else // we do have enough space to give at least min sizes to all items
    {
        // Second and maybe more passes in the non-degenerated case: deal with
        // fixed size items and items whose min size is greater than what we
        // would allocate to them taking their proportion into account. For
        // both of them, we will just use their min size, but for the latter we
        // also need to reexamine all the items as the items which fitted
        // before we adjusted their size upwards might not fit any more. This
        // does make for a quadratic algorithm but it's not obvious how to
        // avoid it and hopefully it's not a huge problem in practice as the
        // sizers don't have many items usually (and, of course, the algorithm
        // still reduces into a linear one if there is enough space for all the
        // min sizes).
        bool nonFixedSpaceChanged = false;
        for ( i = m_children.begin(), n = 0; ; ++i, ++n )
        {
            if ( nonFixedSpaceChanged )
            {
                i = m_children.begin();
                n = 0;
                nonFixedSpaceChanged = false;
            }

            // check for the end of the loop only after the check above as
            // otherwise we wouldn't do another pass if the last child resulted
            // in non fixed space reduction
            if ( i == m_children.end() )
                break;

            UISizerItem * const item = *i;

            if ( !item->IsShown() )
                continue;

            // don't check the item which we had already dealt with during a
            // previous pass (this is more than an optimization, the code
            // wouldn't work correctly if we kept adjusting for the same item
            // over and over again)
            if ( majorSizes[n] != dkDefaultCoord )
                continue;

            int minMajor = GetSizeInMajorDir(item->GetMinSizeWithBorder());

            // it doesn't make sense for min size to be negative but right now
            // it's possible to create e.g. a spacer with (-1, 10) as size and
            // people do it in their code apparently (see #11842) so ensure
            // that we don't use this -1 as real min size as it conflicts with
            // the meaning we use for it here and negative min sizes just don't
            // make sense anyhow (which is why it might be a better idea to
            // deal with them at UISizerItem level in the future but for now
            // this is the minimal fix for the bug)
            if ( minMajor < 0 )
                minMajor = 0;

            const int propItem = item->GetProportion();
            if ( propItem )
            {
                // is the desired size of this item big enough?
                if ( (remaining*propItem)/totalProportion >= minMajor )
                {
                    // yes, it is, we'll determine the real size of this
                    // item later, for now just leave it as dkDefaultCoord
                    continue;
                }

                // the proportion of this item won't count, it has
                // effectively become fixed
                totalProportion -= propItem;
            }

            // we can already allocate space for this item
            majorSizes[n] = minMajor;

            // change the amount of the space remaining to the other items,
            // as this can result in not being able to satisfy their
            // proportions any more we will need to redo another loop
            // iteration
            remaining -= minMajor;

            nonFixedSpaceChanged = true;
        }


        // Last by one pass: distribute the remaining space among the non-fixed
        // items whose size weren't fixed yet according to their proportions.
        for ( i = m_children.begin(), n = 0; i != m_children.end(); ++i, ++n )
        {
            UISizerItem * const item = *i;

            if ( !item->IsShown() )
                continue;

            if ( majorSizes[n] == dkDefaultCoord )
            {
                const int propItem = item->GetProportion();
                majorSizes[n] = (remaining*propItem)/totalProportion;

                remaining -= majorSizes[n];
                totalProportion -= propItem;
            }
        }
    }


    // the position at which we put the next child
    dkPoint pt(m_position);


    // Final pass: finally do position the items correctly using their sizes as
    // determined above.
    for ( i = m_children.begin(), n = 0; i != m_children.end(); ++i, ++n )
    {
        UISizerItem * const item = *i;

        if ( !item->IsShown() )
            continue;

        const int majorSize = majorSizes[n];

        const dkSize sizeThis(item->GetMinSizeWithBorder());

        // apply the alignment in the minor direction
        dkPoint posChild(pt);

        int minorSize = GetSizeInMinorDir(sizeThis);
        const int flag = item->GetFlag();
        if ( (flag & (dkEXPAND | dkSHAPED)) || (minorSize > totalMinorSize) )
        {
            // occupy all the available space if dkEXPAND was given and also if
            // the item is too big to fit -- in this case we truncate it below
            // its minimal size which is bad but better than not showing parts
            // of the window at all
            minorSize = totalMinorSize;
        }
        else if ( flag & (IsVertical() ? dkALIGN_RIGHT : dkALIGN_BOTTOM) )
        {
            PosInMinorDir(posChild) += totalMinorSize - minorSize;
        }
        // NB: dkCENTRE is used here only for backwards compatibility,
        //     dkALIGN_CENTRE should be used in new code
        else if ( flag & (dkCENTER | (IsVertical() ? dkALIGN_CENTRE_HORIZONTAL
                                                   : dkALIGN_CENTRE_VERTICAL)) )
        {
            PosInMinorDir(posChild) += (totalMinorSize - minorSize) / 2;
            //PosInMinorDir(posChild) += (totalMinorSize - minorSize + 1) / 2;
        }


        // apply RTL adjustment for horizontal sizers:
        // TODO by zhy RTL support
        /*
         *if ( !IsVertical() && m_containingWindow )
         *{
         *    posChild.x = m_containingWindow->AdjustForLayoutDirection
         *                                     (
         *                                        posChild.x,
         *                                        majorSize,
         *                                        m_size.x
         *                                     );
         *}
         */

        // finally set size of this child and advance to the next one
        item->SetDimension(posChild, SizeFromMajorMinor(majorSize, minorSize));

        PosInMajorDir(pt) += majorSize;
    }
}

dkSize UIBoxSizer::CalcMin()
{
    m_totalProportion = 0;
    m_totalReduceFactor = 0;
    m_minSize = dkSize(0, 0);

    // The minimal size for the sizer should be big enough to allocate its
    // element at least its minimal size but also, and this is the non trivial
    // part, to respect the children proportion. To satisfy the latter
    // condition we must find the greatest min-size-to-proportion ratio for all
    // elements with non-zero proportion.
    float maxMinSizeToProp = 0.;
    for (UISizerItemList::const_iterator i = m_children.begin();i != m_children.end();++i)
    {
        UISizerItem * const item = *i;

        if ( !item->IsShown() )
            continue;

        const dkSize sizeMinThis = item->CalcMin();
        if ( const int propThis = item->GetProportion() )
        {
            float minSizeToProp = GetSizeInMajorDir(sizeMinThis);
            minSizeToProp /= propThis;

            if ( minSizeToProp > maxMinSizeToProp )
                maxMinSizeToProp = minSizeToProp;

            m_totalProportion += item->GetProportion();
        }
        else // fixed size item
        {
            // Just account for its size directly
            SizeInMajorDir(m_minSize) += GetSizeInMajorDir(sizeMinThis);
        }
        if (item->GetReduceFactor())
        {
            m_totalReduceFactor += item->GetReduceFactor();
        }

        // In the transversal direction we just need to find the maximum.
        if ( GetSizeInMinorDir(sizeMinThis) > GetSizeInMinorDir(m_minSize) )
            SizeInMinorDir(m_minSize) = GetSizeInMinorDir(sizeMinThis);
    }

    // Using the max ratio ensures that the min size is big enough for all
    // items to have their min size and satisfy the proportions among them.
    SizeInMajorDir(m_minSize) += (int)(maxMinSizeToProp*m_totalProportion);

    return m_minSize;
}

//---------------------------------------------------------------------------
// UIGridSizer
//---------------------------------------------------------------------------

UIGridSizer::UIGridSizer( int cols, int vgap, int hgap )
    : m_rows( cols == 0 ? 1 : 0 ),
      m_cols( cols ),
      m_vgap( vgap ),
      m_hgap( hgap )
{
    assert(cols >= 0);
}

UIGridSizer::UIGridSizer( int cols, const dkSize& gap )
    : m_rows( cols == 0 ? 1 : 0 ),
      m_cols( cols ),
      m_vgap( gap.GetHeight() ),
      m_hgap( gap.GetWidth() )
{
    assert(cols >= 0);
}

UIGridSizer::UIGridSizer( int rows, int cols, int vgap, int hgap )
    : m_rows( rows || cols ? rows : 1 ),
      m_cols( cols ),
      m_vgap( vgap ),
      m_hgap( hgap )
{
    assert(rows >= 0 && cols >= 0);
}

UIGridSizer::UIGridSizer( int rows, int cols, const dkSize& gap )
    : m_rows( rows || cols ? rows : 1 ),
      m_cols( cols ),
      m_vgap( gap.GetHeight() ),
      m_hgap( gap.GetWidth() )
{
    assert(rows >= 0 && cols >= 0);
}

UISizerItem *UIGridSizer::DoInsert(size_t index, UISizerItem *item)
{
    // if only the number of columns or the number of rows is specified for a
    // sizer, arbitrarily many items can be added to it but if both of them are
    // fixed, then the sizer can't have more than that many items -- check for
    // this here to ensure that we detect errors as soon as possible
    if ( m_cols && m_rows )
    {
        const int nitems = m_children.size();
        if ( nitems == m_cols*m_rows )
        {
            printf( "too many items (%d > %d*%d) in grid sizer (maybe you "
                    "should omit the number of either rows or columns?)",
                    nitems + 1, m_cols, m_rows);

            // additionally, continuing to use the specified number of columns
            // and rows is not a good idea as callers of CalcRowsCols() expect
            // that all sizer items can fit into m_cols-/m_rows-sized arrays
            // which is not the case if there are too many items and results in
            // crashes, so let it compute the number of rows automatically by
            // forgetting the (wrong) number of rows specified (this also has a
            // nice side effect of giving only one assert even if there are
            // many more items than allowed in this sizer)
            m_rows = 0;
        }
    }

    return UISizer::DoInsert(index, item);
}

int UIGridSizer::CalcRowsCols(int& nrows, int& ncols) const
{
    const int nitems = m_children.size();

    ncols = GetEffectiveColsCount();
    nrows = GetEffectiveRowsCount();

    // Since Insert() checks for overpopulation, the following
    // should only assert if the grid was shrunk via SetRows() / SetCols()
    if (nitems > ncols*nrows)
    {
        //printf("logic error in UIGridSizer\n");
    }

    return nitems;
}

void UIGridSizer::RecalcSizes()
{
    int nitems, nrows, ncols;
    if ( (nitems = CalcRowsCols(nrows, ncols)) == 0 )
        return;

    dkSize sz( GetSize() );
    dkPoint pt( GetPosition() );

    int w = (sz.x - (ncols - 1) * m_hgap) / ncols;
    int h = (sz.y - (nrows - 1) * m_vgap) / nrows;

    int x = pt.x;
    for (int c = 0; c < ncols; c++)
    {
        int y = pt.y;
        for (int r = 0; r < nrows; r++)
        {
            int i = r * ncols + c;
            if (i < nitems)
            {
                UISizerItemListIterator node = ItemIteratorAt(i);
                if (node == m_children.end())
                {
                    //printf("Failed to find SizerItemList node");
                    return;
                }

                SetItemBounds( *node, x, y, w, h);
            }
            y = y + h + m_vgap;
        }
        x = x + w + m_hgap;
    }
}

dkSize UIGridSizer::CalcMin()
{
    int nrows, ncols;
    if ( CalcRowsCols(nrows, ncols) == 0 )
        return dkSize();

    // Find the max width and height for any component
    int w = 0;
    int h = 0;

    UISizerItemListIterator node = m_children.begin();
    while (node != m_children.end())
    {
        UISizerItem     *item = *node;
        dkSize           sz( item->CalcMin() );

        w = w > sz.x ? w : sz.x;
        h = h > sz.y ? h : sz.y;

        node++;
    }

    // In case we have a nested sizer with a two step algo , give it
    // a chance to adjust to that (we give it width component)
    node = m_children.begin();
    bool didChangeMinSize = false;
    while (node != m_children.end())
    {
        UISizerItem     *item = *node;
        didChangeMinSize |= item->InformFirstDirection( dkHORIZONTAL, w, -1 );

        node++;
    }

    // And redo iteration in case min size changed
    if( didChangeMinSize )
    {
        node = m_children.begin();
        w = h = 0;
        while (node != m_children.end())
        {
            UISizerItem     *item = *node;
            dkSize           sz( item->GetMinSizeWithBorder() );

            w = w > sz.x ? w : sz.x;
            h = h > sz.y ? h : sz.y;

            node++;
        }
    }

    return dkSize( ncols * w + (ncols-1) * m_hgap,
                   nrows * h + (nrows-1) * m_vgap );
}

void UIGridSizer::SetItemBounds( UISizerItem *item, int x, int y, int w, int h )
{
    dkPoint pt( x,y );
    dkSize sz( item->GetMinSizeWithBorder() );
    int flag = item->GetFlag();

    if ((flag & dkEXPAND) || (flag & dkSHAPED))
    {
       sz = dkSize(w, h);
    }
    else
    {
        if (flag & dkALIGN_CENTER_HORIZONTAL)
        {
            pt.x = x + (w - sz.x) / 2;
        }
        else if (flag & dkALIGN_RIGHT)
        {
            pt.x = x + (w - sz.x);
        }

        if (flag & dkALIGN_CENTER_VERTICAL)
        {
            pt.y = y + (h - sz.y) / 2;
        }
        else if (flag & dkALIGN_BOTTOM)
        {
            pt.y = y + (h - sz.y);
        }
    }

    item->SetDimension(pt, sz);
}
