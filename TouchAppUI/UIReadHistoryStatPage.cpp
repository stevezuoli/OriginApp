////////////////////////////////////////////////////////////////////////
// UIReadHistoryStatPage.cpp
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIReadHistoryStatPage.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "interface.h"
#include "I18n/StringManager.h"
#include "ReadingHistoryStat.h"
#include "SQM.h"

UIReadHistoryStatPage::UIReadHistoryStatPage()
{    
    Init();
}
UIReadHistoryStatPage::~UIReadHistoryStatPage()
{
    Dispose();
}

void UIReadHistoryStatPage::Dispose()
{
    UIPage::Dispose();
    UINT uLen = m_vAverageHoursPerBook.size();
    if(uLen)
    {
        for(UINT i = 0; i < uLen; i++)
        {
            SafeDeletePointerEx(m_vAverageHoursPerBook[i]);
        }
        m_vAverageHoursPerBook.clear();
    }
    
    uLen = m_vReadingPagesPerHour.size();
    if(uLen)
    {
        for(UINT i = 0; i < uLen; i++)
        {
            SafeDeletePointerEx(m_vReadingPagesPerHour[i]);
        }
        m_vReadingPagesPerHour.clear();
    }

    uLen = m_vTotalHours.size();
    if(uLen)
    {
        for(UINT i = 0; i < uLen; i++)
        {
            SafeDeletePointerEx(m_vTotalHours[i]);
        }
        m_vTotalHours.clear();
    }

    uLen = m_vTotalReadedBooks.size();
    if(uLen)
    {
        for(UINT i = 0; i < uLen; i++)
        {
            SafeDeletePointerEx(m_vTotalReadedBooks[i]);
        }
        m_vTotalReadedBooks.clear();
    }
}

void UIReadHistoryStatPage::InitImages()
{
    UINT uReadingBookNum = 0, uCompletedBookNum = 0;
    BOOL bBlankImage = FALSE;
    ReadingHistoryStat* pStat = ReadingHistoryStat::GetInstance();
    if(!pStat)
    {
        bBlankImage = TRUE;
    }
    ReadingHistory* pHistory = (pStat == NULL) ? NULL : pStat->GetData();
    if(!pHistory)
    {
        bBlankImage = TRUE;
    }

    long lTotalSeconds = (pHistory == NULL) ? 0: pHistory->GetTotalSeconds();
    long lTotalTurndownPages = (pHistory == NULL) ? 0: pHistory->GetTotalTurndownPages();

    float fTotalHours = lTotalSeconds/3600.0;

    if(!bBlankImage)
    {
        pStat->GetBookNumbers(uReadingBookNum, uCompletedBookNum);
    }
    DebugPrintf(DLC_LIUJT, "UIReadHistoryStatPage::InitImages totalHour=%.2f, total turndown pages=%d, reading books=%d, completed books=%d", fTotalHours, lTotalTurndownPages, uReadingBookNum, uCompletedBookNum);
    InitImage(m_vTotalHours, fTotalHours, 1);
    InitImage(m_vTotalReadedBooks, uCompletedBookNum, 0);
    DebugPrintf(DLC_LIUJT, "UIReadHistoryStatPage::InitImages uReadingBookNum=%d, uCompletedBookNum=%d", uReadingBookNum, uCompletedBookNum);
    
    float fBooksPerHour = bBlankImage ? 0.0: fTotalHours/uReadingBookNum;
    InitImage(m_vAverageHoursPerBook, fBooksPerHour, 2);

    float fPagesPerHour = bBlankImage ? 0.0: lTotalTurndownPages/fTotalHours;

    InitImage(m_vReadingPagesPerHour, fPagesPerHour, 1);
    DebugPrintf(DLC_LIUJT, "UIReadHistoryStatPage::InitImages fBooksPerHour=%.2f, fPagesPerHour=%.2f", fBooksPerHour, fPagesPerHour);
}

void UIReadHistoryStatPage::InitImage(vector<UIImage*> &vImageList, float fNumber, int decimalLen)
{
    DebugPrintf(DLC_LIUJT, "UIReadHistoryStatPage::InitImage Entering with %.5f and decimalLen=%d", fNumber, decimalLen);
    char temp[40] = {0};
    sprintf(temp, "%.2f", fNumber);
    string sStr(temp);
    string::size_type pos = sStr.find(".");
    if(string::npos != pos)
    {
        if(decimalLen)
        {
            int iLen = sStr.substr(pos+1).size();
            int iTrunctedLen = decimalLen < iLen ? decimalLen :  iLen;
            sStr = sStr.substr(0, pos+1+iTrunctedLen);

            if(sStr.size() >= 8)//屏幕上最多显示6个数字加上一个点号
            {
                string::size_type tPos = sStr.find(".");
                if(string::npos != tPos)
                {
                    if(tPos == 6)//如果点号刚好处于第7个，则把点号及其后部分过滤掉
                    {
                        sStr = sStr.substr(0,6);
                    }
                    else if(tPos > 6)//如果点号大于7，则让屏幕去显示(不太可能)
                    {
                        sStr = sStr.substr(0, tPos);
                    }
                }
            }
        }
        else
        {
            sStr = sStr.substr(0, pos);
        }
    }

    UINT uImageNum = vImageList.size();
    for(UINT i = 0; i < uImageNum; i++)
    {
        SafeDeletePointerEx(vImageList[i]);
    }
    vImageList.clear();
    
    UINT uLen = sStr.size();
    for(UINT i = 0; i < uLen; i++)
    {
        int iImageID = -1;
        if(sStr[i] != '.')
        {
            iImageID = sStr[i]-'0'+IMAGE_BIGICON_0;
        }
        else
        {
            iImageID = IMAGE_BIGICON_DOT;
        }
        
        if(-1 != iImageID)
        {
            SPtr<ITpImage> pImage = ImageManager::GetImage(iImageID);
            if(pImage)
            {
                UIImage* image = new UIImage(this, IDSTATIC);
                if(image)
                {
                    image->SetImage(pImage);
                    vImageList.push_back(image);
                }
            }
        }
    }
}

int  UIReadHistoryStatPage::GetStartingX_Images(vector<UIImage*> &vImages, BOOL bLeft)
{

    UINT uLen = vImages.size();
    int iTotalImageWidth = 0;
    for(UINT i = 0; i < uLen; i++)
    {
        UIImage* pImage = vImages[i];
        if(pImage)
        {
            iTotalImageWidth += pImage->GetImageWidth();
        }
    }

    if(bLeft)
    {
        if(MiddleX > iTotalImageWidth)
        {
            return (MiddleX - iTotalImageWidth)/2;        
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if(iTotalImageWidth < HALFWIDTH)
        {
            return (MiddleX +HALFWIDTH/2 - iTotalImageWidth/2);
        }
        else
        {
            return MiddleX;
        }
    }

}

void UIReadHistoryStatPage::DrawImages()
{
    InitImages();
    UINT uLen = m_vTotalHours.size();
    if(uLen)
    {
        int iStartX = GetStartingX_Images(m_vTotalHours, TRUE);
        int iStartY = MinStartY-YDISTANCE;
        for(UINT i = 0; i < uLen; i++)
        {
            UIImage *pImage = m_vTotalHours[i];
            if(pImage)
            {
                pImage->MoveWindow(iStartX, iStartY, pImage->GetImageWidth(), pImage->GetImageHeight());
                iStartX += pImage->GetImageWidth();
            }
        }
    }

    uLen = m_vAverageHoursPerBook.size();
    if(uLen)
    {
        int iStartX = GetStartingX_Images(m_vAverageHoursPerBook, FALSE);
        int iStartY = MinStartY-YDISTANCE;
        for(UINT i = 0; i < uLen; i++)
        {
            UIImage *pImage = m_vAverageHoursPerBook[i];
            if(pImage)
            {
                pImage->MoveWindow(iStartX, iStartY, pImage->GetImageWidth(), pImage->GetImageHeight());
                iStartX += pImage->GetImageWidth();
            }
        }
    }

    uLen = m_vReadingPagesPerHour.size();
    if(uLen)
    {
        int iStartX = GetStartingX_Images(m_vReadingPagesPerHour, TRUE);
        int iStartY = MaxStartY-YDISTANCE;
        for(UINT i = 0; i < uLen; i++)
        {
            UIImage *pImage = m_vReadingPagesPerHour[i];
            if(pImage)
            {
                pImage->MoveWindow(iStartX, iStartY, pImage->GetImageWidth(), pImage->GetImageHeight());
                iStartX += pImage->GetImageWidth();
            }
        }
    }

    uLen = m_vTotalReadedBooks.size();
    if(uLen)
    {
        int iStartX = GetStartingX_Images(m_vTotalReadedBooks, FALSE);
        int iStartY = MaxStartY-YDISTANCE;
        for(UINT i = 0; i < uLen; i++)
        {
            UIImage *pImage = m_vTotalReadedBooks[i];
            if(pImage)
            {
                pImage->MoveWindow(iStartX, iStartY, pImage->GetImageWidth(), pImage->GetImageHeight());
                iStartX += pImage->GetImageWidth();
            }
        }
    }
}

HRESULT UIReadHistoryStatPage::Init()
{
    HRESULT hr(S_OK);
    TitleBarSignalManager::GetTitleBarSignalManager()->Stop();

    UITitleBar *pTitleBar = (CDisplay::GetDisplay()->GetCurrentPage())->GetTitleBar();
    if(pTitleBar && pTitleBar->IsVisible())
    {
        pTitleBar->ClearTitleBar();
    }
    
    SPtr<ITpImage> pImage = ImageManager::GetImage(IMAGE_READINGSTAT);
    
    m_imgTitle.SetImage(pImage);
    m_imgTitle.MoveWindow(0, 0, pImage->GetWidth(), pImage->GetHeight());

    m_btnReset.Initialize(ID_BTN_RESETREADINGHISTORYSTAT, StringManager::GetStringById(CLEARREADINGSTAT), 18, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnReset.MoveWindow(MaxStartX+158, 80, 80, 25);
    m_btnReset.SetFocus(FALSE);

    m_txtTotalReadingHours.SetFontSize(24);
    m_txtTotalReadingHours.SetText(StringManager::GetStringById(READINGHISTORY_TOTALHOURS));
    m_txtTotalReadingHours.MoveWindow(MinStartX, MinStartY, 200 , 30);


    m_txtAverageReadingHoursPerBook.SetFontSize(24);
    m_txtAverageReadingHoursPerBook.SetText(StringManager::GetStringById(READINGHISTORY_AVERAGEHOURSPERBOOK));
    m_txtAverageReadingHoursPerBook.MoveWindow(MaxStartX, MinStartY, 200, 30);


    m_txtReadingPagesPerHour.SetFontSize(24);
    m_txtReadingPagesPerHour.SetText(StringManager::GetStringById(READINGHISTORY_PAGESPERHOUR));
    m_txtReadingPagesPerHour.MoveWindow(MinStartX, MaxStartY, 250 , 30);

    m_txtReadedBooks.SetFontSize(24);
    m_txtReadedBooks.SetText(StringManager::GetStringById(READINGHISTORY_TOTALCOMPLETEDEDBOOKS));
    m_txtReadedBooks.MoveWindow(MaxStartX, MaxStartY, 200, 30);

    AppendChild(&m_imgTitle);
    AppendChild(&m_btnReset);
    AppendChild(&m_txtTotalReadingHours);
    AppendChild(&m_txtAverageReadingHoursPerBook);
    AppendChild(&m_txtReadingPagesPerHour);
    AppendChild(&m_txtReadedBooks);
    
    DrawImages();

    return hr;
}

HRESULT UIReadHistoryStatPage::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_FALSE;
    }

    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }
    UIPage::Draw(drawingImg);
    HRESULT hr(S_OK);

    DK_IMAGE imgSelf;

    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));
    CTpGraphics grf(imgSelf);
    
    RTN_HR_IF_FAILED(grf.DrawLine(299, 95, 2, 650, DOTTED_STROKE));
    RTN_HR_IF_FAILED(grf.DrawLine(38, 407, 520, 1, SOLID_STROKE));

    return hr;
}

void UIReadHistoryStatPage::OnLeave()
{
    TitleBarSignalManager::GetTitleBarSignalManager()->Start();
}

void UIReadHistoryStatPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_MESSAGE, "UIReadHistoryStatPage::OnCommand(dwCmdId=%d, pSender=%X, dwParam=%d)", dwCmdId, pSender, dwParam);
    switch (dwCmdId)
    {
        case ID_BTN_RESETREADINGHISTORYSTAT:
            {

                DebugPrintf(DLC_LIUJT, "UIReadHistoryStatPage::OnCommand click reset button!!!");
                SQM::GetInstance()->IncCounter(SQM_ACTION_READINGHISTORY_RESET);
                ReadingHistoryStat::GetInstance()->ClearHistory();
                InitImages();
                DrawImages();
                Repaint();
                break;
            }


        default:
            {
                break;
            }
    }
}


