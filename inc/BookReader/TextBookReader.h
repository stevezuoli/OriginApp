//===========================================================================
// Summary:
//        TEXTBOOKREADER 实现TEXT阅读
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

#ifndef __KINDLEAPP_INC_TEXTBOOKREADER_H__
#define __KINDLEAPP_INC_TEXTBOOKREADER_H__

#include "IBookReader.h"

#include "../../Kernel/include/TxtKit/TxtLib/IDKTBook.h"
#include "../../Kernel/include/TxtKit/TxtLib/DKTDef.h"
#include "GUI/dkImage.h"
#include "KernelEncoding.h"
#include "DkStream.h"
#include "DKXManager/DKX/DKXManager.h"
#include "Utility/ThreadHelper.h"
#include <string>
#include <vector>
#include "BookReader/PageTable.h"

using std::string;
using std::vector;

//-------------------------------------------
// Summary:
//        TextBookReader 接口
//-------------------------------------------

#define     MAX_CHARS_PER_PAGE        256

class TextBookReader:public IBookReader
{
public:
    TextBookReader();
    ~TextBookReader();

	//仅供LayoutReader使用
    bool    OpenDocumentFromString(const string& strDemo);
    virtual bool    OpenDocument(const string &_strFilename);
    virtual bool    CloseDocument();

	virtual void SetTypeSetting(const DKTYPESETTING& typeSetting);
    virtual bool    SetPageSize(int _iLeftMargin, int _iTopMargin, int _iWidth, int _iLength);
    virtual void SetScreenSize(int screenWidth, int screenHeight)
    {
        DK_UNUSED(screenWidth);
        DK_UNUSED(screenHeight);
    }

    virtual bool IsCurrentPageFullScreen()
    {
        return false;
    }

    //remove it
    //bool SetFontSize(int _iFontSize);
    //bool    AddFont(const string _strFontPath);

    virtual bool    SetDefaultFont(const wstring &_strFaceName, DK_CHARSET_TYPE charset);

	virtual void SetFontChange(const bool bFontChange);
    /*
    *返回false，表示当前已是第一物理页，无法再往前翻
    */
    virtual int    GotoPrevPage();
    /*
    *返回false，表示当前已到达文件结尾，无法再往后翻
    */
    virtual int    GotoNextPage();

    virtual vector<DK_TOCINFO*>* GetTOC(DKXBlock* curDKXBlock = NULL);

	virtual int    MoveToPage(long pageNumber);
	virtual int    MoveToFlowPosition(const DK_FLOWPOSITION & targetPos);
	//仅供LayoutReader使用
	int MoveToPageDirect(long _lPageOffSet);

    virtual long    GetCurrentPageIndex();
    virtual string  GetCurrentPageText();
	virtual long 	GetPageIndex(const CT_RefPos& startPos);
	virtual bool GetPosByPageIndex(int pageIndex, DK_FLOWPOSITION* pos);

    virtual bool    GetCurrentPageATOM(ATOM & beginATOM, ATOM& endATOM);
    virtual string  GetText(const ATOM & _clsStartPos, const string _strSearchText, unsigned int & _uHighLightenStartPos, unsigned int & _uHighLightenEndPos);

    virtual bool    SearchInCurrentBook(const string& _strText, const DK_FLOWPOSITION& _startPos, const int& _iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* _puResultCount);

    virtual bool    IsPositionInCurrentPage(const DK_FLOWPOSITION &_pos);
    virtual long    GetTotalPageNumber() const;
    virtual long    GetPageNumber4CurrentChapter();

    virtual bool    AddBMPProcessor(IBMPProcessor* _pclsProcessor);
    virtual bool    RemoveBMPProcessor(IBMPProcessor* _pclsProcessor);
    virtual bool    GetAllBMPProcessor(vector<IBMPProcessor*>& _vProcessorList);

    virtual bool    GotoBookMark(const ICT_ReadingDataItem* _pReadingEntry);
    virtual bool    SetLayoutSettings(const DK_LAYOUTSETTING &_pclsLayoutSetting);

    virtual bool    SetPdfModeController(const PdfModeController* pPdfModeCtl){ return DK_FALSE; }
    
    virtual DK_IMAGE*   GetPageBMP();

    virtual const   PdfModeController*  GetPdfModeController() { return NULL; }

    virtual BookTextController* GetBookTextControlerOfCurPage() { return m_pBookTextController; }

    virtual BOOKREADER_PARSE_STATUS     GetParseStatus();

    virtual ICT_ReadingDataItem* GetCurPageProgress();
    virtual bool SetTextGrayScaleLevel(double _dGamma);
	virtual bool SetFontSmoothType(DK_FONT_SMOOTH_TYPE _eFontSmoothType);
    virtual bool SetGraphicsGrayScaleLevel(double _dGamma) { return true; };	

    virtual bool SetInstantTurnPageModel(bool _bIsInstant);   //设置瞬翻模式


    virtual bool SetFamiliarToTraditional(bool toTraditional);

    virtual bool UpdateReadingData(
            const char*,
            const std::vector<ICT_ReadingDataItem *> * /*_pvOld*/,
            const char*,
            std::vector<ICT_ReadingDataItem *> * /*_pvNew*/) 
    {
        return false;
    }

	//virtual bool GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem *>* vBookmarks);
	virtual bool GetDKXProgressForUpgrade(DKXBlock* curDKXBlock, ICT_ReadingDataItem* localReadingDataItem);
	virtual bool ProcessCloudBookmarks(ReadingBookInfo& bookInfo);
    virtual bool ProcessCloudDKXProgress(ICT_ReadingDataItem* localReadingDataItem);
	
    virtual void CompletePosRange(CT_RefPos* startPos, CT_RefPos* endPos);
    virtual PAGE_PROGRESS_DISPLAY_MODE GetPageProgressDisplayMode() const
    {
        return PPDM_COUNT;
    }

    virtual double GetPercent(const CT_RefPos& startPos);
	virtual string GetChapterTitle(const DK_FLOWPOSITION& posChapter, DKXBlock* curDKXBlock = NULL);
	virtual const DK_TOCINFO* GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode, DKXBlock* curDKXBlock = NULL);
	void StopParseThread();
	
private:
    bool    ReParseContent();
    bool    GotoPage(unsigned int pageOffset, IDKTBook::PAGE_POSITION_TYPE position);
    bool    ReRenderPage();
    bool    ProduceBMPPage();
    int     GetChapterIndex(unsigned int pageOffset, DKXBlock* curDKXBlock);
    void    ReadTocFromDKX(DKXBlock* curDKXBlock);
    void DestroyCurPage();

	//从解析结果(m_vPageStartOffset)中获取该offset属于那一页，因为m_vPageStartOffset中可能会是-1，所以不能直接取
	int		GetPageIndexFormParseResult(int iOffset);

	//根据索引获取offset,如果该页offset为-1的话，解析该页附近的页数据
	//只有在有分页结果时才能调用此函数
	int		GetOffSetByPageIndex(int pageIndex);

	//解析局部的数据，必须保证m_pBook已经设置相关变量
	//且m_vPageStartOffset[startPageIndex]的值是正确的
	bool	ParsePartContent(int startPageIndex, int endPageIndex);
	void Parse();
	static void*  ParseThread(void* _pParam);
	bool GetPageTableData();
#ifdef DEBUG 
	void    ReadTocFromDKX2(); 
#endif
    void    SaveTocToDKX(DKXBlock* curDKXBlock);
    bool    AddTag(string _strTagName);
    bool    RemoveTag(string _strTagName);

private:
    string              m_strBookPath;
    IDKTBook*           m_pBook;
    IDKTPage*           m_pCurPage;
    BookTextController*   m_pBookTextController;
    DK_LAYOUTSETTING    m_LayoutSetting;
    DKT_PARSER_OPTION   m_ParserOption;
    long                m_lFileSize;

    int                 m_iBookID;
    DK_IMAGE            m_tCurPageImage;
    int                 m_iPageWidth;
    int                 m_iPageHeight;
    double              m_dTextGamma;
	DK_FONT_SMOOTH_TYPE m_eFontSmoothType;
    bool                m_bTypeset;			//是否应该重新解析该书
	unsigned int		m_uLastPageOffset;
    vector<IBMPProcessor*>  m_vBMPProcessor;
    vector<string>      m_vBookTags;
	vector<DK_TOCINFO*> m_vTOCList;
	bool 				m_bIsInstant;		//是否为瞬翻模式
	bool				m_bIsFirstOpen;		//是否是首次打开书，用于判断是不是应该重新解析

	pthread_t    m_pParseThread;
	bool		 m_bIsCancelThread;
	BOOKREADER_PARSE_STATUS m_eParseStatus;
	DK_FLOWPOSITION      m_tDKEFlowPos; 
    DK_FLOWPOSITION      m_tDKEEndPos; 
	long		m_lCurPageIndex;

	vector<long> m_vPageStartOffset;

	//此处供阅读设置页面使用，在GetPageBMP前要要保持此内存
	//否则有可能引起因m_strDemo内存释放而使DKT_OpenDocumentFromStream 指向野指针
	string m_strDemo;
};

#endif

