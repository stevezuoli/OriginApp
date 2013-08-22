#ifndef __KINDLEAPP_INC_IPDFKERNELHANDLE_H__
#define __KINDLEAPP_INC_IPDFKERNELHANDLE_H__

#include "IBookReader.h"

class IDKPDoc;
class IDKPOutline;
class PDFTextIterator;

class IPdfKernelHandle
{    
public:
    virtual ~IPdfKernelHandle() {}    

    virtual bool Initialize(const wchar_t* pFilePathName) = 0;
    virtual DK_IMAGE* GetPage(bool needPreRendering = true) = 0;
    virtual DK_IMAGE* GetPageStatic() = 0;
    virtual bool SetPage(DK_FLOWPOSITION pos) = 0;
    virtual bool Next() = 0;
    virtual bool Prev() = 0;
    virtual unsigned int GetCurPageNum() = 0;
    virtual DK_FLOWPOSITION GetCurPageLocation() = 0;
    virtual bool SetScreenRes(unsigned int width, unsigned int height) = 0;
    virtual void GetScreenRes(unsigned int* pWidth, unsigned int* pHeight) = 0;
    virtual unsigned int GetPageCount() = 0;
    virtual bool GetPageText(unsigned int pageNum, char** ppTextBuf, unsigned int* pTextLen = DK_NULL) = 0;
    virtual void CloseCache() = 0;
    virtual bool AddBMPProcessor(IBMPProcessor* pProcessor) = 0;
    virtual bool RemoveBMPProcessor(IBMPProcessor* pProcessor) = 0;
    virtual bool GetAllBMPProcessor(std::vector<IBMPProcessor*>& ProcessorList) = 0;
    virtual bool SetModeController(const PdfModeController& modeController) = 0;
    virtual const PdfModeController* GetModeController() = 0;
    virtual vector<DK_TOCINFO*>* GetTOC() = 0;
    virtual void RegisterFont (const wchar_t* pFontFaceName, const wchar_t* pFontFilePath) = 0;
    virtual void SetDefaultFont (const wchar_t* pDefaultFontFaceName, DK_CHARSET_TYPE charset) = 0;
    virtual void SetLayoutSettings(const DK_LAYOUTSETTING& layoutSetting) = 0;
    virtual bool IsPositionInCurrentPage(const DK_FLOWPOSITION& pos) = 0;
    virtual bool SetTextGrayScaleLevel(double gamma) = 0;
	virtual bool SetFontSmoothType(DK_FONT_SMOOTH_TYPE fontSmoothType) =0;
    virtual DK_FONT_SMOOTH_TYPE GetFontSmoothType() = 0;
    virtual bool SetGraphicsGrayScaleLevel(double gamma) = 0;
    virtual BookTextController* GetBookTextControlerOfCurPage() = 0;
	virtual void SetInstantPageModel(bool _bIsInstantMode) = 0;
    virtual bool SearchInCurrentBook(const string& strText, const DK_FLOWPOSITION& startPos, const int& iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* puResultCount) = 0;

    virtual bool IsFirstPageInChapter() = 0;
    virtual bool IsLastPageInChapter() = 0;

public:
    virtual wchar_t* GetTextContentOfRange(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos) = 0;
    virtual void FreeTextContent(wchar_t* pContent) = 0;
    virtual DK_ReturnCode GetTextRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppTextRects, unsigned int* pRectCount) = 0;
    virtual void FreeRects(DK_BOX* pRects) = 0;
    virtual PDFTextIterator* GetTextIterator() = 0;
    virtual void FreeTextIterator(PDFTextIterator* pPDFTextIterator) = 0;
    virtual DK_ReturnCode HitTestTextRange(const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;
    virtual bool GetPageStartPos (DK_FLOWPOSITION* pPos) = 0;
    virtual bool GetPageEndPos (DK_FLOWPOSITION* pPos) = 0;
    virtual void ParseSinglePage(bool parseSingle) = 0;
    virtual bool GetSelectionRange(const DK_POS& startPoint, const DK_POS& endPoint, DKP_SELECTION_MODE selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;
};

#endif // #ifndef __KINDLEAPP_INC_IPDFKERNELHANDLE_H__
