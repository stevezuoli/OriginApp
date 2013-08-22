///////////////////////////////////////////////////////////////////////
// UIDictionaryDialog.h
// Contact: wang mingyin
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef  _UIDICTIONARYDIALOG_H_
#define _UIDICTIONARYDIALOG_H_

#include "GUI/UIButton.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UIDialog.h"
#include "GUI/UIImage.h"
#include "GUI/UIMessageBox.h"
#include "Utility/ImageManager.h"
#include "GUI/UITextBox.h"
#include "GUI/UITextSingleLine.h"
#include "Utility/ColorManager.h"
#include "DkSPtr.h"
#include "GUI/UIText.h"
#include "Utility/Dictionary.h"
#include "GUI/GestureDetector.h"
#include "BookReader/EpubBookReader.h"
#include "DKEAPI.h"
#include "IDKESection.h"

struct DictInfo
{
	string dictName;
	string dictMean;
	int dictPageNum;
};

#define MAX_WORD_SIZE   128

//using namespace DkFormat;

class UIDictionaryDialog:public UIDialog
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIDictionaryDialog";
    }
    UIDictionaryDialog(UIContainer* pParent, const char* pWord);
    UIDictionaryDialog(UIContainer* pParent);
    virtual ~UIDictionaryDialog();
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    void MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height);
    void TranslateAndDisplay();
   
protected:
    void Init();
	void Clear();
    void Dispose();
    void UpdatePageNoBtn();
    void HandleDlgPageUpDown(bool bPageDown);
	string GetTextFromHtml(const string& strMean);
    void DictNotInstall();
    virtual bool OnTouchEvent(MoveEvent* moveEvent);

private:
	UITouchBackButton			m_btnBack;
    UITextSingleLine   				m_txtPageNo;
    UITextSingleLine  			 	m_txtDictName;
    UITextSingleLine   				m_txtNoDict;

    UITextBox           			m_txtLookupWord;
	UIText 			  				m_txtTranslate;
    string 							m_strWord;

	int m_iTotalPageNum;
	vector<DictInfo>  				m_vDictInfo;
	int                 			m_iCurPageIndex;   //在当前字典中的index
	int								m_iCurDictIndex;	//当前字典在vector 中的index

    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
    bool OnSingleTapUp(MoveEvent* moveEvent);
    class DictionDialogGestureListener: public SimpleGestureListener
    {
    public:
        void SetDictionDialog(UIDictionaryDialog* dlg)
        {
            m_pDlg = dlg;
        }
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            return m_pDlg->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
        }
        virtual bool OnSingleTapUp(MoveEvent* moveEvent)
        {
            return m_pDlg->OnSingleTapUp(moveEvent);
        }
    private:
        UIDictionaryDialog* m_pDlg;
    };

    GestureDetector m_gestureDetector;
    DictionDialogGestureListener m_gestureListener;
    UITitleBar m_titleBar;
};
#endif

