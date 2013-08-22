//===========================================================================
// Summary:
//        IBookReader接口定义
// Version:
//    0.1 draft - creating.
//    
// Usage:
//        Null
// Remarks:
//        Null
// Date:
//        2011-11-21
// Author:
//        Chen Mao (chenm@duokan.com)
//===========================================================================

#ifndef __KINDLEAPP_INC_EPUBBOOKREADER_H__
#define __KINDLEAPP_INC_EPUBBOOKREADER_H__

#include "IBookReader.h"
#include "IDKEBook.h" 
#include "Common/LockObject.h"

// defined in this header
class EpubBookReader;
class CPageTable;
class IDKEGallery;
class BookTextController;
//-------------------------------------------
// Summary:
//        EpubBookReader 接口
//-------------------------------------------

struct SortingVector
{
    SortingVector(const vector<DK_TOCINFO*>& tocList)
        : m_tocList(tocList)
    {
    }
    bool operator()(const int& key1, const int& key2) const
    {
        return m_tocList[key1]->GetBeginPos() < m_tocList[key2]->GetBeginPos();
    }
private:
    const vector<DK_TOCINFO*> m_tocList;
};

class EpubBookReader : public IBookReader
{
public:
    EpubBookReader();
    virtual ~EpubBookReader();

    virtual bool CloseDocument();
    virtual bool OpenDocument(const string &_strFilename);
	virtual void SetTypeSetting(const DKTYPESETTING& typeSetting);
    virtual bool SetPageSize(int _iLeftMargin, int _iTopMargin, int _iWidth, int _iHeight);
    virtual void SetScreenSize(int screenWidth, int screenHeight);
    virtual bool IsCurrentPageFullScreen();

    virtual bool SetDefaultFont(const wstring &_strFaceName, DK_CHARSET_TYPE _charset);
    virtual void SetFontChange(const bool bFontChange);

    virtual long GetCurrentPageIndex();

    virtual int GotoPrevPage();
    virtual int GotoNextPage();

    virtual vector<DK_TOCINFO*>* GetTOC(DKXBlock* curDKXBlock = NULL);
    virtual int MoveToChapter(int _iChapterNumber );
    virtual int MoveToPage(long _lPageNumber);
    virtual int MoveToFlowPosition(const DK_FLOWPOSITION &_pos);
    virtual int MoveToTOC(DK_TOCINFO* pCurToc);

    virtual string GetCurrentPageText();
    virtual bool   SearchInCurrentBook(const string& _strText, const DK_FLOWPOSITION& _startPos, const int& _iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* _puResultCount);
    virtual bool   GetCurrentPageATOM(ATOM & beginATOM, ATOM& endATOM);
    virtual string GetText(const ATOM & _clsStartPos, const string _strSearchText, unsigned int & _uHighLightenStartPos, unsigned int & _uHighLightenEndPos);
    virtual bool   IsPositionInCurrentPage(const DK_FLOWPOSITION &_pos);
    virtual long   GetTotalPageNumber() const;
    virtual long   GetPageNumber4CurrentChapter();

    virtual bool AddBMPProcessor(IBMPProcessor* _pProcessor);
    virtual bool RemoveBMPProcessor(IBMPProcessor* _pProcessor);
    virtual bool GetAllBMPProcessor(std::vector<IBMPProcessor*>& _ProcessorList);

    virtual bool      SetLayoutSettings(const DK_LAYOUTSETTING &_clsLayoutSetting);
    virtual DK_IMAGE* GetPageBMP();
    virtual bool      SetPdfModeController(const PdfModeController* _pPdfModeCtl) { return false; }
    virtual const PdfModeController* GetPdfModeController() { return NULL; }

    virtual BookTextController* GetBookTextControlerOfCurPage() { return m_pBookTextController; }

    virtual BOOKREADER_PARSE_STATUS   GetParseStatus() { return m_eParseStatus; }
    IDKEBook*        GetDKEBooK()    {    return m_pDKEBook;    }
    virtual bool GotoBookMark(const ICT_ReadingDataItem* _pReadingEntry);
    virtual ICT_ReadingDataItem* GetCurPageProgress();
    //virtual bool GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem *>* vBookmarks);
    virtual bool GetDKXProgressForUpgrade(DKXBlock* curDKXBlock, ICT_ReadingDataItem* localReadingDataItem);
    virtual bool ProcessCloudBookmarks(ReadingBookInfo& bookInfo) { return true; };
    virtual bool ProcessCloudDKXProgress(ICT_ReadingDataItem* localReadingDataItem) { return true; };

    virtual bool SetTextGrayScaleLevel(double _dGamma);
    virtual bool SetFontSmoothType(DK_FONT_SMOOTH_TYPE _eFontSmoothType);
    virtual bool SetGraphicsGrayScaleLevel(double _dGamma);

    virtual bool SetInstantTurnPageModel(bool _bIsInstant);   //设置瞬翻模式

    virtual bool SetFamiliarToTraditional(bool toTraditional);


    virtual bool UpdateReadingData(
        const char* oldBookRevision,
        const std::vector<ICT_ReadingDataItem *> * /*_pvOld*/,
        const char* newBookRevision,
        std::vector<ICT_ReadingDataItem *> * /*_pvNew*/);

    virtual void CompletePosRange(CT_RefPos* startPos, CT_RefPos* endPos);
    virtual BOOK_INFO_DATA GetBookInfo();

    virtual PAGE_PROGRESS_DISPLAY_MODE GetPageProgressDisplayMode() const
    {
        return PPDM_COUNT;
    }
    virtual double GetPercent(const CT_RefPos& startPos);
    virtual long GetPageIndex(const CT_RefPos& startPos);
	virtual bool GetPosByPageIndex(int pageIndex, DK_FLOWPOSITION* pos);
    virtual std::string GetBookRevision();
    virtual bool IsPurchasedDuoKanBook();
    virtual std::string GetBookId()
    {
        return m_bookId;
    }

    virtual string GetChapterTitle(const DK_FLOWPOSITION& posChapter, DKXBlock* curDKXBlock = NULL);
    virtual const DK_TOCINFO* GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode, DKXBlock* curDKXBlock = NULL);
    virtual BOOK_DIRECTION_TYPE GetBookDirection() const;
    virtual bool IsComicsChapter(long chapterIndex) const;

    virtual bool SetComicsFrameMode(bool comicsFrame, long frameIndex = -1);
    virtual bool IsComicsFrameMode() const;

    virtual bool GotoNextFrameInGallery(const DK_POS& pos);
    virtual bool GotoPreFrameInGallery(const DK_POS& pos);

    virtual bool IsFirstPageInChapter();
    virtual bool IsLastPageInChapter();

    void       SetStopParsingFlag();
    //仅供文内注渲染使用
    bool RenderSinglePage(IDKEPage* epubPage);
private:
    static bool  NeedsDrmOpen(const char* file);
    bool      ReParseContent(int chapter);
    IDKEBook* DrmOpen(const char* file);
    static void*  ParseThread(void* _pParam);
    void       ParseChapter(long _lChapterIndex);
    DKE_CHAPTER_TYPE GetPageChapterType(IDKEPage* page);
    // 根据三级索引获取跳转到所在页，只在GotoBookMark接口中调用
    bool       GotoPage(const DK_FLOWPOSITION& pos);
    // 根据指定的章索引跳转到所在页
    bool       GotoPage(long _lChapterIndex, long _lPageIndex, bool bReverse = false);

    bool       ProduceBMPPage(IDKEPage* epubPage);
    bool       RenderPage();
    void       DitherImage(IDKEPage* epubPage);
    bool ProcessImage(DK_IMAGE& image);
    bool       ParseTocInfo(IDKETocPoint* _pTocPoint);
    void       ClearVector();
    bool	   UpdateAnchorChapter(DK_TOCINFO* pTocInfo);
    bool ComicsFrameModeValid() const;
    void ResetComicsFrameMode();
    int NextPage();
    int NextFrame();
    int PrevPage();
    int PrevFrame();
    bool InstantValid();
    void ClearParsedPages();
    void UpdateGallery(IDKEGallery* pGallery);
    void GetRenderOption(const DK_IMAGE& image, DK_BITMAPBUFFER_DEV& dev, DK_FLOWRENDEROPTION& renderOption);

    enum GALLERY_TURNPAGE
    {
        PAGEDOWN = 0,
        PAGEUP = 1,
        PAGECUR = 2,
    };
    bool UpdateFrameInGallery(const DK_POS& pos, GALLERY_TURNPAGE turnPage);
    pthread_t    m_pParseThread;
    IDKEBook*    m_pDKEBook;
    IDKEPage*    m_pDKEPage;
    string       m_strDKEPageText; 
    string       m_strDKEFilePath;
    string       m_bookId;
    long         m_lPageIndex;
    long         m_lPageCount;
    long         m_lChapterCount;
    bool         m_bReParse;
    bool         m_bStopParse;
    bool 		 m_bIsInstant;		//是否为瞬翻模式

    DKE_PARSER_OPTION       m_clsDKEParseOption; 
    BOOKREADER_PARSE_STATUS m_eParseStatus;

    // 记录当前页的索引位置
    DK_FLOWPOSITION      m_tDKEFlowPos;
    vector<DK_TOCINFO*>	 m_vTOCList;
    DK_IMAGE             m_tCurPageImage;   //应该不用指针就行，只要打开页面都会存在改图像 
    DK_LAYOUTSETTING     m_tLayoutSetting;

    double              m_iTextGamma;
    double              m_iGraphicsGamma;
    DK_FONT_SMOOTH_TYPE m_eFontSmoothType;
    int                 m_screenWidth;
    int                 m_screenHeight;


    std::vector<IBMPProcessor*> m_lstBMPProcessor;
    std::vector<bool>    m_vChapterParseStatus;
    std::vector<long>    m_vPageIndexOfChapter;
    std::vector<long>    m_vPageCountOfChapter;
    std::vector<std::vector<CT_RefPos> > m_vEndPosOfChapter;
    BookTextController* m_pBookTextController;
    BOOK_INFO_DATA       m_bookInfoData;
    bool                 m_bookInfoDataSet;
    bool                 m_familiarToTraditional;
    CPageTable* m_pPageTable;
    bool m_comicsFrame;
    long m_globalFrame;
    dk::common::LockObject m_parseLock;
};
#endif // __KINDLEAPP_INC_EPUBBOOKREADER_H__

