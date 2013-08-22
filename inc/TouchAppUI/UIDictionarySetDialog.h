///////////////////////////////////////////////////////////////////////
// UIDictionarySetDialog.h
// Contact: wang mingyin
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef _UIDICTIONARYSETDIALOG_H_
#define _UIDICTIONARYSETDIALOG_H_

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

#define MAX_WORD_SIZE   128
//using namespace DkFormat;

class UIDictionarySetDialog:public UIDialog
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIDictionarySetDialog";
    }
    UIDictionarySetDialog(UIContainer* pParent,const char* pWord);
    ~UIDictionarySetDialog();
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    BOOL OnKeyPress(INT32 iKeyCode);
    char* GetWords();
    virtual INT32 DoModal();
    virtual void EndDialog(INT32 iEndCode,BOOL fRepaintIt = TRUE);
    void OnDialogUnLoad(void *pSender);
    void OnDialogLoad(void *pSender);
    void OnLoad();

protected:
    void Init();
    HRESULT DrawBackGround(DK_IMAGE drawingImg); 
    UIComplexButton* GetFocusBn(int& index);
    void HandleKeyPress();
    
    
private:
    void ChangeBtnName(int first,int second);
    void DeleteCorrespondingIndex(int index);
    void ClearVectorItem();
    void AddVectorItem(int iStartNo,int iEndNo);
    
    
    void WriteToFile();
    string AddOrDeleteTitle(bool isAdd,const char* pOriName);

private:
    UIComplexButton  m_btnBack;
    UIComplexButton  m_btnUp;
    UIComplexButton  m_btnDown;
    UIComplexButton  m_btnDelete;
    char                   m_szWord[MAX_WORD_SIZE];
    UIMessageBox*     m_pClearMsgBox;
    vector<UIComplexButton*> vec_uibtn;
    int                        m_iIndex;
    vector<Dict_Info>   m_vecDictInfo;
    int             m_iDictCount;
    
};


#endif
