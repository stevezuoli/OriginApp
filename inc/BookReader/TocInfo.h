//===========================================================================
// Summary:
//        目录结构定义
// Version:
//    0.1 draft - creating.
// Usage:
//        Null
// Remarks:
//        Null
// Date:
//        2011-12-25 从 IBookReader 分离
// Author:
//        Chen Mao (chenm@duokan.com)
//===========================================================================

#ifndef __KINDLEAPP_INC_TOCINFO_H__
#define __KINDLEAPP_INC_TOCINFO_H__

#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include <vector>

class DK_TOCINFO
{
public:
    enum ExpandStatus
    {
        ES_PLUS,
        ES_MINUS,
        ES_NONE
    };
    DK_TOCINFO();
    ~DK_TOCINFO();

    bool SetCurTOC(const string& chapterTitle, const DK_FLOWPOSITION& chapterPosition);
    bool SetBeginPos(const DK_FLOWPOSITION& beginPos);
    bool SetBeginPos(const CT_RefPos& beginPos);
    bool SetChapter(const string& chapterTitle);
    void SetAnchor(const wchar_t* pAnchor);
    void SetDepth(int iDepth);

    void SetValid(bool isvalid);
    bool IsValid() const;
    
    string GetChapter() const;
    DK_FLOWPOSITION GetBeginPos() const;
    CT_RefPos GetBeginRefPos() const;

    bool IsAnchorAnalysised();
    void SetAnchorAnalysised(bool analysised);
    
    bool SetCurTOC(const ICT_ReadingDataItem& tocItem);
    const ICT_ReadingDataItem* GetCurrentTOC() const;
    std::wstring GetAnchor() const;
    bool HasAnchor() const;
    
    int GetDepth() const;
    void SetExpanded(bool expanded);
    bool IsExpanded() const;

    void SetHasChild(bool hasChild);
    bool HasChild() const;

    bool IsVisible() const;
    void SetAsCurrentToc();

    static void InitTocListExpandStatus(std::vector<DK_TOCINFO*>* pTocList);
    static size_t GetVisibleItemCount(const std::vector<DK_TOCINFO*>* pTocList);
    static DK_TOCINFO* GetVisibleItemByIndex(std::vector<DK_TOCINFO*>* pTocList, size_t index);
    static int GetVisibleIndexOfItem(std::vector<DK_TOCINFO*>* pTocList, const DK_TOCINFO* toc);
    ExpandStatus GetExpandStatus() const;
    void ShiftExpandStatus();

private:
    void SetParent(DK_TOCINFO* tocInfo);
    DK_TOCINFO* GetParent() const;
    ICT_ReadingDataItem*     m_pCurTOC;
    CT_RefPos                m_clsPos;
    bool                     m_bValid;           //目录项是否有效，部分书籍试读本章节目录存在，但点击无法跳转到正文，为false，否则为true
    std::wstring             m_wstrAnchor;       //文内跳转的锚点，现在仅epub 支持
    bool                     m_anchorAnalysised; //标识epub 中的anchor 是否解析过
    int                      m_iDepth;           //目录为第几级
    bool m_hasChild;
    bool m_expanded;
    DK_TOCINFO* m_parent;
};

#endif // __KINDLEAPP_INC_TOCINFO_H__
