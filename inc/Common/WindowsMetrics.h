/*
  =====================================================================================
        Filename:  WindowsMetrics.h
     Description:  当某些窗口需要强制指定大小或者坐标时，记录该值
 
         Version:  1.0
         Created:  09/25/2012 11:46:29 AM
  =====================================================================================
 */

#ifndef WINDOWSMETRICS_H
#define WINDOWSMETRICS_H

#define METRICS_LIST(MACRO_NAME) \
    MACRO_NAME(UIPixelValue3, 3, 4)\
    MACRO_NAME(UIPixelValue5, 5, 7)\
    MACRO_NAME(UIPixelValue8, 8, 10)\
    MACRO_NAME(UIPixelValue10, 10, 13)\
    MACRO_NAME(UIPixelValue12, 12, 15)\
    MACRO_NAME(UIPixelValue13, 13, 16)\
    MACRO_NAME(UIPixelValue14, 14, 18)\
    MACRO_NAME(UIPixelValue15, 15, 19)\
    MACRO_NAME(UIPixelValue16, 16, 20)\
    MACRO_NAME(UIPixelValue20, 20, 26)\
    MACRO_NAME(UIPixelValue22, 22, 28)\
    MACRO_NAME(UIPixelValue23, 23, 29)\
    MACRO_NAME(UIPixelValue26, 26, 33)\
    MACRO_NAME(UIPixelValue30, 30, 38)\
    MACRO_NAME(UIPixelValue32, 32, 41)\
    MACRO_NAME(UIPixelValue40, 40, 51) \
    MACRO_NAME(UIPixelValue42, 42, 53) \
    MACRO_NAME(UIPixelValue45, 45, 57) \
    MACRO_NAME(UIPixelValue48, 48, 61) \
    MACRO_NAME(UIPixelValue50, 50, 64) \
    MACRO_NAME(UIPixelValue55, 55, 70) \
    MACRO_NAME(UIPixelValue58, 58, 74) \
    MACRO_NAME(UIPixelValue60, 60, 77) \
    MACRO_NAME(UIPixelValue61, 61, 78) \
    MACRO_NAME(UIPixelValue80, 80, 102) \
    MACRO_NAME(UIPixelValue95, 95, 121)\
    MACRO_NAME(UIPixelValue97, 97, 124)\
    MACRO_NAME(UIPixelValue100, 100, 128)\
    MACRO_NAME(UIPixelValue105, 105, 134)\
    MACRO_NAME(UIPixelValue112, 112, 143)\
    MACRO_NAME(UIPixelValue115, 115, 147)\
    MACRO_NAME(UIPixelValue120, 120, 153)\
    MACRO_NAME(UIPixelValue128, 128, 163)\
    MACRO_NAME(UIPixelValue135, 135, 172)\
    MACRO_NAME(UIPixelValue142, 142, 181)\
    MACRO_NAME(UIPixelValue150, 150, 192)\
    MACRO_NAME(UIPixelValue155, 155, 198)\
    MACRO_NAME(UIPixelValue400, 400, 512)\
    MACRO_NAME(UIHorizonMargin, 30, 38)\
    MACRO_NAME(UIElementSpacing, 3, 5)\
    MACRO_NAME(UIElementMidSpacing, 10, 13)\
    MACRO_NAME(UIElementLargeSpacing, 15, 19)\
    MACRO_NAME(UIDefaultInternalBorder, 5, 6)\
    MACRO_NAME(UIBtnBorderLine, 2, 2)\
    MACRO_NAME(UIBtnDefaultHorizontalMargin, 12, 15)\
    MACRO_NAME(UIBtnDefaultVerticalMargin, 8, 10)\
    MACRO_NAME(UIDialogBorderLine, 2, 3)\
    MACRO_NAME(UINormalBtnHeight, 42, 54) \
    MACRO_NAME(UINormalBtnWidth, 160, 205) \
    MACRO_NAME(UINormalSmallBtnWidth, 73, 93)\
    MACRO_NAME(UIBackBtnHeight, 40, 51)\
    MACRO_NAME(UIBackBtnWidth, 60, 76)\
    MACRO_NAME(UIBackBtnRightBorder, 10, 13)\
    MACRO_NAME(UIBackBtnSeperatorHeight, 32, 40)\
    MACRO_NAME(UITitleBarHeight, 30, 38)\
    MACRO_NAME(UITitleBarElementSpace, 4, 5)\
    MACRO_NAME(UITitleBarTopOffSet, 2, 2)\
    MACRO_NAME(UIBottomBarHeight, 62, 79)\
    MACRO_NAME(UICartPageBottomBarHeight, 170, 220)\
    MACRO_NAME(UICartSpecialEventWidth, 460, 600)\
    MACRO_NAME(UIMiddleBarHeight, 50, 68)\
    MACRO_NAME(UIBottomBarSplitLineHeight, 31, 40)\
    MACRO_NAME(UIBottomBarFocusedLinePixel, 7, 9)\
    MACRO_NAME(UIButtonGroupTopLinePixel, 1, 1)\
    MACRO_NAME(UIButtonGroupBottomLinePixel, 1, 1)\
    MACRO_NAME(UIButtonGroupSplitLineTopPadding, 5, 6)\
    MACRO_NAME(UIBottomBoxHeight, 61, 78)\
    MACRO_NAME(UIProgressBarBorder, 1, 1)\
    MACRO_NAME(UIProgressBarTopMargin, 3, 3)\
    MACRO_NAME(UIProgressBarLineHeight, 3, 3)\
    MACRO_NAME(UIProgressBarDistance, 1, 1)\
    MACRO_NAME(UIBookReaderChapterTitleTopMargin, 10, 13)\
    MACRO_NAME(UIBookReaderChapterTitleHeight, 16, 20)\
    MACRO_NAME(UIBookReaderProgressBarHeight, 18, 24)\
    MACRO_NAME(UIBookMenuHeight, 156, 200)\
    MACRO_NAME(UIBookMenuWidth, 400, 512)\
    MACRO_NAME(UIBookMenuMargin, 26, 33)\
    MACRO_NAME(UIBookMenuBtnHeight, 40, 51)\
    MACRO_NAME(UIHomePageTopBarHeight, 60, 77)\
    MACRO_NAME(UIHomePageSpaceOverBottomBar, 20, 26)\
    MACRO_NAME(UIHomePageSearchBoxTopMargin, 24, 30)\
    MACRO_NAME(UIHomePageSearchBoxBottomMargin, 6, 7)\
    MACRO_NAME(UIHomePageSearchBoxWidth, 380, 486)\
    MACRO_NAME(UIHomePageSearchBoxHeight, 48, 61)\
    MACRO_NAME(UIHomePageToolsBarTopMargin, 24, 30)\
    MACRO_NAME(UIHomePageToolsBarBottomMargin, 6, 7)\
    MACRO_NAME(UIHomePageSearchDlgTopMargin, 95, 121)\
    MACRO_NAME(UIHomePageSearchDlgWidth, 560, 729)\
    MACRO_NAME(UIHomePageSearchDlgHeight, 41, 52)\
    MACRO_NAME(UIHomePageUpperFolderWidth, 90, 115)\
    MACRO_NAME(UIHomePageUpperFolderHeight, 40, 51)\
    MACRO_NAME(UIHomePageListItemHeight, 64, 82)\
    MACRO_NAME(UIHomePageListTextHeight, 40, 51)\
    MACRO_NAME(UIHomePageListTipsBorder, 5, 6)\
    MACRO_NAME(UIHomePageListItemSpacing, 3, 3)\
    MACRO_NAME(UIHomePageListAuthorLeftBorder, 10, 12)\
    MACRO_NAME(UIHomePageMargin, 30, 38)\
    MACRO_NAME(UIHomePageAuthorMinWidth, 156, 200)\
    MACRO_NAME(UIHomePageFileNameMinWidth, 156, 200)\
    MACRO_NAME(UIHomePageBookNameMaxWidth, 300, 384)\
    MACRO_NAME(UIHomePageImageMaxWidth, 46, 58)\
    MACRO_NAME(UIHomePageSortTypeButtonWidth, 124, 159)\
    MACRO_NAME(UIHomePageSearchButtonWidth, 59, 76)\
    MACRO_NAME(UIHomePageSearchButtonHeight, 50, 64)\
    MACRO_NAME(UIHomePageCoverViewItemLeft, 30, 38)\
    MACRO_NAME(UIHomePageCoverViewItemWidth, 174, 224)\
    MACRO_NAME(UIHomePageCoverViewItemHeight, 306, 390)\
    MACRO_NAME(UIHomePageCoverViewHorzInterval, 188, 237)\
    MACRO_NAME(UIHomePageCoverViewVertInterval, 312, 390)\
    MACRO_NAME(UIReaderSettingDlgBackToShelfButtonWidth, 73, 93)\
    MACRO_NAME(UIReaderSettingDlgButtonWidth, 64, 81)\
    MACRO_NAME(UIReaderSettingDlgTopBarHeight, 82, 104)\
    MACRO_NAME(UIReaderSettingDlgBottomBarHeight, 104, 133)\
    MACRO_NAME(UIReaderSettingDlgBottomBarButtonMargin, 20, 25)\
    MACRO_NAME(UIReaderSettingDlgMiddleButtonsVerticalMargin, 20, 25)\
    MACRO_NAME(UIReaderSettingDlgSplitButtonWidth, 135, 173)\
    MACRO_NAME(UIReaderSettingDlgSplitButtonHeight, 56, 72)\
    MACRO_NAME(UIReaderSettingDlgReaderSettingButtonWidth, 120, 153)\
    MACRO_NAME(UIReaderSettingDlgReaderSettingSubTitleWidth, 200, 256)\
    MACRO_NAME(UIReaderSettingDlgReaderSettingFontSetting, 270, 346)\
    MACRO_NAME(UIReaderSettingDlgFontSettingButtonDlgWidth, 240, 307)\
    MACRO_NAME(UIReaderSettingDlgFontSettingButtonWidth, 216, 276)\
    MACRO_NAME(UIReaderSettingDlgFontSettingButtonMargin, 26, 34)\
    MACRO_NAME(UIReaderSettingDlgJumpTextBoxWidth, 80, 102)\
    MACRO_NAME(UIDKBookStoreIndexPageSearchBarHeight, 80, 102)\
    MACRO_NAME(UIDKBookStoreIndexPageSearchBoxHeight, 48, 61)\
    MACRO_NAME(UIDKBookStoreIndexPageVerticalMargin, 15, 20)\
    MACRO_NAME(UIDKBookStoreIndexPageHorizonMargin, 9, 12)\
    MACRO_NAME(UIDKBookStoreIndexPageCoverHeight, 142, 181)\
    MACRO_NAME(UIDKBookStoreIndexPageCoverItemSpacing, 25, 32)\
    MACRO_NAME(UIDKBookStoreIndexPageLineLeftMargin, 8, 10)\
    MACRO_NAME(UIDKBookStoreIndexPageTextLeftMargin, 22, 28)\
    MACRO_NAME(UIDKBookStoreIndexPageItemRightMargin, 27, 35)\
    MACRO_NAME(UIDKBookStoreIndexPageItemTopMargin, 22, 28)\
    MACRO_NAME(UIDKBookStoreIndexPageItemWidth, 261, 334)\
    MACRO_NAME(UIDKBookStoreIndexPageItemHeight, 95, 122)\
    MACRO_NAME(UIFootNoteDlgMargin, 20, 25)\
    MACRO_NAME(UIFootNoteDlgBottomMargin, 10, 12)\
    MACRO_NAME(UIFootNoteDlgSpace, 5, 6)\
    MACRO_NAME(UIFootNoteDlgBottomHeight, 20, 25)\
    MACRO_NAME(UIFootNoteDlgWidth, 520, 665)\
    MACRO_NAME(UIFootNoteDlgHeight, 270, 345)\
    MACRO_NAME(UIBookContentSearchDlgBackButtonWidth, 73, 93)\
    MACRO_NAME(UIBookContentSearchDlgButtonMargin, 20, 25)\
    MACRO_NAME(UIBookContentSearchDlgBarMargin, 15, 19)\
    MACRO_NAME(UIBookContentSearchDlgBottomMargin, 30, 38)\
    MACRO_NAME(UIBookContentSearchListItemTopMargin, 20, 25)\
    MACRO_NAME(UITocDlgTitleTopMargin, 20, 25)\
    MACRO_NAME(UITocDlgTitleBottomMargin, 26, 33)\
    MACRO_NAME(UITocDlgBottomButtonHeight, 60, 76)\
    MACRO_NAME(UITocDlgContinueReadButtonWidth, 180, 230)\
    MACRO_NAME(UITocDlgListTopMargin, 6, 7)\
    MACRO_NAME(UITocDlgHorizonMargin, 40, 51)\
    MACRO_NAME(UITocDlgAddBookMarkImageX, 342, 443)\
    MACRO_NAME(UITocDlgAddBookMarkImageTOP, 187, 236)\
	MACRO_NAME(UITocDlgExportBtnHeight, 38, 48)\
    MACRO_NAME(UIBookMarkListItemHeight, 138, 176)\
    MACRO_NAME(UIBookCommentListItemHeight, 184, 236)\
    MACRO_NAME(UIBookMarkContentHeight, 66, 84)\
    MACRO_NAME(UIBookDigestContentHeight, 61, 78)\
    MACRO_NAME(UIBookCommentHeight, 108, 138)\
    MACRO_NAME(UIBookMarkBottomBorder, 10, 13)\
    MACRO_NAME(UIBookMarkTimeLabelBorder, 10, 13)\
    MACRO_NAME(UIBookMarkContentLabelBorder, 15, 19)\
    MACRO_NAME(UIBasicListItemHeight, 70, 89)\
    MACRO_NAME(UIBookCoverViewSmallWidth, 66, 85)\
    MACRO_NAME(UIBookCoverViewSmallHeight, 90, 115)\
    MACRO_NAME(UIBookCoverViewBigWidth, 120, 154)\
    MACRO_NAME(UIBookCoverViewBigHeight, 160, 205)\
    MACRO_NAME(UIBookStoreBookAbstractPageBackButtonAbstractMaxWidth, 540, 708)\
    MACRO_NAME(UIBookStoreBookAbstractPageBackButtonVerticalMargin, 26, 34)\
    MACRO_NAME(UIBookStoreRecommendListItemTopBorder, 15, 19)\
    MACRO_NAME(UIBookStoreRecommendListItemMargin, 10, 13)\
    MACRO_NAME(UIBookStoreRecommendListItemCoverRightBorder, 26, 33)\
    MACRO_NAME(UIBookStoreRecommendListItemButtonHeight, 30, 38)\
    MACRO_NAME(UIBookStoreRecommendListItemButtonWidth, 75, 96)\
    MACRO_NAME(UIBookStoreCategoryListItemTopBorder, 30, 28)\
    MACRO_NAME(UIBookStoreCategoryListItemSpace, 12, 15)\
    MACRO_NAME(UIBookStoreCategoryListItemCoverBorder, 16, 20)\
    MACRO_NAME(UIBookStoreTurnPageButtonWidth, 72, 92)\
    MACRO_NAME(UIBookStoreTurnPageButtonHeight, 36, 46)\
    MACRO_NAME(UIBookStoreTurnPageMaring, 120, 154)\
    MACRO_NAME(UIModalDialogTitleVertBorder, 19, 24)\
    MACRO_NAME(UIModalDialogHorizonMargin, 22, 28)\
    MACRO_NAME(UIModalDialogEditBox, 42, 54)\
    MACRO_NAME(UIModalDialogBtnsHeight, 63, 80)\
    MACRO_NAME(UIModalDialogWidth, 497, 636)\
    MACRO_NAME(UIAccountLoginDialogMargin, 24, 31)\
    MACRO_NAME(UIForgetPasswordDialogMargin, 10, 13)\
    MACRO_NAME(UIUtilityDlgTopY, 125, 160)\
    MACRO_NAME(UISoftKeyboardIMETopY, 448, 573)\
    MACRO_NAME(UISoftKeyboardIMETopMargin, 30, 38)\
    MACRO_NAME(UIUINumInputIMEBtnWidth, 115, 144)\
    MACRO_NAME(UIUINumInputIMEBtnHeight, 61, 77)\
	MACRO_NAME(UIUINumInputIMEElementMargin, 8, 10)\
	MACRO_NAME(UIUINumInputIMEBorder, 16, 20)\
    MACRO_NAME(UINavigateToDlgVertMargin, 22, 28)\
    MACRO_NAME(UINavigateToDlgEditBoxRightMargin, 12, 15)\
    MACRO_NAME(UINavigateToDlgEditBoxWidth, 78, 100)\
    MACRO_NAME(UINavigateToDlgBtnWidth, 72, 92)\
    MACRO_NAME(UINavigateToDlgBottomMargin, 28, 36)\
    MACRO_NAME(UICommentExportDlgHorizonMargin, 27, 36)\
    MACRO_NAME(UICommentExportDlgTopMargin, 16, 20)\
    MACRO_NAME(UICommentExportDlgWidgetsMargin, 10, 13)\
    MACRO_NAME(UICommentExportDlgButtonWidth, 70, 90)\
    MACRO_NAME(UICommentExportDlgButtonHeight, 30, 38)\
    MACRO_NAME(UICommentExportDlgWidth, 400, 512)\
    MACRO_NAME(UICommentExportDlgHeight, 162, 207)\
    MACRO_NAME(UISystemUpdateDlgTextHeight, 360, 461)\
    MACRO_NAME(UISoftKeyboardIMEHorizonMargin, 11, 18)\
    MACRO_NAME(UISoftKeyboardIMEKeyboardSize, 50, 64)\
    MACRO_NAME(UISoftKeyboardIMEKeyboardWidth, 79, 100)\
    MACRO_NAME(UISoftKeyboardIMESpaceKeyWidth, 188, 238)\
    MACRO_NAME(UISoftKeyboardIMEKeyboardSpacing, 7, 6)\
    MACRO_NAME(UISoftKeyboardIMEPYLabelMinHeight, 32, 41)\
    MACRO_NAME(UISoftKeyboardIMEPYLabelVerticalMargin, 7, 9)\
    MACRO_NAME(UISoftKeyboardIMEHeight, 352, 451)\
    MACRO_NAME(UIModifyAccountInfoDlgVertMargin, 20, 25)\
    MACRO_NAME(UIWifiPasswordDlgPwdLabelTopMargin, 16, 21)\
    MACRO_NAME(UIWifiPasswordDlgPwdLabelBottomMargin, 12, 15)\
    MACRO_NAME(UIWifiPasswordDlgButtonTopMargin, 24, 31)\
    MACRO_NAME(UIWifiPasswordDlgHeight, 215, 275)\
    MACRO_NAME(UIWifiSSIDPasswordDialogEditBoxWidth, 284, 364)\
    MACRO_NAME(UIWifiSSIDPasswordDialogTextWidth, 90, 115)\
    MACRO_NAME(UIWifiSSIDPasswordDialogTopMargin, 18, 23)\
    MACRO_NAME(UIWifiSSIDPasswordDialogWidgetsMargin, 14, 18)\
    MACRO_NAME(UIWifiSSIDPasswordDialogWidth, 420, 537)\
    MACRO_NAME(UIWifiSSIDPasswordDialogHeight, 385, 495)\
    MACRO_NAME(UIWifiDialogTitleTopMargin, 26, 33)\
    MACRO_NAME(UIWifiDialogTitleBottomMargin, 52, 67)\
    MACRO_NAME(UIWifiDialogWifiStatusLabelTopMargin, 7, 9)\
    MACRO_NAME(UIWifiDialogWifiStatusLabelBottomMargin, 40, 51)\
    MACRO_NAME(UIWifiDialogListTopMargin, 3, 5)\
    MACRO_NAME(UIWifiDialogRefreshBtnWidth, 110, 141)\
    MACRO_NAME(UIWifiDialogRefreshBtnHeight, 30, 39)\
    MACRO_NAME(UIWifiDialogBottomHeight, 30, 39)\
    MACRO_NAME(UIWifiListItemHeight, 64, 82)\
    MACRO_NAME(UIWifiListItemStatusImgRightMargin, 10, 13)\
    MACRO_NAME(UIWifiListItemSSIDBottomMargin, 13, 17)\
    MACRO_NAME(UIComplexDlgVerticalMargin, 12, 15)\
    MACRO_NAME(UIComplexDlgHorizonMargin, 12, 15)\
    MACRO_NAME(UIComplexDlgControlMargin, 12, 15)\
    MACRO_NAME(UICommentOrSummaryMaxHeight, 320, 410)\
    MACRO_NAME(UICommentOrSummaryBtnHeight, 40, 51)\
    MACRO_NAME(UICommentOrSummaryImageWidth, 500, 640)\
    MACRO_NAME(UICommentOrSummaryImageHeiht, 120, 153)\
    MACRO_NAME(UIShortCutBarHeight, 108, 138)\
    MACRO_NAME(UIShortCutPagePushUpHeight, 44, 56)\
    MACRO_NAME(UIDownloadListItemHeight, 110, 141)\
    MACRO_NAME(UIDownloadListItemButtonWidth, 44, 56)\
    MACRO_NAME(UIDownloadListItemButtonHeight, 42, 54)\
    MACRO_NAME(UIDownloadListItemButtonsMargin, 14, 18)\
    MACRO_NAME(UIDownloadListItemNameLabelBottomMargin, 6, 8)\
    MACRO_NAME(UIDownloadListItemStatusTopMargin, 20, 27)\
    MACRO_NAME(UIDownloadListItemButtonMargin, 10, 13)\
    MACRO_NAME(UITabBarHeight, 48, 61) \
    MACRO_NAME(UITouchComplexButtonInternalHorzSpacing, 5, 6) \
    MACRO_NAME(UITouchComplexButtonInternalVertSpacing, 5, 6) \
    MACRO_NAME(UITouchComplexButtonLeftMargin, 8, 10) \
    MACRO_NAME(UITouchComplexButtonTopMargin, 2, 3) \
    MACRO_NAME(UITextBoxLeftMargin, 4, 6) \
    MACRO_NAME(UITextBoxTopMargin, 4, 6) \
    MACRO_NAME(UITextBoxBorderLinePixel, 1, 1) \
    MACRO_NAME(UIPlainTextEditBorderLinePixel, 2, 3) \
    MACRO_NAME(UISystemSettingPageNewTopMargin, 4, 5) \
    MACRO_NAME(UISettingPanelSectionLeftMargin, 32, 41) \
    MACRO_NAME(UISettingPanelItemLeftMargin, 54, 69) \
    MACRO_NAME(UISettingPanelLeftHalfWidth, 272, 348) \
    MACRO_NAME(UISettingPanelRightHalfWidth, 328, 410) \
    MACRO_NAME(UISettingPanelTitleUpSpacing, 56, 72) \
    MACRO_NAME(UISettingPanelTitleDownSpacing, 26, 33) \
    MACRO_NAME(UISettingPanelItemVertSpacing, 30, 38) \
    MACRO_NAME(UISettingPanelItemTipVertSpacing, 5, 6) \
    MACRO_NAME(UISettingPanelItemHorzSpacing, 30, 38) \
    MACRO_NAME(UISettingPanelButtonLeftMargin, 15, 19) \
    MACRO_NAME(UISettingPanelButtonTopMargin, 5, 6) \
    MACRO_NAME(UISettingPanelDateTimeHorzSpacing, 3, 4) \
	MACRO_NAME(UISettingPanelWifiItemWidth, 200, 256) \
    MACRO_NAME(UIButtonDlgVertSpacingFromAttachedButton, 5, 6) \
    MACRO_NAME(UIButtonDlgButtonHeight, 38, 49) \
    MACRO_NAME(UIButtonDlgButtonLeftMargin, 22, 29) \
    MACRO_NAME(UIButtonDlgBorderLine, 2, 3) \
    MACRO_NAME(FontMenuWidth, 200, 256) \
    MACRO_NAME(FontSizeMenuWidth, 85, 108) \
    MACRO_NAME(LayoutModeMenuWidth, 113, 145) \
    MACRO_NAME(InputMethodMenuWidth, 130, 166) \
    MACRO_NAME(IMEUpVertSpacing, 30, 38) \
    MACRO_NAME(UIMessageBoxWidth, 497, 636) \
    MACRO_NAME(UIMessageBoxTextVertBorder, 23, 30) \
    MACRO_NAME(UIMessageBoxTextHorzBorder, 20, 25) \
    MACRO_NAME(UIMessageBoxBtnHeight, 64, 82) \
    MACRO_NAME(UIDateSettingDlgWidth, 340, 430)\
    MACRO_NAME(UIDateSettingDlgHeight, 290, 370)\
    MACRO_NAME(UIBookDetailPageLeftMargin, 10, 13)\
    MACRO_NAME(UIBookDetailPageTablePageHeight, 710, 909)\
    MACRO_NAME(UIBookDetailPageBtnBackWidth, 73, 93)\
    MACRO_NAME(UIBookDetailPageBtnBackHeight, 40, 51)\
    MACRO_NAME(UIBookDetailPageControlHorizonMargin, 10, 13)\
    MACRO_NAME(UITablePageTableBorder, 2, 2)\
    MACRO_NAME(UITablePageBtnTitleHeight, 60, 77)\
    MACRO_NAME(UITablePageTableMargin, 10, 13)\
    MACRO_NAME(UIBookStoreBookInfoPageHeight, 640, 819)\
    MACRO_NAME(UIBookStoreBookInfoPageWidth, 570, 730)\
    MACRO_NAME(UIBookStoreBookInfoControlSpeacing, 10, 13)\
    MACRO_NAME(UIBookStoreBookInfoSepterSpacing, 30, 39)\
    MACRO_NAME(UIBookStoreBookInfoHorizontalMargin, 10, 13)\
    MACRO_NAME(UIBookStoreBookInfoItemLeftMargin, 50, 65)\
    MACRO_NAME(UIBookStoreBookInfoAbstractHeight, 185, 237)\
    MACRO_NAME(UIBookStoreBookInfoBtnWidth, 94, 120)\
    MACRO_NAME(UIBookStoreBookInfoBtnHeight, 27, 34)\
    MACRO_NAME(UIBookStoreBookInfoRelatedBookCoverWidth, 60, 77)\
    MACRO_NAME(UIBookStoreBookInfoRelatedBookCoverHeight, 78, 100)\
    MACRO_NAME(UIBookStoreBookInfoRelatedBookTitleHeight, 55, 70)\
    MACRO_NAME(UIBookStoreBookInfoRelatedBookTitleWidth, 90, 115)\
    MACRO_NAME(UIBookStoreCommentItemCommentHeight, 55, 68)\
    MACRO_NAME(UIBookStoreCommentListBoxItemHeight, 165, 211)\
    MACRO_NAME(UIBookStoreCommentLineTopMargin, 53, 68)\
    MACRO_NAME(UIFrontLightTopMargin, 2, 2)\
    MACRO_NAME(UIBookStoreAddCommentPageLeftOffset, 13, 17)\
    MACRO_NAME(UIBookStoreAddCommentPageBtnWidth, 73, 93)\
    MACRO_NAME(UIBookStorePayDlgWidth, 300, 384)\
    MACRO_NAME(UIBookStorePayDlgHeight, 160, 205)\
    MACRO_NAME(UIBookStorePayControlSpacing, 15, 19)\
    MACRO_NAME(UIBookStorePayHorizontalMargin, 30, 38)\
    MACRO_NAME(UIBookStorePayBtnHeight, 34, 44)\
    MACRO_NAME(UIBookStorePayBtnWidth, 220, 282)\
    MACRO_NAME(UIBookStorePayInfoDlgWidth, 340, 435)\
    MACRO_NAME(UIBookStorePayInfoDlgHeight, 235, 300)\
    MACRO_NAME(UIBookStorePayInfoTextHeight, 110, 140)\
    MACRO_NAME(UIBookStorePayInfoDlgBtnHeight, 30, 38)\
    MACRO_NAME(UIBookStorePayInfoDlgControlSpacing, 15, 19)\
    MACRO_NAME(UIBookStorePayInfoDlgHorizontalMargin, 20, 26)\
    MACRO_NAME(UIBookStorePayInfoDlgfootNoteMargin, 5, 6)\
    MACRO_NAME(UIBookStoreBookCommentReplyItemHeight, 110, 140)\
    MACRO_NAME(UIBookStoreChartsPageTopSpace, 16, 20)\
    MACRO_NAME(UIBookStoreChartsPageMiddleSpace, 40, 52)\
    MACRO_NAME(UIBookStoreChartsPageItemLeftPadding, 13, 16)\
    MACRO_NAME(UIBookStoreChartsPageItemRightPadding, 28, 36)\
    MACRO_NAME(UIBookStoreChartsPageItemTitleLeftPadding, 14, 17)\
    MACRO_NAME(UIBookStoreChartsPageItemLinePixel, 3, 4)\
    MACRO_NAME(UIBookStoreChartsPageItemWidth, 261, 334)\
    MACRO_NAME(UIBookStoreChartsPageItemHeight, 59, 75)\
    MACRO_NAME(UIButtonTopPadding, 5, 6) \
    MACRO_NAME(UIButtonLeftPadding, 15, 19) \
    MACRO_NAME(UIDirectoryListItemProgressBarWidth, 128, 164)\
    MACRO_NAME(UIDirectoryListItemProgressBarHeight, 2, 2)\
    MACRO_NAME(UIDictionaryDialogTranslateImageHeight, 590, 755)\
    MACRO_NAME(UIBookStoreBookCatalogueItemHeight, 50, 64)\
    MACRO_NAME(UIWeiboSharePageTitleVPadding, 8, 10)\
    MACRO_NAME(UIWeiboSharePageUserNameVPadding, 17, 22)\
    MACRO_NAME(UIWeiboSharePageContentEditVPadding, 11, 14)\
    MACRO_NAME(UIWeiboSharePageContentHeight, 220, 282)\
	MACRO_NAME(UIWeiboSharePagePictureWidth, 165, 211)\
	MACRO_NAME(UIWeiboSharePageContentHorzPadding, 10, 13)\
    MACRO_NAME(UIWeiboBindDlgLogoVPadding, 30, 37)\
    MACRO_NAME(UIWeiboBindDlgEditBoxHeight, 49, 63)\
    MACRO_NAME(UIWeiboBindDlgButtonHeight, 55, 70)\
    MACRO_NAME(UIWeiboBindDlgButtonTopPadding, 14, 18)\
    MACRO_NAME(UIWeiboBindDlgButtonBottomPadding, 22, 34)\
    MACRO_NAME(UICoverViewItemImageTrialWidth, 88, 111)\
    MACRO_NAME(UICoverViewItemImageTrialHeight, 89, 111)\
    MACRO_NAME(UICoverViewItemImageNewWidth, 34, 44)\
    MACRO_NAME(UICoverViewItemImageNewHeight, 11, 14)\
    MACRO_NAME(UICoverViewItemInnerCoverWidth, 162, 206)\
    MACRO_NAME(UICoverViewItemInnerCoverHeight, 216, 274)\
    MACRO_NAME(UICoverViewItemFormatLeft, 70, 89)\
    MACRO_NAME(UICoverViewItemFormatTop, 122, 156)\
    MACRO_NAME(UICoverViewItemFormatWidth, 75, 93)\
    MACRO_NAME(UICoverViewItemFormatHeight, 30, 37)\
    MACRO_NAME(UICoverViewItemSelectLeft, 102, 131)\
    MACRO_NAME(UICoverViewItemSelectTop, 143, 183)\
    MACRO_NAME(UICoverViewItemSelectWidth, 28, 36)\
    MACRO_NAME(UICoverViewItemSelectHeight, 28, 36)\
    MACRO_NAME(UICoverViewItemDirNameBookNumberInterval, 11, 14)\
    MACRO_NAME(UICoverViewItemDirNameBookNumberTotalheight, 96, 123)\
    MACRO_NAME(UICoverViewItemDirNameTop, 31, 40)\
    MACRO_NAME(UICoverViewItemDirNameMaxWidth, 80, 100)\
    MACRO_NAME(UICoverViewItemDirNameMaxHeight, 55, 70)\
    MACRO_NAME(UICoverViewItemBookNameMaxHeight, 78, 100)\
    MACRO_NAME(UICoverViewItemBookNameTop, 23, 30)\
    MACRO_NAME(UICoverViewItemAuthorNameTop, 102, 130)\
    MACRO_NAME(UIBookStoreBookItemTitleMaxWidth, 350, 448) \
    MACRO_NAME(UICommentWndHorizonMargin, 16, 20) \
    MACRO_NAME(UICommentWndEditHeight1, 180, 230) \
    MACRO_NAME(UICommentWndEditHeight2, 133, 170) \
    MACRO_NAME(UICommentWndViewCommentHeight, 49, 63) \
    MACRO_NAME(UICommentWndSaveCommentHeight, 55, 71) \
    MACRO_NAME(UICommentWndBtnTopPadding, 10, 13) \
	MACRO_NAME(UIPersonalUILabelWidth, 135, 173) \
	MACRO_NAME(UIPersonalUILabelHorizontalSpacing, 12, 15) \
	MACRO_NAME(UIPersonalUILabelVerticalSpacing, 12, 15) \
	MACRO_NAME(UIPersonalExperienceElementSpacing, 6, 8) \
	MACRO_NAME(UIPersonalExperienceLineMinHeight, 1, 1) \
    MACRO_NAME(UIPersonalExperienceDistributionHieght, 250, 320) \
	MACRO_NAME(UIPersonalPageUserInfoHeight, 320, 410) \
	MACRO_NAME(UIPersonalPageButtonHeight, 55, 70) \
	MACRO_NAME(UIPersonalPageButtonWidth, 250, 320) \
	MACRO_NAME(UIPersonalPageUserInfoMargin, 29, 29) \
	MACRO_NAME(UIPersonalPageIndexPageItemWidth, 250, 320) \
    MACRO_NAME(UIPersonalPageIndexPageItemHeight, 110, 140) \
    MACRO_NAME(UIBookStorePageTableControllerBottomLinePixel, 2, 2) \
    MACRO_NAME(UISystemSettingPageTableControllerBottomLinePixel, 2, 3) \
    MACRO_NAME(UITableControllerHeight, 58, 74) \
    MACRO_NAME(UITableControllerSplitLineHeight, 42, 54) \
    MACRO_NAME(UIBookStoreNavigationTitleBottomLineHeight, 1, 1)\
    MACRO_NAME(UIBookStoreTablePageListBoxLeftMargin, 25, 32) \
    MACRO_NAME(UIBookStoreTablePageListBoxRightMargin, 26, 34) \
    MACRO_NAME(UIBookStoreTablePageListBoxItemCount, 5, 5) \
    MACRO_NAME(UIBookStoreTablePageListBoxItemHeight, 108, 138) \
    MACRO_NAME(UIBookStoreListBoxPageItemCount, 5, 5)\
    MACRO_NAME(UIBookStoreListBoxPageItemHeight, 118, 151)\
    MACRO_NAME(UIBookStoreBookCommentItemHeight, 163, 208)\
    MACRO_NAME(UIBookStorePublishPageItemCount, 10, 10)\
    MACRO_NAME(UIBookStorePublishPageItemHeight, 58, 74)\
    MACRO_NAME(UIPersonalCommentsPageItemCount, 3, 3)\
    MACRO_NAME(UIPersonalCommentsPageItemHeight, 203, 260)\
    MACRO_NAME(UIPersonalMessagesPageItemCount, 4, 4)\
    MACRO_NAME(UIPersonalMessagesPageItemHeight, 150, 192)\
    MACRO_NAME(UIBookStoreSearchPageItemCount, 6, 6)\
    MACRO_NAME(UIBookStoreSearchPageItemHeight, 103, 131)\
    MACRO_NAME(UIBookStoreTopicPageCoverWidth, 580, 718)\
    MACRO_NAME(UIBookStoreTopicPageCoverHeight, 290, 358)\
    MACRO_NAME(UIBookStoreBookCommentDetailedPageContentHeight, 500, 640)\
    MACRO_NAME(UIAddCategoryDlgHorPadding, 16, 20)\
    MACRO_NAME(UIAddCategoryDlgVerPadding, 16, 20)\
    MACRO_NAME(UIAddCategoryDlgEditHeight, 44, 56)\
    MACRO_NAME(UIAddCategoryDlgBtnGroupHeight, 64, 82)\
    MACRO_NAME(UIAddCategoryDlgBtnHeight, 56, 72)\
    MACRO_NAME(UIAddCategoryDlgBtnHorPadding, 6, 7)\
    MACRO_NAME(UIAddCategoryDlgBtnVerPadding, 6, 7)\
    MACRO_NAME(UITouchMenuItemHeight, 67, 86)\
    MACRO_NAME(UITouchMenuItemWidth, 163, 209)\
    MACRO_NAME(UITouchMenuItemLeftPadding, 12, 15)\
    MACRO_NAME(UITouchMenuButtonLeftMargin, 8, 10)\
    MACRO_NAME(UIAddBookToCategoryPageVertSepHeight, 30, 38)\
    MACRO_NAME(UIAddBookToCategoryPageSaveButtonWidth, 300, 380)\
    MACRO_NAME(UIBookStorePublishingItemHeight, 128, 164)\
    MACRO_NAME(UIBookStorePublishingItemCoverWidth, 184, 235)\
    MACRO_NAME(UIBookStorePublishingItemCoverHeight, 61, 78)\
    MACRO_NAME(UIIndexPageItemLineWidth, 2, 2)\
    MACRO_NAME(UIBackButtonTopMargin, 5, 7)\
    MACRO_NAME(UIBackButtonLeftMargin, 8, 10)\
    MACRO_NAME(UIBackButtonWidth, 76, 97)\
    MACRO_NAME(UIBackButtonHeight, 60, 78)\
    MACRO_NAME(UIInteractiveImagePageBottomBorder, 24, 31)\
    MACRO_NAME(UIInteractiveImagePageImageBottomBorder, 27, 35)\
    MACRO_NAME(UIInteractiveImagePageTopMargin, 13, 17)\
    MACRO_NAME(ICON_FACE_SIZE, 232, 297)\
    MACRO_NAME(BookMarkShortCutAreaSize, 55, 50)\
    MACRO_NAME(UIBookStoreNavigationRefreshRightMargin, 28, 37)\
    MACRO_NAME(UIPersonalCommentsItemContentWidth, 402, 514)\
    MACRO_NAME(UIPersonalCommentsItemContentHeight, 70, 90)\
    MACRO_NAME(UIPersonalCommentsItemCoverWidth, 105, 134)\
    MACRO_NAME(UIPersonalCommentsItemCoverHeight, 139, 178)\
    MACRO_NAME(UIPersonalCommentsItemTopBorder, 17, 21)\
    MACRO_NAME(UIPersonalCommentsItemCoverRightBorder, 27, 34)\
    MACRO_NAME(UIPersonalCommentsItemTitleBottomBorder, 17, 22)\
    MACRO_NAME(UIPersonalCommentsItemElementBorder, 8, 10)\
    MACRO_NAME(UIPersonalCommentsItemContentTopBorder, 22, 28)\
    MACRO_NAME(UIPersonalCommentsItemDeviceTopBorder, 21, 27)\
    MACRO_NAME(UIPersonalCommentsItemBottomElementBorder, 17, 22)\
    MACRO_NAME(UIPersonalNoteSummaryItemBorder, 10, 12)\
	MACRO_NAME(UIPersonalBookNotesPageItemCount, 3, 3)\
	MACRO_NAME(UIPersonalBookNotesPageItemHeight, 156, 200)\
	MACRO_NAME(UIPersonalBookNotesPageBookCoverWidth, 116, 149)\
	MACRO_NAME(UIPersonalBookNotesPageBookCoverHeight, 156, 200)\
	MACRO_NAME(UIPersonalBookNotesPageBtnWidth, 182, 233)\
	MACRO_NAME(UIPersonalBookNotesPageBtnHeight, 38, 49)\
	MACRO_NAME(UIPersonalBookNotesPageBtnTopSpace, 14, 18)\
	MACRO_NAME(UIPersonalBookNotesPageFetchInfoTop, 293, 376)\
	MACRO_NAME(UIPersonalBookNotesPageBookCoverTopSpace, 23, 30)\
	MACRO_NAME(UIPersonalBookNotesPageBookCoverBottomSpace, 22, 28)\
	MACRO_NAME(UIPersonalBookNotesPageBookCoverRightSpace, 17, 22)\
	MACRO_NAME(UIPersonalBookNotesPageBookTitleTopBorder, 4, 5)\
	MACRO_NAME(UIPersonalBookNotesPageBookTitleBottomBorder, 17, 22)\
	MACRO_NAME(UIPersonalBookNotesPageBookInfoItemBorder, 4, 5)\
	MACRO_NAME(UIPersonalBookNotesPagePageInfoTopSpace, 12, 15)\
	MACRO_NAME(UIPersonalBookNoteItemHorizonBorder, 20, 26)\
    MACRO_NAME(UIPersonalNoteSummaryItemHeight, 130, 166)\
    MACRO_NAME(UIBookStoreCartSummaryTextMargin, 30, 40)\
    MACRO_NAME(UIFrontLightHorizonMargin, 0, 18)\
    MACRO_NAME(UIBookStoreBookInfoTagTitleTopMargin, 0, 3)\

#define FONTSIZE_LIST(MACRO_NAME)\
    MACRO_NAME(FontSize10, 10, 14)\
    MACRO_NAME(FontSize12, 12, 16)\
    MACRO_NAME(FontSize14, 14, 18)\
    MACRO_NAME(FontSize16, 16, 20)\
    MACRO_NAME(FontSize18, 18, 23)\
    MACRO_NAME(FontSize20, 20, 26)\
    MACRO_NAME(FontSize22, 22, 28)\
    MACRO_NAME(FontSize24, 24, 30)\
    MACRO_NAME(FontSize25, 25, 32)\
    MACRO_NAME(FontSize26, 26, 34)\
    MACRO_NAME(FontSize28, 28, 36)\
    MACRO_NAME(FontSize30, 30, 38)\
    MACRO_NAME(FontSize32, 32, 40)\
    MACRO_NAME(FontSize36, 36, 46)\
    MACRO_NAME(FontSize42, 42, 52)\
    MACRO_NAME(FontSize60, 60, 76)\
    MACRO_NAME(UIPageNum, 16, 20)\
    MACRO_NAME(UIButtonDlg, 18, 23)\
    MACRO_NAME(UITextBox, 22, 28)\
    MACRO_NAME(UIListItemBaseTitleFontSize, 18, 24)\
    MACRO_NAME(UIListItemBaseMidFontSize, 16, 20)\
    MACRO_NAME(UIListItemBaseSmallFontSize, 14, 18)\
    MACRO_NAME(UIHomePageSearchBox, 20, 26)\
    MACRO_NAME(UIHomePageUpperFolder, 30, 38)\
    MACRO_NAME(UIHomePageSortTypeButton, 18, 24)\
    MACRO_NAME(UIDKBookStoreIndexPageSearchBox, 20, 26)\
    MACRO_NAME(UIBookContentSearchDlg, 20, 26)\
    MACRO_NAME(UIBookContentSearchDlgFileName, 24, 31)\
    MACRO_NAME(UIReaderSettingDlgFileName, 24, 31)\
    MACRO_NAME(UIBookContentSearchListItem, 18, 23)\
    MACRO_NAME(UIBookListItemFileName, 24, 31)\
    MACRO_NAME(UIBookListItemAuthor, 18, 23)\
    MACRO_NAME(UIBookListItemTrial, 14, 18)\
    MACRO_NAME(UIFootNoteDlgText, 20, 26)\
    MACRO_NAME(UITocDlgFileName, 24, 31)\
    MACRO_NAME(UIBasicListItem, 20, 26)\
    MACRO_NAME(UIBookmarkComment, 18, 23)\
    MACRO_NAME(UIBottomBar, 22, 28)\
    MACRO_NAME(UIProgressBar, 16, 20)\
    MACRO_NAME(UIBookStoreIndexPageItemTitle, 26, 33)\
    MACRO_NAME(UIBookStoreIndexPageItemAbstract, 18, 23)\
    MACRO_NAME(UIBookStoreCategoryListItemTitle, 26, 33)\
    MACRO_NAME(UIBookStoreCategoryListItemSpecific, 20, 26)\
    MACRO_NAME(UIBookStoreCategoryListItemBookCount, 18, 23)\
    MACRO_NAME(UIBookStoreRecommendListItemTitle, 22, 28)\
    MACRO_NAME(UIBookStoreRecommendListItemAuthor, 16, 20)\
    MACRO_NAME(UIBookStoreRecommendListPriceBtn, 16, 20)\
    MACRO_NAME(UIBookStoreRecommendScoreCount, 14, 18)\
    MACRO_NAME(UIBookStoreNoSearchResultText, 18, 23)\
    MACRO_NAME(UIBookStoreSearchBox, 20, 26)\
    MACRO_NAME(UIBookStoreLoading, 22, 28)\
    MACRO_NAME(UIBookStoreSortBtn, 18, 23)\
    MACRO_NAME(UIBookStorePageBtn, 18, 23)\
    MACRO_NAME(UIBookStoreCartSummaryText, 24, 31)\
    MACRO_NAME(UIBookStoreCartDiscountText, 16, 20)\
    MACRO_NAME(UIBookStoreCartPurchaseBtn, 22, 28)\
    MACRO_NAME(UIModalDialogTitle, 24, 30)\
    MACRO_NAME(UIModalDialogButton, 20, 26)\
    MACRO_NAME(UIForgetPasswordText, 28, 36)\
    MACRO_NAME(UINavigateToDialog, 20, 26)\
    MACRO_NAME(UISystemUpdateDialogText, 22, 28)\
    MACRO_NAME(UIModifyAccountInfoDlg, 19, 24)\
    MACRO_NAME(UIWifiDialogTitle, 24, 31)\
    MACRO_NAME(UIWifiDialogButton, 18, 23)\
    MACRO_NAME(UIWifiDialogSubTitle, 24, 31)\
    MACRO_NAME(UIWifiDialogListSSID, 24, 31)\
    MACRO_NAME(UIShortCutBarBtn, 18, 23)\
    MACRO_NAME(UIShortCutPagePushUpTips, 22, 28)\
    MACRO_NAME(UIDownloadListItem, 16, 20)\
    MACRO_NAME(UIDownloadListItemNameLabel, 20, 26)\
    MACRO_NAME(UISystemUpdateDialogPageNum, 16, 20)\
    MACRO_NAME(UISoftKeyboardIMEKeyboardFontSize, 20, 25)\
    MACRO_NAME(UISettingPanelSection, 24, 31)\
    MACRO_NAME(UISettingPanelItemTitle, 24, 31)\
    MACRO_NAME(UISettingPanelItemButton, 18, 23)\
    MACRO_NAME(UITablePageBtnTitle, 20, 26)\
    MACRO_NAME(UIBookStoreCommentItemPublishTimeFontSize, 16, 20)\
    MACRO_NAME(UIBookStoreCommentItemDeviceTypeFontSize, 16, 20)\
    MACRO_NAME(UIBookStoreCommentItemContentFontSize, 16, 20)\
    MACRO_NAME(UIBookStoreCommentItemLastLineFontSize, 12, 16)\
    MACRO_NAME(UIBookStoreCommentItemLblTitleFontSize, 20, 26)\
    MACRO_NAME(UIBookStoreCommentItemErnickNameFontSize, 16, 20)\
    MACRO_NAME(UIBookStoreCommentLblNoScore, 16, 20)\
    MACRO_NAME(UIBookStoreCommentLblError, 26, 33)\
    MACRO_NAME(UIBookStoreCommentLoadingInfo, 22, 28)\
    MACRO_NAME(UIBookStoreCommentLblUserScore, 24, 31)\
    MACRO_NAME(UIBookStoreCommentPageNun, 14, 18)\
    MACRO_NAME(UIBookStoreCommentLblStar, 12, 16)\
    MACRO_NAME(UIBookStoreCommentLblScore, 16, 20)\
    MACRO_NAME(UIBookStoreCommentBtn, 16, 20)\
    MACRO_NAME(UIBookStoreCommentLblUserComment, 24, 31)\
	MACRO_NAME(UIBookStorePayInfoHint, 18, 23)\
	MACRO_NAME(UIBookStorePayInfoBtn, 20, 26)\
	MACRO_NAME(UIBookStorePayInfofoolNote, 14, 18)\
    MACRO_NAME(UITabBarFontSize, 22, 28)\
    MACRO_NAME(UITipFont, 24, 31)\
    MACRO_NAME(UIMessageBoxButton, 20, 26)\
	MACRO_NAME(UIDistributionCharDescription, 20, 20)\
	MACRO_NAME(UIBookStoreChartsPageItem, 24, 30)\
	MACRO_NAME(UIPersonalCommentsItemTitle, 19, 24)\
	MACRO_NAME(UIPersonalCommentsItemInfo, 14, 18)\
	MACRO_NAME(UIPersonalCommentsItemNote, 12, 16)\
	MACRO_NAME(UIPersonalCommentsItemContent, 16, 20)\
	MACRO_NAME(UIPersonalBookNotesPageBookTitle, 20, 24)\
	MACRO_NAME(UIPersonalBookNotesPageBookInfo, 16, 20)\
	MACRO_NAME(UIPersonalBookNoteItemContent, 19, 24)\
	MACRO_NAME(UIPersonalBookNoteItemNote, 14, 18)\
	MACRO_NAME(UIPersonalBookNotesPageCoverTitle, 16, 20)\

namespace WindowsMetrics
{
    enum WindowsMetricsIndex
    {
#define MAKE_WINDOWS_METRICS_INDEX(x,y,z) x##Index,    
        METRICS_LIST(MAKE_WINDOWS_METRICS_INDEX)
#undef MAKE_WINDOWS_METRICS_INDEX
        UIMetricsCounterIndex
    };
    enum WindowsFontSizeIndex
    {
#define MAKE_WINDOWS_FONTSIZE_INDEX(x,y,z) x##Index,    
        FONTSIZE_LIST(MAKE_WINDOWS_FONTSIZE_INDEX)
#undef MAKE_WINDOWS_FONTSIZE_INDEX
        UIFontSizeCounterIndex
    };
    enum DeviceIndex
    {
        DI_TOUCH,
        DI_TOUCH_KPW,
        DEVICE_NUMBER,
        DI_DEFAULT = DI_TOUCH
    };

    int GetWindowMetrics(int index);
    int GetWindowFontSize(int index);
    void SetDeviceIndex(DeviceIndex deviceIndex);

};//WindowCoordinate
#endif//WINDOWSMETRICS_H

