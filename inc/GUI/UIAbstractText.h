#ifndef UIABSTRACTTEXT_H
#define UIABSTRACTTEXT_H

#include "GUI/UIWindow.h"
#include "GUI/TextDrawer.h"
#include "dkBuffer.h"
using DkFormat::DkBuffer;
using DkFormat::DkBufferBase;

class UIAbstractText : public UIWindow
{
public:
    enum TextMode {
        TextSingleLine,
        TextMultiLine,
        TextMultiPage,
    };

    virtual LPCSTR GetClassName() const
    {
        return "UIAbstractText";
    }

    UIAbstractText();
    UIAbstractText(UIContainer* pParent, const DWORD dwId);

    virtual ~UIAbstractText();
    void SetEraseBackground(bool eraseBackground);

    /**
     * @brief SetAlign 设置对齐方式,默认居左对齐
     *
     * @param iAlign ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
     */
    void SetAlign(INT32 iAlign);
    void SetVAlign(INT32 iVAlign);
    void SetFirstLineIndent(double firstLineIndent);
    void SetTabStop(double tabStop);
    void SetForeGroundColor(INT32 iColor);
    void SetBackGroundColor(INT32 iColor);
    void SetForeColor(INT32 iColor);
    void SetBackColor(INT32 iColor);
    /**
     * @brief SetFontSize 设置字体大小
     *
     * @param iSize 字体大小
     */
    void SetFontSize(INT32 iSize);
    void SetFont(INT32 iFace, INT32 iStyle, INT32 iSize);
    /**
     * @brief SetEndWithEllipsis 设置是否以省略号结尾，默认以省略号结尾
     *
     * @param endWithEllipsis
     */
    void SetEndWithEllipsis(bool endWithEllipsis);
    /**
     * @brief SetStrikeThrough 设置中划线
     *
     * @param strikeThrough 是否绘制中划线
     */
    void SetStrikeThrough(bool strikeThrough);

    /**
     * @brief SetUnderLine 设置下划线
     *
     * @param underLine 是否绘制下划线
     */
    void SetUnderLine(bool underLine);
    /**
     * @brief GetText 获取所有文本内容
     *
     * @return 
     */
    virtual LPCSTR GetText() const;

    unsigned int GetTextLength() const
    {
        return m_TextDrawer.GetTextLength();
    }

    /**
     * @brief SetText 设置所有文本内容
     *
     * @param rstrTxt
     */
    virtual void SetText(const CString& rstrTxt);
    virtual void SetText(const char* pchTxt);
    virtual void SetText(int strId);
    virtual void SetText(const std::wstring& wstrTxt);
    virtual void SetText(const std::string& strTxt)
    {
        return SetText(strTxt.c_str());
    }
    virtual void SetText(SPtr<DkBufferBase> spText);

    void SetKeyword(PCCH _pcchKeyword, UINT _uHightLightStartPos, UINT _uHighLightEndPos);

    /**
     * @brief GetTextHeight 获取文本内容的高度
     *
     * @return 
     */
    virtual int GetTextHeight() const;
    /**
     * @brief GetTextWidth 获取文本内容的宽度，结果可能与SetMaxWidth()函数的调用有关。
     *
     * @return 
     */
    virtual int GetTextWidth() const;

    //virtual dkSize GetMinSize() const
    //{
    //    return dkSize(GetTextWidth(), GetTextHeight());
    //}

    virtual void SetMaxWidth(int maxWidth);
    virtual void SetMaxHeight(int maxHeight);

    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void SetBackTransparent(BOOL bIsTransparent);
    virtual void SetTransparent(BOOL bIsTransparent);
    /**
     * @brief SetFontPath 为TextDrawer指定字体路径
     */
    virtual void SetFontPath();

	void SetAnsiFontFile(const char* ansiFontFile);
    void SetGbFontFile(const char* ansiFontFile);
	
    /**
     * @brief GetAutoModifyWidth 是否自动修改宽度，默认否
     */
    bool GetAutoModifyWidth() const;
    /**
     * @brief SetAutoModifyWidth 设置是否自动修改宽度 
     */
    void SetAutoModifyWidth(bool autoModifyWidth);

    /**
     * @brief GetAutoModifyHeight 是否自动修改高度，默认否
     */
    bool GetAutoModifyHeight() const;
    /**
     * @brief SetAutoModifyHeight 设置是否自动修改高度 
     */
    void SetAutoModifyHeight(bool autoModifyHeight);
    void SetUseTraditionalChinese(bool useTraditionalChinese);

protected:
   void Init();
   void Dispose();
   virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

    /**
     * @brief IColor2ARGBColor 外部使用颜色转换到内核颜色
     *
     * @param iColor 外部使用颜色
     *
     * @return 内核颜色
     */
    DK_ARGBCOLOR IColor2ARGBColor(INT32 iColor);

protected:
    mutable TextDrawer m_TextDrawer;

    void OnDispose(BOOL bIsDisposed);
    
    /// <remark>
    /// This flag is used to avoid unnecessary timer message when this window is not visible.
    /// </remark>
    BOOL m_bIsDisposed;
    BOOL  m_bBackTransparent;
    bool m_bIsAutoModifyWidth;
    bool m_bIsAutoModifyHeight;
    bool m_eraseBackground;
};//UIAbstractText



#endif//UIABSTRACTTEXT_H

