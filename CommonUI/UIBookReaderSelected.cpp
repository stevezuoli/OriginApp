#include "CommonUI/UIBookReaderSelected.h"
#include "Utility/ImageManager.h"
#include "GUI/CTpGraphics.h"
#include "interface.h"

UIBookReaderSelected::UIBookReaderSelected()
    : m_pBookTextController(NULL)
    , m_uDrawFlag(0)
{
}

UIBookReaderSelected::~UIBookReaderSelected()
{
}

bool UIBookReaderSelected::Initialize(BookTextController *pBookTextControler)
{
    m_pBookTextController = pBookTextControler;
    return true;
}

bool UIBookReaderSelected::IsInitialized() const 
{
    return m_pBookTextController && m_pBookTextController->IsInitialized(); 
}

bool UIBookReaderSelected::IsSelectionMode() const
{
    return m_pBookTextController && m_pBookTextController->IsSelectionMode();
}

void UIBookReaderSelected::SetSelectionMode(bool bInSelection)
{
    if (m_pBookTextController)
    {
        m_pBookTextController->SetSelectionMode(bInSelection);
    }
}

bool UIBookReaderSelected::SetStartPos(int x, int y, int iDiffDistance, bool _isVerticalForms)
{
    return m_pBookTextController && m_pBookTextController->SetStartPos(x, y, iDiffDistance, _isVerticalForms);
}

void UIBookReaderSelected::SetBasePoint(int iLeft, int iTop)
{
    if (m_pBookTextController)
        m_pBookTextController->SetBasePoint(iLeft, iTop);
}

void UIBookReaderSelected::SetPageUpLeftAndRightDownPoint(const DK_POS& pointUpLeft, const DK_POS& pointDownRight)
{
     if (m_pBookTextController)
        m_pBookTextController->SetPageUpLeftAndRightDownPoint(pointUpLeft, pointDownRight);
}


void UIBookReaderSelected::ResetDrawMode()
{
    m_uDrawFlag = 0;
}

void UIBookReaderSelected::SetDrawMode(unsigned int uDrawFlag)
{
    m_uDrawFlag ^= uDrawFlag;
}

bool UIBookReaderSelected::Draw(DK_IMAGE drawingImg, DK_RECT *pRect)
{
    if (NULL == pRect || 0 == m_uDrawFlag || !m_pBookTextController)
    {
        return false;
    }

    std::vector<DK_RECT> selectRect;
    if (DRAW_ALL_SELECTION & m_uDrawFlag)
    {
        m_pBookTextController->GetSelectionRectsOfThisPage(&selectRect);
        //DebugPrintf(DLC_DIAGNOSTIC, "UIBookReaderSelected::DrawAll, select rect size: %d", selectRect.size());
    }
    else if (DRAW_NEW_SELECTION & m_uDrawFlag)
    {
        m_pBookTextController->GetSelectionRectsOfLastMove(&selectRect);
        //DebugPrintf(DLC_DIAGNOSTIC, "UIBookReaderSelected::DrawLastMove, select rect size: %d", selectRect.size());
    }
    else
    {
    }

    if (selectRect.size() > 0)
    {
        std::vector<DK_RECT>::iterator vi = selectRect.begin();
        *pRect = *vi;
        MakeRectValid(pRect, drawingImg.iWidth, drawingImg.iHeight);
        for (; vi != selectRect.end(); ++vi)
        {
            //DebugPrintf(DLC_DIAGNOSTIC, "DrawSelectRect: (%d, %d)~(%d, %d)", vi->left, vi->top, vi->right, vi->bottom);
            MakeRectValid(&(*vi), drawingImg.iWidth, drawingImg.iHeight);
            DrawSelection(drawingImg, *vi);
            RectUpdate(pRect, *vi);
        }
    }
    
    m_uDrawFlag = 0;
    return (selectRect.size() > 0);
}

bool UIBookReaderSelected::Move(int x, int y, bool rightHandler)
{
    return m_pBookTextController && m_pBookTextController->Move(x, y, rightHandler);
}

bool UIBookReaderSelected::IsEndOfPage(int x, int y)
{
    return m_pBookTextController && m_pBookTextController->IsEndOfPage(x, y);
}

bool UIBookReaderSelected::IsBeginOfPage(int x, int y)
{
    return m_pBookTextController && m_pBookTextController->IsBeginOfPage(x, y);
}

bool UIBookReaderSelected::MoveToEndOfPage()
{
    return m_pBookTextController && m_pBookTextController->MoveToEndOfPage();
}

DK_RECT UIBookReaderSelected::GetStartRect() const
{
    DK_RECT rect = {0, 0, 0, 0};
    if (m_pBookTextController)
    {
        rect = m_pBookTextController->GetStartRect();
    }
    return rect;
}

DK_RECT UIBookReaderSelected::GetEndRect() const
{
    DK_RECT rect = {0, 0, 0, 0};
    if (m_pBookTextController)
    {
        rect = m_pBookTextController->GetEndRect();
    }
    return rect;
}

bool UIBookReaderSelected::MoveToBeginOfPage()
{
    return m_pBookTextController && m_pBookTextController->MoveToBeginOfPage();
}

void UIBookReaderSelected::ValidPositionAndRects()
{
    if (m_pBookTextController)
    {
        m_pBookTextController->ValidPositionAndRects();
    }
}

bool UIBookReaderSelected::GetHitObject(const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo)
{
    return m_pBookTextController && m_pBookTextController->GetHitObject(point, pObjInfo);
}

void UIBookReaderSelected::FreeHitObject(DKE_HITTEST_OBJECTINFO* pObjInfo)
{
    if (m_pBookTextController)
        m_pBookTextController->FreeHitObject(pObjInfo);
}

void UIBookReaderSelected::GetRectsBetweenPos(const DK_FLOWPOSITION &_pos1, const DK_FLOWPOSITION &_pos2, std::vector<DK_RECT> *_pvRects)
{
    if (m_pBookTextController)
    {
        m_pBookTextController->GetRectsBetweenPos(_pos1, _pos2, _pvRects);
    }
}

bool UIBookReaderSelected::HitTestFootnote(const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo)
{
    return m_pBookTextController && m_pBookTextController->HitTestFootnote(point, pFootnoteInfo);
}

void UIBookReaderSelected::FreeHitTestFootnote(DKE_FOOTNOTE_INFO* pFootnoteInfo)
{
    if (m_pBookTextController)
        m_pBookTextController->FreeHitTestFootnote(pFootnoteInfo);
}

DK_LONG UIBookReaderSelected::HitTestComicsFrame(DK_POS point)
{
    return m_pBookTextController  ? m_pBookTextController->HitTestComicsFrame(point) : -1;
}

bool UIBookReaderSelected::HitTestGallery(const DK_POS& point, IDKEGallery** ppGallery)
{
    return m_pBookTextController && m_pBookTextController->HitTestGallery(point, ppGallery);
}

void UIBookReaderSelected::FreeHitTestGallery(IDKEGallery* pGallery)
{
    if (m_pBookTextController)
    {
        m_pBookTextController->FreeHitTestGallery(pGallery);
    }
}

bool UIBookReaderSelected::SelectingInGallery() const
{
    return m_pBookTextController && m_pBookTextController->SelectingInGallery();
}

void UIBookReaderSelected::ReSetSelectionRangeMode()
{
    if (m_pBookTextController)
    {
#ifdef KINDLE_FOR_TOUCH
        m_pBookTextController->SetSelectionRangeMode(DKE_SELECTION_INTERCROSS);
#else
        m_pBookTextController->SetSelectionRangeMode(DKE_SELECTION_INTERCROSS_WORD);
#endif
    }
}

void UIBookReaderSelected::SetSelectionRangeMode(int mode)
{
    if (m_pBookTextController)
    {
        m_pBookTextController->SetSelectionRangeMode(mode);
    }
}

bool UIBookReaderSelected::InitInteractiveObjects(std::vector<PageObjectInfo>& interactiveObjects)
{
   return m_pBookTextController && m_pBookTextController->InitInteractiveObjects(interactiveObjects);
}

bool UIBookReaderSelected::InitComicsObjects(std::vector<PageObjectInfo>& comicsObjects)
{
    return m_pBookTextController && m_pBookTextController->InitComicsObjects(comicsObjects);
}

bool UIBookReaderSelected::InitGalleryObjects(std::vector<GalleryObjectInfo>& galleryObjects)
{
    return m_pBookTextController && m_pBookTextController->InitGalleryObjects(galleryObjects);
}

bool UIBookReaderSelected::GetFootnoteContentHandle(const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
{
    return m_pBookTextController && m_pBookTextController->GetFootnoteContentHandle(pNoteId, showType, pHandle);
}

void UIBookReaderSelected::FreeContentHandle(DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle)
{
    if (m_pBookTextController)
    {
        m_pBookTextController->FreeContentHandle(showType, pHandle);
    }
}

bool UIBookReaderSelected::GetSelection(ATOM *pBeginPos, ATOM *pEndPos, string& bookComment)
{
    if (!m_pBookTextController)
        return false;
    
    DK_FLOWPOSITION beginPos;
    DK_FLOWPOSITION endPos;
    bool bRet = m_pBookTextController->GetSelectionContent(&beginPos, &endPos, bookComment);
    if (bRet)
    {
        pBeginPos->SetChapterIndex(beginPos.nChapterIndex);
        pBeginPos->SetParaIndex(beginPos.nParaIndex);
        pBeginPos->SetAtomIndex(beginPos.nElemIndex);
        pEndPos->SetChapterIndex(endPos.nChapterIndex);
        pEndPos->SetParaIndex(endPos.nParaIndex);
        pEndPos->SetAtomIndex(endPos.nElemIndex);
    }
    
    return bRet;
}

bool UIBookReaderSelected::GetHitTestBoundBox(
        const DK_POS& point,
        DK_BOX** pBoundingBox,
        DK_UINT* pBoundBoxCount,
        bool* bIsHitTest)
{
    if (!m_pBookTextController || !pBoundingBox)
    {
        return false;
    }
    return m_pBookTextController->GetHitTestBoundBox(
            point,
            pBoundingBox,
            pBoundBoxCount,
            bIsHitTest);
}

bool UIBookReaderSelected::FreeHitTestBoundBox(DK_BOX* pBoundingBox)
{
    if(!pBoundingBox || !m_pBookTextController)
    {
        return false;
    }
    m_pBookTextController->FreeHitTestBoundBox(pBoundingBox);
    return true;
}

bool UIBookReaderSelected::CheckIsLink(const DK_POS& point, bool& isInternalLink)
{
    return m_pBookTextController && m_pBookTextController->CheckIsLink(point, isInternalLink);
}

bool UIBookReaderSelected::CheckInGallery(const DK_POS& point)
{
    return m_pBookTextController && m_pBookTextController->CheckInGallery(point);
}

bool UIBookReaderSelected::GetLinkFlowPosition(const DK_POS& point, DK_FLOWPOSITION* posLink)
{
    return m_pBookTextController && m_pBookTextController->GetLinkFlowPosition(point, posLink);
}

bool UIBookReaderSelected::GetExternelLink(const DK_POS& _point, std::string& url)
{
    return m_pBookTextController && m_pBookTextController->GetLinkExternalURL(_point, url);
}

bool UIBookReaderSelected::GetPosition(int x, int y, CT_RefPos& CurPos)
{
    return m_pBookTextController && m_pBookTextController->GetPosition(x, y, CurPos);
}

DK_FLOWPOSITION UIBookReaderSelected::GetStartPos()
{
    if(!m_pBookTextController)
    {
        return DK_FLOWPOSITION();
    }
    return m_pBookTextController->GetStartPos();
}

DK_FLOWPOSITION UIBookReaderSelected::GetEndPos()
{
    if(!m_pBookTextController)
    {
        return DK_FLOWPOSITION();
    }
    return m_pBookTextController->GetEndPos();
}

DK_POS UIBookReaderSelected::GetStartPoint()
{
    if(!m_pBookTextController)
    {
        return DK_POS();
    }
    return m_pBookTextController->GetStartPoint();
}

DK_POS UIBookReaderSelected::GetLastPoint()
{
    if(!m_pBookTextController)
    {
        return DK_POS();
    }
    return m_pBookTextController->GetLastPoint();
}

DK_POS UIBookReaderSelected::GetEndPoint()
{
    if(!m_pBookTextController)
    {
        return DK_POS();
    }
    return m_pBookTextController->GetEndPoint();
}

DK_RECT UIBookReaderSelected::DrawComment(const ATOM &beginAtom, const ATOM &endAtom, DK_IMAGE drawingImg, CommentDrawType drawType)
{
    DK_RECT lastRect;
    lastRect.top = lastRect.left = lastRect.right = lastRect.bottom = -1;
    if (!m_pBookTextController)
        return lastRect;
    
    DK_FLOWPOSITION beginPos(beginAtom.GetChapterIndex(), beginAtom.GetParaIndex(), beginAtom.GetAtomIndex());
    DK_FLOWPOSITION endPos(endAtom.GetChapterIndex(), endAtom.GetParaIndex(), endAtom.GetAtomIndex());

    std::vector<DK_RECT> digestRect;
    m_pBookTextController->GetRectsBetweenPos(beginPos, endPos, &digestRect);

    unsigned int size = digestRect.size();
    for (unsigned int i = 0; i < size; ++i)
    {
        if (drawType == CDT_GrayRect)
        {
            DrawGrayBG(drawingImg, digestRect[i]);
        }
        else
        {
            DrawLine(digestRect[i], drawingImg);
        }
    }
    if (size > 0)
    {
        return digestRect[size - 1];
    }
    return lastRect;
}

int UIBookReaderSelected::DrawDigest(const ATOM &beginAtom, const ATOM &endAtom, DK_IMAGE drawingImg)
{
    if (!m_pBookTextController)
        return -1;
    
    DK_FLOWPOSITION beginPos(beginAtom.GetChapterIndex(), beginAtom.GetParaIndex(), beginAtom.GetAtomIndex());
    DK_FLOWPOSITION endPos(endAtom.GetChapterIndex(), endAtom.GetParaIndex(), endAtom.GetAtomIndex());

    std::vector<DK_RECT> digestRect;
    m_pBookTextController->GetRectsBetweenPos(beginPos, endPos, &digestRect);

    int ret = -1;
    for (unsigned int i = 0; i < digestRect.size(); ++i)
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "######UIBookReaderSelected::DrawDigest %d, %d, %d, %d", 
                //digestRect[i].left, digestRect[i].top, digestRect[i].right, digestRect[i].bottom);
        MakeRectValid(&digestRect[i], drawingImg.iWidth, drawingImg.iHeight);
        //DebugPrintf(DLC_DIAGNOSTIC, "######adjusted, UIBookReaderSelected::DrawDigest %d, %d, %d, %d", 
                //digestRect[i].left, digestRect[i].top, digestRect[i].right, digestRect[i].bottom);
        DrawGrayBG(drawingImg, digestRect[i]);
    }
    return ret;
}

void UIBookReaderSelected::DrawGrayBG(const DK_IMAGE &drawingImg, DK_RECT rect)
{
    if (!m_pBookTextController)
        return;
    
    BYTE *pbData = drawingImg.pbData;
    pbData += rect.top * drawingImg.iWidth;
    
    for (int i = rect.top; i < rect.bottom && i < drawingImg.iHeight; ++i)
    {
        for (int j = rect.left; j < rect.right && j < drawingImg.iWidth; ++j)
        {
            BYTE* pDraw = pbData + j;
            *pDraw = (*pDraw < 0x40)? 0x33: *pDraw;
            //*pDraw = (*pDraw < 0x80)? 0x33: 0xFF;
        }

        pbData += drawingImg.iWidth;
    }
}

void UIBookReaderSelected::DrawSelection(const DK_IMAGE &drawingImg, DK_RECT rect)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "UIBookReaderSelected::DrawSelectRect: %d,%d,%d,%d", rect.left, rect.top, rect.right, rect.bottom);
    if (!m_pBookTextController)
        return;
    
    BYTE *pbData = drawingImg.pbData;
    pbData += rect.top * drawingImg.iWidth;
    
    for (int i = rect.top; i < rect.bottom && i < drawingImg.iHeight; ++i)
    {
        for (int j = rect.left; j < rect.right && j < drawingImg.iWidth; ++j)
        {
            Gray2InvBin(pbData + j);
        }

        pbData += drawingImg.iWidth;
    }
}

void UIBookReaderSelected::DrawLine(DK_RECT rect, const DK_IMAGE &drawingImg)
{
    if (!m_pBookTextController)
        return;
    
    SPtr<ITpImage> spImageCursor = ImageManager::GetImage(IMAGE_WAVE_LINE);
    if (0 == spImageCursor->bmWidth || 0 == spImageCursor->bmHeight)
    {
        return;
    }

    CTpGraphics grf(drawingImg);
    int iYInTarget = rect.bottom - spImageCursor->bmHeight + 2;
    int iXInTarget = rect.left;

    for (; iXInTarget < rect.right - (int)spImageCursor->bmWidth; iXInTarget += spImageCursor->bmWidth)
    {
        grf.DrawImageBlend(spImageCursor.Get(), iXInTarget, iYInTarget, 0, 0, spImageCursor->bmWidth, spImageCursor->bmHeight);
    }

    if (iXInTarget < rect.right)
    {
        grf.DrawImageBlend(spImageCursor.Get(), iXInTarget, iYInTarget, 0, 0, rect.right - iXInTarget, spImageCursor->bmHeight);
    }
}

void UIBookReaderSelected::MakeRectValid(DK_RECT* pRect, int maxX, int maxY)
{
    pRect->top = pRect->top > 0 ? pRect->top : 0;
    pRect->bottom = pRect->bottom > 0 ? pRect->bottom : 0;
    pRect->left = pRect->left > 0 ? pRect->left : 0;
    pRect->right = pRect->right > 0 ? pRect->right : 0;

    pRect->top = pRect->top > maxY ? maxY : pRect->top;
    pRect->bottom = pRect->bottom > maxY ? maxY : pRect->bottom;
    pRect->left = pRect->left > maxX ? maxX : pRect->left;
    pRect->right = pRect->right > maxX ? maxX : pRect->right;
}

void UIBookReaderSelected::RectUpdate(DK_RECT *pResult, const DK_RECT &add)
{
    pResult->top = dk_min(pResult->top, add.top);
    pResult->bottom = dk_max(pResult->bottom, add.bottom);
    pResult->left = dk_min(pResult->left, add.left);
    pResult->right = dk_max(pResult->right, add.right);
}

void UIBookReaderSelected::Gray2InvBin(BYTE *color)
{
    *color = (*color & 0x80)? 0x00: 0xFF;
}

bool UIBookReaderSelected::GetStringBetweenPos(const ATOM &pos1, const ATOM &pos2, std::string &bookContent)
{
    DK_FLOWPOSITION beginPos(pos1.GetChapterIndex(), pos1.GetParaIndex(), pos1.GetAtomIndex());
    DK_FLOWPOSITION endPos(pos2.GetChapterIndex(), pos2.GetParaIndex(), pos2.GetAtomIndex());
    return m_pBookTextController && m_pBookTextController->GetStringBetweenPos(beginPos, endPos, bookContent);
}

