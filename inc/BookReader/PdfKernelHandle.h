//===========================================================================
// Summary:
//    PdfKernelHandle.h pdf库接口.
// Version:
//    0.1 draft - creating.
// Usage:
//    Null
// Remarks:
//    Null
// Date:
//    2011-12-06
// Author:
//        Zhang JingDan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef __KINDLEAPP_INC_PDFKERNELHANDLE_H__
#define __KINDLEAPP_INC_PDFKERNELHANDLE_H__

#include <vector>
#include "IBookReader.h"
#include "IDKPDoc.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/SelectedControler.h"
#else
#include "AppUI/CursorControler.h"
#endif
#include "BookReader/PDFTextElement.h"
#include "BookReader/IPDFKernelHandle.h"
#include "Mutex.h"

using std::vector;

typedef enum
{
    E_THREAD_ACTIVE,
    E_THREAD_SLEEP,
    E_THREAD_EXIT,
}EHandleThreadStatus;

// 用于记录重排页面时的重排区域
struct REARRANGE_SUBPAGE_EDGE
{
    double leftEdge;
    double rightEdge;
    double topEdge;
    double bottomEdge;

    REARRANGE_SUBPAGE_EDGE()
        : leftEdge(0.0),
          rightEdge(0.0),
          topEdge(0.0),
          bottomEdge(0.0)
    {}
};

class PdfKernelHandle : public IPdfKernelHandle
{
public:
    struct CacheData
    {
        unsigned int uPageNum;
        unsigned int uSubPageNum;//以0开始
        unsigned int uSubPageCount;
        DK_IMAGE *pImage;
        PdfModeController mode;
        double scale;
        DK_BOX pageBox;
        std::vector<PDFTextElement> textElementsInPage;
    };
    
    struct ReflowCacheData
    {
        IDKPPageEx *pReflowPage;
        DK_IMAGE *pImage;
        PdfModeController mode;
        DK_BOX pageBox;
        DKP_REARRANGE_PARSER_OPTION parserOption;

        ReflowCacheData()
            : pReflowPage(0)
            , pImage(0)
        {}
    };
public:
    PdfKernelHandle();
    virtual ~PdfKernelHandle();    

    virtual bool Initialize(const wchar_t* pFilePathName);
	virtual void SetTypeSetting(const DKTYPESETTING& typeSetting);
    virtual DK_IMAGE* GetPage(bool needPreRendering = true);
    virtual DK_IMAGE* GetPageStatic();   // 获取当前页面且不启动缓存
    virtual bool SetPage(DK_FLOWPOSITION pos);
    virtual bool Next();
    virtual bool Prev();
    virtual unsigned int GetCurPageNum();
    virtual DK_FLOWPOSITION GetCurPageLocation();
	virtual DK_FLOWPOSITION GetCurPageEndLocation();
    virtual bool SetScreenRes(unsigned int width, unsigned int height);
    virtual void GetScreenRes(unsigned int* pWidth, unsigned int* pHeight);
    virtual unsigned int GetPageCount();
    virtual bool GetPageText(unsigned int pageNum, char** ppTextBuf, unsigned int* pTextLen = DK_NULL);
    virtual void CloseCache(); //文本模式调用此接口停止图片的输出
    virtual bool AddBMPProcessor(IBMPProcessor* pProcessor);
    virtual bool RemoveBMPProcessor(IBMPProcessor* pProcessor);
    virtual bool GetAllBMPProcessor(std::vector<IBMPProcessor*>& ProcessorList);
    virtual bool SetModeController(const PdfModeController& modeController);
    virtual const PdfModeController* GetModeController();
    virtual vector<DK_TOCINFO*>* GetTOC();
    virtual void RegisterFont (const wchar_t* pFontFaceName, const wchar_t* pFontFilePath);
    virtual void SetDefaultFont (const wchar_t* pDefaultFontFaceName, DK_CHARSET_TYPE charset);
    virtual void SetLayoutSettings(const DK_LAYOUTSETTING& layoutSetting);
    virtual bool IsPositionInCurrentPage(const DK_FLOWPOSITION& pos);
    virtual bool SetTextGrayScaleLevel(double gamma);
    virtual bool SetFontSmoothType(DK_FONT_SMOOTH_TYPE fontSmoothType){m_eFontSmoothType = fontSmoothType; return true; };
    virtual DK_FONT_SMOOTH_TYPE GetFontSmoothType() { return m_eFontSmoothType;}
    virtual bool SetGraphicsGrayScaleLevel(double gamma) { m_graphicGamma = gamma; return true; };
    virtual BookTextController* GetBookTextControlerOfCurPage();
    virtual void SetInstantPageModel(bool _bIsInstantMode);
    virtual bool SearchInCurrentBook(const string& strText, const DK_FLOWPOSITION& startPos, const int& iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* puResultCount);

public:
    virtual wchar_t* GetTextContentOfRange(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos);
    virtual void FreeTextContent(wchar_t* pContent) { SafeFreePointerEx(pContent); }
    virtual DK_ReturnCode GetTextRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppTextRects, unsigned int* pRectCount);
    virtual void FreeRects(DK_BOX* pRects);
    virtual PDFTextIterator* GetTextIterator();
    virtual void FreeTextIterator(PDFTextIterator* pPDFTextIterator);
    virtual DK_ReturnCode HitTestTextRange(const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos);
    virtual bool GetPageStartPos (DK_FLOWPOSITION* pPos);
    virtual bool GetPageEndPos (DK_FLOWPOSITION* pPos);
    virtual const DK_TOCINFO* GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode);
    virtual void ParseSinglePage(bool parseSingle);
    virtual bool GetSelectionRange(const DK_POS& startPoint, const DK_POS& endPoint, DKP_SELECTION_MODE selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos);

    virtual bool IsFirstPageInChapter();
    virtual bool IsLastPageInChapter();

private:
    static void* CacheThread(void *pHandler);
    void StopThread();
    void CleanCache();
    void DeleteCacheData(PdfKernelHandle::CacheData *pData);
    bool WakeUpThread();
    void DeleteBeyondPage();
    void LoadCache();
    void LoadCachePage(unsigned int pageNum);
    bool CheckPageIsExistence(unsigned int pageNum);
    DK_IMAGE* PdfDrawPage(unsigned int pageNum, 
        unsigned int width, 
        unsigned int height, 
        double* pScale = NULL, 
        DK_BOX* pPageBox = NULL, 
        std::vector<PDFTextElement>* pTextElementsInPage = NULL);
    DK_IMAGE* PdfDrawPage(unsigned int pageNum, 
        double* pScale = NULL, 
        DK_BOX* pPageBox = NULL, 
        PdfModeController* pModeController = NULL, 
        std::vector<PDFTextElement>* pTextElementsInPage = NULL);
    void InitPageSize();
    bool GetImageEdge(const DK_IMAGE& rImage, DK_RECT* pRect);
    bool NeedReCutEdge(DK_RECT& rCurImageEdgeRect, int imageWidth, int imageHeight, PdfModeController* pModeController = NULL);
    DK_IMAGE* GetStripEdgeImage(unsigned int pageNum, const DK_IMAGE* pSrc, PdfModeController* pModeController = NULL);
    DK_IMAGE* GetPage(unsigned int pageNum, unsigned int* pCacheIndex = NULL, bool needPreRendering = true);
    DK_IMAGE* ScrollModeGetPage();
    bool ScrollModeNext();
    bool ScrollModePrev();
    void FormatToc (IDKPOutline* pOutline, int parentIndex, int depth);
    void GetOutlineCount (IDKPOutline* pOutline, DK_WORD parentIndex, int* pCount);

    // Reflow
    void LoadReflowPages();
    void LoadReflowImages();
    bool ReleaseReflowPage(IDKPPageEx* page);
    bool UpdateReflowScreen();
    ReflowCacheData* GetReflowPage(const DK_FLOWPOSITION& pos,
                                   IDKPDoc::REARRANGE_PAGE_POSITION_TYPE posType);
    DK_IMAGE* GetReflowPageImage(ReflowCacheData* pReflowPage,
                                 bool needPreRendering = true);
    bool PdfGetReflowPage(const DK_FLOWPOSITION& pos,
                          const IDKPDoc::REARRANGE_PAGE_POSITION_TYPE& posType,
                          DKP_REARRANGE_PARSER_OPTION& reflowParserOption,
                          IDKPPageEx** pRefPage);
    IDKPPageEx* GetRenderPageEx(const DK_FLOWPOSITION& pos,
                                const IDKPDoc::REARRANGE_PAGE_POSITION_TYPE& posType,
                                DKP_REARRANGE_PARSER_OPTION& reflowParserOption,
                                IDKPPage** pPage);
    DK_IMAGE* PdfDrawReflowPage(IDKPPageEx *pRearrangePage,
                                DK_IMAGE** pImage,
                                int width,
                                int height,
                                DK_FONT_SMOOTH_TYPE fontSmoothType);
    PdfKernelHandle::ReflowCacheData* LoadReflowCachePage(DK_FLOWPOSITION& currentPos,
                                    IDKPDoc::REARRANGE_PAGE_POSITION_TYPE posType);
    DK_IMAGE* LoadReflowCacheImage(PdfKernelHandle::ReflowCacheData* page);
    bool IsWantedReflowPage(PdfKernelHandle::ReflowCacheData* page,
                            const DK_FLOWPOSITION& currentPos,
                            const DKP_REARRANGE_PARSER_OPTION& reflowParserOption,
                            IDKPDoc::REARRANGE_PAGE_POSITION_TYPE posType);
    PdfKernelHandle::ReflowCacheData* FindCachedReflowPage(const DK_FLOWPOSITION& currentPos,
                                                           const DKP_REARRANGE_PARSER_OPTION& reflowParserOption,
                                                           IDKPDoc::REARRANGE_PAGE_POSITION_TYPE posType);
    PdfKernelHandle::ReflowCacheData* FindCachedReflowPage(IDKPPageEx* target);
    void RemoveRedundantReflowPages(PdfKernelHandle::ReflowCacheData* pCurrentPageCache);
    void ClearReflowCacheData(ReflowCacheData *pData);

    // 建立原版式页的光标表，如果是分栏或者横屏，则需要涉及到单个页面文本元素分拆和多个页面文本元素的拼接。
    void MakeCursorTable(unsigned int cacheIndex);
    inline bool IsZoomedPdf() const;
    //TODO:目前只处理放大的pdf与屏幕坐标的转换，切边，居中保留之前的处理方式
    /**
     * @brief ScreenCoord2ZoomedPdfCoord 坐标转换函数:屏幕坐标--->pdf坐标
     */
    DK_POS ScreenCoord2ZoomedPdfCoord(const DK_POS& screenCoodinate) const;
    /**
     * @brief ZoomedPdfCoord2ScreenCoord 坐标转换函数:pdf坐标---->屏幕坐标
     */
    DK_POS ZoomedPdfCoord2ScreenCoord(const DK_POS& originalCoodinate) const;
    /**
     * @brief GetZoomedAttrbutes 获取缩放的属性
     *
     * @param zoomX0 缩放的右上角坐标x值
     * @param zoomY0 缩放的右上角坐标y值
     *
     * @return 返回是否获取到缩放属性
     */
    bool GetZoomedAttrbutes(double& zoomX0, double& zoomY0) const;

    DK_BOX GetCuttingPageBox(unsigned int pageNum, const PdfModeController& modeController, const DK_BOX& pageBox, double scale);
    inline void ConvertPdfRectToDocRect(const DK_BOX& pageBox, const DK_BOX& cuttingPageBox, double scale, DKP_DRECT& rect);
    inline void ConvertDocRectToViewRect(const PdfModeController& modeController, double xScreenOffset, double yScreenOffset, DKP_DRECT& rect);
    inline void ConvertViewRect(int offset, DKP_DRECT& rect);
//     inline void ConvertViewBox(int offset, DK_BOX& rect) const;
    inline DKP_DRECT GetCuttingPageRect(const DK_BOX& cuttingPageBox, const DK_BOX& pageBox, int topOffset, int bottomOffset, double scale);
    DK_POS ConvertViewPointToDocPoint(const PdfModeController& modeController, const DK_POS& originalCoodinate) const;

	DK_IMAGE* GetZoomRenderImage(const DK_BOX& pageBox, double dScale, DK_RENDERINFO& renderInfo);
	DK_IMAGE* GetRenderImage(int width, int height, DK_RENDERINFO& renderInfo);
    void AddTextElement(unsigned int cacheIndex);
    bool GetScreenOffset(const PdfModeController& modeController, const DK_BOX& pageBox, double& xScreenOffset, double& yScreenOffset);
    void ValidTextRects(DK_BOX** ppTextRects, unsigned int* pRectCount);
    void ValidEdge(unsigned int pageNum, const PdfModeController& modeController, double& left, double& top, double& right, double& bottom, bool forCuttingPageBox = false);
    void ValidZoomedXY(double& zoomX0, double& zoomY0) const;
    void ValidZoomedImageOffset(const DK_BOX& pageBox, double scale, double& xOffset, double& yOffset);
private:
    //线程本身相关
    unsigned int m_uCurSubPageNum;
    unsigned int m_uCurSubPageCount;
    unsigned int m_uPageCount;
    unsigned int m_uCacheListSize;
    unsigned int m_uCurPageNum;
    unsigned int m_uCurPageEndNum;
    unsigned int m_uScreenWidth;
    unsigned int m_uScreenHeight;
    bool m_bMainWait;
    bool m_bMainWaitForReflowPage;
    bool m_bMainWaitForReflowImage;
    bool m_bUpdateCache;
    bool m_bCancelThread;
    unsigned int m_uPageWidth;
    unsigned int m_uPageHeight;
    unsigned int m_uPriorityLoadPage;//优先加载页
    double m_dLastPageZoom;
    unsigned int m_uBookSetScreenWidth;
    unsigned int m_uBookSetScreenHeight;

    IDKPDoc* m_pDoc;
    ReflowCacheData* m_pReflowPage;                     // Current Reflowed Page in Cache
    DKP_REARRANGE_PARSER_OPTION m_rearrangeOption;      // 重排页参数
    DK_FLOWPOSITION m_startPos;                         // 记录重排页位置
    DK_FLOWPOSITION m_endPos;
    DK_FLOWPOSITION m_posForReflowReference;            // Reference for preload
    IDKPDoc::REARRANGE_PAGE_POSITION_TYPE m_posTypeForReflowReference;

    REARRANGE_SUBPAGE_EDGE m_subPageEdge;
    double m_textGamma;
    double m_graphicGamma;
    DK_FONT_SMOOTH_TYPE m_eFontSmoothType;

    EHandleThreadStatus m_eThreadStatus;
    pthread_mutex_t m_listLock;
    pthread_cond_t m_listSignal;
    pthread_cond_t m_threadSignal;

    std::vector<CacheData*> m_cacheList;
    std::vector<ReflowCacheData*> m_cacheReflowData;
    
    std::vector<IBMPProcessor*> m_lstProcessor;
	std::vector<DK_TOCINFO*> m_vTOCList;
    
    PdfModeController m_stModeController;
    DK_IMAGE* m_pOutputImage;
    pthread_mutex_t m_pdfLibLock;

    BookTextController* m_pBookTextController;

    PDFTextElementHandler m_textElmHandler;           // 原版式页面元素管理，包含的元素需已针对应用层的分栏、横屏等模式做过合并等操作。

    bool        m_bIsInstant;       //是否为瞬翻模式
    bool m_parseSingle;
};

#endif

