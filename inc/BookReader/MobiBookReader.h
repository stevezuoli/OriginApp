//===========================================================================
//  Summary:
//      MobiBookReader接口定义
//  Version:
//      0.1 draft - creating.
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2011-12-13
//  Author:
//      juguanhui (juguanhui@duokan.com)
//===========================================================================

#ifndef __KINDLEAPP_INC_MOBIBOOKREADER_H__
#define __KINDLEAPP_INC_MOBIBOOKREADER_H__

#include "BookReader/IBookReader.h"
#include "IDKMBook.h"

class IDKMPage;
class IDKMTocPoint;

//-------------------------------------------
// Summary:
//        MobiBookReader 接口
//-------------------------------------------
class MobiBookReader : public IBookReader
{
public:
    MobiBookReader();
    virtual ~MobiBookReader();
    virtual bool OpenDocument(const string &_strFilename);
    virtual bool CloseDocument();

	virtual void SetTypeSetting(const DKTYPESETTING& typeSetting);
    virtual bool SetPageSize(int _iLeftMargin, int _iTopMargin, int _iWidth, int _iHeight);
    virtual void SetScreenSize(int screenWidth, int screenHeight)
    {
        DK_UNUSED(screenWidth);
        DK_UNUSED(screenHeight);
    }
    virtual bool IsCurrentPageFullScreen()
    {
        return false;
    }
    virtual bool SetDefaultFont(const wstring &_strFaceName, DK_CHARSET_TYPE _charset);
    virtual void SetFontChange(const bool /*_bFontChange*/) {}

    virtual long GetCurrentPageIndex();

    virtual int GotoPrevPage();
    virtual int GotoNextPage();

    virtual vector<DK_TOCINFO*>* GetTOC(DKXBlock* curDKXBlock = NULL);
    virtual int MoveToPage(long _lPageNumber);
	virtual int MoveToFlowPosition(const DK_FLOWPOSITION &_pos);

    virtual string GetCurrentPageText();
    virtual bool SearchInCurrentBook(const string& _strText, const DK_FLOWPOSITION& _startPos, const int& _iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* _puResultCount);
    virtual bool GetCurrentPageATOM(ATOM & beginATOM, ATOM& endATOM);
    virtual string GetText(const ATOM & _clsStartPos, const string _strSearchText, unsigned int & _uHighLightenStartPos, unsigned int & _uHighLightenEndPos);
    virtual bool IsPositionInCurrentPage(const DK_FLOWPOSITION &_pos);
    virtual long GetTotalPageNumber() const { return m_lLogicPageCount; }
    virtual long GetPageNumber4CurrentChapter();

    virtual bool AddBMPProcessor(IBMPProcessor* _pProcessor);
    virtual bool RemoveBMPProcessor(IBMPProcessor* _pProcessor);
    virtual bool GetAllBMPProcessor(std::vector<IBMPProcessor*>& _ProcessorList);

    virtual bool SetLayoutSettings(const DK_LAYOUTSETTING &_clsLayoutSetting);
    virtual DK_IMAGE * GetPageBMP() { return &m_tCurPageImage; }
    virtual bool SetPdfModeController(const PdfModeController *_pPdfModeCtl) { return false; }
    virtual const PdfModeController * GetPdfModeController() { return NULL; }

    virtual BookTextController* GetBookTextControlerOfCurPage() { return m_pBookTextController; }

    virtual BOOKREADER_PARSE_STATUS GetParseStatus() { return BOOKREADER_PARSE_FINISHED; }
    virtual bool GotoBookMark(const ICT_ReadingDataItem* _pReadingEntry);

	//由调用者负责释放返回的对象
    virtual ICT_ReadingDataItem * GetCurPageProgress();

    virtual bool SetTextGrayScaleLevel(double _dGamma) { m_dTextGamma = _dGamma; return true; }
	virtual bool SetFontSmoothType(DK_FONT_SMOOTH_TYPE fontSmoothType);
    virtual bool SetGraphicsGrayScaleLevel(double _dGamma) { m_dGraphicsGamma = _dGamma; return true; }
    virtual bool SetInstantTurnPageModel(bool _bIsInstant) {m_bIsInstant = _bIsInstant; return true; }
    virtual bool SetFamiliarToTraditional(bool toTraditional){m_familiarToTraditional = toTraditional; return true;}

    virtual bool UpdateReadingData(
            const char*,
            const std::vector<ICT_ReadingDataItem *> *_pvOld,
            const char*,
            std::vector<ICT_ReadingDataItem *> *_pvNew);

	//virtual bool GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem *>* vBookmarks);
	virtual bool GetDKXProgressForUpgrade(DKXBlock* curDKXBlock, ICT_ReadingDataItem* localReadingDataItem);
	virtual bool ProcessCloudBookmarks(ReadingBookInfo& bookInfo);
    virtual bool ProcessCloudDKXProgress(ICT_ReadingDataItem* localReadingDataItem);

    virtual void CompletePosRange(CT_RefPos* startPos, CT_RefPos* endPos);
    virtual BOOK_INFO_DATA GetBookInfo();
    virtual PAGE_PROGRESS_DISPLAY_MODE GetPageProgressDisplayMode() const
    {
        return PPDM_PERCENT;
    }

    virtual double GetPercent(const CT_RefPos& startPos);
	virtual bool GetPosByPageIndex(int pageIndex, DK_FLOWPOSITION* pos);
	virtual string GetChapterTitle(const DK_FLOWPOSITION& posChapter, DKXBlock* curDKXBlock = NULL);
	virtual const DK_TOCINFO* GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode, DKXBlock* curDKXBlock = NULL);
protected:
    bool CreateCurPage();
    void DestroyCurPage();
    bool RenderCurPage();
    void DitherImage();
    bool ParseTocInfo(const IDKMTocPoint* tocPoint);
    unsigned int GetChapterIndexByPosition(unsigned int _uPosition);
	void SaveTocToDKX(DKXBlock* curDKXBlock);
	void ReadTocFromDKX(DKXBlock* curDKXBlock);

private:
    IDKMBook *      m_pBook;
    IDKMPage *      m_pPage;
    unsigned int    m_uFileLength;
    long            m_lLogicPageCount;

    DK_UINT                      m_uNewPagePosition;
    IDKMBook::PAGE_POSITION_TYPE m_ePositionType;

    DKM_PARSER_OPTION m_parseOption;
    double            m_dTextGamma;
	DK_FONT_SMOOTH_TYPE m_eFontSmoothType;
    double            m_dGraphicsGamma;
    bool              m_bIsInstant; //是否为瞬翻模式

	vector<DK_TOCINFO*> m_vTOCList;
    DK_IMAGE m_tCurPageImage;
    std::vector<IBMPProcessor *> m_lstBMPProcessor;
    BookTextController* m_pBookTextController;
    std::string m_strBookPath; // 为数据升级使用
    BOOK_INFO_DATA m_bookInfoData;
    bool m_bookInfoDataSet;
    bool m_familiarToTraditional;

};

#endif // __KINDLEAPP_INC_MOBIBOOKREADER_H__

