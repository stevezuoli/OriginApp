#ifndef __APPUI_UIBOOKREADERSELECTED_H__
#define __APPUI_UIBOOKREADERSELECTED_H__

#include "BookReader/IBookReader.h"
#include "CommonUI/BookTextController.h"

class UIBookReaderSelected
{
public:
    UIBookReaderSelected();
    ~UIBookReaderSelected();
    static const char* GetClassName()
    {
        return "UIBookReaderSelected";
    }
    enum CommentDrawType
    {
        CDT_WaveLine,
        CDT_GrayRect
    };
    bool Initialize(BookTextController *pBookTextControler);
    bool IsInitialized() const;
public:
    bool IsSelectionMode() const;
    void SetSelectionMode(bool _bInSelection);
    bool SetStartPos(int _x, int _y,int _iDiffDistance, bool _isVerticalForms);
    void SetPageUpLeftAndRightDownPoint(const DK_POS& _pointUpLeft, const DK_POS& _pointDownRight);

    void SetBasePoint(int _iLeft, int _iTop);
    void ResetDrawMode();
    void SetDrawMode(unsigned int _uDrawFlag);
    bool Draw(DK_IMAGE _drawingImg, DK_RECT *_pRect);

    bool Move(int _x, int _y, bool rightHandler);
    bool IsEndOfPage(int _x, int _y);
    bool IsBeginOfPage(int _x, int _y);
    bool MoveToEndOfPage();
    bool MoveToBeginOfPage();

    void ValidPositionAndRects();

    DK_RECT GetStartRect() const ;
    DK_RECT GetEndRect() const ;

    bool GetSelection(ATOM *_pBeginPos, ATOM *_pEndPos, string& bookComment);
    bool GetHitTestBoundBox(const DK_POS& _Point, DK_BOX** _pBoundingBox, DK_UINT* _pBoundBoxCount, bool* _bIsHitTest);
    bool FreeHitTestBoundBox(DK_BOX* _pBoundingBox);

    bool CheckIsLink(const DK_POS& _point, bool& isInternalLink);
    bool CheckInGallery(const DK_POS& _point);
    bool GetLinkFlowPosition(const DK_POS& _point, DK_FLOWPOSITION* _posLink);
    bool GetExternelLink(const DK_POS& _point, std::string& url);

    bool GetPosition(int _x, int _y, CT_RefPos& _CurPos);
    DK_FLOWPOSITION GetStartPos();
    DK_FLOWPOSITION GetEndPos();
    DK_POS GetStartPoint();
    DK_POS GetLastPoint();
    DK_POS GetEndPoint();
    bool GetHitObject(const DK_POS& _point, DKE_HITTEST_OBJECTINFO* pObjInfo);
    void FreeHitObject(DKE_HITTEST_OBJECTINFO* pObjInfo);

    void GetRectsBetweenPos(const DK_FLOWPOSITION &_pos1, const DK_FLOWPOSITION &_pos2, std::vector<DK_RECT> *_pvRects);


    bool HitTestFootnote(const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo);
    void FreeHitTestFootnote(DKE_FOOTNOTE_INFO* pFootnoteInfo);
    bool GetFootnoteContentHandle(const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle);
    void FreeContentHandle(DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle);
    bool InitInteractiveObjects(std::vector<PageObjectInfo>& interactiveObjects);
    bool InitComicsObjects(std::vector<PageObjectInfo>& comicsObjects);
    bool InitGalleryObjects(std::vector<GalleryObjectInfo>& galleryObjects);

    bool GetStringBetweenPos(const ATOM &pos1, const ATOM &pos2, std::string &bookContent);

    //返回最后一个rect，用于阅读界面标“注”的位置
    DK_RECT  DrawComment(const ATOM &_beginPos, const ATOM &_endPos, DK_IMAGE _drawingImg, CommentDrawType drawType);
    int  DrawDigest(const ATOM &_beginPos, const ATOM &_endPos, DK_IMAGE _drawingImg);

    //===========================================================================
    //  漫画页面相关
    //===========================================================================
    DK_LONG HitTestComicsFrame(DK_POS point);

    bool HitTestGallery(const DK_POS& point, IDKEGallery** ppGallery);
    void FreeHitTestGallery(IDKEGallery* pGallery);

    bool SelectingInGallery() const;
    void SetSelectionRangeMode(int mode);
    void ReSetSelectionRangeMode();

private:
    void DrawGrayBG(const DK_IMAGE &_drawingImg, DK_RECT _rect);
    void DrawSelection(const DK_IMAGE &_drawingImg, DK_RECT _rect);
    void DrawLine(DK_RECT _rect, const DK_IMAGE &_drawingImg);
    void RectUpdate(DK_RECT *_pResult, const DK_RECT &_add);
    void MakeRectValid(DK_RECT* pRect, int maxX, int maxY);
    // 灰阶转二值，并反色
    inline void Gray2InvBin(BYTE *_color);
    //inline void ReverseColor(BYTE *_color) { *_color = 255 - *_color; }
public:
    // 设置绘图模式，便于外部控制，Draw采用异或（取反），两次Draw即为擦除
    static const unsigned int DRAW_ALL_SELECTION = 0x0004;
    static const unsigned int DRAW_NEW_SELECTION = 0x0008;
private:
    BookTextController *m_pBookTextController;
    unsigned int       m_uDrawFlag;
};

#endif //__APPUI_UIBOOKREADERSELECTED_H__

