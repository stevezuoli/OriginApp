#ifndef UINAVIGATETODLG_H
#define UINAVIGATETODLG_H

#include "I18n/StringManager.h"

#include "GUI/UIModalDialog.h"
#include "GUI/UITextBox.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"

class UINavigateToDlg : public UIModalDlg
{
public:

	enum {
		Invalid,
		//Entry,
		//Tag,
		Internal
	};

	enum DisplayWay {
		DisplayPercent,
		DisplayPage
	};

	virtual LPCSTR GetClassName() const
	{
		return "UINavigateToDlg";
	}

	UINavigateToDlg(UIContainer* parent,DisplayWay dw = DisplayPage);
	~UINavigateToDlg();

	virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
	virtual void Popup();
	virtual void EndDialog(INT32 iEndCode,BOOL fRepaintIt = TRUE);

	//note:if displayway is DisplayPercent, these functionsdo nothing
	//set the default current page
	void SetCurPage( long page);
	//set the total page
	void SetTotalPage(long total);
	long GetGotoPage() const;

	//note:if displayway is DisplayPage these functionsdo nothing
	//set the default current percent
	void SetCurPercent( double percent);
	double GetGotoPercent() const;

private:
	//internal method of initialize UI
	void InitUI();

private:
	DisplayWay m_displayWay;
	string m_strEdit;
	long m_lTotalPage;
	long m_lGotoPage;
	double m_fGotoPercent;

	UITouchComplexButton m_btnToEntry;//开始
	UITouchComplexButton m_btnToTag;//末尾
	UITouchComplexButton m_btnCancel;//取消
	UITouchComplexButton m_btnNavigateTo;//跳转

	UITextSingleLine	 m_textSuffix;//%(页)
	
	UITextBox			 m_editArgs;

};//UINavigateToDlg
#endif//UINAVIGATETODLG_H
