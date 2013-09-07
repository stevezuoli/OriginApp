#include "CommonUI/BookTextController.h"
#include "interface.h"
#include "Common/WindowsMetrics.h"
#include "Common/LaddishChar.h"

using namespace WindowsMetrics;

#define SELECTEDCONTROLER_DIFFDISTANCE 15

BookTextController::BookTextController()
    : m_pPage(NULL)
    , m_pModeController(NULL)
    , m_bSelectionMode(false)
    , m_iBaseLeft(0)
    , m_iBaseTop(0)
    , m_pointUpLeft(0, 0)
    , m_pointDownRight(0, 0)
    , m_pointStart(0, 0)
    , m_pointLast(0, 0)
    , m_pointEnd(0, 0)
    , m_posBegin(0, 0, 0)
    , m_posLast(0, 0, 0)
    , m_posEnd(0, 0, 0)
#ifdef KINDLE_FOR_TOUCH
    , m_selectionMode(DKE_SELECTION_INTERCROSS)
#else
    , m_selectionMode(DKE_SELECTION_INTERCROSS_WORD)
#endif
    , m_rightSelecter(true)
    , m_pGallery(NULL)
    , m_endOfGallery(false)
    , m_lastIsEndOfGallery(false)
    , m_selectingInGallery(false)
{
}

BookTextController::~BookTextController()
{
    Release();
}

bool BookTextController::IsInitialized() const 
{
    return (NULL != m_pPage);
}

void BookTextController::Release()
{
    m_pModeController = NULL;
    if(NULL != m_pPage)
    {
        SafeDeletePointer(m_pPage);
    }
}

void BookTextController::SetModeController(const PdfModeController* pModeController)
{
    m_pModeController = pModeController;
}

bool BookTextController::SetStartPos(int x, int y,int diffDistance, bool verticalForms)
{
    return false;
}

bool BookTextController::IsSelectionMode() const 
{
    return (NULL != m_pPage) && m_bSelectionMode;
}

void BookTextController::SetSelectionMode(bool selection)
{
    m_bSelectionMode = selection;
}

void BookTextController::SetBasePoint(int left, int top)
{
    m_iBaseLeft = left;
    m_iBaseTop  = top;
}

bool BookTextController::HitTestTextRangeByMode(const DK_POS& point, int hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
{
    DK_POS pointStart(point.X - m_iBaseLeft, point.Y - m_iBaseTop);

    DK_BOOL ret = false;
    if (m_pPage && pStartPos && pEndPos)
    {
        if (m_selectingInGallery)
        {
            IDKEGallery* pGallery = NULL;
            ret = m_pPage->HitTestGallery(m_pointStart, &pGallery) && pGallery;
            if (ret)
            {
                ret = DK_SUCCEEDED(pGallery->HitTestTextRangeByMode(pointStart, (DKE_HITTEST_TEXT_MODE)hitTestTextMode, pStartPos, pEndPos));
                m_pPage->FreeHitTestGallery(pGallery);
            }
        }
        else
        {
            ret = m_pPage->HitTestTextRangeByMode(pointStart, hitTestTextMode, pStartPos, pEndPos);
        }
    }
    return ret;
}

bool BookTextController::SelectingInGallery() const
{
    return m_selectingInGallery;
}

void BookTextController::SetSelectionRangeMode(int mode)
{
    if (mode >= DKE_SELECTION_UNKNOWN && mode <= DKE_SELECTION_INTERCROSS_WORD)
    {
        m_selectionMode = (DKE_SELECTION_MODE)mode;
    }
}

#ifndef KINDLE_FOR_TOUCH
DKE_PAGEOBJ_TYPE BookTextController::GetLastObjType() const
{
    return DKE_PAGEOBJ_TEXT;
}

DKE_PAGEOBJ_TYPE BookTextController::GetCurObjType() const
{
    return DKE_PAGEOBJ_TEXT;
}

bool BookTextController::IsEndOfGallery() const
{
    return m_endOfGallery;
}

bool BookTextController::LastIsEndOfGallery() const
{
    return m_lastIsEndOfGallery;
}
#endif

bool BookTextController::GetSelectionRange(const DK_POS& startPoint, const DK_POS& endPoint, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
{
    DK_POS pointStart(startPoint.X - m_iBaseLeft, startPoint.Y - m_iBaseTop);
    DK_POS pointEnd(endPoint.X - m_iBaseLeft, endPoint.Y - m_iBaseTop);

    DK_BOOL ret = false;
    if (m_pPage && pStartPos && pEndPos)
    {
        if (m_selectingInGallery)
        {
            IDKEGallery* pGallery = NULL;
            ret = m_pPage->HitTestGallery(m_pointStart, &pGallery) && pGallery;
            if (ret)
            {
                ret = DK_SUCCEEDED(pGallery->GetSelectionRangeByMode(pointStart, pointEnd, DKE_SELECTION_INTERCROSS, pStartPos, pEndPos));
                m_pPage->FreeHitTestGallery(pGallery);
            }
        }
        else
        {
            ret = m_pPage->GetSelectionRange(pointStart, pointEnd, pStartPos, pEndPos);
        }
    }
    return ret;
}

bool BookTextController::GetSelectionRangeByMode(const DK_POS& startPoint, const DK_POS& endPoint, int selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
{
    DK_POS pointStart(startPoint.X - m_iBaseLeft, startPoint.Y - m_iBaseTop);
    DK_POS pointEnd(endPoint.X - m_iBaseLeft, endPoint.Y - m_iBaseTop);

    DK_BOOL ret = false;
    if (m_pPage && pStartPos && pEndPos)
    {
        if (m_selectingInGallery)
        {
            IDKEGallery* pGallery = NULL;
            ret = m_pPage->HitTestGallery(m_pointStart, &pGallery) && pGallery;
            if (ret)
            {
                ret = DK_SUCCEEDED(pGallery->GetSelectionRangeByMode(pointStart, pointEnd, (DKE_SELECTION_MODE)selectionMode, pStartPos, pEndPos));
                m_pPage->FreeHitTestGallery(pGallery);
            }
        }
        else
        {
            ret = m_pPage->GetSelectionRangeByMode(pointStart, pointEnd, selectionMode, pStartPos, pEndPos);
        }
    }
    return ret;
}

void BookTextController::GetSelectionRectsOfThisPage(std::vector<DK_RECT> *_pvRects)
{
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    GetRectsBetweenPos(m_posBegin, m_posEnd, _pvRects);
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void BookTextController::GetSelectionRectsOfLastMove(std::vector<DK_RECT> *_pvRects)
{
    std::vector<DK_RECT> rectsBeforeMove;
    if (m_posBegin == m_posLast || m_posBegin == m_posEnd || m_posLast == m_posEnd)
    {
        return;
    }
    if (m_rightSelecter)
    {
        GetRectsBetweenPos(m_posBegin, m_posLast, &rectsBeforeMove);
        GetRectsBetweenPos(m_posBegin, m_posEnd, _pvRects);
    }
    else
    {
        GetRectsBetweenPos(m_posBegin, m_posEnd, &rectsBeforeMove);
        GetRectsBetweenPos(m_posLast, m_posEnd, _pvRects);
    }
    for (unsigned int i = 0; i < rectsBeforeMove.size(); ++i)
    {
        _pvRects->push_back(rectsBeforeMove[i]);
    }
}

bool BookTextController::GetSelectionContent(DK_FLOWPOSITION *_pBeginPos, DK_FLOWPOSITION *_pEndPos, std::string& bookComment)
{
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!_pBeginPos || !_pEndPos || m_posBegin == m_posEnd)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s ERROR 1", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    DK_FLOWPOSITION beginPos = m_posBegin;
    DK_FLOWPOSITION endPos   = m_posEnd;
    if (beginPos > endPos)
    {
        beginPos = m_posEnd;
        endPos   = m_posBegin;
    }

    DK_WCHAR *pContent = m_pPage->GetTextContentOfRange(beginPos, endPos);
    if (DK_NULL == pContent)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s DK_NULL == pContent", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    *_pBeginPos = beginPos;
    *_pEndPos   = endPos;
    DK_WCHAR *pCheckedContent = pContent;
    while (*pCheckedContent && 0x0A == *pCheckedContent)
    {
        ++pCheckedContent;
    }

    char *pUtf8 = EncodingUtil::WCharToChar(pCheckedContent);
    m_pPage->FreeTextContent(pContent);
    if (pUtf8)
    {
        bookComment = pUtf8;
        DK_FREE(pUtf8);
    }
    return true;
}

void BookTextController::GetRectsBetweenPos(const DK_FLOWPOSITION &_pos1, const DK_FLOWPOSITION &_pos2, std::vector<DK_RECT> *_pvRects)
{
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
	if(!m_pPage)
	{
		return;
	}
    // 获取顺序的开始结束位置
    DK_FLOWPOSITION posBegin = _pos1;
    DK_FLOWPOSITION posEnd   = _pos2;
    if (posBegin > posEnd)
    {
        posBegin = _pos2;
        posEnd   = _pos1;
    }
    posBegin = dk_max(posBegin, m_pPage->GetBeginPosition());
    posEnd   = dk_min(posEnd,   m_pPage->GetEndPosition());

    _pvRects->clear();
    if (posBegin == posEnd)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s beginPos == endPos", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return;
    }

    DK_BOX *pTextBoxes = DK_NULL;
    DK_UINT uBoxNum    = 0;
    if (!GetTextRects(posBegin, posEnd, &pTextBoxes, &uBoxNum)) 
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s GetTextRects error", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return;
    }

    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s GetTextRects boxNum %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, uBoxNum);
    for (DK_UINT i = 0; i < uBoxNum; ++i)
    {
        _pvRects->push_back(GetRectFromBox(pTextBoxes[i]));
    }

    FreeRects(pTextBoxes);
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

DK_FLOWPOSITION BookTextController::GetStartPos()
{
    return m_posBegin;
}

DK_FLOWPOSITION BookTextController::GetEndPos()
{
    return m_posEnd;
}

DK_POS BookTextController::GetStartPoint()
{
    return m_pointStart;
}

DK_POS BookTextController::GetLastPoint() const
{
    return m_pointLast;
}

DK_POS BookTextController::GetEndPoint()
{
    return m_pointEnd;
}

DK_RECT BookTextController::GetRectFromBox(const DK_BOX &_box) const
{
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    DK_RECT vRet;
    vRet.left = int(_box.X0 + 0.5) + m_iBaseLeft;
    vRet.top = int(_box.Y0 + 0.5) + m_iBaseTop;
    vRet.right = int(_box.X1 + 0.5) + m_iBaseLeft;
    vRet.bottom = int(_box.Y1 + 0.5) + m_iBaseTop;
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return vRet;
}

bool BookTextController::GetHitTestBoundBox(const DK_POS& point, DK_BOX** pBoundingBox, DK_UINT* pBoundBoxCount, bool* bIsHitTest)
{
    if (!bIsHitTest || !pBoundingBox)
    {
        return false;
    }
    DK_FLOWPOSITION beginPos;
    DK_FLOWPOSITION endPos;

    const int mode = DKE_HITTEST_TEXT_WORD;
    if(HitTestTextRangeByMode(point, mode, &beginPos, &endPos))
    {
        return GetTextRects(beginPos, endPos, pBoundingBox, pBoundBoxCount);
    }
    return false;
}

bool BookTextController::FreeHitTestBoundBox(DK_BOX* _pBoundingBox)
{
    FreeRects(_pBoundingBox);
    return true;
}

bool BookTextController::GetTextRects(const DK_FLOWPOSITION &startPos, const DK_FLOWPOSITION &endPos, DK_BOX **ppTextRects, DK_UINT *pRectCount)
{
    assert (NULL != ppTextRects && NULL != pRectCount && NULL != m_pPage);
    if (m_pPage->GetTextRects(startPos, endPos, ppTextRects, pRectCount) && (*pRectCount > 0))
    {
        return true;
    }

    if (m_pPage->HitTestGallery(m_pointStart, &m_pGallery) && m_pGallery)
    {
        return DK_SUCCEEDED(m_pGallery->GetInlineEleRects(startPos, endPos, ppTextRects, pRectCount)) && (*pRectCount > 0);
    }
    return false;
}

void BookTextController::FreeRects(DK_BOX* pTextRect)
{
    if(pTextRect)
    {
        if (m_pGallery)
        {
            m_pGallery->FreeRects(pTextRect);
            m_pPage->FreeHitTestGallery(m_pGallery);
            m_pGallery = NULL;
        }
        else
        {
            m_pPage->FreeRects(pTextRect);
        }
    }
    return ;
}

DK_RECT BookTextController::GetStartRect() const
{
    return m_rectStart;
}

DK_RECT BookTextController::GetEndRect() const
{
    return m_rectEnd;
}

bool BookTextController::CheckLinkInfo(const DKE_LINK_INFO& linkInfo, bool& isInternalLink) const
{
    bool ret = false;
    if (m_pPage)
    {
        if(linkInfo.linkType == DKE_LINK_URL)
        {
            ret = true;
            int iChapterIndex =  m_pPage->GetChapterIndexByURL(linkInfo.linkTarget);
            isInternalLink = (iChapterIndex >= 0);
        }
        else if(linkInfo.linkType == DKE_LINK_FILEPOS)
        {
            ret = true;
        }
    }
    return ret;
}

bool BookTextController::CheckInGallery(const DK_POS& _point)
{
    bool ret = false;
    if (m_pPage)
    {
        DK_POS point(_point.X - m_iBaseLeft, _point.Y - m_iBaseTop);
        IDKEGallery* pGallery = NULL;
        if (m_pPage->HitTestGallery(point, &pGallery) && pGallery)
        {
            ret = true;
            m_pPage->FreeHitTestGallery(pGallery);
        }
    }
    return ret;
}

bool BookTextController::CheckIsLink(const DK_POS& _point, bool& isInternalLink) const
{
    return m_pPage
        && (CheckIsLinkOfGallery(_point) || CheckIsLinkOfURL(_point, isInternalLink));
}

bool BookTextController::CheckCurPosIsLink(bool& isInternalLink) const
{
    return CheckIsLink(GetLastPoint(), isInternalLink);
}

bool BookTextController::CheckIsLinkOfGallery(const DK_POS& _point) const
{
    assert (NULL != m_pPage);
    DKE_LINK_INFO linkInfo;
    IDKEGallery* pGallery = NULL;
    bool ret = false;
    DK_POS point(_point.X - m_iBaseLeft, _point.Y - m_iBaseTop);
    if (m_pPage->HitTestGallery(point, &pGallery) && pGallery)
    {
        if (DK_SUCCEEDED(pGallery->HitTestLink(point, &linkInfo)))
        {
            bool isInternalLink = true;
            ret = CheckLinkInfo(linkInfo, isInternalLink);
            pGallery->FreeHitTestLink(&linkInfo);
        }
        m_pPage->FreeHitTestGallery(pGallery);
    }
    return ret;
}

bool BookTextController::CheckIsLinkOfURL(const DK_POS& _point, bool& isInternalLink) const
{
    assert (NULL != m_pPage);
    DKE_LINK_INFO linkInfo;
    bool ret = false;
    DK_POS point(_point.X - m_iBaseLeft, _point.Y - m_iBaseTop);
    if (m_pPage->HitTestLink(point, &linkInfo))
    {
        ret = CheckLinkInfo(linkInfo, isInternalLink);
        m_pPage->FreeHitTestLink(&linkInfo);
    }
    return ret;
}

bool BookTextController::GetLink(const DKE_LINK_INFO& linkInfo, DK_FLOWPOSITION* posLink)
{
    bool isInternalLink = true;
    bool ret = CheckLinkInfo(linkInfo, isInternalLink);
    if (ret && isInternalLink)
    {
        if(linkInfo.linkType == DKE_LINK_URL)
        {
            int iChapterIndex =  m_pPage->GetChapterIndexByURL(linkInfo.linkTarget);
            *posLink = DK_FLOWPOSITION(iChapterIndex, 0, 0);
            if(linkInfo.linkAnchor != NULL)
            {
                m_pPage->GetAnchorPosition(iChapterIndex, linkInfo.linkAnchor, posLink);
            }
        }
        else if(linkInfo.linkType == DKE_LINK_FILEPOS)
        {
            *posLink = DK_FLOWPOSITION(0, 0, linkInfo.filePos);
        }
    }
    return ret;
}

bool BookTextController::GetLinkExternalURL(const DK_POS& _point, std::string& url)
{
    url.clear();
    DKE_LINK_INFO linkInfo;
    bool ret = false;
    if(m_pPage->HitTestLink(_point, &linkInfo))
    {
        bool isInternalLink = true;
        ret = CheckLinkInfo(linkInfo, isInternalLink);

        DK_CHAR* urlString = EncodingUtil::WCharToChar(linkInfo.linkTarget);
        url = std::string(urlString);
        DK_FREE(urlString);
        m_pPage->FreeHitTestLink(&linkInfo);
        if (!ret || isInternalLink)
        {
            return false;
        }  
    }
    return ret;
}

bool BookTextController::GetLinkFlowPosition(const DK_POS& _point, DK_FLOWPOSITION* _posLink)
{
    if(!_posLink)
    {
        return false;
    }
    DKE_LINK_INFO linkInfo;

    IDKEGallery* pGallery = NULL;
    bool ret = false;
    DK_POS point(_point.X - m_iBaseLeft, _point.Y - m_iBaseTop);
    if (m_pPage->HitTestGallery(point, &pGallery) && pGallery)
    {
        if (DK_SUCCEEDED(pGallery->HitTestLink(point, &linkInfo)))
        {
            ret = GetLink(linkInfo, _posLink);
            pGallery->FreeHitTestLink(&linkInfo);
        }
        m_pPage->FreeHitTestGallery(pGallery);
    }
    else
    {
        if(m_pPage->HitTestLink(point, &linkInfo))
        {
            ret = GetLink(linkInfo, _posLink);
            m_pPage->FreeHitTestLink(&linkInfo);
        }
    }
    return ret;
}

bool BookTextController::GetHitObject(const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo)
{
	DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
	return m_pPage && pObjInfo && m_pPage->HitTestObject(point, pObjInfo);
}

void BookTextController::FreeHitObject(DKE_HITTEST_OBJECTINFO* pObjInfo)
{
    if (m_pPage)
    {
        m_pPage->FreeHitTestObject(pObjInfo);
    }
}

bool BookTextController::HitTestFootnote(const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
{
	DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_pPage && !pFootnoteInfo)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s DK_NULL == m_pPage", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }
    return HitGalleryFootnote(point, pFootnoteInfo)
        || HitPageFootnote(point, pFootnoteInfo);
}

bool BookTextController::CheckFootnote(DK_POS& pos, IDKEFootnoteIterator* pFootNoteIterator)
{
    assert (NULL != pFootNoteIterator);
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const DK_DOUBLE footnoteOffset = (DK_DOUBLE)GetWindowMetrics(UIPixelValue15Index);
    while (pFootNoteIterator->MoveToNext())
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "HitTestObject while ----");
        DKE_FOOTNOTE_INFO footNoteInfo;
        pFootNoteIterator->GetCurrentFootnoteInfo(&footNoteInfo);
        const DK_BOX boundingBox = footNoteInfo.boundingBox;
        const DK_BOX touchBox(boundingBox.X0 - footnoteOffset, boundingBox.Y0 - footnoteOffset, boundingBox.X1 + footnoteOffset, boundingBox.Y1 + footnoteOffset);
        pFootNoteIterator->FreeFootnoteInfo(&footNoteInfo);
        if (touchBox.PosInBox(pos))
        {
            DebugPrintf(DLC_SELECTEDCONTROLER, "IsIntersect");
            pos = DK_POS(boundingBox.X0 + (boundingBox.X1 - boundingBox.X0) / 2, boundingBox.Y0 + (boundingBox.Y1 - boundingBox.Y0) / 2);
            return true;
        }
    }
    return false;
}

bool BookTextController::HitGalleryFootnote(const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
{
    if (!m_pPage && !pFootnoteInfo)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s DK_NULL == m_pPage", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    bool ret = false;
    DK_POS pos(point.X, point.Y);
    IDKEGalleryIterator* pGalleryIterator = m_pPage->GetGalleryIterator();
    if (pGalleryIterator)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s pGalleryIterator", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        while (!ret && pGalleryIterator->MoveToNext())
        {
            IDKEGallery* pGallery = NULL;
            pGalleryIterator->GetCurGallery(&pGallery);
            if (pGallery)
            {
                IDKEFootnoteIterator* pFootNoteIterator = pGallery->GetFootnoteIterator();
                if (pFootNoteIterator)
                {
                    ret = CheckFootnote(pos, pFootNoteIterator);
                    pGallery->FreeFootnoteIterator(pFootNoteIterator);
                }
            }
        }
        m_pPage->FreeGalleryIterator(pGalleryIterator);
    }

    if (m_pPage->HitTestGallery(pos, &m_pGallery) && m_pGallery)
    {
        ret = DK_SUCCEEDED(m_pGallery->HitTestFootnote(pos, pFootnoteInfo));
    }
    return ret;
}

bool BookTextController::HitPageFootnote(const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
{
    if (!m_pPage && !pFootnoteInfo)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s DK_NULL == m_pPage", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    DK_POS pos(point.X, point.Y);
    IDKEFootnoteIterator* pFootNoteIterator = m_pPage->GetFootnoteIterator();
    if (pFootNoteIterator)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s pFootNoteIterator", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        CheckFootnote(pos, pFootNoteIterator);
        m_pPage->FreeFootnoteIterator(pFootNoteIterator);
    }

    return m_pPage->HitTestFootnote(pos, pFootnoteInfo);
}

int BookTextController::CompareDKBox(const DK_BOX& leftBox, const DK_BOX& rightBox)
{
    int result = -1;
    if (m_pPage)
    {
        enum NEW_WRITING_MODE_TYPE
        {
            WRITING_MODE_HTB,
            WRITING_MODE_VRL,
            WRITING_MODE_VLR,
            WRITING_MODE_HBT,
        };
        DKEWRITINGOPT opt = m_pPage->GetBookWritingOpt();
        DKE_WRITING_MODE_TYPE modeType = opt.writingMode;
        NEW_WRITING_MODE_TYPE newWritingType = (NEW_WRITING_MODE_TYPE)modeType;
        if (m_pModeController)
        {
            switch (m_pModeController->m_iRotation)
            {
            case 0:
                newWritingType = WRITING_MODE_HTB;
                break;
            case 90:
                newWritingType = WRITING_MODE_VLR;
                break;
            case 180:
                newWritingType = WRITING_MODE_HBT;
                break;
            case 270:
                newWritingType = WRITING_MODE_VRL;
                break;
            default:
                break;
            }
        }
        const int leftX = ((int)leftBox.X0 + (int)leftBox.X1) >> 1;
        const int leftY = ((int)leftBox.Y0 + (int)leftBox.Y1) >> 1;
        const int rightX = ((int)rightBox.X0 + (int)rightBox.X1) >> 1;
        const int rightY = ((int)rightBox.Y0 + (int)rightBox.Y1) >> 1;
        const int xDiff = leftX - rightX;
        const int yDiff = leftY - rightY;
        const int positiveThreshold = 3;
        const int negativeThreshold = -3;
        const bool leftLessRightX = xDiff < negativeThreshold;
        const bool leftGreaterRightX = xDiff > positiveThreshold;
        const bool leftLessRightY = yDiff < negativeThreshold;
        const bool leftGreaterRightY = yDiff > positiveThreshold;
        switch (newWritingType)
        {
        case WRITING_MODE_HTB:
            {
                if (leftLessRightY)
                {
                    result = -1;
                }
                else if (leftGreaterRightY)
                {
                    result = 1;
                }
                else
                {
                    if (leftGreaterRightX)
                    {
                        result = -1;
                    }
                    else if (leftLessRightX)
                    {
                        result = 1;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
            break;
        case WRITING_MODE_VRL:
            {
                if (leftGreaterRightX)
                {
                    result = -1;
                }
                else if (leftLessRightX)
                {
                    result = 1;
                }
                else
                {
                    if (leftGreaterRightY)
                    {
                        result = 1;
                    }
                    else if (leftLessRightY)
                    {
                        result = -1;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
            break;
        case WRITING_MODE_VLR:
            {
                if (leftGreaterRightX)
                {
                    result = 1;
                }
                else if (leftLessRightX)
                {
                    result = -1;
                }
                else
                {
                    if (leftGreaterRightY)
                    {
                        result = -1;
                    }
                    else if (leftLessRightY)
                    {
                        result = 1;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
            break;
        case WRITING_MODE_HBT:
            {
                if (leftLessRightY)
                {
                    result = 1;
                }
                else if (leftGreaterRightY)
                {
                    result = -1;
                }
                else
                {
                    if (leftGreaterRightX)
                    {
                        result = 1;
                    }
                    else if (leftLessRightX)
                    {
                        result = -1;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    return result;
}

int BookTextController::GetMinBoxIndex(DK_BOX *pTextRects, int rectCount)
{
    int index = -1;
    if (pTextRects && rectCount > 0)
    {
        index = 0;
        DK_BOX minBox = pTextRects[index];
        for (int i = 0; i < rectCount; ++i)
        {
            if (CompareDKBox(pTextRects[i], minBox) < 0)
            {
                minBox = pTextRects[i];
                index = i;
            }
        }
    }
    return index;
}

int BookTextController::GetMaxBoxIndex(DK_BOX *pTextRects, int rectCount)
{
    int index = -1;
    if (pTextRects && rectCount > 0)
    {
        index = 0;
        DK_BOX maxBox = pTextRects[index];
        for (int i = 0; i < rectCount; ++i)
        {
            if (CompareDKBox(pTextRects[i], maxBox) > 0)
            {
                maxBox = pTextRects[i];
                index = i;
            }
        }
    }
    return index;
}

void BookTextController::FreeHitTestFootnote(DKE_FOOTNOTE_INFO* pFootnoteInfo)
{
    if (m_pPage)
    {
        if (m_pGallery)
        {
            m_pGallery->FreeHitTestFootnote(pFootnoteInfo);
            m_pPage->FreeHitTestGallery(m_pGallery);
            m_pGallery = NULL;
        }
        else
        {
            m_pPage->FreeHitTestFootnote(pFootnoteInfo);
        }
    }
}

bool BookTextController::GetFootnoteContentHandle(const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
{
	return m_pPage && m_pPage->GetFootnoteContentHandle(pNoteId, showType, pHandle);
}

void BookTextController::FreeContentHandle(DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
{
	if (m_pPage)
    {
        m_pPage->FreeContentHandle(showType, pHandle);
    }
}

bool BookTextController::InitInteractiveObjects(std::vector<PageObjectInfo>& interactiveObjects)
{
    if (!m_pPage)
    {
        return false;
    }

    interactiveObjects.clear();

    //更新交互图信息
    IDKEImageIterator* imageIterator = m_pPage->GetImageIterator();
    if (imageIterator)
    {
        while (imageIterator->MoveToNext())
        {
            DKE_PAGEOBJ_TYPE imageType = imageIterator->GetCurrentImageType();
            if ((imageType == DKE_PAGEOBJ_INTERACTIVEIMAGE)
                || imageType == DKE_PAGEOBJ_STATICIMAGE
#ifdef KINDLE_FOR_TOUCH
                || imageType == DKE_PAGEOBJ_IMAGE
#endif
                )
            {
                PageObjectInfo objInfo;
                objInfo.m_objectType = imageType;
                objInfo.m_bounding = GetRectFromBox(imageIterator->GetCurrentImageBox());
                objInfo.m_objectFlowposition = imageIterator->GetCurrentFlowPosition();
                interactiveObjects.push_back(objInfo);
            }
        }
        m_pPage->FreeImageIterator(imageIterator);
    }

    // get info of galleries
    IDKEGalleryIterator* pGalleryIterator = m_pPage->GetGalleryIterator();
    if (pGalleryIterator)
    {
        while (pGalleryIterator->MoveToNext())
        {
            IDKEGallery* pGallery = NULL;
            pGalleryIterator->GetCurGallery(&pGallery);
            if (pGallery)
            {
                PageObjectInfo objInfo;
                objInfo.m_objectType = DKE_PAGEOBJ_INTERACTIVEIMAGE;
                objInfo.m_bounding = GetRectFromBox(pGallery->GetImageBoundary());
                //objInfo.m_objectFlowposition;
                interactiveObjects.push_back(objInfo);
                pGalleryIterator->FreeCurGallery(pGallery);
            }
        }
        m_pPage->FreeGalleryIterator(pGalleryIterator);
    }

    // get info of PreBlock
    IDKEPreBlockIterator* pPreBlockIterator = m_pPage->GetPreBlockIterator();
    if (pPreBlockIterator)
    {
        while (pPreBlockIterator->MoveToNext())
        {
            DKE_PREBLOCK_INFO preBlock;
            pPreBlockIterator->GetCurPreBlockInfo(&preBlock);
            if (NULL != preBlock.pPreFlexPage)
            {
                PageObjectInfo objInfo;
                objInfo.m_objectType = DKE_PAGEOBJ_PREBLOCK;
                objInfo.m_bounding = GetRectFromBox(preBlock.boundingBox);
                objInfo.m_objectFlowposition = preBlock.startPos;
                interactiveObjects.push_back(objInfo);
                pPreBlockIterator->FreeCurPreBlockInfo(&preBlock);
            }
        }
        m_pPage->FreePreBlockIterator(pPreBlockIterator);
    }
    return interactiveObjects.size();
}

bool BookTextController::InitComicsObjects(std::vector<PageObjectInfo>& comicsObjects)
{
    comicsObjects.clear();
    // get info of comics
    IDKEComicsFrameIterator* pComicsFrameIterator = m_pPage->GetComicsFrameIterator();
    if (pComicsFrameIterator)
    {
        while (pComicsFrameIterator->MoveToNext())
        {
            PageObjectInfo objInfo;
            objInfo.m_objectType = DKE_PAGEOBJ_IMAGE;
            objInfo.m_bounding = GetRectFromBox(pComicsFrameIterator->GetCurFrameBoundaryOnPage());
            DK_FLOWPOSITION pos(0, 0, pComicsFrameIterator->GetComicsFrameIndex());
            objInfo.m_objectFlowposition = pos;
            comicsObjects.push_back(objInfo);
        }
        m_pPage->FreeComicsFrameIterator(pComicsFrameIterator);
    }
    return comicsObjects.size();
}

bool BookTextController::InitGalleryObjects(std::vector<GalleryObjectInfo>& galleryObjects)
{
    galleryObjects.clear();
    // get info of galleries
    IDKEGalleryIterator* pGalleryIterator = m_pPage->GetGalleryIterator();
    if (pGalleryIterator)
    {
        while (pGalleryIterator->MoveToNext())
        {
            IDKEGallery* pGallery = NULL;
            pGalleryIterator->GetCurGallery(&pGallery);
            if (pGallery)
            {
                GalleryObjectInfo objInfo;
                objInfo.m_bounding = GetRectFromBox(pGallery->GetBoundary());
#ifdef KINDLE_FOR_TOUCH
                objInfo.m_boundingImage = GetRectFromBox(pGallery->GetImageBoundary());
#endif
                objInfo.m_activeCellIndex = pGallery->GetCurActiveCell();
                objInfo.m_cellCount = pGallery->GetCellCount();
                galleryObjects.push_back(objInfo);
                pGalleryIterator->FreeCurGallery(pGallery);
            }
        }
        m_pPage->FreeGalleryIterator(pGalleryIterator);
    }
    return galleryObjects.size();
}

bool BookTextController::GetStringBetweenPos(const DK_FLOWPOSITION &pos1, const DK_FLOWPOSITION &pos2, std::string &content)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "BookTextController::GetStringBetweenPos");
    if (NULL == m_pPage)
    {
        return false;
    }

    DK_FLOWPOSITION beginPos = pos1;
    DK_FLOWPOSITION endPos   = pos2;
    if (beginPos > endPos)
    {
        beginPos = pos2;
        endPos   = pos1;
    }

    DK_WCHAR *pContent = m_pPage->GetTextContentOfRange(beginPos, endPos);
    if (DK_NULL == pContent)
    {
        return false;
    }

    DK_WCHAR *pCheckedContent = pContent;
    while (*pCheckedContent && 0x0A == *pCheckedContent)
    {
        ++pCheckedContent;
    }

    char *pUtf8 = EncodingUtil::WCharToChar(pCheckedContent);
    m_pPage->FreeTextContent(pContent);
    if (pUtf8)
    {
        content = pUtf8;
        DK_FREE(pUtf8);
    }
    return true;
}

DK_LONG BookTextController::HitTestComicsFrame(DK_POS point)
{
    return m_pPage ? m_pPage->HitTestComicsFrame(point) : -1;
}

bool BookTextController::HitTestGallery(const DK_POS& point, IDKEGallery** ppGallery)
{
    return m_pPage && m_pPage->HitTestGallery(point, ppGallery);
}

void BookTextController::FreeHitTestGallery(IDKEGallery* pGallery)
{
    if (m_pPage)
    {
        m_pPage->FreeHitTestGallery(pGallery);
    }
}

bool BookTextController::HitTestPreBlock(const DK_POS& point, DKE_PREBLOCK_INFO* pPreBlockInfo)
{
    return m_pPage && m_pPage->HitTestPreBlock(point, pPreBlockInfo);
}

void BookTextController::FreeHitTestPreBlock(DKE_PREBLOCK_INFO* pPreBlockInfo)
{
    if (m_pPage)
    {
        m_pPage->FreeHitTestPreBlock(pPreBlockInfo);
    }
}

IDKEHitableObjIterator* BookTextController::GetHitableObjIterator()
{
    return m_pPage ? m_pPage->GetHitableObjIterator() : NULL;
}

DK_VOID BookTextController::FreeHitableObjIterator(IDKEHitableObjIterator* pHitableObjIterator)
{
    if (m_pPage)
    {
        m_pPage->FreeHitableObjIterator(pHitableObjIterator);
    }
}

bool BookTextController::IsEndOfPage(int _x, int _y)
{
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == m_pPage)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s NULL == m_pPage", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    const DK_FLOWPOSITION clsBegin = m_pPage->GetBeginPosition();
    const DK_FLOWPOSITION clsEnd = m_pPage->GetEndPosition();

    DK_BOX *pTextBoxes = DK_NULL;
    DK_UINT boxNum = 0;
    if (!GetTextRects(clsBegin, clsEnd, &pTextBoxes, &boxNum))
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s GetTextRects error %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, boxNum);
        return false;
    }

    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s GetTextRects %d\n", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, boxNum);
    const DK_RECT rtEnd = GetRectFromBox(pTextBoxes[boxNum - 1]);
    FreeRects(pTextBoxes);

    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s _x = %d   [%d, %d]", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, _x, rtEnd.left, rtEnd.right);
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s _y = %d,  [%d, %d]", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, _y, rtEnd.top,  rtEnd.bottom);

    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return (_x >= rtEnd.left && _y >= rtEnd.top);
}

bool BookTextController::IsBeginOfPage(int _x, int _y)
{
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == m_pPage)
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s NULL == m_pPage", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    const DK_FLOWPOSITION clsBegin = m_pPage->GetBeginPosition();
    const DK_FLOWPOSITION clsEnd   = m_pPage->GetEndPosition();
    DK_BOX *pTextBoxes = DK_NULL;
    DK_UINT boxNum = 0;
    if (!GetTextRects(clsBegin, clsEnd, &pTextBoxes, &boxNum))
    {
        DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s GetTextRects error %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, boxNum);
        return false;
    }

    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s GetTextRects %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, boxNum);

    const DK_RECT rtBegin = GetRectFromBox(pTextBoxes[0]);
    FreeRects(pTextBoxes);

    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s _x = %d   [%d, %d]", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, _x, rtBegin.left, rtBegin.right);
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s _y = %d,  [%d, %d]", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, _y, rtBegin.top,  rtBegin.bottom);

    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return (_x <= rtBegin.right && _y <= rtBegin.bottom);
}

void BookTextController::ValidPositionAndRects()
{
    if (NULL == m_pPage)
    {
        return ;
    }
    if (m_posBegin > m_posEnd)
    {
        DK_FLOWPOSITION temp = m_posEnd;
        m_posEnd = m_posBegin;
        m_posBegin = temp;
    }

//     DebugPrintf(DLC_TRACE, "m_posBegin = (%d, %d, %d)", m_posBegin.nChapterIndex, m_posBegin.nParaIndex, m_posBegin.nElemIndex);
//     DebugPrintf(DLC_TRACE, "m_posEnd  = (%d, %d, %d)", m_posEnd.nChapterIndex, m_posEnd.nParaIndex, m_posEnd.nElemIndex);
    DK_BOX *pTextBoxes = DK_NULL;
    DK_UINT uBoxNum    = 0;
    DK_FLOWPOSITION posEndOfPage = m_pPage->GetEndPosition();
    const bool beginPosInCurPage = PosInCurPage(m_posBegin);
    const bool endPosInCurPage = PosInCurPage(m_posEnd) || (m_posEnd == posEndOfPage);
    DK_FLOWPOSITION posStart = beginPosInCurPage ? m_posBegin : m_pPage->GetBeginPosition();
    DK_FLOWPOSITION posEnd = endPosInCurPage ? m_posEnd : posEndOfPage;
    if (GetTextRects(posStart, posEnd, &pTextBoxes, &uBoxNum)) 
    {
//         DebugPrintf(DLC_TRACE, "uBoxNum = %d", uBoxNum);
        if (beginPosInCurPage)
        {
            int minIndex = GetMinBoxIndex(pTextBoxes, (int)uBoxNum);
            const DK_BOX& boxStart = (minIndex >= 0 && minIndex < (int)uBoxNum) ? pTextBoxes[minIndex] : pTextBoxes[0];
//             DebugPrintf(DLC_TRACE, "boxStart = (%f, %f, %f, %f)", boxStart.X0, boxStart.Y0, boxStart.X1, boxStart.Y1);
            m_rectStart = GetRectFromBox(boxStart);
        }
        else
        {
            m_rectStart.left = m_rectStart.top = m_rectStart.right = m_rectStart.bottom = 0;
        }

        if (endPosInCurPage)
        {
            int maxIndex = GetMaxBoxIndex(pTextBoxes, (int)uBoxNum);
            const DK_BOX& boxEnd = (maxIndex >= 0 && maxIndex < (int)uBoxNum) ? pTextBoxes[maxIndex] : pTextBoxes[uBoxNum - 1];
//             DebugPrintf(DLC_TRACE, "boxEnd   = (%f, %f, %f, %f)", boxEnd.X0, boxEnd.Y0, boxEnd.X1, boxEnd.Y1);
            m_rectEnd = GetRectFromBox(boxEnd);
            m_pointLast = m_pointEnd = DK_POS(boxEnd.X1 - 5, boxEnd.Y1 - 5);
        }
        else
        {
            m_rectEnd.left = m_rectEnd.top = m_rectEnd.right = m_rectEnd.bottom = 0;
            m_pointLast = m_pointEnd = DK_POS(0, 0);
        }
    }
}

bool BookTextController::PosInCurPage(const DK_FLOWPOSITION& pos) const
{
    if (m_pPage)
    {
        DK_FLOWPOSITION  posBegin = m_pPage->GetBeginPosition();
        DK_FLOWPOSITION  posEnd = m_pPage->GetEndPosition();
        return (pos >= posBegin && pos < posEnd);
    }
    return false;
}

void BookTextController::SetPageUpLeftAndRightDownPoint(const DK_POS& _pointUpLeft, const DK_POS& _pointDownRight)
{
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_pointUpLeft = _pointUpLeft;
    m_pointDownRight  = _pointDownRight;
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s END [%d, %d]", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_iBaseLeft, m_iBaseTop);
}

bool BookTextController::GetPosition(int _x, int _y, CT_RefPos& _CurPos)
{
    if (NULL == m_pPage)
    {
        return false;
    }

    const DK_POS pointStart = DK_POS(_x - SELECTEDCONTROLER_DIFFDISTANCE, _y);
    const DK_POS pointEnd   = DK_POS(_x + SELECTEDCONTROLER_DIFFDISTANCE, _y);
    DK_FLOWPOSITION posStart, posEnd;

    BOOL bRet = GetSelectionRange(pointStart, pointEnd, &posStart, &posEnd);
    if (bRet)
    {
        _CurPos = CT_RefPos(posStart.nChapterIndex, posStart.nParaIndex, posStart.nElemIndex, 0);
    }

    return bRet;
}

bool BookTextController::GetPosition(int _x, int _y, DK_FLOWPOSITION& position, bool getStart)
{
    if (NULL == m_pPage)
    {
        return false;
    }

    const DK_POS pointStart = getStart ? DK_POS(_x - 1, _y - 1) : m_pointUpLeft;
    const DK_POS pointEnd   = getStart ? m_pointDownRight : DK_POS(_x + 1, _y + 1);
    DK_FLOWPOSITION posStart, posEnd;

    BOOL bRet = GetSelectionRange(pointStart, pointEnd, &posStart, &posEnd);

    string selectionString;
    if (bRet && GetStringBetweenPos(posStart, posEnd, selectionString) && (selectionString.length() > 0))
    {
        wstring selection = EncodeUtil::ToWString(selectionString);
        DK_WCHAR curChar = getStart? selection[0] : selection[selection.length() - 1];

        if (LaddishCharUtil::IsIdentifierChar(curChar)
            || LaddishCharUtil::IsLatinExLetter(curChar)
            || LaddishCharUtil::IsCyrillicLetter(curChar))
        {
            bRet = GetSelectionRangeByMode(pointStart, pointEnd, (int)DKE_SELECTION_INTERCROSS_WORD, &posStart, &posEnd);
        }
    }

    position = getStart ? posStart : posEnd;
    return bRet;
}
