#include "BookReader/TocInfo.h"
#include "interface.h"

DK_TOCINFO::DK_TOCINFO()
    : m_pCurTOC(NULL)
	, m_bValid(true)
	, m_wstrAnchor(L"")
	, m_anchorAnalysised(false)
	, m_iDepth(0)
    , m_hasChild(false)
    , m_expanded(true)
    , m_parent(NULL)
{
    m_pCurTOC = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::BOOKMARK);
    if(!m_pCurTOC)
    {
        return;
    }
}

DK_TOCINFO::~DK_TOCINFO()
{
    SafeDeletePointerEx(m_pCurTOC);
}

bool DK_TOCINFO::SetCurTOC(const string& chapterTitle, const DK_FLOWPOSITION& chapterPosition)
{
    if(chapterTitle.empty())
    {
        return false;
    }
    
    return SetChapter(chapterTitle) && SetBeginPos(chapterPosition);
}

bool DK_TOCINFO::SetCurTOC(const ICT_ReadingDataItem& tocItem)
{
    if(!m_pCurTOC)
    {
        return false ;
    }
    
    return SetChapter(tocItem.GetBookContent()) && SetBeginPos(tocItem.GetBeginPos());
}


void DK_TOCINFO::SetValid(bool isvalid)
{
	m_bValid = isvalid;
}

bool DK_TOCINFO::IsValid() const
{
	return m_bValid;
}

bool DK_TOCINFO::SetBeginPos(const DK_FLOWPOSITION& beginPos)
{
    if(!m_pCurTOC)
    {
        return false ;
    }
    m_clsPos.SetChapterIndex(beginPos.nChapterIndex);
    m_clsPos.SetParaIndex(beginPos.nParaIndex);
    m_clsPos.SetAtomIndex(beginPos.nElemIndex);
    return m_pCurTOC->SetBeginPos(m_clsPos);
}

bool DK_TOCINFO::SetBeginPos(const CT_RefPos& beginPos)
{
    if(!m_pCurTOC)
    {
        return false ;
    }
    return m_pCurTOC->SetBeginPos(beginPos);
}

DK_FLOWPOSITION DK_TOCINFO::GetBeginPos() const
{
	return m_clsPos.ToFlowPosition();
}

bool DK_TOCINFO::SetChapter(const string& chapterTitle)
{
    if(!m_pCurTOC || chapterTitle.empty())
    {
        return false;
    }
    return m_pCurTOC->SetBookContent(chapterTitle);
}

void DK_TOCINFO::SetAnchor(const wchar_t* pAnchor)
{
	if(!m_pCurTOC || !pAnchor)
	{
		return;
	}
	m_wstrAnchor = pAnchor;
	return;
}

void DK_TOCINFO::SetDepth(int iDepth)
{
	m_iDepth = iDepth;
}

int DK_TOCINFO::GetDepth() const
{
	return m_iDepth;
}

bool DK_TOCINFO::HasAnchor() const
{
	return m_pCurTOC && !m_wstrAnchor.empty();
}

std::wstring DK_TOCINFO::GetAnchor() const
{
	if(!m_pCurTOC)
    {
        return L"";
    }
    return m_wstrAnchor;
}

bool DK_TOCINFO::IsAnchorAnalysised()
{
	return m_anchorAnalysised;
}

void DK_TOCINFO::SetAnchorAnalysised(bool analysised)
{
	m_anchorAnalysised = analysised;
}

string DK_TOCINFO::GetChapter() const
{
    if(!m_pCurTOC)
    {
        return "";
    }
    return m_pCurTOC->GetBookContent();
}

const ICT_ReadingDataItem* DK_TOCINFO::GetCurrentTOC() const
{
    if(!m_pCurTOC)
    {
        return NULL;
    }
    return m_pCurTOC;
}

void DK_TOCINFO::SetExpanded(bool expanded)
{
    m_expanded = expanded;    
}

bool DK_TOCINFO::IsExpanded() const
{
    return m_expanded;
}

void DK_TOCINFO::SetHasChild(bool hasChild)
{
    m_hasChild = hasChild;
}

bool DK_TOCINFO::HasChild() const
{
    return m_hasChild;
}

void DK_TOCINFO::SetParent(DK_TOCINFO* parent)
{
    m_parent = parent;
    if (NULL != m_parent)
    {
        m_parent->SetHasChild(true);
    }
}

DK_TOCINFO* DK_TOCINFO::GetParent() const
{
    return m_parent;
}

bool DK_TOCINFO::IsVisible() const
{
    if (NULL == GetParent() || 
            (GetParent()->IsVisible() && GetParent()->IsExpanded()))
    {
        return true;
    }
    return false;
}

void DK_TOCINFO::SetAsCurrentToc()
{
    DK_TOCINFO* parent = GetParent();
    while (NULL != parent)
    {
        parent->SetExpanded(true);
        parent = parent->GetParent();
    }
}

static const int MAX_EXPAND_LEVEL = 2;

DK_TOCINFO::ExpandStatus DK_TOCINFO::GetExpandStatus() const
{
    if (GetDepth() > MAX_EXPAND_LEVEL || !HasChild())
    {
        return ES_NONE;
    }
    return IsExpanded() ? ES_MINUS : ES_PLUS;
}

void DK_TOCINFO::InitTocListExpandStatus(std::vector<DK_TOCINFO*>* pTocList)
{
    if(NULL == pTocList)
    {
        return;
    }
    std::vector<DK_TOCINFO*>& tocList = *pTocList;
    for (size_t i = 0; i < tocList.size(); ++i)
    {
        tocList[i]->SetExpanded(true);
        if (i > 0 && tocList[i]->GetDepth() > tocList[i-1]->GetDepth())
        {
            tocList[i]->SetParent(tocList[i-1]);

            if (tocList[i-1]->GetDepth() == MAX_EXPAND_LEVEL)
            {
                tocList[i-1]->SetExpanded(false);
            }
        }
        else if (i > 0 && tocList[i]->GetDepth() > 1)
        {
            DK_TOCINFO* parent = tocList[i-1];
            while (parent && parent->GetDepth() >= tocList[i]->GetDepth())
            {
                parent = parent->GetParent();
            }
            tocList[i]->SetParent(parent);
        }
    }
}

size_t DK_TOCINFO::GetVisibleItemCount(const std::vector<DK_TOCINFO*>* pTocList)
{
    if(NULL == pTocList)
    {
        return 0;
    }
    const std::vector<DK_TOCINFO*>& tocList = *pTocList;
    size_t count = 0;
    for (size_t i = 0; i < tocList.size(); ++i)
    {
        if (tocList[i]->IsVisible())
        {
            ++count;
        }
    }
    return count;
}

DK_TOCINFO* DK_TOCINFO::GetVisibleItemByIndex(
        std::vector<DK_TOCINFO*>* pTocList,
        size_t index)
{
    if (NULL == pTocList || index >= pTocList->size())
    {
        return NULL;
    }

    std::vector<DK_TOCINFO*>& tocList = *pTocList;
    int visibleIndex = -1;
    for (size_t i = 0; i < tocList.size(); ++i)
    {
        if (tocList[i]->IsVisible())
        {
            ++visibleIndex;
        }
        if (visibleIndex == (int)index)
        {
            return tocList[i];
        }
    }
    return NULL;
}

int DK_TOCINFO::GetVisibleIndexOfItem(
        std::vector<DK_TOCINFO*>* pTocList,
        const DK_TOCINFO* toc)
{
    if (NULL == pTocList || NULL == toc)
    {
        return -1;
    }

    std::vector<DK_TOCINFO*>& tocList = *pTocList;
    int visibleIndex = -1;
    for (size_t i = 0; i < tocList.size(); ++i)
    {
        if (tocList[i]->IsVisible())
        {
            ++visibleIndex;
        }
        if (tocList[i] == toc)
        {
            return visibleIndex;
        }
    }
    return -1;
}

void DK_TOCINFO::ShiftExpandStatus()
{
    m_expanded = !m_expanded;
}

CT_RefPos DK_TOCINFO::GetBeginRefPos() const
{
    return m_clsPos;
}
