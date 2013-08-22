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

#ifndef __KINDLEAPP_INC_IBOOKREADER_H__
#define __KINDLEAPP_INC_IBOOKREADER_H__

#include "DKXManager/BaseType/CT_RefPos.h"
#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "BMPProcessor/IBMPProcessor.h"
#include "BookReader/TocInfo.h"
#include "KernelBaseType.h"
#include <vector>
#include <stdint.h>
#include "Common/File_DK.h"
#include "BookStore/ReadingBookInfo.h"
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"

class BookTextController;
class DKXBlock;
using namespace dk::bookstore::sync;
using std::string;
using std::wstring;

#define  DoublePrecision 0.005 //定义导致重新排版的参数精度
#define LINEMAXSTRETCHRATE 2.0
//-------------------------------------------
// Summary:
//        排版参数
//-------------------------------------------
struct DK_LAYOUTSETTING
{
    int    dFontSize;		// 正文字号大小
    double dLineGap;		// 行间距倍数
    double dParaSpacing;	// 段间距倍数
    double dTabStop;		// Tab的大小，以字符宽度为单位
    double dIndent;			// 首行缩进
    bool   bOriginalLayout; // 使用原始版式，会忽略行间距，段间距，首行缩进的设置
    //Fontsize 最好是4的整数，以 24, 28, 32, 48为佳
    DK_LAYOUTSETTING()
        : dFontSize(24)
        , dLineGap(1.5)
        , dParaSpacing(1)
        , dTabStop(2)
        , dIndent(2)
        , bOriginalLayout(false)
    {
    }
};

typedef CT_RefPos ATOM;

//-------------------------------------------
// Summary:
//        PDF阅读模式
//-------------------------------------------
enum TurnPageMode
{
    PDF_TPM_SinglePage,//单页模式
    PDF_TPM_ScrollPage,//滚轴模式
};

enum ReadingMode
{
    PDF_RM_NormalPage,//正常模式
    PDF_RM_Split4Page,//四分栏
    PDF_RM_Split2Page,//两分栏
    PDF_RM_ZoomPage,//放大模式
    PDF_RM_AdaptiveWidth,//宽度自适应
    PDF_RM_Rearrange //重排模式
};

enum CuttingEdgeMode
{
    PDF_CEM_Normal,//正常不切边
    PDF_CEM_Smart,//智能切边
    PDF_CEM_Custom,//自定义手动切边
};

//有对此数值的操作
enum CHAPTER_POSITION_MODE
{
    PRIVIOUS_CHAPTER = 0,
    CURRENT_CHAPTER = 1,
    NEXT_CHAPTER = 2,
};

struct PdfModeController
{
    DK_INT m_iRotation;//当前角度

    TurnPageMode m_eTurnPageMode;
    DK_UINT m_uCurSubPage;//当前子页(分栏模式)
    DK_UINT m_uCurPageStartPos;//当页面上方的位置

    ReadingMode  m_eReadingMode;
    DK_DOUBLE    m_dUserZoom;//放大模式的放大倍数
    DK_DOUBLE    m_zoomX0; //左上角X
    DK_DOUBLE    m_zoomY0; // 左上角Y
    DK_DOUBLE    m_imageX; //图像在放大图片的左上角X
    DK_DOUBLE	 m_imageY;//图片在放大图片的左上角Y
    DK_UINT      m_uSubPageOrder;//分栏后的子页的翻页顺序
    DK_DOUBLE    m_dWidth;//分栏点占宽度百分比
    DK_DOUBLE    m_dHeight;//分栏点占高度百分比

    CuttingEdgeMode m_eCuttingEdgeMode;
    DK_BOOL      m_bIsSmartCutting;
    DK_BOOL      m_bIsNormalCutting; //如果为true为正常切边；false，则存储的是奇页切边数据
    DK_DOUBLE    m_dTopEdge;//上边的边距占高度百分比
    DK_DOUBLE    m_dBottomEdge;//下边边距占高度百分比
    DK_DOUBLE    m_dLeftEdge;//左边边距占宽度百分比
    DK_DOUBLE    m_dRightEdge;//右边边距占宽度百分比
    PdfModeController()
        : m_iRotation(0)
        , m_eTurnPageMode(PDF_TPM_SinglePage)
        , m_uCurSubPage(0)
        , m_uCurPageStartPos(0)
        , m_eReadingMode(PDF_RM_NormalPage)
        , m_dUserZoom(1)
        , m_zoomX0(0)
        , m_zoomY0(0)
        , m_uSubPageOrder(0)
        , m_dWidth(0)
        , m_dHeight(0)
        , m_eCuttingEdgeMode(PDF_CEM_Normal)
        , m_bIsSmartCutting(false)
        , m_bIsNormalCutting(true)
        , m_dTopEdge(0)
        , m_dBottomEdge(0)
        , m_dLeftEdge(0)
        , m_dRightEdge(0)
    {
    }
};

enum BOOKREADER_PARSE_STATUS
{
    BOOKREADER_PARSE_START = 0,
    BOOKREADER_PARSE_ERROR,
    BOOKREADER_PARSE_FINISHED,
};

struct SEARCH_RESULT_DATA
{
    ICT_ReadingDataItem** pSearchResultTable;
    unsigned int* pHighlightStartPosTable;
    unsigned int* pHighlightEndPosTable;
    int* pLocationTable;
};

// 显示页面进度的模式
enum PAGE_PROGRESS_DISPLAY_MODE
{
    PPDM_COUNT,
    PPDM_PERCENT
};

struct BOOK_INFO_DATA
{
    std::string author;
    std::string title;
    const uint8_t* coverData;
    uint32_t coverDataLen;
    BOOK_INFO_DATA()
        : coverData(0)
        , coverDataLen(0)
    {
    }
};

enum BOOK_DIRECTION_TYPE
{
    BOOK_MODE_HTB = 0x10,
    BOOK_MODE_HBT = 0x20,
    BOOK_MODE_VRL = 0x40,
    BOOK_MODE_VLR = 0x80,
    BOOK_DIRECTION_LTR = 0x1,
    BOOK_DIRECTION_RTL = 0x2,
    BOOK_MODE_HTB_DIRECTION_LTR = BOOK_MODE_HTB | BOOK_DIRECTION_LTR,
    BOOK_MODE_HTB_DIRECTION_RTL = BOOK_MODE_HTB | BOOK_DIRECTION_RTL,
    BOOK_MODE_HBT_DIRECTION_LTR = BOOK_MODE_HBT | BOOK_DIRECTION_LTR,
    BOOK_MODE_HBT_DIRECTION_RTL = BOOK_MODE_HBT | BOOK_DIRECTION_RTL,
    BOOK_MODE_VRL_DIRECTION_LTR = BOOK_MODE_VRL | BOOK_DIRECTION_LTR,
    BOOK_MODE_VRL_DIRECTION_RTL = BOOK_MODE_VRL | BOOK_DIRECTION_RTL,
    BOOK_MODE_VLR_DIRECTION_LTR = BOOK_MODE_VLR | BOOK_DIRECTION_LTR,
    BOOK_MODE_VLR_DIRECTION_RTL = BOOK_MODE_VLR | BOOK_DIRECTION_RTL
};

class ParseStatusEventArgs: public EventArgs
{
public:
    ParseStatusEventArgs()
    {}
    virtual EventArgs* Clone() const
    {
        return new ParseStatusEventArgs(*this);
    }
};

class UIBookParseStatusListener: public EventSet
{
public:
    static const char* EventBookParseFinished;
    static UIBookParseStatusListener* GetInstance();
    void FireBookParseFinishedEvent();
};

#define BOOKREADER_GOTOPAGE_SUCCESS 0x0
#define BOOKREADER_GOTOPAGE_FAIL    0x1
#define BOOKREADER_GOTOPAGE_WAIT    0x2

//-------------------------------------------
// Summary:
//        IBookReader 接口
//-------------------------------------------
class IBookReader
{
public:
    virtual ~IBookReader(){};

    //-------------------------------------------
    // Summary:
    //        打开图书
    //-------------------------------------------
    virtual bool OpenDocument(const string &_strFilename) = 0;

    //-------------------------------------------
    // Summary:
    //        关闭图书
    //-------------------------------------------
    virtual bool CloseDocument() = 0;

	//-------------------------------------------
    // Summary:
    //        设置排版模式
    //-------------------------------------------
	virtual void SetTypeSetting(const DKTYPESETTING& typeSetting) = 0;

    //-------------------------------------------
    // Summary:
    //        设置排版页面大小
    //-------------------------------------------
    virtual bool SetPageSize(int _iLeftMargin, int _iTopMargin, int _iWidth, int _iHeight) = 0;
    //
    //-------------------------------------------
    // Summary:
    //        设置屏幕大小
    //-------------------------------------------
    virtual void SetScreenSize(int screenWidth, int screenHeight) = 0;

    //-------------------------------------------
    // Summary:
    //        获得当前页是否全屏页
    //-------------------------------------------
    virtual bool IsCurrentPageFullScreen() = 0;

    //-------------------------------------------
    // Summary:
    //        设置系统缺省字体
    //-------------------------------------------
    virtual bool SetDefaultFont(const wstring &_strFaceName, DK_CHARSET_TYPE _clsCharSet) = 0;

    //-------------------------------------------
    // Summary:
    //        获得当前页的索引
    //-------------------------------------------
    virtual long GetCurrentPageIndex() = 0;

    //-------------------------------------------
    // Summary:
    //        上翻页
    //-------------------------------------------
    virtual int GotoPrevPage() = 0;

    //-------------------------------------------
    // Summary:
    //        下翻页
    //-------------------------------------------
    virtual int GotoNextPage() = 0;

    //-------------------------------------------
    // Summary:
    //        取得目录
    // Remarks:
    //        由于epub 有章内段，所以该段的position 是指向当前章的
    //-------------------------------------------
    virtual vector<DK_TOCINFO*>* GetTOC(DKXBlock* curDKXBlock = NULL) = 0;

    //-------------------------------------------
    // Summary:
    //        跳转到某一页
    //-------------------------------------------
    virtual int MoveToPage(long _lPageNumber) = 0;

    //-------------------------------------------
    // Summary:
    //        取得当前页内文本（For TTS）
    //-------------------------------------------
    virtual string GetCurrentPageText() = 0;

    //-------------------------------------------
    // Summary:
    //        跳转到三级索引制定页
    //-------------------------------------------
    virtual int MoveToFlowPosition(const DK_FLOWPOSITION &_pos) = 0;

    //-------------------------------------------
    // Summary:
    //        跳转到指定的目录
    //-------------------------------------------
    virtual int MoveToTOC(DK_TOCINFO* pCurToc)
    {
        if(pCurToc)
        {
            return MoveToFlowPosition(pCurToc->GetBeginPos());
        }
        return BOOKREADER_GOTOPAGE_FAIL;
    }

    //-------------------------------------------
    // Summary:
    //        取得给定搜索关键字的搜索结果文本。
    //    [in] _clsStartPos 搜索开始的偏移（三级索引）
    //    [in] _strSearchText 待搜索的字符串
    //    [out] _uHighLightenStartPos _strSearchText在返回结果串中的起始位置（用于搜索关键字在结果中的高亮显示）
    //    [out] _uHighLightenEndPos _strSearchText在返回结果串中的结束位置（用于搜索关键字在结果中的高亮显示）
    //    [return] 搜索关键字在该起始位置的搜索结果
    //-------------------------------------------
    virtual string GetText(const ATOM & _clsStartPos, const string _strSearchText, unsigned int & _uHighLightenStartPos, unsigned int & _uHighLightenEndPos) = 0;

    //-------------------------------------------
    // Summary:
    //        取得当前显示内容的三级索引，作为搜索的起点
    //        text mobi 的三级索引是没有chapterIndex 值的
    //-------------------------------------------
    virtual bool GetCurrentPageATOM(ATOM & beginATOM, ATOM& endATOM) = 0;


    //-------------------------------------------
    // Summary:
    //       获取dkx 文件中的readingData
    //-------------------------------------------
    //virtual bool GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem *>* vBookmarks) = 0;

    //-------------------------------------------
    // Summary:
    //       
    //-------------------------------------------
    virtual bool GetDKXProgressForUpgrade(DKXBlock* curDKXBlock, ICT_ReadingDataItem* localReadingDataItem) = 0;

    virtual bool ProcessCloudBookmarks(ReadingBookInfo& bookInfo) = 0;

    virtual bool ProcessCloudDKXProgress(ICT_ReadingDataItem* localReadingDataItem) = 0;

    //-------------------------------------------
    // Summary:
    //        书内搜索
    //-------------------------------------------
    virtual bool SearchInCurrentBook(const string& _strText, const DK_FLOWPOSITION& _startPos, const int& _iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* _puResultCount) = 0;

    //-------------------------------------------
    // Summary:
    //        判断流式坐标是否在当前页
    //-------------------------------------------
    virtual bool IsPositionInCurrentPage(const DK_FLOWPOSITION &_pos) = 0;

    //-------------------------------------------
    // Summary:
    //        获取本书所有页数
    //-------------------------------------------
    virtual long GetTotalPageNumber() const = 0;

    //-------------------------------------------
    // Summary:
    //        获取本章所有页数
    //-------------------------------------------
    virtual long GetPageNumber4CurrentChapter() = 0;

    //-------------------------------------------
    // Summary:
    //        添加图片预处理器
    // 可实现功能：
    //      可以实现页面旋转，锐化
    //-------------------------------------------
    virtual bool AddBMPProcessor(IBMPProcessor* _pProcessor) = 0;

    //-------------------------------------------
    // Summary:
    //        删除图片预处理器
    // 可实现功能：
    //      可以实现页面旋转，锐化
    //-------------------------------------------
    virtual bool RemoveBMPProcessor(IBMPProcessor* _pProcessor) = 0;

    //-------------------------------------------
    // Summary:
    //        枚举图片预处理器
    // 可实现功能：
    //      可以实现页面旋转，锐化
    //-------------------------------------------
    virtual bool GetAllBMPProcessor(std::vector<IBMPProcessor*> &_ProcessorList) = 0;

    //-------------------------------------------
    // Summary:
    //        设置排版参数
    //-------------------------------------------
    virtual bool SetLayoutSettings(const DK_LAYOUTSETTING &_clsLayoutSetting) = 0;
    //-------------------------------------------
    // Summary:
    //        目前设计中reader不知道排版字体是否被改变，因此需要提供接口供外部设置
    //-------------------------------------------
    virtual void SetFontChange(const bool bFontChange) = 0;
    //-------------------------------------------
    // Summary:
    //        取出当前页的渲染BMP结果
    // 可实现功能：
    //      仅有接口，由各个格式支持去添加 cache
    //-------------------------------------------
    virtual DK_IMAGE* GetPageBMP() = 0;

    //-------------------------------------------
    // Summary:
    //        获取当前的解析状态
    //-------------------------------------------
    virtual BOOKREADER_PARSE_STATUS GetParseStatus() = 0;

    //-------------------------------------------
    // Summary:
    //        设置PDF阅读模式
    //-------------------------------------------
    virtual bool SetPdfModeController(const PdfModeController* pPdfModeCtl) = 0;

    //-------------------------------------------
    // Summary:
    //        获取PDF阅读模式
    //-------------------------------------------
    virtual const PdfModeController* GetPdfModeController() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取本书当前页的光标控制器
    //-------------------------------------------
    virtual BookTextController* GetBookTextControlerOfCurPage() = 0;

    //-------------------------------------------
    //  Summary:
    //      从DKX中读取当前阅读信息: 如进度/书摘/书签
    //-------------------------------------------
    virtual bool GotoBookMark(const ICT_ReadingDataItem* _pReadingEntry) = 0;

    //-------------------------------------------
    //  Summary:
    //      从当前阅读界面获取阅读进度，以书签形式返回。由外部调用者负责释放返回对象的内存，并将阅读进度存入DKX
    //      text mobi 的三级索引是没有chapterIndex 值的
    //-------------------------------------------
    virtual ICT_ReadingDataItem* GetCurPageProgress() = 0;


    //-------------------------------------------
    //  Summary:
    //      更改字体渲染颜色
    //-------------------------------------------
    virtual bool SetTextGrayScaleLevel(double _dGamma) = 0;

    //-------------------------------------------
    //  Summary:
    //      更改字体渲染方式(平滑、锐化)
    //-------------------------------------------
    virtual bool SetFontSmoothType(DK_FONT_SMOOTH_TYPE _eFontSmoothType) =0;

    //-------------------------------------------
    //  Summary:
    //      更改字体渲染颜色
    //-------------------------------------------
    virtual bool SetGraphicsGrayScaleLevel(double _dGamma) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置是否进行简体转繁体
    //-------------------------------------------
    virtual bool SetFamiliarToTraditional(bool toTraditional) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置是否为瞬翻
    //-------------------------------------------
    virtual bool SetInstantTurnPageModel(bool _bIsInstant) = 0;

    //-------------------------------------------
    //  Summary:
    //      进行数据升级，返回是否需要重写dkx
    //-------------------------------------------
    virtual bool UpdateReadingData(
        const char* oldBookRevision,
        const std::vector<ICT_ReadingDataItem *> *_pvOld,
        const char* newBookRevision,
        std::vector<ICT_ReadingDataItem *> *_pvNew) = 0;

    //-------------------------------------------
    //  Summary:
    //      补充完ct_refpos,对于只有flow position,没有offset的填充offset
    //-------------------------------------------
    virtual void CompletePosRange(CT_RefPos* startPos, CT_RefPos* endPos) = 0;

    virtual PAGE_PROGRESS_DISPLAY_MODE GetPageProgressDisplayMode() const = 0;

    static IBookReader* CreateBookReaderFromFormat(DkFileFormat fileFormat);

    virtual BOOK_INFO_DATA GetBookInfo()
    {
        return BOOK_INFO_DATA(); 
    }

    //-------------------------------------------
    //  Summary:
    //      给定三级索引，求对应全文的百分比，未解析完返回-1
    //-------------------------------------------
    virtual double GetPercent(const CT_RefPos& startPos) = 0;

    //
    //-------------------------------------------
    //  Summary:
    //      给定三级索引，求对应全文的页码,以0开始，未解析完返回-1
    //-------------------------------------------
    virtual long GetPageIndex(const CT_RefPos& startPos)
    {
        return -1;
    }

	//-------------------------------------------
    //  Summary:
    //      给定页码，返回该页码起始位置的三级索引
    //-------------------------------------------
    virtual bool GetPosByPageIndex(int pageIndex, DK_FLOWPOSITION* pos)
    {
        return false;
    }

    //-------------------------------------------
    //  Summary:
    //      给定章节索引，返回该章节的title
    //-------------------------------------------
    virtual string GetChapterTitle(const DK_FLOWPOSITION& posChapter, DKXBlock* curDKXBlock = NULL)
    {
        return "";
    }

    //-------------------------------------------
    //  Summary:
    //      给定三级索引，章节位置，返回所要章节的目录信息
    //-------------------------------------------
    virtual const DK_TOCINFO* GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode, DKXBlock* curDKXBlock = NULL) = 0;


    //-------------------------------------------
    //  Summary:
    //      获取文件版本号
    //-------------------------------------------
    virtual std::string GetBookRevision()
    {
        return "";
    }
    //
    //-------------------------------------------
    //  Summary:
    //      获取文件id
    //-------------------------------------------
    virtual std::string GetBookId()
    {
        return "";
    }

    //-------------------------------------------
    //  Summary:
    //      是否是已购买的多看书籍
    //-------------------------------------------
    virtual bool IsPurchasedDuoKanBook()
    {
        return false;
    }

    //-------------------------------------------
    //  Summary:
    //      获取书籍排版方向
    //-------------------------------------------
    virtual BOOK_DIRECTION_TYPE GetBookDirection() const
    {
        return BOOK_MODE_HTB_DIRECTION_LTR;
    }

    //-------------------------------------------
    //  Summary:
    //      是否对应章节是漫画章节
    //-------------------------------------------
    virtual bool IsComicsChapter(long chapterIndex) const
    {
        return false;
    }

    virtual bool SetComicsFrameMode(bool comicsFrame, long frameIndex = -1)
    {
        return true;
    }
    virtual bool IsComicsFrameMode() const
    {
        return false;
    }

    virtual bool GotoNextFrameInGallery(const DK_POS& pos) { return false; }
    virtual bool GotoPreFrameInGallery(const DK_POS& pos) { return false; }

    virtual bool IsFirstPageInChapter() { return false; }
    virtual bool IsLastPageInChapter() { return false; }
};

#endif

