//===========================================================================
// Summary:
//    PdfBookReader.
// Version:
//    0.1 draft - creating.
// Usage:
//    Null
// Remarks:
//    Null
// Date:
//    2011-12-06
// Author:
//    Null
//===========================================================================

#ifndef __KINDLEAPP_INC_PDFBOOKREADER_H__
#define __KINDLEAPP_INC_PDFBOOKREADER_H__

#include "IBookReader.h"
#include <vector>
#include "PdfKernelHandle.h"

using std::vector;

class PdfBookReader: public IBookReader
{
public:
    PdfBookReader();
    virtual ~PdfBookReader();

    virtual bool OpenDocument(const string &_strFilename);
    virtual bool CloseDocument();

	virtual void SetTypeSetting(const DKTYPESETTING& typeSetting);
    virtual bool SetPageSize(int _iLeftMargin, int _iTopMargin, int Width, int Length);
    virtual void SetScreenSize(int screenWidth, int screenHeight)
    {
        DK_UNUSED(screenWidth);
        DK_UNUSED(screenHeight);
    }

    virtual bool IsCurrentPageFullScreen()
    {
        return false;
    }

	virtual long GetPageIndex(const CT_RefPos& startPos);

    virtual bool SetDefaultFont(const wstring &_strFaceName, DK_CHARSET_TYPE charset);
	virtual void SetFontChange(const bool bFontChange);

    virtual int GotoPrevPage();
    virtual int GotoNextPage();

    virtual vector<DK_TOCINFO*>* GetTOC(DKXBlock* curDKXBlock = NULL);

    virtual int MoveToChapter(int ChapterNumber ) { return MoveToPage(ChapterNumber); }
    virtual int MoveToPage(long pageNum);
	virtual int MoveToFlowPosition(const DK_FLOWPOSITION &_pos);
    virtual bool GotoPage(int pageIndex) { return BOOKREADER_GOTOPAGE_SUCCESS == MoveToPage((long)pageIndex); }
    virtual long GetCurrentPageIndex();

    virtual bool IsFirstPageInChapter();
    virtual bool IsLastPageInChapter();

    virtual string GetCurrentPageText();
    virtual bool GetCurrentPageATOM(ATOM & beginATOM, ATOM& endATOM)
    {
        if (m_pPdfHandler)
        {
            DK_FLOWPOSITION curPos = m_pPdfHandler->GetCurPageLocation();
            DK_FLOWPOSITION endPos = m_pPdfHandler->GetCurPageEndLocation();
            beginATOM.SetChapterIndex(curPos.nChapterIndex);
            beginATOM.SetParaIndex(curPos.nParaIndex);
            beginATOM.SetAtomIndex(curPos.nElemIndex);
            endATOM.SetChapterIndex(endPos.nChapterIndex);
            endATOM.SetParaIndex(endPos.nParaIndex);
            endATOM.SetAtomIndex(endPos.nElemIndex);
            return true;
        }
        return false;
    }
	//virtual bool GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem *>* vBookmarks);
	virtual bool GetDKXProgressForUpgrade(DKXBlock* curDKXBlock, ICT_ReadingDataItem* localReadingDataItem);
	virtual bool ProcessCloudBookmarks(ReadingBookInfo& bookInfo);
    virtual bool ProcessCloudDKXProgress(ICT_ReadingDataItem* localReadingDataItem);
	
    virtual string GetText(const ATOM & _clsStartPos, const string _strSearchText, unsigned int & _uHighLightenStartPos, unsigned int & _uHighLightenEndPos)    {    return "";    }

    virtual bool SearchInCurrentBook(const string& _strText, const DK_FLOWPOSITION& _startPos, const int& _iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* _puResultCount)  
    { 
        return m_pPdfHandler && m_pPdfHandler->SearchInCurrentBook(_strText, _startPos, _iMaxResultCount, _pSearchResultData, _puResultCount);
    }
    virtual bool IsPositionInCurrentPage(const DK_FLOWPOSITION &_pos);
    virtual long GetTotalPageNumber() const;
    virtual long GetPageNumber4CurrentChapter();

    virtual bool AddBMPProcessor(IBMPProcessor* pProcessor);
    virtual bool RemoveBMPProcessor(IBMPProcessor* pProcessor);
    virtual bool GetAllBMPProcessor(vector<IBMPProcessor*>& ProcessorList);
    virtual BOOK_DIRECTION_TYPE GetBookDirection() const;

    virtual bool SetLayoutSettings(const DK_LAYOUTSETTING &_clsLayoutSetting);
    virtual DK_IMAGE* GetPageBMP();
    virtual bool SetPdfModeController(const PdfModeController* pPdfModeCtl);
    virtual const PdfModeController* GetPdfModeController() { return m_pPdfHandler->GetModeController(); }
    
    virtual BookTextController* GetBookTextControlerOfCurPage() { return m_pPdfHandler->GetBookTextControlerOfCurPage(); }

    virtual BOOKREADER_PARSE_STATUS   GetParseStatus() { return BOOKREADER_PARSE_FINISHED; }
    virtual bool GotoBookMark(const ICT_ReadingDataItem* _pReadingEntry);
    virtual ICT_ReadingDataItem* GetCurPageProgress();

    virtual bool SetTextGrayScaleLevel(double _dGamma) { return m_pPdfHandler && m_pPdfHandler->SetTextGrayScaleLevel(_dGamma); };
	virtual bool SetFontSmoothType(DK_FONT_SMOOTH_TYPE _eFontSmoothType) { return m_pPdfHandler && m_pPdfHandler->SetFontSmoothType(_eFontSmoothType); };
    virtual bool SetGraphicsGrayScaleLevel(double _dGamma) { return false; };

    virtual bool SetInstantTurnPageModel(bool _bIsInstant);   //…Ë÷√À≤∑≠ƒ£ Ω
    virtual bool SetFamiliarToTraditional(bool toTraditional){return false;}
    virtual bool UpdateReadingData(
            const char*,
            const std::vector<ICT_ReadingDataItem *> * /*_pvOld*/,
            const char*,
            std::vector<ICT_ReadingDataItem *> * /*_pvNew*/)
    { 
        return false;
    }
    virtual void CompletePosRange(CT_RefPos* /*startPos*/, CT_RefPos* /*endPos*/){};
    virtual PAGE_PROGRESS_DISPLAY_MODE GetPageProgressDisplayMode() const
    {
        return PPDM_COUNT;
    }

    virtual double GetPercent(const CT_RefPos& startPos);
	virtual string GetChapterTitle(const DK_FLOWPOSITION& posChapter, DKXBlock* curDKXBlock = NULL);
	virtual const DK_TOCINFO* GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode, DKXBlock* curDKXBlock = NULL);
    void ParseSinglePage(bool parseSingle);
private:
    void InitFont();

private:
    PdfKernelHandle* 	m_pPdfHandler;
    bool 				m_bInitialized;
	string				m_strFilePath;
};

#endif
