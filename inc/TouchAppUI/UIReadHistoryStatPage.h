////////////////////////////////////////////////////////////////////////
//
// UIReadHistoryStatPage.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIREADHISTORYSTATISTICS_H__
#define __UIREADHISTORYSTATISTICS_H__

#include "GUI/UIImage.h"
#include "GUI/UIPage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIComplexButton.h"
#include <string>

#define MinStartX 70
#define MinStartY 300
#define MaxStartX 360
#define MaxStartY 600
#define XDISTANCE 5
#define YDISTANCE 120

#define MiddleX  299
#define HALFWIDTH  300


class UIReadHistoryStatPage : public UIPage
{
public:
    UIReadHistoryStatPage();
    static LPCSTR GetClassNameStatic()
    {
        return ("UIReadHistoryStatPage");
    }

    virtual LPCSTR GetClassName() const
    {
        return this->GetClassNameStatic();
    }
    virtual ~UIReadHistoryStatPage();
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);    

protected:
    HRESULT Init();
    void InitImages();
    void InitImage(std::vector<UIImage*> &vImageList, float fNumber, int decimalLen);    
    int  GetStartingX_Images(std::vector<UIImage*> &vImages, BOOL bLeft);
    void Dispose();
    void OnLeave();
    HRESULT Draw(DK_IMAGE drawingImg);
    void DrawImages();

private:

    UIImage                   m_imgTitle;//阅读统计
    UIComplexButton           m_btnReset;
        
    UITextSingleLine          m_txtTotalReadingHours;//阅读总小时数
    UITextSingleLine          m_txtAverageReadingHoursPerBook;//平均每本书阅读时间
    UITextSingleLine          m_txtReadingPagesPerHour;//每小时阅读页数
    UITextSingleLine          m_txtReadedBooks;    //阅读完成书籍数量
    
    std::vector<UIImage*>     m_vTotalHours;//阅读总小时数的图像列表
    std::vector<UIImage*>     m_vAverageHoursPerBook;//平均每本书阅读时间的图像列表
    std::vector<UIImage*>     m_vReadingPagesPerHour;//每小时阅读页数的图像列表
    std::vector<UIImage*>     m_vTotalReadedBooks;//阅读完成书籍数量的图像列表
};

#endif


