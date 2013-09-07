//===========================================================================
//  Summary:
//      对IDK?Page相似接口的接管
//  Version:
//      
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2011-12-15
//  Author:
//      Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KINDLEAPP_INC_BOOKREADER_IDKPAGEADAPTER_H__
#define __KINDLEAPP_INC_BOOKREADER_IDKPAGEADAPTER_H__

#include "IDKEBook.h"
#include "IDKEPage.h"
#include "IDKETextIterator.h"

#include "IDKMBook.h"
#include "IDKMPage.h"
#include "IDKMTextIterator.h"

#include "IDKTBook.h"
#include "IDKTPage.h"
#include "IDKTTextIterator.h"

#include "IDKPPageEx.h"
#include "IDKPTextIterator.h"
#include "BookReader/IPDFKernelHandle.h"
#include "BookReader/PDFTextIterator.h"

struct DK_CHAR_INFO
{
    DK_CHAR_INFO()
        : objType(DKE_PAGEOBJ_TEXT)
    {
        bounding.left = bounding.top = bounding.right = bounding.bottom = 0;
    };

    DK_RECT bounding;
    DK_FLOWPOSITION pos;
    DK_WCHAR charCode;
    DKE_PAGEOBJ_TYPE objType;

    // ePub
    DK_CHAR_INFO & operator = (const DKE_CHAR_INFO &rhs)
    {
        DK_BOX boundingBox = rhs.boundingBox;
        bounding.left = int(boundingBox.X0 + 0.5);
        bounding.top = int(boundingBox.Y0 + 0.5);
        bounding.right = int(boundingBox.X1 + 0.5);
        bounding.bottom = int(boundingBox.Y1 + 0.5);
        pos = rhs.pos;
        charCode = rhs.charCode;
        return *this;
    }

    // Txt
    DK_CHAR_INFO & operator = (const DKT_CHAR_INFO &rhs)
    {
        DK_BOX boundingBox = rhs.boundingBox;
        bounding.left = int(boundingBox.X0 + 0.5);
        bounding.top = int(boundingBox.Y0 + 0.5);
        bounding.right = int(boundingBox.X1 + 0.5);
        bounding.bottom = int(boundingBox.Y1 + 0.5);
        pos.nChapterIndex = 0;
        pos.nParaIndex = 0;
        pos.nElemIndex = rhs.offsetInBytes;
        charCode = rhs.ch;
        return *this;
    }

    // Pdf
    DK_CHAR_INFO & operator = (const DKP_CHAR_INFO &rhs)
    {
        DK_BOX boundingBox = rhs.boundingBox;
        bounding.left = int(boundingBox.X0 + 0.5);
        bounding.top = int(boundingBox.Y0 + 0.5);
        bounding.right = int(boundingBox.X1 + 0.5);
        bounding.bottom = int(boundingBox.Y1 + 0.5);
        pos = rhs.pos;
        --(pos.nChapterIndex);
        charCode = rhs.charCode;
        return *this;
    }

    // Mobi
    DK_CHAR_INFO & operator = (const DKM_CHAR_INFO &rhs)
    {
        DK_BOX boundingBox = rhs.boundingBox;
        bounding.left = int(boundingBox.X0 + 0.5);
        bounding.top = int(boundingBox.Y0 + 0.5);
        bounding.right = int(boundingBox.X1 + 0.5);
        bounding.bottom = int(boundingBox.Y1 + 0.5);
        pos.nChapterIndex = 0;
        pos.nParaIndex = 0;
        pos.nElemIndex = rhs.offset;
        charCode = rhs.charCode;
        return *this;
    }
};

class IDKTextIterator
{
public:
    virtual ~IDKTextIterator() {}
public:
    virtual DK_BOOL      MoveToNext() = 0;
    virtual DK_BOOL      MoveToPrev() = 0;
    virtual DK_BOOL      MoveToPrevAdjacentLine() = 0;
    virtual DK_BOOL      MoveToNextAdjacentLine() = 0;
    virtual DK_CHAR_INFO GetCurrentCharInfo() = 0;
};

template <typename TextIteratorType, typename CharInfoType>
class DKTextIterator: public IDKTextIterator
{
public:
    DKTextIterator(TextIteratorType *pTextIterator)
    {
        m_pTextIterator = pTextIterator;
        m_isFirstPos = DK_TRUE;
    }

    virtual ~DKTextIterator() {}

public:
    virtual DK_BOOL MoveToNext()
    {
        return m_pTextIterator->MoveToNext();
    }

    virtual DK_BOOL MoveToPrev()
    {
        return m_pTextIterator->MoveToPrev();
    }

    virtual DK_BOOL MoveToPrevAdjacentLine()
    {
        return m_pTextIterator->MoveToPrevAdjacentLine();
    }

    virtual DK_BOOL MoveToNextAdjacentLine()
    {
        return m_pTextIterator->MoveToNextAdjacentLine();
    }

    virtual DK_CHAR_INFO GetCurrentCharInfo()
    {
        if (m_isFirstPos)
        {
            m_isFirstPos = DK_FALSE;
            m_pTextIterator->MoveToNext();
        }

        CharInfoType charInfo;
        m_pTextIterator->GetCurrentCharInfo(&charInfo);
        DK_CHAR_INFO ret;
        ret = charInfo;
        return ret;
    }
private:
    TextIteratorType * m_pTextIterator;
    DK_BOOL            m_isFirstPos;
};

class IDKPageAdapter
{
public:
    virtual ~IDKPageAdapter() {}
public:
    virtual IDKTextIterator * GetTextIterator() = 0;
    virtual DK_VOID           FreeTextIterator(IDKTextIterator *pDKTextIterator) = 0;
public:
    virtual DK_FLOWPOSITION GetBeginPosition() = 0;
    virtual DK_FLOWPOSITION GetEndPosition() = 0;
    virtual DK_WCHAR *      GetTextContentOfRange(const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos) = 0;
    virtual DK_VOID         FreeTextContent(DK_WCHAR *pContent) = 0;
    virtual DKEWRITINGOPT GetBookWritingOpt() = 0;
    virtual DK_BOOL         GetTextRects(const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos, DK_BOX **ppTextRects, DK_UINT *pRectCount) = 0;
    virtual DK_VOID         FreeRects(DK_BOX *pRects) = 0;
    virtual DK_BOOL HitTestTextRange(const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;
    virtual DK_BOOL HitTestTextRangeByMode(const DK_POS& point, int hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)= 0;
    virtual DK_BOOL GetSelectionRange(const DK_POS& startPoint, const DK_POS& endPoint, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;
    virtual DK_BOOL GetSelectionRangeByMode(const DK_POS& startPoint, const DK_POS& endPoint, int selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;
    virtual DK_BOOL         HitTestObject(const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo) = 0;
    virtual DK_VOID         FreeHitTestObject(DKE_HITTEST_OBJECTINFO* pObjInfo) = 0;

    virtual DK_BOOL         HitTestFootnote(const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo) = 0;
    virtual DK_VOID         FreeHitTestFootnote(DKE_FOOTNOTE_INFO* pFootnoteInfo) = 0;
    virtual DK_BOOL         GetFootnoteContentHandle(const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle) = 0;
    virtual DK_VOID         FreeContentHandle(DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle) = 0;

    virtual DK_BOOL         HitTestLink(const DK_POS& point, DKE_LINK_INFO* pLinkInfo) = 0;
    virtual DK_VOID         FreeHitTestLink(DKE_LINK_INFO* pLinkInfo) = 0;
    virtual DK_BOOL         GetAnchorPosition(DK_LONG chapterIndex, const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pPos) = 0;
    virtual DK_INT          GetChapterIndexByURL(const DK_WCHAR* pChapterURL) const = 0;

    virtual IDKEFootnoteIterator* GetFootnoteIterator() = 0;
    virtual DK_VOID FreeFootnoteIterator(IDKEFootnoteIterator* pFootnoteIterator) = 0;

    virtual IDKEImageIterator* GetImageIterator() = 0;
    virtual DK_VOID FreeImageIterator(IDKEImageIterator* pImageIterator) = 0;

    virtual DK_LONG HitTestComicsFrame(const DK_POS& point) = 0;
    virtual IDKEComicsFrameIterator* GetComicsFrameIterator() = 0;
    virtual DK_VOID FreeComicsFrameIterator(IDKEComicsFrameIterator* pFrameIterator) = 0;

    virtual IDKEGalleryIterator* GetGalleryIterator() = 0;
    virtual DK_VOID FreeGalleryIterator(IDKEGalleryIterator* pGalleryIterator) = 0;
    virtual DK_BOOL HitTestGallery(const DK_POS& point, IDKEGallery** ppGallery) = 0;
    virtual DK_VOID FreeHitTestGallery(IDKEGallery* pGallery) = 0;

    virtual IDKEPreBlockIterator* GetPreBlockIterator() = 0;
    virtual DK_VOID FreePreBlockIterator(IDKEPreBlockIterator* pPreBlockIterator) = 0;
    virtual DK_BOOL HitTestPreBlock(const DK_POS& point, DKE_PREBLOCK_INFO* pPreBlockInfo) = 0;
    virtual DK_VOID FreeHitTestPreBlock(DKE_PREBLOCK_INFO* pPreBlockInfo) = 0;

    virtual IDKEHitableObjIterator* GetHitableObjIterator() = 0;
    virtual DK_VOID FreeHitableObjIterator(IDKEHitableObjIterator* pHitableObjIterator) = 0;
};

template <typename Book, typename Page, typename TextIterator, typename CharInfo>
struct IDKPageTraitsBase
{
    typedef Book         BookType;
    typedef Page         PageType;
    typedef TextIterator TextIteratorType;
    typedef CharInfo     CharInfoType;
};

struct IDKEPageTraits: public IDKPageTraitsBase<IDKEBook, IDKEPage, IDKETextIterator, DKE_CHAR_INFO>
{
    static DK_FLOWPOSITION GetBeginPosition(PageType *pPage)
    {
        return pPage->GetBeginPosition();
    }

    static DK_FLOWPOSITION GetEndPosition(PageType *pPage)
    {
        return pPage->GetEndPosition();
    }

    static DK_WCHAR * GetTextContentOfRange(BookType *pBook, PageType * /*pPage*/, 
        const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos)
    {
        if (pBook)
        {
            DK_WCHAR *pText = DK_NULL;
            pBook->GetChapterTextOfRange(startPos, endPos, &pText);
            return pText;
        }
        return NULL;
    }

    static DK_VOID FreeTextContent(BookType *pBook, PageType * /*pPage*/, DK_WCHAR *pContent)
    {
        pBook->FreeChapterText(pContent);
    }

    static DKEWRITINGOPT GetBookWritingOpt(BookType *pBook)
    {
        if (pBook)
        {
            return pBook->GetBookWritingOpt();
        }
        return DKEWRITINGOPT();
    }

    static DK_BOOL GetTextRects(PageType *pPage, const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos, 
        DK_BOX **ppTextRects, DK_UINT *pRectCount)
    {
        return DKR_OK == pPage->GetTextRects(startPos, endPos, ppTextRects, pRectCount);
    }

    static DK_VOID FreeRects(PageType *pPage, DK_BOX *pRects)
    {
        pPage->FreeRects(pRects);
    }

    static DK_BOOL HitTestTextRange(PageType *pPage, const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return DKR_OK == (pPage->HitTestTextRange(point, pStartPos, pEndPos));
    }

    static DK_BOOL HitTestTextRangeByMode(PageType *pPage, const DK_POS& point, int hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && DK_SUCCEEDED(pPage->HitTestTextRangeByMode(point, (DKE_HITTEST_TEXT_MODE)hitTestTextMode, pStartPos, pEndPos));
    }

    static DK_BOOL GetSelectionRange(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && pStartPos && pEndPos && DK_SUCCEEDED(pPage->GetSelectionRange(startPoint, endPoint, pStartPos, pEndPos));
    }

    static DK_BOOL GetSelectionRangeByMode(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, int selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && pStartPos && pEndPos 
            && DK_SUCCEEDED(pPage->GetSelectionRangeByMode(startPoint, endPoint, (DKE_SELECTION_MODE)selectionMode, pStartPos, pEndPos));
    }

    static DK_BOOL HitTestObject(PageType *pPage, const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return pPage && DK_SUCCEEDED(pPage->HitTestObject(point, pObjInfo));
    }

    static DK_VOID FreeHitTestObject(PageType *pPage, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        if(pPage)
        {
            pPage->FreeHitTestObject(pObjInfo);
        }
    }

    static DK_BOOL HitTestFootnote(PageType *pPage, const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return pPage && DK_SUCCEEDED(pPage->HitTestFootnote(point, pFootnoteInfo));
    }

    static DK_VOID FreeHitTestFootnote(PageType *pPage, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        if(pPage)
        {
            pPage->FreeHitTestFootnote(pFootnoteInfo);
        }
    }

    static DK_BOOL GetFootnoteContentHandle(PageType *pPage, const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return pPage && pPage-> GetFootnoteContentHandle(pNoteId, showType, pHandle);
    }

    static DK_VOID FreeContentHandle(PageType *pPage, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        if(pPage)
        {
            pPage->FreeContentHandle(showType, pHandle);
        }
    }

    static DK_BOOL HitTestLink(PageType *pPage, const DK_POS& point, DKE_LINK_INFO* pLinkInfo)
    {
        return pPage && DK_SUCCEEDED(pPage->HitTestLink(point, pLinkInfo));
    }

    static DK_VOID FreeHitTestLink(PageType *pPage, DKE_LINK_INFO* pLinkInfo)
    {
        if(pPage)
        {
            pPage->FreeHitTestLink(pLinkInfo);
        }
    }

    static DK_BOOL GetAnchorPosition(BookType *pBook, DK_LONG chapterIndex, const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pPos)
    {
        return pBook && DK_SUCCEEDED(pBook->GetAnchorPosition(chapterIndex, pAnchor, pPos));
    }

    static DK_INT GetChapterIndexByURL(BookType *pBook, const DK_WCHAR* pChapterURL)
    {
        if(pBook && pChapterURL)
        {
            return pBook->GetChapterIndexByURL(pChapterURL);
        }
        return -1;
    }

    static IDKEFootnoteIterator* GetFootnoteIterator(PageType *pPage)
    {
        return pPage ? pPage-> GetFootnoteIterator() : NULL;
    }

    static DK_VOID FreeFootnoteIterator(PageType *pPage, IDKEFootnoteIterator* pFootnoteIterator)
    {
        if(pPage)
        {
            pPage->FreeFootnoteIterator(pFootnoteIterator);
        }
    }

    static IDKEImageIterator* GetImageIterator(PageType* pPage)
    {
        return pPage ? pPage->GetImageIterator() : NULL;
    }

    static  DK_VOID FreeImageIterator(PageType* pPage, IDKEImageIterator* pImageIterator)
    {
        if (pPage)
        {
            pPage->FreeImageIterator(pImageIterator);
        }
    }

    static DK_LONG HitTestComicsFrame(PageType* pPage, const DK_POS& point)
    {
        DK_LONG frameIndex = -1;
        if (pPage && DK_SUCCEEDED(pPage->HitTestComicsFrame(point, &frameIndex)))
        {
            return frameIndex;
        }
        return -1;
    }

    static IDKEComicsFrameIterator* GetComicsFrameIterator(PageType* pPage)
    {
        return pPage ? pPage->GetComicsFrameIterator() : NULL;
    }
    static DK_VOID FreeComicsFrameIterator(PageType* pPage, IDKEComicsFrameIterator* pFrameIterator)
    {
        if (pPage)
        {
            pPage->FreeComicsFrameIterator(pFrameIterator);
        }
    }

    static IDKEGalleryIterator* GetGalleryIterator(PageType* pPage) 
    {
        return pPage ? pPage->GetGalleryIterator() : NULL;
    }
    static DK_VOID FreeGalleryIterator(PageType* pPage, IDKEGalleryIterator* pGalleryIterator)
    {
        if (pPage)
        {
            pPage->FreeGalleryIterator(pGalleryIterator);
        }
    }
    static DK_BOOL HitTestGallery(PageType* pPage, const DK_POS& point, IDKEGallery** ppGallery)
    {
        return (pPage && DK_SUCCEEDED(pPage->HitTestGallery(point, ppGallery)));
    }
    static DK_VOID FreeHitTestGallery(PageType* pPage, IDKEGallery* pGallery)
    {
        if (pPage)
        {
            pPage->FreeHitTestGallery(pGallery);
        }
    }

    static IDKEPreBlockIterator* GetPreBlockIterator(PageType* pPage) 
    {
        return pPage ? pPage->GetPreBlockIterator() : NULL;
    }
    static DK_VOID FreePreBlockIterator(PageType* pPage, IDKEPreBlockIterator* pPreBlockIterator)
    {
        if (pPage)
        {
            pPage->FreePreBlockIterator(pPreBlockIterator);
        }
    }
    static DK_BOOL HitTestPreBlock(PageType* pPage, const DK_POS& point, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
        return (pPage && DK_SUCCEEDED(pPage->HitTestPreBlock(point, pPreBlockInfo)));
    }
    static DK_VOID FreeHitTestPreBlock(PageType* pPage, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
        if (pPage)
        {
            pPage->FreeHitTestPreBlock(pPreBlockInfo);
        }
    }

    static IDKEHitableObjIterator* GetHitableObjIterator(PageType* pPage)
    {
        if (pPage)
        {
            return pPage->GetHitableObjIterator();
        }
        return NULL;
    }

    static DK_VOID FreeHitableObjIterator(PageType* pPage, IDKEHitableObjIterator* pHitableObjIterator)
    {
        if (pPage)
        {
            pPage->FreeHitableObjIterator(pHitableObjIterator);
        }
    }
};

struct IDKMPageTraits: public IDKPageTraitsBase<IDKMBook, IDKMPage, IDKMTextIterator, DKM_CHAR_INFO>
{
    static DK_FLOWPOSITION GetBeginPosition(PageType *pPage)
    {
        return DK_FLOWPOSITION(0, 0, pPage->GetBeginPosition());
    }

    static DK_FLOWPOSITION GetEndPosition(PageType *pPage)
    {
        return DK_FLOWPOSITION(0, 0, pPage->GetEndPosition());
    }

    static DK_WCHAR * GetTextContentOfRange(BookType *pBook, PageType * /*pPage*/, 
        const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos)
    {
        if (pBook)
        {
            DK_WCHAR *pText = DK_NULL;
            pBook->GetTextOfRange(startPos.nElemIndex, endPos.nElemIndex, &pText);
            return pText;
        }
        return NULL;
    }

    static DK_VOID FreeTextContent(BookType *pBook, PageType * /*pPage*/, DK_WCHAR *pContent)
    {
        pBook->FreeText(pContent);
    }

    static DKEWRITINGOPT GetBookWritingOpt(BookType *pBook)
    {
        return DKEWRITINGOPT();
    }

    static DK_BOOL GetTextRects(PageType *pPage, const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos, 
        DK_BOX **ppTextRects, DK_UINT *pRectCount)
    {
        return DKR_OK == pPage->GetTextRects(startPos.nElemIndex, endPos.nElemIndex, ppTextRects, pRectCount);
    }

    static DK_VOID FreeRects(PageType *pPage, DK_BOX *pRects)
    {
        pPage->FreeRects(pRects);
    }

    static DK_BOOL HitTestTextRange(PageType *pPage, const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && DK_SUCCEEDED(pPage->HitTestTextRange(point, &(pStartPos->nElemIndex), &(pEndPos->nElemIndex)));
    }

    static DK_BOOL HitTestTextRangeByMode(PageType *pPage, const DK_POS& point, int hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && DK_SUCCEEDED(pPage->HitTestTextRangeByMode(point, (DKE_HITTEST_TEXT_MODE)hitTestTextMode, &(pStartPos->nElemIndex), &(pEndPos->nElemIndex)));
    }

    static DK_BOOL GetSelectionRange(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && pStartPos && pEndPos && DK_SUCCEEDED(pPage->GetSelectionRange(startPoint, endPoint, &(pStartPos->nElemIndex), &(pEndPos->nElemIndex)));
    }

    static DK_BOOL GetSelectionRangeByMode(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, int selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && pStartPos && pEndPos && DK_SUCCEEDED(pPage->GetSelectionRangeByMode(startPoint, endPoint, (DKE_SELECTION_MODE)selectionMode, &(pStartPos->nElemIndex), &(pEndPos->nElemIndex)));
    }

    static DK_BOOL HitTestObject(PageType *pPage, const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return pPage && DK_SUCCEEDED(pPage->HitTestObject(point, pObjInfo));
    }

    static DK_VOID FreeHitTestObject(PageType *pPage, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        if(pPage)
            pPage->FreeHitTestObject(pObjInfo);
    }

    static DK_BOOL HitTestFootnote(PageType *pPage, const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestFootnote(PageType *pPage, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return;
    }

    static DK_BOOL GetFootnoteContentHandle(PageType *pPage, const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return false;
    }
    static DK_VOID FreeContentHandle(PageType *pPage, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return;
    }

    static DK_BOOL HitTestLink(PageType *pPage, const DK_POS& point, DKE_LINK_INFO* pLinkInfo)
    {
        return pPage && DK_SUCCEEDED(pPage->HitTestLink(point, pLinkInfo));
    }

    static DK_VOID FreeHitTestLink(PageType *pPage, DKE_LINK_INFO* pLinkInfo)
    {
        if(pPage)
        {
            pPage->FreeHitTestLink(pLinkInfo);
        }
    }

    static DK_BOOL GetAnchorPosition(BookType *pBook, DK_LONG chapterIndex, const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pPos)
    {
        return false;
    }

    static DK_INT GetChapterIndexByURL(BookType *pBook, const DK_WCHAR* pChapterURL)
    {
        return -1;
    }

    static IDKEFootnoteIterator* GetFootnoteIterator(PageType *pPage)
    {
        return NULL;
    }

    static DK_VOID FreeFootnoteIterator(PageType *pPage, IDKEFootnoteIterator* pFootnoteIterator)
    {
        return;
    }

    static IDKEImageIterator* GetImageIterator(PageType* pPage)
    {
        return NULL;
    }

    static  DK_VOID FreeImageIterator(PageType* pPage, IDKEImageIterator* pImageIterator)
    {
        return;
    }

    static DK_LONG HitTestComicsFrame(PageType* pPage, const DK_POS& point)
    {
        return -1;
    }

    static IDKEComicsFrameIterator* GetComicsFrameIterator(PageType* pPage)
    {
        return NULL;
    }
    static DK_VOID FreeComicsFrameIterator(PageType* pPage, IDKEComicsFrameIterator* pFrameIterator)
    {
        return ;
    }

    static IDKEGalleryIterator* GetGalleryIterator(PageType* pPage) 
    {
        return NULL;
    }
    static DK_VOID FreeGalleryIterator(PageType* pPage, IDKEGalleryIterator* pGalleryIterator)
    {
    }
    static DK_BOOL HitTestGallery(PageType* pPage, const DK_POS& point, IDKEGallery** ppGallery)
    {
        return false;
    }
    static DK_VOID FreeHitTestGallery(PageType* pPage, IDKEGallery* pGallery)
    {
    }

    static IDKEPreBlockIterator* GetPreBlockIterator(PageType* pPage) 
    {
        return NULL;
    }
    static DK_VOID FreePreBlockIterator(PageType* pPage, IDKEPreBlockIterator* pPreBlockIterator)
    {
    }
    static DK_BOOL HitTestPreBlock(PageType* pPage, const DK_POS& point, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
        return false;
    }
    static DK_VOID FreeHitTestPreBlock(PageType* pPage, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
    }

    static IDKEHitableObjIterator* GetHitableObjIterator(PageType* pPage)
    {
        return NULL;
    }

    static DK_VOID FreeHitableObjIterator(PageType* pPage, IDKEHitableObjIterator* pHitableObjIterator)
    {
    }
};

struct IDKTPageTraits: public IDKPageTraitsBase<IDKTBook, IDKTPage, IDKTTextIterator, DKT_CHAR_INFO>
{
    static DK_FLOWPOSITION GetBeginPosition(PageType *pPage)
    {
        return DK_FLOWPOSITION(0, 0, pPage->GetOffsetInByte());
    }

    static DK_FLOWPOSITION GetEndPosition(PageType *pPage)
    {
        return DK_FLOWPOSITION(0, 0, pPage->GetOffsetInByte() + pPage->GetSizeInByte());
    }

    static DK_WCHAR * GetTextContentOfRange(BookType *pBook, PageType */*pPage*/, 
        const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos)
    {
        if (pBook)
        {
            return pBook->GetTextContentOfRange(startPos.nElemIndex, endPos.nElemIndex);
        }
        return NULL;
    }

    static DK_VOID FreeTextContent(BookType * pBook, PageType */*pPage*/, DK_WCHAR *pContent)
    {
        pBook->FreeTextContent(pContent);
    }

    static DKEWRITINGOPT GetBookWritingOpt(BookType *pBook)
    {
        return DKEWRITINGOPT();
    }

    static DK_BOOL GetTextRects(PageType *pPage, const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos, 
        DK_BOX **ppTextRects, DK_UINT *pRectCount)
    {
        return DKR_OK == pPage->GetTextRects(startPos.nElemIndex, endPos.nElemIndex, ppTextRects, pRectCount);
    }

    static DK_VOID FreeRects(PageType *pPage, DK_BOX *pRects)
    {
        pPage->FreeRects(pRects);
    }

    static DK_BOOL HitTestTextRange(PageType *pPage, const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return DKR_OK == (pPage->HitTestTextRange(point, &(pStartPos->nElemIndex), &(pEndPos->nElemIndex)));
    }

    static DK_BOOL HitTestTextRangeByMode(PageType *pPage, const DK_POS& point, int hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && DK_SUCCEEDED(pPage->HitTestTextRangeByMode(point, (DKT_HITTEST_TEXT_MODE)hitTestTextMode, &(pStartPos->nElemIndex), &(pEndPos->nElemIndex)));
    }

    static DK_BOOL GetSelectionRange(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && pStartPos && pEndPos && DK_SUCCEEDED(pPage->GetSelectionRange(startPoint, endPoint, &(pStartPos->nElemIndex), &(pEndPos->nElemIndex)));
    }

    static DK_BOOL GetSelectionRangeByMode(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, int selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return pPage && pStartPos && pEndPos
            && DK_SUCCEEDED(pPage->GetSelectionRangeByMode(startPoint, endPoint, (DKT_SELECTION_MODE)selectionMode, &(pStartPos->nElemIndex), &(pEndPos->nElemIndex)));
    }

    static DK_BOOL HitTestObject(PageType *pPage, const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestObject(PageType *pPage, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return ;
    }

    static DK_BOOL HitTestFootnote(PageType *pPage, const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestFootnote(PageType *pPage, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return;
    }

    static DK_BOOL GetFootnoteContentHandle(PageType *pPage, const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return false;
    }
    static DK_VOID FreeContentHandle(PageType *pPage, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return;
    }

    static DK_BOOL HitTestLink(PageType *pPage, const DK_POS& point, DKE_LINK_INFO* pLinkInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestLink(PageType *pPage, DKE_LINK_INFO* pLinkInfo)
    {
        return;
    }

    static DK_BOOL GetAnchorPosition(BookType *pBook, DK_LONG chapterIndex, const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pPos)
    {
        return false;
    }

    static DK_INT GetChapterIndexByURL(BookType *pBook, const DK_WCHAR* pChapterURL)
    {
        return -1;
    }

    static IDKEFootnoteIterator* GetFootnoteIterator(PageType *pPage)
    {
        return NULL;
    }

    static DK_VOID FreeFootnoteIterator(PageType *pPage, IDKEFootnoteIterator* pFootnoteIterator)
    {
        return;
    }

    static IDKEImageIterator* GetImageIterator(PageType* pPage)
    {
        return NULL;
    }

    static  DK_VOID FreeImageIterator(PageType* pPage, IDKEImageIterator* pImageIterator)
    {
        return;
    }

    static DK_LONG HitTestComicsFrame(PageType* pPage, const DK_POS& point)
    {
        return -1;
    }

    static IDKEComicsFrameIterator* GetComicsFrameIterator(PageType* pPage)
    {
        return NULL;
    }
    static DK_VOID FreeComicsFrameIterator(PageType* pPage, IDKEComicsFrameIterator* pFrameIterator)
    {
        return ;
    }

    static IDKEGalleryIterator* GetGalleryIterator(PageType* pPage) 
    {
        return NULL;
    }
    static DK_VOID FreeGalleryIterator(PageType* pPage, IDKEGalleryIterator* pGalleryIterator)
    {
    }
    static DK_BOOL HitTestGallery(PageType* pPage, const DK_POS& point, IDKEGallery** ppGallery)
    {
        return false;
    }
    static DK_VOID FreeHitTestGallery(PageType* pPage, IDKEGallery* pGallery)
    {
    }

    static IDKEPreBlockIterator* GetPreBlockIterator(PageType* pPage) 
    {
        return NULL;
    }
    static DK_VOID FreePreBlockIterator(PageType* pPage, IDKEPreBlockIterator* pPreBlockIterator)
    {
    }
    static DK_BOOL HitTestPreBlock(PageType* pPage, const DK_POS& point, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
        return false;
    }
    static DK_VOID FreeHitTestPreBlock(PageType* pPage, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
    }

    static IDKEHitableObjIterator* GetHitableObjIterator(PageType* pPage)
    {
        return NULL;
    }

    static DK_VOID FreeHitableObjIterator(PageType* pPage, IDKEHitableObjIterator* pHitableObjIterator)
    {
    }
};

struct IDKPPageExTraits: public IDKPageTraitsBase<IPdfKernelHandle, IDKPPageEx, IDKPTextIterator, DKP_CHAR_INFO>
{
    static DK_FLOWPOSITION GetBeginPosition(PageType *pPage)
    {
        DK_FLOWPOSITION ret;
        pPage->GetPageStartPos(&ret);
        --(ret.nChapterIndex);
        return ret;
    }

    static DK_FLOWPOSITION GetEndPosition(PageType *pPage)
    {
        DK_FLOWPOSITION ret;
        pPage->GetPageEndPos(&ret);
        --(ret.nChapterIndex);
        return ret;
    }

    static DK_WCHAR * GetTextContentOfRange(BookType * pPdfKernelHandle, PageType *pPage, 
        const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos)
    {
        if (pPdfKernelHandle)
        {
            return pPdfKernelHandle->GetTextContentOfRange(startPos, endPos);
        }
        return NULL;
    }

    static DK_VOID FreeTextContent(BookType * /*pBook*/, PageType *pPage, DK_WCHAR *pContent)
    {
        pPage->FreeTextContent(pContent);
    }

    static DKEWRITINGOPT GetBookWritingOpt(BookType *pBook)
    {
        return DKEWRITINGOPT();
    }

    static DK_BOOL GetTextRects(PageType *pPage, const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos, 
        DK_BOX **ppTextRects, DK_UINT *pRectCount)
    {
        return DKR_OK == pPage->GetTextRects(DK_FLOWPOSITION(startPos.nChapterIndex + 1, startPos.nParaIndex, startPos.nElemIndex), 
            DK_FLOWPOSITION(endPos.nChapterIndex + 1, endPos.nParaIndex, endPos.nElemIndex), 
            ppTextRects, pRectCount);
    }

    static DK_VOID FreeRects(PageType *pPage, DK_BOX *pRects)
    {
        pPage->FreeRects(pRects);
    }

    static DK_BOOL HitTestTextRange(PageType *pPage, const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        DK_BOOL ret = (DKR_OK == (pPage->HitTestTextRange(point, pStartPos, pEndPos)));
        if (ret)
        {
            --(pStartPos->nChapterIndex);
            --(pEndPos->nChapterIndex);
        }

        return ret;
    }

    static DK_BOOL HitTestTextRangeByMode(PageType *pPage, const DK_POS& point, int hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        if (DK_SUCCEEDED(pPage->HitTestTextRangeByMode(point, (DKP_HITTEST_TEXT_MODE)hitTestTextMode, pStartPos, pEndPos)))
        {
            --(pStartPos->nChapterIndex);
            --(pEndPos->nChapterIndex);
            return true;
        }

        return false;
    }

    static DK_BOOL GetSelectionRange(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return GetSelectionRangeByMode(pPage, startPoint, endPoint, DKP_SELECTION_MIDDLECROSS, pStartPos, pEndPos);
    }

    static DK_BOOL GetSelectionRangeByMode(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, int selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        if (pPage && pStartPos && pEndPos)
        {
            DK_ReturnCode r = (pPage->GetSelectionRangeByMode(startPoint, endPoint, (DKP_SELECTION_MODE)selectionMode, pStartPos, pEndPos));
            if (r == DKR_OK)
            {
                //TODO, 应用层索引从0开始，内核从1开始。
                --(pStartPos->nChapterIndex);
                --(pEndPos->nChapterIndex);
                return true;
            }
        }
        return false;
    }

    static DK_BOOL HitTestObject(PageType *pPage, const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestObject(PageType *pPage, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return ;
    }

    static DK_BOOL HitTestFootnote(PageType *pPage, const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestFootnote(PageType *pPage, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return;
    }

    static DK_BOOL GetFootnoteContentHandle(PageType *pPage, const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return false;
    }
    static DK_VOID FreeContentHandle(PageType *pPage, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return;
    }

    static DK_BOOL HitTestLink(PageType *pPage, const DK_POS& point, DKE_LINK_INFO* pLinkInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestLink(PageType *pPage, DKE_LINK_INFO* pLinkInfo)
    {
        return;
    }

    static DK_BOOL GetAnchorPosition(BookType *pBook, DK_LONG chapterIndex, const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pPos)
    {
        return false;
    }

    static DK_INT GetChapterIndexByURL(BookType *pBook, const DK_WCHAR* pChapterURL)
    {
        return -1;
    }

    static IDKEFootnoteIterator* GetFootnoteIterator(PageType *pPage)
    {
        return NULL;
    }

    static DK_VOID FreeFootnoteIterator(PageType *pPage, IDKEFootnoteIterator* pFootnoteIterator)
    {
        return;
    }

    static IDKEImageIterator* GetImageIterator(PageType* pPage)
    {
        return NULL;
    }

    static  DK_VOID FreeImageIterator(PageType* pPage, IDKEImageIterator* pImageIterator)
    {
        return;
    }

    static DK_LONG HitTestComicsFrame(PageType* pPage, const DK_POS& point)
    {
        return -1;
    }

    static IDKEComicsFrameIterator* GetComicsFrameIterator(PageType* pPage)
    {
        return NULL;
    }
    static DK_VOID FreeComicsFrameIterator(PageType* pPage, IDKEComicsFrameIterator* pFrameIterator)
    {
        return ;
    }

    static IDKEGalleryIterator* GetGalleryIterator(PageType* pPage) 
    {
        return NULL;
    }
    static DK_VOID FreeGalleryIterator(PageType* pPage, IDKEGalleryIterator* pGalleryIterator)
    {
    }
    static DK_BOOL HitTestGallery(PageType* pPage, const DK_POS& point, IDKEGallery** ppGallery)
    {
        return false;
    }
    static DK_VOID FreeHitTestGallery(PageType* pPage, IDKEGallery* pGallery)
    {
    }

    static IDKEPreBlockIterator* GetPreBlockIterator(PageType* pPage) 
    {
        return NULL;
    }
    static DK_VOID FreePreBlockIterator(PageType* pPage, IDKEPreBlockIterator* pPreBlockIterator)
    {
    }
    static DK_BOOL HitTestPreBlock(PageType* pPage, const DK_POS& point, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
        return false;
    }
    static DK_VOID FreeHitTestPreBlock(PageType* pPage, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
    }

    static IDKEHitableObjIterator* GetHitableObjIterator(PageType* pPage)
    {
        return NULL;
    }

    static DK_VOID FreeHitableObjIterator(PageType* pPage, IDKEHitableObjIterator* pHitableObjIterator)
    {
    }
};

struct IDKPPageTraits: public IDKPageTraitsBase<DK_VOID *, IPdfKernelHandle, PDFTextIterator, DKP_CHAR_INFO>
{
    static DK_FLOWPOSITION GetBeginPosition(PageType *pPage)
    {
        DK_FLOWPOSITION ret;
        pPage->GetPageStartPos(&ret);
        return ret;
    }

    static DK_FLOWPOSITION GetEndPosition(PageType *pPage)
    {
        DK_FLOWPOSITION ret;
        pPage->GetPageEndPos(&ret);
        return ret;
    }

    static DK_WCHAR * GetTextContentOfRange(BookType * /*pBook*/, PageType *pPage, 
        const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos)
    {
        if (pPage)
        {
            return pPage->GetTextContentOfRange(DK_FLOWPOSITION(startPos.nChapterIndex + 1, startPos.nParaIndex, startPos.nElemIndex), 
                DK_FLOWPOSITION(endPos.nChapterIndex + 1, endPos.nParaIndex, endPos.nElemIndex));
        }
        return NULL;
    }

    static DK_VOID FreeTextContent(BookType * /*pBook*/, PageType *pPage, DK_WCHAR *pContent)
    {
        pPage->FreeTextContent(pContent);
    }

    static DKEWRITINGOPT GetBookWritingOpt(BookType *pBook)
    {
        return DKEWRITINGOPT();
    }

    static DK_BOOL GetTextRects(PageType *pPage, const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos, 
        DK_BOX **ppTextRects, DK_UINT *pRectCount)
    {
        return DKR_OK == pPage->GetTextRects(DK_FLOWPOSITION(startPos.nChapterIndex + 1, startPos.nParaIndex, startPos.nElemIndex), 
            DK_FLOWPOSITION(endPos.nChapterIndex + 1, endPos.nParaIndex, endPos.nElemIndex), 
            ppTextRects, pRectCount);
    }

    static DK_VOID FreeRects(PageType *pPage, DK_BOX *pRects)
    {
        pPage->FreeRects(pRects);
    }

    static DK_BOOL HitTestTextRange(PageType *pPage, const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        DK_BOOL ret = (DKR_OK == (pPage->HitTestTextRange(point, pStartPos, pEndPos)));
        if (ret)
        {
            --(pStartPos->nChapterIndex);
            --(pEndPos->nChapterIndex);
        }

        return ret;
    }

    static DK_BOOL HitTestTextRangeByMode(PageType *pPage, const DK_POS& point, int hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return HitTestTextRange(pPage, point, pStartPos, pEndPos);
    }

    static DK_BOOL GetSelectionRange(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return GetSelectionRangeByMode(pPage, startPoint, endPoint, DKP_SELECTION_MIDDLECROSS, pStartPos, pEndPos);
    }

    static DK_BOOL GetSelectionRangeByMode(PageType *pPage, const DK_POS& startPoint, const DK_POS& endPoint, int selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
#ifdef KINDLE_FOR_TOUCH
        DK_BOOL ret = pPage && pPage->GetSelectionRange(startPoint, endPoint, (DKP_SELECTION_MODE)selectionMode, pStartPos, pEndPos);
        if (ret)
        {
            if (pStartPos->nChapterIndex > 0)
                --(pStartPos->nChapterIndex);
            if (pEndPos->nChapterIndex > 0)
                --(pEndPos->nChapterIndex);
        }
        return ret;
#else
        return false;
#endif
    }

    static DK_BOOL HitTestObject(PageType *pPage, const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestObject(PageType *pPage, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return ;
    }

    static DK_BOOL HitTestFootnote(PageType *pPage, const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestFootnote(PageType *pPage, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return;
    }

    static DK_BOOL GetFootnoteContentHandle(PageType *pPage, const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return false;
    }
    static DK_VOID FreeContentHandle(PageType *pPage, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return;
    }

    static DK_BOOL HitTestLink(PageType *pPage, const DK_POS& point, DKE_LINK_INFO* pLinkInfo)
    {
        return false;
    }

    static DK_VOID FreeHitTestLink(PageType *pPage, DKE_LINK_INFO* pLinkInfo)
    {
        return;
    }

    static DK_BOOL GetAnchorPosition(BookType *pBook, DK_LONG chapterIndex, const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pPos)
    {
        return false;
    }

    static DK_INT GetChapterIndexByURL(BookType *pBook, const DK_WCHAR* pChapterURL)
    {
        return -1;
    }

    static IDKEFootnoteIterator* GetFootnoteIterator(PageType *pPage)
    {
        return NULL;
    }

    static DK_VOID FreeFootnoteIterator(PageType *pPage, IDKEFootnoteIterator* pFootnoteIterator)
    {
        return;
    }

    static IDKEImageIterator* GetImageIterator(PageType* pPage)
    {
        return NULL;
    }

    static  DK_VOID FreeImageIterator(PageType* pPage, IDKEImageIterator* pImageIterator)
    {
        return;
    }

    static DK_LONG HitTestComicsFrame(PageType* pPage, const DK_POS& point)
    {
        return -1;
    }

    static IDKEComicsFrameIterator* GetComicsFrameIterator(PageType* pPage)
    {
        return NULL;
    }
    static DK_VOID FreeComicsFrameIterator(PageType* pPage, IDKEComicsFrameIterator* pFrameIterator)
    {
        return ;
    }

    static IDKEGalleryIterator* GetGalleryIterator(PageType* pPage) 
    {
        return NULL;
    }
    static DK_VOID FreeGalleryIterator(PageType* pPage, IDKEGalleryIterator* pGalleryIterator)
    {
    }
    static DK_BOOL HitTestGallery(PageType* pPage, const DK_POS& point, IDKEGallery** ppGallery)
    {
        return false;
    }
    static DK_VOID FreeHitTestGallery(PageType* pPage, IDKEGallery* pGallery)
    {
    }

    static IDKEPreBlockIterator* GetPreBlockIterator(PageType* pPage) 
    {
        return NULL;
    }
    static DK_VOID FreePreBlockIterator(PageType* pPage, IDKEPreBlockIterator* pPreBlockIterator)
    {
    }
    static DK_BOOL HitTestPreBlock(PageType* pPage, const DK_POS& point, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
        return false;
    }
    static DK_VOID FreeHitTestPreBlock(PageType* pPage, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
    }

    static IDKEHitableObjIterator* GetHitableObjIterator(PageType* pPage)
    {
        return NULL;
    }

    static DK_VOID FreeHitableObjIterator(PageType* pPage, IDKEHitableObjIterator* pHitableObjIterator)
    {
    }
};

template <typename IDKPageTraits>
class DKPageAdapter: public IDKPageAdapter
{
public:
    typedef typename IDKPageTraits::BookType           BookType;
    typedef typename IDKPageTraits::PageType           PageType;
    typedef typename IDKPageTraits::TextIteratorType   TextIteratorType;
    typedef typename IDKPageTraits::CharInfoType       CharInfoType;
public:
    DKPageAdapter(BookType *pBook, PageType *pPage)
        : m_pBook(pBook)
        , m_pPage(pPage)
        , m_pTextIterator(NULL)
    {
#ifndef KINDLE_FOR_TOUCH
        if (m_pPage)
        {
            m_pTextIterator = m_pPage->GetTextIterator();
        }
#endif
    }

    virtual ~DKPageAdapter()
    {
#ifndef KINDLE_FOR_TOUCH
        if (m_pTextIterator)
        {
            m_pPage->FreeTextIterator(m_pTextIterator);
            m_pTextIterator = NULL;
        }
#endif
    }

public:
    virtual IDKTextIterator * GetTextIterator()
    {
#ifdef KINDLE_FOR_TOUCH
        return DK_NULL;
#else
        if (DK_NULL == m_pTextIterator)
        {
            return DK_NULL;
        }

        return new DKTextIterator<TextIteratorType, CharInfoType>(m_pTextIterator);
#endif
    }

    virtual DK_VOID FreeTextIterator(IDKTextIterator *pDKTextIterator)
    {
        if (pDKTextIterator)
        {
            delete pDKTextIterator;
        }
    }

public:
    virtual DK_FLOWPOSITION GetBeginPosition()
    {
        return IDKPageTraits::GetBeginPosition(m_pPage);
    }

    virtual DK_FLOWPOSITION GetEndPosition()
    {
        return IDKPageTraits::GetEndPosition(m_pPage);
    }

    virtual DK_WCHAR * GetTextContentOfRange(const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos)
    {
        return IDKPageTraits::GetTextContentOfRange(m_pBook, m_pPage, startPos, endPos);
    }

    virtual DK_VOID FreeTextContent(DK_WCHAR *pContent)
    {
        IDKPageTraits::FreeTextContent(m_pBook, m_pPage, pContent);
    }

    virtual DKEWRITINGOPT GetBookWritingOpt()
    {
        return IDKPageTraits::GetBookWritingOpt(m_pBook);
    }

    virtual DK_BOOL GetTextRects(const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos, DK_BOX **ppTextRects, DK_UINT *pRectCount)
    {
        return IDKPageTraits::GetTextRects(m_pPage, startPos, endPos, ppTextRects, pRectCount);
    }

    virtual DK_VOID FreeRects(DK_BOX *pRects)
    {
        IDKPageTraits::FreeRects(m_pPage, pRects);
    }

    virtual DK_BOOL HitTestTextRange(const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return IDKPageTraits::HitTestTextRange(m_pPage, point, pStartPos, pEndPos);
    }

    virtual DK_BOOL HitTestTextRangeByMode(const DK_POS& point, int hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return IDKPageTraits::HitTestTextRangeByMode(m_pPage, point, hitTestTextMode, pStartPos, pEndPos);
    }

    virtual DK_BOOL GetSelectionRange(const DK_POS& startPoint, const DK_POS& endPoint, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return IDKPageTraits::GetSelectionRange(m_pPage, startPoint, endPoint, pStartPos, pEndPos);
    }

    virtual DK_BOOL GetSelectionRangeByMode(const DK_POS& startPoint, const DK_POS& endPoint, int selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
    {
        return IDKPageTraits::GetSelectionRangeByMode(m_pPage, startPoint, endPoint, selectionMode, pStartPos, pEndPos);
    }

    virtual DK_BOOL HitTestObject(const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return IDKPageTraits::HitTestObject(m_pPage, point, pObjInfo);
    }

    virtual DK_VOID FreeHitTestObject(DKE_HITTEST_OBJECTINFO* pObjInfo)
    {
        return IDKPageTraits::FreeHitTestObject(m_pPage, pObjInfo);
    }

    virtual DK_BOOL HitTestFootnote(const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return IDKPageTraits::HitTestFootnote(m_pPage, point, pFootnoteInfo);
    }

    virtual DK_VOID FreeHitTestFootnote(DKE_FOOTNOTE_INFO* pFootnoteInfo)
    {
        return IDKPageTraits::FreeHitTestFootnote(m_pPage, pFootnoteInfo);
    }

    virtual DK_BOOL GetFootnoteContentHandle(const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return IDKPageTraits::GetFootnoteContentHandle(m_pPage, pNoteId, showType, pHandle);
    }
    virtual DK_VOID FreeContentHandle(DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
    {
        return IDKPageTraits::FreeContentHandle(m_pPage, showType, pHandle);
    }

    virtual DK_BOOL HitTestLink(const DK_POS& point, DKE_LINK_INFO* pLinkInfo)
    {
        return IDKPageTraits::HitTestLink(m_pPage, point, pLinkInfo);
    }

    virtual DK_VOID FreeHitTestLink(DKE_LINK_INFO* pLinkInfo)
    {
        return IDKPageTraits::FreeHitTestLink(m_pPage, pLinkInfo);
    }

    virtual DK_BOOL GetAnchorPosition(DK_LONG chapterIndex, const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pPos)
    {
        return IDKPageTraits::GetAnchorPosition(m_pBook, chapterIndex, pAnchor, pPos);
    }

    virtual DK_INT GetChapterIndexByURL(const DK_WCHAR* pChapterURL) const
    {
        return IDKPageTraits::GetChapterIndexByURL(m_pBook, pChapterURL);
    }

    virtual IDKEImageIterator* GetImageIterator()
    {
        return IDKPageTraits::GetImageIterator(m_pPage);
    }

    virtual  DK_VOID FreeImageIterator(IDKEImageIterator* pImageIterator)
    {
        return IDKPageTraits::FreeImageIterator(m_pPage, pImageIterator);
    }

    virtual DK_LONG HitTestComicsFrame(const DK_POS& point)
    {
        return IDKPageTraits::HitTestComicsFrame(m_pPage, point);
    }

    virtual IDKEComicsFrameIterator* GetComicsFrameIterator()
    {
        return IDKPageTraits::GetComicsFrameIterator(m_pPage);
    }
    virtual DK_VOID FreeComicsFrameIterator(IDKEComicsFrameIterator* pFrameIterator)
    {
        return IDKPageTraits::FreeComicsFrameIterator(m_pPage, pFrameIterator);
    }

    virtual IDKEGalleryIterator* GetGalleryIterator() 
    {
        return IDKPageTraits::GetGalleryIterator(m_pPage);
    }
    virtual DK_VOID FreeGalleryIterator(IDKEGalleryIterator* pGalleryIterator)
    {
        return IDKPageTraits::FreeGalleryIterator(m_pPage, pGalleryIterator);
    }
    virtual DK_BOOL HitTestGallery(const DK_POS& point, IDKEGallery** ppGallery)
    {
        return IDKPageTraits::HitTestGallery(m_pPage, point, ppGallery);
    }
    virtual DK_VOID FreeHitTestGallery(IDKEGallery* pGallery)
    {
        return IDKPageTraits::FreeHitTestGallery(m_pPage, pGallery);
    }

    virtual IDKEPreBlockIterator* GetPreBlockIterator() 
    {
        return IDKPageTraits::GetPreBlockIterator(m_pPage);
    }
    virtual DK_VOID FreePreBlockIterator(IDKEPreBlockIterator* pPreBlockIterator)
    {
        return IDKPageTraits::FreePreBlockIterator(m_pPage, pPreBlockIterator);
    }
    virtual DK_BOOL HitTestPreBlock(const DK_POS& point, DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
        return IDKPageTraits::HitTestPreBlock(m_pPage, point, pPreBlockInfo);
    }
    virtual DK_VOID FreeHitTestPreBlock(DKE_PREBLOCK_INFO* pPreBlockInfo)
    {
        return IDKPageTraits::FreeHitTestPreBlock(m_pPage, pPreBlockInfo);
    }

    virtual IDKEFootnoteIterator* GetFootnoteIterator()
    {
        return IDKPageTraits::GetFootnoteIterator(m_pPage);
    }

    virtual DK_VOID FreeFootnoteIterator(IDKEFootnoteIterator* pFootnoteIterator)
    {
        return IDKPageTraits::FreeFootnoteIterator(m_pPage, pFootnoteIterator);
    }

    virtual IDKEHitableObjIterator* GetHitableObjIterator()
    {
        return IDKPageTraits::GetHitableObjIterator(m_pPage);
    }

    virtual DK_VOID FreeHitableObjIterator(IDKEHitableObjIterator* pHitableObjIterator)
    {
        return IDKPageTraits::FreeHitableObjIterator(m_pPage, pHitableObjIterator);
    }

private:
    BookType *             m_pBook;
    PageType *             m_pPage;
    TextIteratorType *     m_pTextIterator;
};

#endif //__KINDLEAPP_INC_BOOKREADER_IDKPAGEADAPTER_H__
