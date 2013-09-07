#include "TouchAppUI/UICommentExportDlg.h"
#include "Common/FileManager_DK.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "CommandID.h"
#include "I18n/StringManager.h"
#include "Utility/ThreadHelper.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "Common/WindowsMetrics.h"

using dk::utility::PathManager;
using dk::utility::StringUtil;
using dk::utility::EncodeUtil;
using namespace WindowsMetrics;

#define COMMENTEXPORT_TOP_MARGIN               16
#define COMMENTEXPORT_HORIZONTAL_MARGIN        27
#define COMMENTEXPORT_BUTTON_WIDTH             70
#define COMMENTEXPORT_BUTTON_HEIGHT            30
#define COMMENTEXPORT_TEXT_HEIGHT              30
#define COMMENTEXPORT_BUTTON_MARGIN            20


//添加书名
static bool addBookName(FILE* fp, const string& fileName)
{
    if(!fp || (fileName.length() == 0))
    {
        return false;
    }
    string strTmp = StringUtil::EscapeHTML(fileName.c_str());
    string before = "<h2 style=\"font-size:18pt;text-align:right;MARGIN-BOTTOM: 0em;\">";
    string after = "</h2>";
    fwrite(before.c_str(), before.length(), 1, fp);
    fwrite(strTmp.c_str(), strTmp.length(), 1, fp);
    fwrite(after.c_str(), after.length(), 1, fp);
    return true;
    
}

//添加作者
static bool addBookCreater(FILE* fp, const string& creater)
{
    if(!fp || (creater.length() == 0))
    {
        return false;
    }
    string strTmp = StringUtil::EscapeHTML(creater.c_str());
    string before = "<h5 style=\"font-size:12pt;text-align:right;color:gray;MARGIN-BOTTOM:0.2em;MARGIN-TOP:0.2em;\">";
    string after = "</h5>";
    fwrite(before.c_str(), before.length(), 1, fp);
    fwrite(strTmp.c_str(), strTmp.length(), 1, fp);
    fwrite(after.c_str(), after.length(), 1, fp);
    return true;
}

//添加书摘
static bool addComment(FILE* fp, const string& comment)
{
    if(!fp || (comment.length() == 0))
    {
        return false;
    }
    string strTmp = StringUtil::EscapeHTML(comment.c_str());
    if (SystemSettingInfo::GetInstance()->GetFontStyle_FamiliarOrTraditional())
    {
        strTmp = EncodeUtil::UTF8ToChineseTraditional(strTmp.c_str());
    }
    string before = "<div style=\"font-size:12pt;\">";
    string after = "</div>";
    fwrite(before.c_str(), before.length(), 1, fp);
    fwrite(strTmp.c_str(), strTmp.length(), 1, fp);
    fwrite(after.c_str(), after.length(), 1, fp);
    return true;
}

//添加书摘添加时间
static bool addCommentTime(FILE* fp, const string& time)
{
    if(!fp || (time.length() == 0))
    {
        return false;
    }
    string strTmp = StringUtil::ReplaceString(time, 'T', " ");
    string before = "<div style=\"font-size:10pt;margin-bottom:0.2em;color:darkgray;margin-left:0.5em;margin-top:0.2em;\">";
    string after = "</div>";
    fwrite(before.c_str(), before.length(), 1, fp);
    fwrite(strTmp.c_str(), strTmp.length(), 1, fp);
    fwrite(after.c_str(), after.length(), 1, fp);
    return true;
}

//添加批注
static bool addUserComment(FILE* fp,const string& userComment)
{
    if(!fp || (userComment.length() == 0))
    {
        return false;
    }
    string strTmp = StringUtil::EscapeHTML(userComment.c_str());
    string before = "<div style=\"font-size:11pt;\"><span style=\"font-weight:bold;\">";
    string middle = "</span><span>";
    string after = "</span></div>";
    fwrite(before.c_str(), before.length(), 1, fp);
    fwrite(StringManager::GetPCSTRById(TOUCH_COMMENTITEM),strlen(StringManager::GetPCSTRById(TOUCH_COMMENTITEM)),1,fp);
    fwrite(middle.c_str(), middle.length(), 1, fp);
    fwrite(strTmp.c_str(), strTmp.length(), 1, fp);
    fwrite(after.c_str(), after.length(), 1, fp);
    return true;
}

//添加分隔符
static bool addSplit(FILE* fp)
{   
    if(!fp)
    {
        return false;
    }
    string split = "<div style=\"height:2px;max-height:2px;border-top:1px dotted gray;margin-top:0.5em;\"></div>";
    fwrite(split.c_str(), split.length(), 1, fp);
    return true;
}

//添加头
static bool addHead(FILE* fp, const string& headTitle)
{
    if(!fp || (headTitle.length() == 0))
    {
        return false;
    }
    string strTmp = StringUtil::EscapeHTML(headTitle.c_str());
    string before = "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><title>";
    string after = "</title></head><body><div style=\"width:90%;max-width:600px;margin:0px auto;padding:5px;font-size:12pt;font-family:Times\">";
    fwrite(before.c_str(), before.length(), 1, fp);
    fwrite(strTmp.c_str(), strTmp.length(), 1, fp);
    fwrite(after.c_str(), after.length(), 1, fp);
    return true;
}

//添加尾
static bool addEnd(FILE* fp)
{
    if(!fp)
    {
        return false;
    }
    string strSplit = "<br><br><hr style=\"BORDER-RIGHT-WIDTH:0px;BACKGROUND-COLOR:#ddd;BORDER-TOP-WIDTH:0px;BORDER-BOTTOM-WIDTH:0px;HEIGHT:2px;BORDER-LEFT-WIDTH:0px\">";
    string strAbustruct = "<div style=\"height:2px;border:0;font-size:10pt;color:gray;margin-bottom:0.2em;\">";
    string strBody = "</div></div><br><br></body></html>";
    fwrite(strSplit.c_str(), strSplit.length(), 1, fp);
    fwrite(strAbustruct.c_str(), strAbustruct.length(), 1, fp);
    fwrite(StringManager::GetPCSTRById(TOUCH_COMMENT_EXPORT_ABUSTRUCT),strlen(StringManager::GetPCSTRById(TOUCH_COMMENT_EXPORT_ABUSTRUCT)),1,fp);
    fwrite(strBody.c_str(), strBody.length(), 1, fp);
    return true;
}


static bool fileClose(FILE** fp)
{
    if(!fp || !(*fp))
    {
        return false;
    }
    fclose(*fp);
    *fp = NULL;
    return true;
}

static bool fileCreate(FILE** fp, string& filePath)
{
    *fp = fopen(filePath.c_str(),"wb+");
    if(!fp || !(*fp))
    {
        return false;
    }
    return true;
}

UICommentExportDlg::UICommentExportDlg(UIContainer* pParent, LPCSTR title)
    :UIModalDlg(pParent, title)
    , m_threadExport(0)
    , m_bIsThreadCanceled(false)
    , m_bIsExportOver(true)
    , m_iRefreshTimerId(-1)
    , m_iExportPercent(0)
    , m_strCommentExportPath("")
{
    m_strCommentExportPath = PathManager::GetCommentExportPath();
    InitUI();
}

UICommentExportDlg::~UICommentExportDlg()
{
    StopExportThread();
    if (-1 != m_iRefreshTimerId)
    {
        UnregisterTimer(m_iRefreshTimerId);
        m_iRefreshTimerId = -1;
    }
}

void UICommentExportDlg::InitUI()
{
    m_btnCancel.Initialize(IDCANCEL, StringManager::GetPCSTRById(TOUCH_CANCEL), GetWindowFontSize(FontSize20Index));

    m_barExportProgress.SetProgressColor(ColorManager::knBlack);
    m_barExportProgress.SetMaximum(100);

    m_statusText.SetFontSize(GetWindowFontSize(FontSize18Index));
    m_statusText.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_statusText.SetText(StringManager::GetPCSTRById(TOUCH_COMMENT_EXPORT_PROCESSING));

    m_textExportOK.SetFontSize(GetWindowFontSize(FontSize22Index));
    m_textExportOK.SetText(StringManager::GetPCSTRById(TOUCH_COMMENT_EXPORT_OK));
    m_textExportOK.SetVisible(false);

    AppendChild(&m_textExportOK);
    AppendChild(&m_statusText);
    AppendChild(&m_barExportProgress);
    m_btnGroup.AddButton(&m_btnCancel);

    if (m_windowSizer)
    {
        m_btnCancel.SetMinSize(dkSize(GetWindowMetrics(UICommentExportDlgButtonWidthIndex), GetWindowMetrics(UICommentExportDlgButtonHeightIndex)));
        m_barExportProgress.SetMinHeight(10);
        const int horizonMargin = GetWindowMetrics(UICommentExportDlgHorizonMarginIndex);

        m_windowSizer->AddSpacer(GetWindowMetrics(UICommentExportDlgTopMarginIndex));
        m_windowSizer->Add(&m_textExportOK, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin));
        m_windowSizer->AddSpacer(GetWindowMetrics(UICommentExportDlgWidgetsMarginIndex));
        m_windowSizer->Add(&m_statusText, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin));
        m_windowSizer->AddSpacer(GetWindowMetrics(UICommentExportDlgWidgetsMarginIndex));
        m_windowSizer->Add(&m_barExportProgress, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin));
        m_windowSizer->AddSpacer(GetWindowMetrics(UICommentExportDlgWidgetsMarginIndex));
        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());

        SetSizer(m_windowSizer);
    }
    InitPosition();
}

void UICommentExportDlg::InitPosition()
{
    if (m_windowSizer)
    {
        const int width = GetWindowMetrics(UIModalDialogWidthIndex);
        const int height = m_windowSizer->GetMinSize().GetHeight();
        MakeCenter(width, height);
        Layout();
    }
}

void UICommentExportDlg::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case IDCANCEL:
        {
            StopExportThread();
            EndDialog(0); 
        }
        break;
    case IDOK:
        {
            EndDialog(0);
        }
        break;
    default:
        break;
    }
}

void UICommentExportDlg::OnTimer(INT32 iId)
{
    if(m_bIsExportOver)
    {
        if (-1 != m_iRefreshTimerId)
        {
            UnregisterTimer(m_iRefreshTimerId);
            m_iRefreshTimerId = -1;
        }
        UpdateUIText();
    }
    else
    {
        m_barExportProgress.SetProgress(m_iExportPercent);
        m_barExportProgress.UpdateWindow();
        string str_statusText = (string)StringManager::GetStringById(TOUCH_COMMENT_EXPORT_PROGRESS);
        char temp[128]={0};
        snprintf(temp , DK_DIM(temp), "%d %%",m_iExportPercent);
        str_statusText += temp;
        m_statusText.SetText(str_statusText.c_str());
        m_statusText.UpdateWindow();
    }
}

void UICommentExportDlg::Popup()
{
    UIModalDlg::Popup();
    PathManager::MakeDirectoryRecursive(m_strCommentExportPath.c_str());
    StopExportThread();
    m_bIsExportOver = false;
    m_iRefreshTimerId = RegisterTimer(1, true);
	ThreadHelper::CreateThread(&m_threadExport, ExportThreadFunc, this, "UICommentExportDlg @ ExportComment", false, 32768);
}

void UICommentExportDlg::EndDialog(INT32 iEndCode)
{
    UIModalDlg::EndDialog(iEndCode);
}

void UICommentExportDlg::ExportComment()
{
    CDKFileManager  *pFileManager = CDKFileManager::GetFileManager();
    if(!pFileManager)
    {
        return;
    }
    vector<int> vFileID;
    if(!pFileManager->GetFileIDList(&vFileID))
    {
        return;
    }
    int bookNum = vFileID.size();
    for(int i = 0; i < bookNum; i++)
    {
        if(m_bIsThreadCanceled)
        {
            pthread_exit(0);
        }
        m_iExportPercent = (i * 100) / bookNum;
        
        PCDKFile pDkFile = pFileManager->GetFileById(vFileID[i]);
        if (NULL == pDkFile)
        {
            continue;
        }
        ExportCommentByBook(pDkFile);
    }
    m_bIsExportOver = true;
}

bool UICommentExportDlg::ExportCommentByBook(const PCDKFile pcdkBook)
{
	string filePath = pcdkBook->GetFilePath();
	string bookName("");

	//书城书用元数据，非书城书用文件名
	if(pcdkBook->IsDuoKanBook())
	{
		bookName = pcdkBook->GetFileName();
		bookName += ".epub";
	}
	else
	{
		const char* pchar = std::strrchr(filePath.c_str(),'/');
	    if(pchar)
	    {
	        pchar++;
	        bookName.assign(pchar);
	    }
	}
    string str_commentFilePath = m_strCommentExportPath + "/" + bookName;
    switch(pcdkBook->GetFileFormat())
    {
        case DFF_Text:
        case DFF_ElectronicPublishing:
        case DFF_MobiPocket:
        case DFF_PortableDocumentFormat:
            break;
        default:
            return false;
    }
    str_commentFilePath += ".html";
    FILE* pCommentFile = NULL;

    DKXManager* _pclsDKXManager = DKXManager::GetInstance();
    if(!_pclsDKXManager)
    {
        return false;
    }
    vector<ICT_ReadingDataItem*> pvtrBookData;
    if(!_pclsDKXManager->FetchAllBookmarks(filePath,&pvtrBookData))
    {
        return false;
    }
    if(pvtrBookData.size() == 0)
    {
        return false;
    }

    int iCommentdigestCount = 0;
    for(int i = 0; i < (int)pvtrBookData.size(); i++)
    {
        if((pvtrBookData[i]->GetUserDataType() == ICT_ReadingDataItem::COMMENT) || (pvtrBookData[i]->GetUserDataType() == ICT_ReadingDataItem::DIGEST))
        {
            iCommentdigestCount++;
        }
    }

    if(iCommentdigestCount > 0)  //书签不导出
    {
        if(!fileCreate(&pCommentFile, str_commentFilePath))
        {
            for(int i = 0;i < (int)pvtrBookData.size(); i++)
            {
                DKXReadingDataItemFactory::DestoryReadingDataItem(pvtrBookData[i]);
            }
            return false;
        }
        addHead(pCommentFile, bookName);
        addBookName(pCommentFile,bookName);
        addBookCreater(pCommentFile, pcdkBook->GetFileArtist());

        for(int i = 0; i < (int)pvtrBookData.size(); i++)
        {
            if((pvtrBookData[i]->GetUserDataType() == ICT_ReadingDataItem::COMMENT) || (pvtrBookData[i]->GetUserDataType() == ICT_ReadingDataItem::DIGEST))
            {
                addSplit(pCommentFile);
                addCommentTime(pCommentFile,pvtrBookData[i]->GetCreateTime());
                addComment(pCommentFile, pvtrBookData[i]->GetBookContent());
                if(pvtrBookData[i]->GetUserContent().length() != 0)
                {
                    addUserComment(pCommentFile,pvtrBookData[i]->GetUserContent());
                }
            }
            DKXReadingDataItemFactory::DestoryReadingDataItem(pvtrBookData[i]);
        }
        addEnd(pCommentFile);
        fileClose(&pCommentFile);
    }
    return true;
}

void UICommentExportDlg::UpdateUIText()
{
    m_barExportProgress.SetProgress(100);
    m_barExportProgress.SetVisible(false);
    m_textExportOK.SetVisible(true);

    string statusText = "";
    statusText += StringManager::GetStringById(TOUCH_COMMENT_EXPORT_FILESAVE);
    statusText += m_strCommentExportPath.substr(PathManager::GetRootPath().length() + 1);
    m_statusText.SetText(statusText.c_str());
    
    m_btnCancel.SetText(StringManager::GetPCSTRById(TOUCH_EXIT));
    m_btnCancel.SetId(IDOK);
    InitPosition();
    UpdateWindow();
}

void UICommentExportDlg::StopExportThread()
{
    if(m_threadExport)
    {
        m_bIsThreadCanceled = true;
        ThreadHelper::JoinThread(m_threadExport, NULL);
        m_threadExport = 0;
        m_bIsThreadCanceled = false;
    }
}

void* UICommentExportDlg::ExportThreadFunc(void* param)
{
    UICommentExportDlg* pThis = (UICommentExportDlg*)param;
    if(!pThis)
    {
        return NULL;
    }
    pThis->ExportComment();
    return (void*)0;
}

