//===========================================================================
//  Summary:
//        UITextMultiLine.h
//  Usage:
//        多行文本排版
//  Remarks:
//        只适用于绘制单页多行文字
//  Date:
//        2012-07-17
//  Author:
//        juguanhui@duokan.com
//===========================================================================
#ifndef __UITEXTMULTILINE_H__
#define __UITEXTMULTILINE_H__

#include "GUI/UIWindow.h"
#include "IDKTRender.h"
#include "KernelRenderType.h"

class UITextMultiLine : public UIWindow
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UITextMultiLine";
    }

    UITextMultiLine();
    UITextMultiLine(UIContainer* pParent, const DWORD dwId);
    virtual ~UITextMultiLine();

    bool RenderText();
    void ClearRenderInfo();

    HRESULT Draw(DK_IMAGE drawingImg);

    void SetMargin(int left, int top, int right, int bottom);
    
    void SetFontSize(int fontSize);
    void SetLineGap(int lineGap);
    void SetIndent(int indent);
    void SetAlign(int alignType);

    void SetEndWithEllipsis(bool endWithEllipsis);
    void SetGbFontFile(const wchar_t* pFontFile);
    void SetAnsiFontFile(const wchar_t* pFontFile);
    void SetTextColor(int textColor);

    void SetText(const char* pText);
    void SetText(const wchar_t* pText);
    
private:
    HRESULT CopyRenderImage(DK_IMAGE* _TargetImg,DK_IMAGE* _pCopyImage,DK_RECT _rcSelf);

private:
    int m_leftMargin;
    int m_topMargin;
    int m_rightMargin;
    int m_bottomMargin;
    int m_fontSize;
    int m_lineGap;
    int m_indent;
    IDKTRender::LAYOUT_ALIGN_TYPE m_alignType;
    DK_BOOL m_endWithEllipsis;
    std::wstring m_GbFontFile;
    std::wstring m_AnsiFontFile;
    DK_ARGBCOLOR m_textColor;

    IDKTRender* m_pDKTRender;
    std::wstring m_text;
    IDKTRender::RenderCharInfo* m_pCharInfo;
    DK_UINT  m_charInfoCount;
    IDKTRender::RenderLineInfo* m_pLineInfo;
    DK_UINT  m_lineInfoCount;
};

#endif // __UITEXTMULTILINE_H__
