#ifndef UIREADERSETTINGDLG_H
#define UIREADERSETTINGDLG_H

#include <pthread.h>
#include <vector>
#include "GUI/UIBookReaderProgressBar.h"
#include "GUI/UIDialog.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "TouchAppUI/LayoutReader.h"
#include "TouchAppUI/LayoutHelper.h"
#include "TouchAppUI/UINavigateToDlg.h"
#include "PersonalUI/UISingleLine.h"
#include "BookReader/IBookReader.h"
#include "Common/File_DK.h"

class LayoutReader;
class UIBookReaderContainer;
class UIPdfTrimColumnedDlg;
class UISizer;
class DKXBlock;

// TODO: 将来与 PdfModeController 合并
struct ImageController
{
    DK_INT m_iRotation;        //当前角度

    ReadingMode  m_eReadingMode;
    DK_UINT      m_uSubPageOrder;   //分栏后的子页的翻页顺序
    DK_UINT      m_uCurSubPage;     //当前子页(分栏模式)
    DK_DOUBLE    m_dWidth;          //分栏点占宽度百分比
    DK_DOUBLE    m_dHeight;         //分栏点占高度百分比
};

enum JumpDirection
{
	GO_NONE = 0,
	GO_RIGHT,
	GO_LEFT
};

class UIReaderSettingDlg : public UIDialog {

public:

    //end flag
    enum {
        Invalid,
        BackToBookShelf,
        GotoToc,
        GotoBookMark,
        Normal,
        GotoPage,
        GotoPercent,
        TTS,
        SHOWTOCDLG,
        SHOWSEARCHDLG,
        SwitchComicsFrameMode,
        NoChange
    };

    virtual LPCSTR GetClassName() const
    {
        return "UIReaderSettingDlg";
    }

    UIReaderSettingDlg(
            UIContainer* pContainer,
            IBookReader* pBookReader,
            DKXBlock* dkxBlock,
            const char* title,
            const char* strFilePath,
            DkFileFormat format=DFF_Unknown);
    virtual ~UIReaderSettingDlg();

    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    virtual void Popup();
    virtual void EndDialog(INT32 iEndCode,BOOL fRepaintIt = TRUE);

    virtual void OnChildSetFocus(UIWindow* pWindow);

    int GetCurFontSize() const;
    LayoutStyle GetCurLayoutStyle() const;
    int GetCurEmBoldenLevel() const; 
    int GetCurFontStyle() const;
	int GetCurReadingLayoutMode() const;
    void SetBookMarkOn(bool existBookMark);
    //记录是否智能重拍模式改变，UIBookReaderContainer根据此采取不同处理
    bool RearRangeStatusChanged() const;
    //设置失败(智能重排)时,UIBookReaderContainer通知恢复
    void SetPdfModeController(PdfModeController* pdfModeController);
    const PdfModeController GetPdfModeController() const;

    void SetIsTTSPlay(bool isTTSPlay);

    // 此接口处理zip，待考虑更好处理方法
    int  GetGotoPageNum() const { return m_iGotoPageNum; }

    bool IsImageSetter() const { return m_bIsImage; }

    int GetEndFlag() const { return m_endFlag; }

protected:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual void PerformClick(MoveEvent* moveEvent);
private:
    enum MidWinsIndex{
        MID_WINDOWS_CHAPTER,
        MID_WINDOWS_VOLUME,
        MID_WINDOWS_FONTSETTING,
        MID_WINDOWS_LAYOUT,
        MID_WINDOWS_READERSETTING,
        MID_WINDOWS_ROTATE,
        MID_WINDOWS_COLUMNED,
        MID_WINDOWS_CUTTINGEDGE,
        //ADD NEW MIDDLE CONTAINER


        MID_WINDOWS_COUNTER//middle container size
    };

    //we need different background images and drawing ways 
    void InitUI();

    //internal method for InitUI();
    //          TopWindows
    //
    //
    //          Middle...Windows
    //          BottomWindows
    void InitTopWindows();
    void InitMiddleChapterWindows();
    void InitMiddleVolumeWindows();
    void InitMiddleEmBoldenWindows();
    void InitMiddleLayoutWindows();
    void InitMiddleReaderSettingWindows();
    void UpdateReaderSettingWindows();
    void InitMiddleRotateWindows();
    void InitMiddleColumnedWindows();
    void InitMiddleCuttingEdge();
    void InitBottomWindows();
    
    void RefreshFontSizeEnable(int fontSize);
    void RefreshFontBoldenEnable(int bolden);
    void RefreshFontStyleFocus(); // 设置简繁转换
    void RefreshRotateFocus();              //设置转屏按钮的焦点
    void RefreshReaderSettingFocus();       //设置阅读设置按钮的焦点
    void RefreshContentDisplayFocus();
    void RefreshTurnPageCustomFocus();
    void RefreshReadingUpDownGestureCustomFocus();
    void RefreshProgressBarFocus();
    void RefreshFontRenderFocus();
    void ProgressBarChangedHandler(DWORD dwCmdId);
    void FontRenderChangedHandler(DWORD dwCmdId);

    void UpdateChapterInfo();

    //internal method of changing UI while rearrango on/off
    void InitWindowForRearRange();
    
    void LoadReaderSettingInfo();
    void SaveReaderSettingInfo();

    void ConfigReadProcessInfo();

    bool IsPageInfoShowPercent() const;

    void MiddlePosWindsChangeHandler(DWORD dwCmdID);//response to the changing middle-pos windows command
    
    //set middle-window nil if argument invalid
    void SetMiddlePosWins(MidWinsIndex m_midContainer);


    //Initialize the CN&EN font vector
    void InitFont();
    bool GetFontNameList (std::vector<DKFont*> &lstFont, std::vector<string>* pLstFontName);

    void ShowCNLangSettingDlg();
    void ShowENLangSettingDlg();
    //internal method of handling the text-attributes changing
    void VolumeChangedHandler(DWORD dwCmdId);
    void FontSizeChangedHandler(DWORD dwCmdId);
    void EmBoldenChangedHandler(DWORD dwCmdId);
    void CuttingEdgeChangedHandler(DWORD dwCmdId);
    void PdfEmBoldenChangedHandler(DWORD dwCmdId);
    void LayoutStyleChangedHandler(DWORD dwCmdId);
	void ReadingLayoutChangeHandler(DWORD dwCmdId);
    void FontStyleChangedHandler(DWORD dwCmdId);
    void TurnPageCustomChangedHandler(DWORD dwCmdId);
    void ReadingUpDownGestureCustomChangedHandler(DWORD dwCmdId);
    void RotateScreenHandler(DWORD dwCmdId);
    void ContentDisplayChangedHandler(DWORD dwCmdId);
    void PDFRearRangeHandler();
    void CuttingEdgeCommand();
    void CloumnedCommand();
    void BookMarkAddHandler();
    void CuttingEdgeHandler(DWORD dwCmdId);
    void ColumnedChangedHandler(DWORD dwCmdId);
    void CNLanguageChangedHandler(int iIndex);
    void ENLanguageChangedHandler(int iIndex);
    void NavigateToHandler();

    void OnBookParseAttributesChanged();
    DK_IMAGE* GetPreviewImage();
    static void* ReparseBook(void*);
    void RenewBookReaderSettings();
    void StartBookParseThread();
    void StopBookParseThread();
    bool IsComicsChapter() const;
    bool IsComicsFrameMode() const;

    // bottom btns start
    enum ICON_TYPE
    {
        ICON_UNFOCUSED=0,
        ICON_FOCUSED=1,
        ICON_DISABLE=2
    };

    void UpdateBottomBtnsStatus();

    // left btns
    DWORD GetCmdIdOfRotationOrTTS();
    DWORD GetCmdIdOfVolume();
    const char* GetStringOfRotationOrTTS();
    const char* GetStringOfVolume();
    SPtr<ITpImage> GetImageOfRotationOrTTS(ICON_TYPE type);
    SPtr<ITpImage> GetImageOfVolume(ICON_TYPE type);
    void UpdateBtnStatusOfRotationOrTTS();
    void UpdateBtnStatusOfVolume();
    void UpdateBottomLeftWndsCornerStyle();

    void InitMiddleFontSettingWindows();
    void UpdateFontSettingWindows();
    // mid btns
    DWORD GetCmdIdOfBottomMidBtns(int index);
    const char* GetStringOfBottomMidBtns(int index);
    SPtr<ITpImage> GetImageOfFontsettingOrTrim(ICON_TYPE type);
    SPtr<ITpImage> GetImageOfFormatOrColumn(ICON_TYPE type);
    SPtr<ITpImage> GetImageOfRearrange(ICON_TYPE type);
    SPtr<ITpImage> GetImageOfBolden(ICON_TYPE type);

    void UpdateBtnStatusOfFontsettingOrTrim();
    void UpdateBtnStatusOfFormatOrColumn();
    void UpdateBtnStatusOfRearrange();
    void UpdateBtnStatusOfBolden();

    void UpdateEnableOfFontsettingOrTrim();
    void UpdateEnableOfFormatOrColumn();

    void UpdateBottomMidWndsCornerStyle();

    // right btns
    void UpdateBtnStatusOfSetting();

    // bottom btns end

	void NextChapterHandler();
	void PrevChapterHandler();
	void NavigateToPage(int pageIndex);
	void RollBackHandler();
	void UpdatePageNum(int curPageIndex, int pageCount);
	void UpdateJumpDirection();
	int GetTotalPageNum();
	int GetCurPageIndex();
	static void RefreshProgress(void* parm, double percentage, bool isStop);
	bool OnParseFinishedEvent(const EventArgs& args);

	void StopTTSAndUpdate();
	
private:
    IBookReader* m_pBookReader;
    bool m_bIsScannedText;//记录当前为扫描or非扫描版本
    bool m_bIsImage; // 记录当前是否为图片
    bool m_bBookMarkExists;
    CString m_strBookName;  
    std::string m_strFilePath;

	UITouchBackButton m_btnBack;
    UITouchComplexButton m_btnLight;//灯光
    UITouchComplexButton m_btnTOC;//目录
    UITouchComplexButton m_btnBookMark;//书签
    UITouchComplexButton m_btnComicsFrame; // 漫画试图模式
    UITouchComplexButton m_btnSearch;
    UITouchComplexButton m_btnSetting;//设置

    UISizer* m_topSizer;
    UISizer* m_middleSizers[MID_WINDOWS_COUNTER];
    UISizer* m_bottomSizer;
    MidWinsIndex m_curMidWins;

	UITouchComplexButton m_nextChapter;
	UITouchComplexButton m_prevChapter;
	UITextBox m_jumpPageNum;
	UISingleLine m_splitLine;
	UITextSingleLine m_totalPageNum;
    UITextSingleLine m_textCurChapterInfo;//当前章节
    UIBookReaderProgressBar m_clsReaderProgressBar;//进度条
    UITouchComplexButton m_btnRollBack;//跳转
    UITextSingleLine m_textBookName;//文件名

    //字号
    UITouchComplexButton m_btnModifyFontSize[2];
    UITouchComplexButton m_btnModifyEmbolden[2];

    // 字体样式
    UITouchComplexButton m_btnModifyFontStyle[2];
    UITouchComplexButton m_btnModifyVolume[2];

    // 切边
    UITextSingleLine     m_textEdgeCuttingTitle;
    UITextSingleLine     m_textSmartCuttingTitle;
    UITextSingleLine     m_textManualCuttingTitle;
    UITouchComplexButton m_btnSmartCuttingSwitch;
    UITouchComplexButton m_btnModifyCuttingEdge[2];

    // 0 - 旋屏/朗读
    // 1 - 声音
    UITouchComplexButton m_bottomLeftBtns[2];

    // 0 - 切边/字号
    // 1 - 分栏/版式
    // 2 - 智能重排/字体样式
    // 4 - 加黑
    UITouchComplexButton m_bottomMidBtns[4];
    vector<UITouchComplexButton*> m_btnsBottom;

    //版式选择
    UITextSingleLine m_textReadingLayout;
	UITextSingleLine m_textLayoutStyle;
    UITouchComplexButton m_btnReadingLayout[2]; //0全屏   1显示书籍信息
    UITouchComplexButton m_btnLayoutStyleChoices[4];

    // 字体设置
    UITextSingleLine m_textFontSize;
    UITextSingleLine m_textFontStyle;
    UITextSingleLine m_textBolden;

    //设置
    UITextSingleLine m_textTurnPageCustom;//翻页习惯
    UITouchComplexButton m_btnTurnPageCustom[2];//0-left 1-right
    UITextSingleLine m_textReadingUpDownGestureCustom;//上下翻章节手势
    UITouchComplexButton m_btnReadingUpDownGestureCustom[2];//0-chpater 1-page
    UITextSingleLine m_textContentDisplay;//内容显示
    UITouchComplexButton m_btnContentDisplay[2];//0-normal 1-adapt wide
    UITextSingleLine m_textProgressBar;//进度条显示
    UITouchComplexButton m_btnProgressBar[2];//0-完整 1-迷你
    UITextSingleLine m_textFontRender;//字体渲染
    UITouchComplexButton m_btnFontRender[2];//0-锐利 1-平滑


    //旋屏
    UITouchComplexButton m_btnRotate[4];
    //UITouchComplexButton m_btnRotate[4];
    //分栏
    UITextSingleLine m_textColumned;
    UITextSingleLine m_textComic;
    UITouchComplexButton m_btnColumns[4];
    UITouchComplexButton m_btnComics[2];

    UIPdfTrimColumnedDlg *m_pPdfTrimColumnedDlg;
    UINavigateToDlg *m_pNavigateToDlg;

    //字体（中文\英文）
    UITextSingleLine m_textLangChoices[2];//0-CN, 1-EN
    UITouchComplexButton m_btnLangChoices[2];//0-CN,1-EN
    int m_iSelectedCNFontIndex;
    int m_iSelectedENFontIndex;
    
    bool m_bIsTTSPlaying;//record if the tts is playing
    bool m_bIsPdfTextMode;

    int m_iCurFontSizeIndex;
    uint32_t *m_fontSizeTable;
    LayoutStyle m_iCurLayoutStyle;//enum defined in LayoutHelper.h,4-kinds style total until now
    int m_iCurEmBoldenLevel;
    int m_iCurPdfEmBoldenLevel;
    int m_curFontStyle;
	int m_iCurReadingLayout;

    ReadingMode             m_eLastReadingMode;              // PDF 浏览的阅读模式，由文本模式返回时切换回原有模式
    PdfModeController       m_PdfModeController;
    bool m_bIsRearRangeStatusChanged;//

    //cn&en fonts
    vector<string>  m_vCNFontName;
    vector<string>  m_vENFontName;

    int m_iGotoPageNum;
    
    bool m_bIsNothingChanged;
    int  m_endFlag;
    DkFileFormat m_eFormat;
    UITitleBar m_titleBar;
    int m_fontsize20;

    pthread_t m_tidParseBook;
    bool m_bIsPreviewImageReady;
    ICT_ReadingDataItem* m_pReadingProcess;
    bool m_bIsPdfRearRanging;
    void OnFrontLightClicked();
    DKXBlock* m_dkxBlock;
    UIContainer* m_pContainer;
	ImageController* m_imageController;
};//UIReaderSettingDlg

#endif//UIREADERSETTINGDLG_H


