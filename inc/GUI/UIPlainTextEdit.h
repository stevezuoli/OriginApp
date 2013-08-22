#ifndef UIPlAINTEXTEDIT_H
#define UIPlAINTEXTEDIT_H

#include "GUI/UIAbstractTextEdit.h"
#include "GUI/UITextSingleLine.h"

class UIPlainTextEdit : public UIAbstractTextEdit
{
public:
    //构造函数
    UIPlainTextEdit();
    UIPlainTextEdit(UIContainer* pParent, DWORD rstrId);
    virtual ~UIPlainTextEdit();
    //系统级
    virtual LPCSTR GetClassName() const {return ("UIPlainTextEdit");}
    virtual bool OnTextEditChange(const EventArgs& args);

    //设置内容相关行为函数  
    virtual int SetTip(const wchar_t *strText,int iStrLen = 0);   //设置提示字符
    virtual int SetTipUTF8(LPCSTR strText,int iStrLen = 0);
    virtual int SetText(const wchar_t *strText,int iStrLen = 0);   //设置字符
    virtual int SetTextUTF8(LPCSTR strText,int iStrLen = 0);
    virtual std::string GetTextUTF8() const;
    virtual int InsertText(const wchar_t *strText,int iStrLen = 0);   //插入字符
    virtual int DeleteChar();

    virtual int InsertTextUTF8(LPCSTR strText, int iStrLen = 0);
    virtual int ClearText();                                                //清除字符

    virtual void SetMaxDataLen(int iMaxLen);
    virtual void SetFontSize(int fontSize);
    virtual int GetCurDataLen() const;


    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void MoveWindow(int iLeft, int iTop, int iWidth, int iHeight);
    virtual BOOL OnKeyPress(int iKeyCode);
    virtual BOOL OnActionPress(int iKeyCode);
    virtual BOOL OnDeletePress();
    virtual BOOL OnEnterPress();
    virtual BOOL OnDirectionPress(int iKeyCode);

    virtual BOOL OnPageUpDownPress(int iKeyCode);
#ifdef KINDLE_FOR_TOUCH
    virtual bool OnTouchEvent(MoveEvent* moveEvent);
#endif
    virtual HRESULT UpdateFocus();
protected:

    void Init();
    void UnInit();
    HRESULT DrawPageBar(DK_IMAGE& image);

protected:
    TextEditController m_textEditController;
    int m_curPage;
    int m_buttonHeight;
    UITextSingleLine m_pageInfos;
    SPtr<ITpImage> m_leftImage;
    SPtr<ITpImage> m_leftDisImage;
    SPtr<ITpImage> m_rightImage;
    SPtr<ITpImage> m_rightDisImage;
};//UIPlainTextEdit
#endif//UIPlAINTEXTEDIT_H
