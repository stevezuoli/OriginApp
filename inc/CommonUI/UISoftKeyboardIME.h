#ifndef __UISOFTKEYBOARDIME_H__
#define __UISOFTKEYBOARDIME_H__

#include "CommonUI/UIIME.h"
#include "GUI/UIAbstractTextEdit.h"
#include "GUI/UITouchButton.h"
#include "GUI/UITextSingleLine.h"
#include "IME/IIMEBase.h" 
#include "singleton.h"
#include <vector>


class UISoftKeyboardIME : public UIIME
{
    SINGLETON_H(UISoftKeyboardIME);
private:
    UISoftKeyboardIME();
    virtual ~UISoftKeyboardIME();

public:
    static const int UISOFTKEYBOARDIME_KEYCOUNT_LINE0 = 10;
    static const int UISOFTKEYBOARDIME_KEYCOUNT_LINE1 = 9;
    static const int UISOFTKEYBOARDIME_KEYCOUNT_LINE2 = 9;
    static const int UISOFTKEYBOARDIME_KEYCOUNT_LINE3 = 6;

    virtual LPCSTR GetClassName() const
    {
        return "UISoftKeyboardIME";
    }

    virtual BOOL OnKeyPress(INT32 iKeyCode);

    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual HRESULT Draw(DK_IMAGE image);
    virtual void OnChildSetFocus(UIWindow* pWindow);

    void ShowIME(UIContainer *pParent = NULL);
	void HideIME();

    void SetBackground(SPtr<ITpImage> pBackgroundImage);

    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    void HandlePYWordsButton();
    void SetInputMode(int mode);
#ifdef KINDLE_FOR_TOUCH
    virtual bool OnHookTouch(MoveEvent* moveEvent);
#endif

private:
	void SetupKeyboard();
    void ClearCandidates();
    void PYWordsPageUpDown(bool _bPageDown);
	void ModifyBoxText(const std::string& keyboardText);
    bool HasCandiatesToShow() const;

private:
    void SwitchMode(INPUTMODE oldMode, INPUTMODE newMode);
    void ClearPinYinData();
    void IMEReset();
    void UpdateShiftKey(INPUTMODE oldMode, INPUTMODE newMode);
    void UpdateABCOrNumKey(INPUTMODE oldMode, INPUTMODE newMode);
    void UpdatePageNameKey(INPUTMODE oldMode, INPUTMODE newMode);
    eProcessResult IMEInputKey(char code);
    eProcessResult IMEDeleteLastInput();
    std::string IMEGetStringByIndex(int index);
    std::string IMEGetInputString();
    std::string IMEGetResultString();
    eProcessResult IMESelectIndex(int index);
    int GetKeyTableIndex(INPUTMODE mode);
    bool IsValidMode(int mode);
    void ProcessSingleChar(int index);

    bool IsChineseMode() const
    {
        return m_inputMode == IUIIME_CHINESE_LOWER;
    }

    // 每一页上可输入的按钮数量(不包括shift, enter等控制) 

    static const int MAX_SELECTORS = 17;
    static const int KEYBOARD_COUNT = 36;
    UITextSingleLine m_pyLabel;
    UITouchButton  m_KeyboardButton[KEYBOARD_COUNT];

    SPtr<ITpImage> m_spBackground;
    bool          m_bUsingBackgroundPicture;

    std::vector<UITouchButton*> m_pvWordButton;

    UIWindow*   m_pPreFocusWnd;
    INPUTMODE m_inputMode;
    // 可能的拼音选项每页第一个的索引
    std::vector<int> m_pyCandidateIndexes;
    int m_curPyIndex;
#ifdef KINDLE_FOR_TOUCH
    UITouchButton* m_backspaceButton;
#endif
    virtual bool OnChildLongPress(UIWindow* child);
    int m_keyTableIndex;
};

#endif //__UISOFTKEYBOARDIME_H__
