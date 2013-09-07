/*
 * =====================================================================================
 *       Filename:  UIWeiboSharePage.h
 *    Description:  新浪微博分享页面
 *
 *        Version:  1.0
 *        Created:  12/28/2012 05:29:11 PM
 * =====================================================================================
 */

#ifndef UIWEIBOSHAREDPAGE_H
#define UIWEIBOSHAREDPAGE_H

#include "GUI/UIPage.h"
#include "GUI/UIPlainTextEdit.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#include "GUI/UIBackButton.h"
#endif
#include "CommonUI/UITitleBar.h"
#include "GUI/UIImageTransparent.h"

class UISizer;
class UIWeiboSharePage : public UIPage
{
public:
    static std::string PruneStringForDKComment(const std::string& bookName, const std::string& originalText);
    UIWeiboSharePage(const string& sharedContent);
    ~UIWeiboSharePage();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    bool OnWeiboShareUpdate(const EventArgs& args);
    bool SetSharedContent(const string& sharedContent);
    bool SetSharedPicture(const char* absoluteFilePath, bool autoDeleteFile = true);
protected:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
    bool InitUI();
    void ShareWeibo();
    bool OnContentEditChanged(const EventArgs& args);

private:
    string m_weiboSharedContent;
    string m_weiboSharedPicturePath;
    bool m_autoDeletePictureFile;
    UITitleBar m_titleBar;
    UIPlainTextEdit m_weiboContentEdit;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_weiboSharedButton;
    UITouchBackButton m_backButton;
#else
    UIComplexButton m_weiboSharedButton;
    UIBackButton m_backButton;
#endif
    UITextSingleLine m_userNameLabel;
    UITextSingleLine m_wordCountTipsLabel;
    UITextSingleLine m_titleShareLabel;
    UISizer* m_topSizer;
	UISizer* m_pictureSizer;
	UIImageTransparent m_sharePicture;
    UITextSingleLine m_textSharePic;
};//UIWeiboSharedPage

#endif//UIWEIBOSHAREDPAGE_H
