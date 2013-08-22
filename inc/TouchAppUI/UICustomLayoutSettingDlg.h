#ifndef _UICUSTOMLAYOUTSETTINGDLG_H_
#define _UICUSTOMLAYOUTSETTINGDLG_H_
#include "GUI/UIDialog.h"
#include "GUI/UIImage.h"
#include "GUI/UIButton.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITextBox.h"
#include <string>
class UICustomLayoutSettingDlg : public UIDialog
{
public:
    UICustomLayoutSettingDlg();
    UICustomLayoutSettingDlg(UIContainer* pParent);
    ~UICustomLayoutSettingDlg();

    virtual LPCSTR GetClassName() const
    {
        return ("UICustomLayoutSettingDlg");
    }

    int GetLineSpacing()const;
    int GetParaSpacing()const;
    int GetIndent()const;
    void InitCurrentSpacing(int _iLineSpacing,int _iParaSpacing,int _iIndent);
private:
    void Init();
    HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
private:
    std::string m_strLineSpacingValue;
    std::string m_strParaSpacingValue;
    std::string m_strIndentValue;

    UITextSingleLine m_txtLineSpacing;
    UITextSingleLine m_txtParaSpacing;
    UITextSingleLine m_txtIndent;

    UITextBox m_boxInputLineSpacing;
    UITextBox m_boxInputParaSpacing;
    UITextBox m_boxInputIndet;

    UIComplexButton m_btnOk;
    UIComplexButton m_btCancel;
};
#endif//_UICUSTOMLAYOUTSETTINGDLG_H_
