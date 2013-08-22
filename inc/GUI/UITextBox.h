////////////////////////////////////////////////////////////////////////
// UITextBox.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UITEXTBOX_H__
#define __UITEXTBOX_H__

#include "GUI/UIAbstractTextEdit.h"
#include "dkBaseType.h"
#include "GUI/EventArgs.h"
#include "GUI/TextEditController.h"

class UIImage;

#ifdef KINDLE_FOR_TOUCH
typedef BOOL (*FuncKeyPress)(const wchar_t* strText, INT32 iStrLen);
BOOL NormalCallBack(const wchar_t* strText, INT32 iStrLen);
BOOL NumberCallBack(const wchar_t* strText,INT32 iStrLen);
BOOL PercentageCallBack(const wchar_t* strText,INT32 iStrLen);
#else
typedef BOOL (*FuncKeyPress)( const wchar_t* strText, wchar_t* keychar);
BOOL NormalCallBack( const wchar_t* strText, wchar_t* keychar);
BOOL NumberCallBack( const wchar_t* strText, wchar_t* keychar);
BOOL PercentageCallBack( const wchar_t* strText, wchar_t* keychar);
#endif

class UITextBox 
    : public UIAbstractTextEdit
{
public:
    enum EchoMode
    {
        Normal,
        Password,
        PasswordEchoOnEdit
    };

    //static const char* EventTextChange;
    //构造函数
    UITextBox();
    UITextBox(UIContainer* pParent, DWORD rstrId);
    virtual ~UITextBox();
    //系统级
    virtual LPCSTR GetClassName() const {return ("UITextBox");}
    virtual HRESULT UpdateFocus();
    //绘图函数 继承自UIWindow
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    void SetFont(INT32 iFace, INT32 iStyle, INT32 iSize);
    void OnResize(INT32 iWidth, INT32 iHeight);
    EchoMode GetEchoMode() const;
    void SetEchoMode(EchoMode echoMode);
    static void EchoAsAsterisksInstead(void* dummy);
    //消息接收函数 继承自UIWindow
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    void SetImage(SPtr<ITpImage> _spImage);
    void SetImage(int imageID, int focusedImageID);
    
    //设置内容相关行为函数  
    virtual INT32 SetTip(const wchar_t* strText, INT32 iStrLen = 0);   //设置提示字符
    virtual INT32 SetTipUTF8(const char* strText, INT32 iStrLen = 0);
    virtual INT32 SetText(const wchar_t* strText, INT32 iStrLen = 0);   //设置字符
    virtual INT32 SetTextUTF8(const char* strText, INT32 iStrLen = 0);
    virtual std::string GetTextUTF8() const;
    virtual INT32 InsertText(const wchar_t *strText, INT32 iStrLen = 0);   //插入字符
    virtual INT32 DeleteChar();
    void ShowBorder(bool showBorder);
    // 有时候会需要在响应事件的函数里面判断输入值是否合法后的动作，如果对当前的UITextBox进行修改，会在FireEvent里面造成死循环
    void DoFireEvent(bool fireEvent);

    virtual INT32 InsertTextUTF8(LPCSTR strText, INT32 iStrLen = 0);
    virtual INT32 ClearText();                                                //清除字符
    virtual void SetMaxDataLen(INT32 iMaxLen);
	void SetTextBoxStyle(TextBoxStyle eTextBoxStyle);
	FuncKeyPress GetFuncCallBack();
	virtual void SetNumberOnlyMode();
    BOOL IsEmpty()
    {
        return (0 == this->m_iDataLen);
    }
    
    virtual void SetFontSize(int fontSize);
    virtual int GetCurDataLen() const;

    virtual BOOL OnDeletePress();
    virtual BOOL OnEnterPress();
    virtual BOOL OnDirectionPress(int iKeyCode);
#ifdef KINDLE_FOR_TOUCH
    virtual bool OnTouchEvent(MoveEvent* moveEvent);
#endif

protected:
    //基本变量
    wchar_t             m_strData[TEXTBOX_DATA_MAX_LEN + 1];    //数据
    wchar_t             m_strEchoData[TEXTBOX_DATA_MAX_LEN + 1];//绘制时显示的数据
    INT32               m_iMaxDataLen;                            //最大数据长度
    INT32               m_iDataIndex;                           //光标位置 从0开始
    INT32               m_iDataLen;                             //数据长度
    

    //提示功能变量
    wchar_t               m_strTip[TEXTBOX_DATA_MAX_LEN + 1]; //存储提示文本
    //单行模式变量
    INT32               m_iStartIndex;                          //超过文本框之后开始的文字的位置 从0开始
    INT32               m_iEndIndex;                            //结束位置
    INT32               m_iStartPosition;                       //像素偏移量
    
    //其他变量
    INT32               m_rgiALLPosition[TEXTBOX_DATA_MAX_LEN];                     //记录所有坐标      
    EchoMode            m_echoMode;

    //文字 风格设置
    DKFontAttributes m_ifontAttr;
    INT32  m_iFontColor;
    SPtr<ITpImage>  m_spImage;
    SPtr<ITpImage>  m_spFocusedImage;

    //功能
    BOOL m_bIsDisposed;
    bool m_showBorder;
    bool m_fireEvent;
	//the function pointed to should return true if justify ok
	FuncKeyPress KeyPressCallBack;

    virtual BOOL OnActionPress(INT32 iKeyCode);

    //onchange
    void OnChange(INT32 iAction);
    void Change_UpdatePosArr();
    void Change_UpdateIndexs();
    void Change_UpdateStartPos(INT32 iAction);
    void CreateEchoDataFromOriginalData();
    void Init();
};
#endif


