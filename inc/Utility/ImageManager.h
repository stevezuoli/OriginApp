////////////////////////////////////////////////////////////////////////
// ImageManager.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IMAGEMANAGER_H__
#define __IMAGEMANAGER_H__

#include "GUI/ITpImage.h"
#include "Utility/CString.h"
#include "DkSPtr.h"
#include <vector>
#include <string>

//using namespace DkFormat;
using  DkFormat::SPtr;


#define IMAGE_LIST(MACRO_NAME) \
    MACRO_NAME(MAIN_LOGO,                   "main_logo.bmp", "")\
    MACRO_NAME(91SEARCH_LOGO,               "search_logo.bmp", "")\
    MACRO_NAME(BUTTON_HEAD_24,              "key_small.bmp", "")\
    MACRO_NAME(BUTTON_HEAD_36,              "key_big.bmp", "")\
    MACRO_NAME(MENU_BACK,                   "bg_menu.bmp", "")\
    MACRO_NAME(TEXTBOX_BACK,                "bg_textbox.bmp", "")\
    MACRO_NAME(IME_AA,                      "inputbutton_Aa.bmp", "")\
    MACRO_NAME(IME_SYM,                     "inputbutton_sym.bmp", "")\
    MACRO_NAME(IME_UP,                      "arrow_up.bmp", "")\
    MACRO_NAME(IME_DOWN,                    "arrow_down.bmp", "")\
    MACRO_NAME(TITLEBAR_WIFIICON,           "status_wifi.bmp", "")\
    MACRO_NAME(TITLEBAR_BATTERYICON ,       "status_battery.bmp", "")\
    MACRO_NAME(TITLEBAR_DOWNLOADSTATUSICON, "touch/status_download.bmp", "touch/status_download@kp.bmp")\
    MACRO_NAME(KEY_0,                       "key_0.bmp", "")\
    MACRO_NAME(KEY_1,                       "key_1.bmp", "")\
    MACRO_NAME(KEY_2,                       "key_2.bmp", "")\
    MACRO_NAME(KEY_3,                       "key_3.bmp", "")\
    MACRO_NAME(KEY_4,                       "key_4.bmp", "")\
    MACRO_NAME(KEY_5,                       "key_5.bmp", "")\
    MACRO_NAME(KEY_6,                       "key_6.bmp", "")\
    MACRO_NAME(KEY_7,                       "key_7.bmp", "")\
    MACRO_NAME(KEY_8,                       "key_8.bmp", "")\
    MACRO_NAME(KEY_9,                       "key_9.bmp", "")\
    MACRO_NAME(KEY_A,                       "key_A.bmp", "")\
    MACRO_NAME(KEY_B,                       "key_B.bmp", "")\
    MACRO_NAME(KEY_C,                       "key_C.bmp", "")\
    MACRO_NAME(KEY_D,                       "key_D.bmp", "")\
    MACRO_NAME(KEY_E,                       "key_E.bmp", "")\
    MACRO_NAME(KEY_F,                       "key_F.bmp", "")\
    MACRO_NAME(KEY_G,                       "key_G.bmp", "")\
    MACRO_NAME(KEY_H,                       "key_H.bmp", "")\
    MACRO_NAME(KEY_I,                       "key_I.bmp", "")\
    MACRO_NAME(KEY_J,                       "key_J.bmp", "")\
    MACRO_NAME(KEY_K,                       "key_K.bmp", "")\
    MACRO_NAME(KEY_L,                       "key_L.bmp", "")\
    MACRO_NAME(KEY_M,                       "key_M.bmp", "")\
    MACRO_NAME(KEY_N,                       "key_N.bmp", "")\
    MACRO_NAME(KEY_O,                       "key_O.bmp", "")\
    MACRO_NAME(KEY_P,                       "key_P.bmp", "")\
    MACRO_NAME(KEY_Q,                       "key_Q.bmp", "")\
    MACRO_NAME(KEY_R,                       "key_R.bmp", "")\
    MACRO_NAME(KEY_S,                       "key_S.bmp", "")\
    MACRO_NAME(KEY_T,                       "key_T.bmp", "")\
    MACRO_NAME(KEY_U,                       "key_U.bmp", "")\
    MACRO_NAME(KEY_V,                       "key_V.bmp", "")\
    MACRO_NAME(KEY_W,                       "key_W.bmp", "")\
    MACRO_NAME(KEY_X,                       "key_X.bmp", "")\
    MACRO_NAME(KEY_Y,                       "key_Y.bmp", "")\
    MACRO_NAME(KEY_Z,                       "key_Z.bmp", "")\
    MACRO_NAME(SMALLKEY_0,                  "keysmall_0.bmp", "")\
    MACRO_NAME(SMALLKEY_1,                  "keysmall_1.bmp", "")\
    MACRO_NAME(SMALLKEY_2,                  "keysmall_2.bmp", "")\
    MACRO_NAME(SMALLKEY_3,                  "keysmall_3.bmp", "")\
    MACRO_NAME(SMALLKEY_4,                  "keysmall_4.bmp", "")\
    MACRO_NAME(SMALLKEY_5,                  "keysmall_5.bmp", "")\
    MACRO_NAME(SMALLKEY_6,                  "keysmall_6.bmp", "")\
    MACRO_NAME(SMALLKEY_7,                  "keysmall_7.bmp", "")\
    MACRO_NAME(SMALLKEY_8,                  "keysmall_8.bmp", "")\
    MACRO_NAME(SMALLKEY_9,                  "keysmall_9.bmp", "")\
    MACRO_NAME(SMALLKEY_A,                  "keysmall_A.bmp", "")\
    MACRO_NAME(SMALLKEY_B,                  "keysmall_B.bmp", "")\
    MACRO_NAME(SMALLKEY_C,                  "keysmall_C.bmp", "")\
    MACRO_NAME(SMALLKEY_D,                  "keysmall_D.bmp", "")\
    MACRO_NAME(SMALLKEY_E,                  "keysmall_E.bmp", "")\
    MACRO_NAME(SMALLKEY_F,                  "keysmall_F.bmp", "")\
    MACRO_NAME(SMALLKEY_G,                  "keysmall_G.bmp", "")\
    MACRO_NAME(SMALLKEY_H,                  "keysmall_H.bmp", "")\
    MACRO_NAME(SMALLKEY_I,                  "keysmall_I.bmp", "")\
    MACRO_NAME(SMALLKEY_J,                  "keysmall_J.bmp", "")\
    MACRO_NAME(SMALLKEY_K,                  "keysmall_K.bmp", "")\
    MACRO_NAME(SMALLKEY_L,                  "keysmall_L.bmp", "")\
    MACRO_NAME(SMALLKEY_M,                  "keysmall_M.bmp", "")\
    MACRO_NAME(SMALLKEY_N,                  "keysmall_N.bmp", "")\
    MACRO_NAME(SMALLKEY_O,                  "keysmall_O.bmp", "")\
    MACRO_NAME(SMALLKEY_P,                  "keysmall_P.bmp", "")\
    MACRO_NAME(SMALLKEY_Q,                  "keysmall_Q.bmp", "")\
    MACRO_NAME(SMALLKEY_R,                  "keysmall_R.bmp", "")\
    MACRO_NAME(SMALLKEY_S,                  "keysmall_S.bmp", "")\
    MACRO_NAME(SMALLKEY_T,                  "keysmall_T.bmp", "")\
    MACRO_NAME(SMALLKEY_U,                  "keysmall_U.bmp", "")\
    MACRO_NAME(SMALLKEY_V,                  "keysmall_V.bmp", "")\
    MACRO_NAME(SMALLKEY_W,                  "keysmall_W.bmp", "")\
    MACRO_NAME(SMALLKEY_X,                  "keysmall_X.bmp", "")\
    MACRO_NAME(SMALLKEY_Y,                  "keysmall_Y.bmp", "")\
    MACRO_NAME(SMALLKEY_Z,                  "keysmall_Z.bmp", "")\
    MACRO_NAME(SYS_OPTION,                  "setting_option.bmp", "")\
    MACRO_NAME(SYS_OPTION_CHECKED,          "setting_option_selected.bmp", "")\
    MACRO_NAME(SYS_OPTION_UNCHECKED,        "setting_option_normal.bmp", "")\
    MACRO_NAME(SYS_TITLE,                   "title_setting.bmp", "")\
    MACRO_NAME(TESTRESULT_SMILE,            "", "")\
    MACRO_NAME(MENU_SELECTION_BACK,         "", "")\
    MACRO_NAME(MENU_SELECTED,               "menu_selected.bmp", "")\
    MACRO_NAME(MSGBOX_ERR_BACK,             "", "")\
    MACRO_NAME(MSGBOX_INFO_BACK,            "", "")\
    MACRO_NAME(ICONLISTBOX_SELECTED,        "", "")\
    MACRO_NAME(NEW_BOOK,                    "icon_new.bmp", "")\
    MACRO_NAME(BOOKCOVER_DEFAULT,           "bookcover_default.bmp", "")\
    MACRO_NAME(TITLE_BOOKINFO,              "title_bookinfo.bmp", "")\
    MACRO_NAME(ICON_SHUQIAN,                "icon_shuqian.bmp", "")\
    MACRO_NAME(ICON_EDTI,                   "icon_edti.bmp", "")\
    MACRO_NAME(ICON_MARK,                   "icon_mark.bmp", "")\
    MACRO_NAME(TITLE_SHUQIAN,               "title_shuqian.bmp", "")\
    MACRO_NAME(LINE_SELECT,                 "line_01.bmp", "")\
    MACRO_NAME(MSGBOX_ICON_INFO,            "icon_info.bmp", "")\
    MACRO_NAME(MSGBOX_ICON_ERROR,           "icon_wrong.bmp", "")\
    MACRO_NAME(MSGBOX_ICON_ASK,             "icon_question.bmp", "")\
    MACRO_NAME(MSGBOX_ICON_COMPLETE,        "icon_done.bmp", "")\
    MACRO_NAME(MSGBOX_ICON_UPDATE,          "icon_update.bmp", "")\
    MACRO_NAME(MSGBOX_ICON_OTHER,           "icon_other.bmp", "")\
    MACRO_NAME(ICON_DOWNLOAD,               "icon_download.bmp", "")\
    MACRO_NAME(TOP_ARROW,                   "shangjiantou.bmp", "shangjiantou@kp.bmp")\
    MACRO_NAME(BOTTOM_ARROW,                "xiajiantou.bmp", "xiajiantou@kp.bmp")\
    MACRO_NAME(RIGHT_ARROW,                 "youjiantou.bmp", "")\
    MACRO_NAME(PROCTED_AP,                  "icon_lock.bmp", "")\
    MACRO_NAME(ICON_PAUSE,                  "icon_pause.bmp", "")\
    MACRO_NAME(ICON_DOWNLOADCOMPLETE,       "icon_downloadcomplete.bmp", "")\
    MACRO_NAME(ICON_DOWNLOADFAILD,          "icon_download_faild.bmp", "")\
    MACRO_NAME(ICON_WAIT,                   "icon_wait.bmp", "")\
    MACRO_NAME(ARROW_DOWN,                  "arrow_down.bmp", "")\
    MACRO_NAME(LEFT_ARROW,                  "zuojiantou.bmp", "")\
    MACRO_NAME(BOOKMARK,                    "book_mark.bmp", "")\
    MACRO_NAME(SELECTICON,                  "setting_option_selected.bmp", "")\
    MACRO_NAME(TITLE_BOOKCONTENT,           "title_booklist.bmp", "")\
    MACRO_NAME(TEXT_MULU,                   "text_mulu.bmp", "")\
    MACRO_NAME(WAIT_HOURGLASS,              "loding.bmp", "loding@kp.bmp")\
    MACRO_NAME(ICON_TAG,                    "icon_tag.bmp", "")\
    MACRO_NAME(ICON_DEL,                    "icon_delete.bmp", "")\
    MACRO_NAME(IMG_ROTATE_0_ICON,           "img_rotate_0.bmp", "")\
    MACRO_NAME(IMG_ROTATE_90_ICON,          "img_rotate_90.bmp", "")\
    MACRO_NAME(IMG_ROTATE_180_ICON,         "img_rotate_180.bmp", "")\
    MACRO_NAME(IMG_ROTATE_270_ICON,         "img_rotate_270.bmp", "")\
    MACRO_NAME(IMG_CARTOON_ALL_ICON,        "img_cartoon_all.bmp", "")\
    MACRO_NAME(IMG_CARTOON_LR_ICON,         "img_cartoon_lr.bmp", "")\
    MACRO_NAME(IMG_CARTOON_RL_ICON,         "img_cartoon_rl.bmp", "")\
    MACRO_NAME(YOUBIAO_LR_ARROW,            "pdf_05.bmp", "")\
    MACRO_NAME(YOUBIAO_UD_ARROW,            "pdf_06.bmp", "")\
    MACRO_NAME(PDF_ORDER_1,                 "pdf_01.bmp", "")\
    MACRO_NAME(PDF_ORDER_2,                 "pdf_02.bmp", "")\
    MACRO_NAME(PDF_ORDER_3,                 "pdf_03.bmp", "")\
    MACRO_NAME(PDF_ORDER_4,                 "pdf_04.bmp", "")\
    MACRO_NAME(AUDIO_PLAYER_BACK,           "musicbg.bmp", "")\
    MACRO_NAME(AUDIO_PAUSE,                 "music_pause.bmp", "")\
    MACRO_NAME(AUDIO_PLAY,                  "music_play.bmp", "")\
    MACRO_NAME(AUDIO_PLAY_STOP,             "anniu_1.bmp", "")\
    MACRO_NAME(AUDIO_PLAY_NEXT,             "anniu_2.bmp", "")\
    MACRO_NAME(AUDIO_PLAY_PRE,              "anniu_3.bmp", "")\
    MACRO_NAME(AUDIO_VOLUME_ON,             "anniu_4.bmp", "")\
    MACRO_NAME(AUDIO_PLAY_LOOP,             "anniu_5.bmp", "")\
    MACRO_NAME(AUDIO_VOLUME_OFF,            "anniu_6.bmp", "")\
    MACRO_NAME(VOLUME_BACK,                 "yinliangbg.bmp", "")\
    MACRO_NAME(VOLUME_MAX,                  "yinliang.bmp", "")\
    MACRO_NAME(VOLUME_MIN,                  "yinliang_1.bmp", "")\
    MACRO_NAME(FOUR_ARROW,                  "jiantou.bmp", "")\
    MACRO_NAME(PDF_ROTATE_0_ICON,           "pic_AA_01.bmp", "")\
    MACRO_NAME(PDF_ROTATE_90_ICON,          "pic_AA_02.bmp", "")\
    MACRO_NAME(PDF_ROTATE_180_ICON,         "pic_AA_03.bmp", "")\
    MACRO_NAME(PDF_ROTATE_270_ICON,         "pic_AA_04.bmp", "")\
    MACRO_NAME(PDF_SINGLE_PAGE_ICON,        "pic_AA_05.bmp", "")\
    MACRO_NAME(LIANZAI,                     "icon_lianzai.bmp", "")\
    MACRO_NAME(LIANZAI_YOUGENGXIN,          "icon_yougengxin.bmp", "")\
    MACRO_NAME(LIANZAI_YIGENGXIN,           "icon_yigengxin.bmp", "")\
    MACRO_NAME(LIANZAI_ZUIXINZHANGJIE,      "icon_zuixinzhangjie.bmp", "")\
    MACRO_NAME(NAVIGATION_UP,               "navigation_up.bmp", "")\
    MACRO_NAME(NAVIGATION_DOWN,             "navigation_down.bmp", "")\
    MACRO_NAME(NAVIGATION_LEFT,             "navigation_left.bmp", "")\
    MACRO_NAME(NAVIGATION_RIGHT,            "navigation_right.bmp", "")\
    MACRO_NAME(BIGICON_0,                   "bigicon_0.bmp", "")\
    MACRO_NAME(BIGICON_1,                   "bigicon_1.bmp", "")\
    MACRO_NAME(BIGICON_2,                   "bigicon_2.bmp", "")\
    MACRO_NAME(BIGICON_3,                   "bigicon_3.bmp", "")\
    MACRO_NAME(BIGICON_4,                   "bigicon_4.bmp", "")\
    MACRO_NAME(BIGICON_5,                   "bigicon_5.bmp", "")\
    MACRO_NAME(BIGICON_6,                   "bigicon_6.bmp", "")\
    MACRO_NAME(BIGICON_7,                   "bigicon_7.bmp", "")\
    MACRO_NAME(BIGICON_8,                   "bigicon_8.bmp", "")\
    MACRO_NAME(BIGICON_9,                   "bigicon_9.bmp", "")\
    MACRO_NAME(BIGICON_DOT,                 "bigicon_dot.bmp", "")\
    MACRO_NAME(READINGSTAT,                 "yuedutongji.bmp", "")\
    MACRO_NAME(PDF_LOCATE_CURSOR,           "pdf_locate_cursor.bmp", "")\
    MACRO_NAME(WAVE_LINE,                   "wave_line.bmp", "")\
    MACRO_NAME(COMMENT_ICON,                "comment_icon.bmp", "")\
    MACRO_NAME(IME_ICON_ARROW_LEFT,         "ime_icon_arrow_left.bmp", "ime_icon_arrow_left@kp.bmp")\
    MACRO_NAME(IME_ICON_ARROW_RIGHT,        "ime_icon_arrow_right.bmp", "ime_icon_arrow_right@kp.bmp")\
    MACRO_NAME(IME_ICON_ARROW_LEFTP,        "ime_icon_arrow_leftp.bmp", "")\
    MACRO_NAME(IME_ICON_ARROW_RIGHTP,       "ime_icon_arrow_rightp.bmp", "")\
    MACRO_NAME(IME_ICON_R1,                 "ime_icon_r1.bmp", "ime_icon_r1@kp.bmp")\
    MACRO_NAME(IME_ICON_R2,                 "ime_icon_r2.bmp", "ime_icon_r2@kp.bmp")\
    MACRO_NAME(IME_ICON_R3,                 "ime_icon_r3.bmp", "ime_icon_r3@kp.bmp")\
    MACRO_NAME(IME_ICON_R4,                 "ime_icon_r4.bmp", "ime_icon_r4@kp.bmp")\
    MACRO_NAME(IME_ICON_R1S,                "ime_icon_r1s.bmp", "")\
    MACRO_NAME(IME_ICON_R2S,                "ime_icon_r2s.bmp", "")\
    MACRO_NAME(IME_ICON_R3S,                "ime_icon_r3s.bmp", "")\
    MACRO_NAME(IME_ICON_R4S,                "ime_icon_r4s.bmp", "")\
    MACRO_NAME(IME_KEY_SQUARE,              "ime_key_square.bmp", "ime_key_square@kp.bmp")\
    MACRO_NAME(IME_KEY_SQUARE_PRESSED,      "ime_key_square_pressed.bmp", "ime_key_square_pressed@kp.bmp")\
    MACRO_NAME(IME_KEY_OBLONG,              "ime_key_oblong.bmp", "ime_key_oblong@kp.bmp")\
    MACRO_NAME(IME_KEY_OBLONG_PRESSED,      "ime_key_oblong_pressed.bmp", "ime_key_oblong_pressed@kp.bmp")\
    MACRO_NAME(IME_KEY_DEL,                 "ime_key_del.bmp", "ime_key_del@kp.bmp")\
    MACRO_NAME(IME_KEY_DELP,                "ime_key_del_pressed.bmp", "ime_key_del_pressed@kp.bmp")\
    MACRO_NAME(IME_KEY_ENTER,               "ime_key_enter.bmp", "ime_key_enter@kp.bmp")\
    MACRO_NAME(IME_KEY_ENTERP,              "ime_key_enter_pressed.bmp", "ime_key_enter_pressed@kp.bmp")\
    MACRO_NAME(IME_KEY_SHIFT,               "ime_key_shift.bmp", "ime_key_shift@kp.bmp")\
    MACRO_NAME(IME_KEY_SHIFTP,              "ime_key_shift_pressed.bmp", "ime_key_shift_pressed@kp.bmp")\
    MACRO_NAME(IME_KEY_SPACE,               "ime_key_space.bmp", "ime_key_space@kp.bmp")\
    MACRO_NAME(IME_KEY_SPACEP,              "ime_key_space_pressed.bmp", "ime_key_space_pressed@kp.bmp")\
    MACRO_NAME(IME_KEY_LOWER_SHIFT,         "ime_key_A_lower.bmp", "ime_key_A_lower@kp.bmp")\
    MACRO_NAME(IME_KEY_LOWER_SHIFTP,        "ime_key_A_lower_pressed.bmp", "ime_key_A_lower_pressed@kp.bmp")\
    MACRO_NAME(IME_KEY_UPPER_SHIFT,         "ime_key_A_upper.bmp", "ime_key_A_upper@kp.bmp")\
    MACRO_NAME(IME_KEY_UPPER_SHIFTP,        "ime_key_A_upper_pressed.bmp", "ime_key_A_upper_pressed@kp.bmp")\
    MACRO_NAME(IME_KEY_TO_EN,               "ime_key_changeEN.bmp", "ime_key_changeEN@kp.bmp")\
    MACRO_NAME(IME_KEY_TO_ENP,              "ime_key_changeEN_pressed.bmp", "ime_key_changeEN_pressed@kp.bmp")\
    MACRO_NAME(IME_KEY_TO_PIN,              "ime_key_changePIN.bmp", "ime_key_changePIN@kp.bmp")\
    MACRO_NAME(IME_KEY_TO_PINP,             "ime_key_changePIN_pressed.bmp", "ime_key_changePIN_pressed@kp.bmp")\
    MACRO_NAME(ICON_FACE,                   "sharedicon/icon_face.bmp", "")\
    MACRO_NAME(ICON_COMICSFRAME_ON,         "touch/icon_comicsframe_on.bmp", "touch/icon_comicsframe_on@kp.bmp")\
    MACRO_NAME(ICON_COMICSFRAME_OFF,        "touch/icon_comicsframe_off.bmp", "touch/icon_comicsframe_off@kp.bmp")\
    MACRO_NAME(ICON_BTN_UPLEFT_DISABLE,     "sharedicon/button_disable_01.bmp", "sharedicon/button_disable_01@kp.bmp")\
    MACRO_NAME(ICON_BTN_UPRIGHT_DISABLE,    "sharedicon/button_disable_02.bmp", "sharedicon/button_disable_02@kp.bmp")\
    MACRO_NAME(ICON_BTN_RIGHTDOWN_DISABLE,  "sharedicon/button_disable_03.bmp", "sharedicon/button_disable_03@kp.bmp")\
    MACRO_NAME(ICON_BTN_LEFTDOWN_DISABLE,   "sharedicon/button_disable_04.bmp", "sharedicon/button_disable_04@kp.bmp")\
    MACRO_NAME(ICON_BACK_BUTTON,   "icon_back_button.bmp", "icon_back_button@kp.bmp")\
    MACRO_NAME(PASSINTO,					"sharedicon/passinto.bmp", "sharedicon/passinto@kp.bmp")\
    MACRO_NAME(ARROW_STRONG_LEFT,            "sharedicon/arrow_strong_left.bmp", "sharedicon/arrow_strong_left@kp.bmp")\
    MACRO_NAME(ARROW_STRONG_RIGHT,           "sharedicon/arrow_strong_right.bmp", "sharedicon/arrow_strong_right@kp.bmp")\
    MACRO_NAME(ARROW_STRONG_RIGHT_WHITE,     "sharedicon/arrow_strong_right_white.bmp", "sharedicon/arrow_strong_right_white@kp.bmp")\
    MACRO_NAME(ARROW_STRONG_RIGHT_DISABLE,   "sharedicon/arrow_strong_right_disable.bmp", "sharedicon/arrow_strong_right_disable@kp.bmp")\
    MACRO_NAME(BOOKSTORE_DEFAULTPUBLISHCOVER,"sharedicon/defaultpublishcover.bmp", "sharedicon/defaultpublishcover.bmp")\
    MACRO_NAME(BOOKSTORE_DEFAULTBOOKCOVER,	"sharedicon/defaultbookcover.bmp", "sharedicon/defaultbookcover@kp.bmp")\
    MACRO_NAME(BOOKSTORE_DEFAULTADCOVER,	"sharedicon/BookStoreADDefaultCover.bmp", "sharedicon/BookStoreADDefaultCover@kp.bmp")\
    MACRO_NAME(BOOKSTORE_STAR_SOLID,        "sharedicon/star_solid.bmp", "sharedicon/star_solid@kp.bmp")\
    MACRO_NAME(BOOKSTORE_STAR_HOLLOW,       "sharedicon/star_hollow.bmp", "sharedicon/star_hollow@kp.bmp")\
    MACRO_NAME(BOOKSTORE_STARBIG_SOLID,     "sharedicon/starbig_solid.bmp", "sharedicon/starbig_solid@kp.bmp")\
    MACRO_NAME(BOOKSTORE_STARBIG_HOLLOW,    "sharedicon/starbig_hollow.bmp", "sharedicon/starbig_hollow@kp.bmp")\
    MACRO_NAME(BOOKSTORE_CART,              "sharedicon/icon_shopping.bmp", "sharedicon/icon_shopping@kp.bmp")\
    MACRO_NAME(BOOKSTORE_CART_EMPTY,        "sharedicon/icon_shopping_empty.bmp", "sharedicon/icon_shopping_empty@kp.bmp")\
    MACRO_NAME(BOOKSTORE_DETAIL_CART,       "sharedicon/icon_cart.bmp", "sharedicon/icon_cart@kp.bmp")\
    MACRO_NAME(BOOKSTORE_EXCHANGE,          "sharedicon/icon_exchange.bmp", "sharedicon/icon_exchange@kp.bmp")\
    MACRO_NAME(BOOKSTORE_SHARE,             "sharedicon/icon_share.bmp", "sharedicon/icon_share@kp.bmp")\
    MACRO_NAME(DK_LOWPOWER_ALARM,           "sharedicon/lowPowerAlarm.bmp", "")\
    MACRO_NAME(BOOKSTORE_TABLEBAR_ARROW_TOP,"sharedicon/tablebar_arrow_top_bookstore.bmp", "sharedicon/tablebar_arrow_top_bookstore@kp.bmp")\
    MACRO_NAME(SYSTEMSETTING_TABLEBAR_ARROW_TOP, "sharedicon/tablebar_arrow_top.bmp", "sharedicon/tablebar_arrow_top@kp.bmp")\
    MACRO_NAME(ICON_SEARCH,                   "sharedicon/icon_search.bmp", "sharedicon/icon_search@kp.bmp")\
    MACRO_NAME(ICON_SEARCH_DISABLE,           "sharedicon/icon_search_disable.bmp", "sharedicon/icon_search_disable@kp.bmp")\
    MACRO_NAME(ICON_POP,                      "sharedicon/icon_pop.bmp", "sharedicon/icon_pop@kp.bmp")\
    MACRO_NAME(ICON_DOWNLOAD_ALL,             "sharedicon/icon_download_all.bmp", "sharedicon/icon_download_all@kp.bmp")\
    MACRO_NAME(ICON_BUTTON_OPEN,              "touch/2013/button_open.bmp", "touch/2013/button_open@kp.bmp")\
    MACRO_NAME(ICON_GO_BACK,                  "touch/2013/icon_go_back.bmp", "touch/2013/icon_go_back@kp.bmp")\
    MACRO_NAME(ICON_COVER_DIR,           "icon_dir_background.bmp", "icon_dir_background@kp.bmp")\
    MACRO_NAME(ICON_COVER_DUOKAN_DIR,           "icon_duokan_dir_background.bmp", "icon_duokan_dir_background@kp.bmp")\
    MACRO_NAME(ICON_COVER_NEW,           "touch/2013/icon_new.bmp", "")\
    MACRO_NAME(ICON_COVER_TRIAL,           "icon_cover_trial.png", "icon_cover_trial@kp.png")\
    MACRO_NAME(ICON_COVER_BACKGROUND,           "icon_cover_background.bmp", "icon_cover_background@kp.bmp")\
    MACRO_NAME(ICON_COVER_SELECTED,           "touch/2013/icon_cover_selected.bmp", "touch/2013/icon_cover_selected@kp.bmp")\
    MACRO_NAME(ICON_COVER_UNSELECTED,           "touch/2013/icon_cover_unselected.bmp", "touch/2013/icon_cover_unselected@kp.bmp")\
    MACRO_NAME(ICON_COVER_SELECTED_TRANSPARENT,           "icon_cover_selected.png", "")\
    MACRO_NAME(ICON_COVER_UNSELECTED_TRANSPARENT,           "icon_cover_unselected.png", "")\
    MACRO_NAME(ICON_CLOUD, "touch/2013/icon_cloud.bmp", "touch/2013/icon_cloud@kp.bmp")\
    MACRO_NAME(ICON_CLOUD_BIG, "touch/2013/icon_cloud_big.bmp", "touch/2013/icon_cloud_big@kp.bmp")\
    MACRO_NAME(ICON_BOOKLIST_COVER,           "touch/2013/icon_booklist_cover.bmp", "touch/2013/icon_booklist_cover@kp.bmp")\
    MACRO_NAME(ICON_BOOKLIST_LIST,           "touch/2013/icon_booklist_list.bmp", "touch/2013/icon_booklist_list@kp.bmp")\
    MACRO_NAME(TOUCH_ARROW_UP,                "touch/arrow_up.bmp", "touch/arrow_up@kp.bmp")\
    MACRO_NAME(TOUCH_ARROW_DOWN,              "touch/arrow_down.bmp", "touch/arrow_down@kp.bmp")\
    MACRO_NAME(TOUCH_ARROW_LEFT,              "touch/arrow_left.bmp", "touch/arrow_left@kp.bmp")\
    MACRO_NAME(TOUCH_ARROW_LEFT_DISABLED,     "touch/arrow_left_disable.bmp", "touch/arrow_left_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ARROW_RIGHT,             "touch/arrow_right.bmp", "touch/arrow_right@kp.bmp")\
    MACRO_NAME(TOUCH_ARROW_RIGHT_DISABLED,    "touch/arrow_right_disable.bmp", "touch/arrow_right_disable@kp.bmp")\
    MACRO_NAME(TOUCH_FOLDER_BACK,             "touch/folder_back.bmp", "touch/ifolder_back@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BACK,               "touch/icon_back.bmp", "touch/icon_back@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BLACK,              "touch/icon_black.bmp", "touch/icon_black@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BLACK_DISABLE,      "touch/icon_black_disable.bmp", "touch/icon_black_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BLACK_WHITE,        "touch/icon_black_white.bmp", "touch/icon_black_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BLACK_INCRE,        "touch/icon_addblack_normal.bmp", "touch/icon_addblack_normal@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BLACK_INCRE_WHITE,  "touch/icon_addblack_pressed.bmp", "touch/icon_addblack_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BLACK_INCRE_DISABLE,"touch/icon_addblack_disable.bmp", "touch/icon_addblack_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BLACK_DECRE,        "touch/icon_addblack00_normal.bmp", "touch/icon_addblack00_normal@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BLACK_DECRE_WHITE,  "touch/icon_addblack00_pressed.bmp", "touch/icon_addblack00_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BLACK_DECRE_DISABLE,"touch/icon_addblack00_disable.bmp", "touch/icon_addblack00_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BOOKMARK,           "touch/icon_bookmark.bmp", "touch/icon_bookmark@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BOOKMARK_ON,        "touch/icon_bookmark_on.bmp", "touch/icon_bookmark_on@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BOOKMARK_ON_WHITE,  "touch/icon_bookmark_on_white.bmp", "touch/icon_bookmark_on_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BOOKMARK_WHITE,     "touch/icon_bookmark_white.bmp", "touch/icon_bookmark_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_CLOSE,			  "touch/icon_close_white.bmp", "touch/icon_close_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_COLUME,             "touch/icon_column.bmp", "touch/icon_column@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_COLUME_DISABLE,     "touch/icon_column_disable.bmp", "touch/icon_column_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_COLUME_QUIT,        "touch/icon_column_quit.bmp", "touch/icon_column_quit@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_COLUME_QUIT_WHITE,  "touch/icon_column_quit_white.bmp", "touch/icon_column_quit_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_COLUME_WHITE,       "touch/icon_column_white.bmp", "touch/icon_column_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_LAYOUT,             "touch/icon_layout.bmp", "touch/icon_layout@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_LAYOUT_WHITE,       "touch/icon_layout_white.bmp", "touch/icon_layout_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_LAYOUT_DISABLE,     "touch/icon_layout_disable.bmp", "touch/icon_layout_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_FLIP,               "touch/icon_flip.bmp", "touch/icon_flip@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TOC,               "touch/icon_mulu.bmp", "touch/icon_mulu@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TOC_WHITE,         "touch/icon_mulu_white.bmp", "touch/icon_mulu_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_READ,               "touch/icon_read.bmp", "touch/icon_read@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_VOLUME_ADD,         "touch/icon_volume_add.bmp", "")\
    MACRO_NAME(TOUCH_ICON_VOLUME_LESS,        "touch/icon_volume_less.bmp", "")\
    MACRO_NAME(TOUCH_ICON_READ_VOLUME,        "touch/icon_read_volume.bmp", "touch/icon_read_volume@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_READ_CLOSE,         "touch/icon_read_close.bmp", "touch/icon_read_close@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_VOLUME_ADD_DISABLE, "touch/icon_volume_add_disable.bmp", "")\
    MACRO_NAME(TOUCH_ICON_VOLUME_LESS_DISABLE,"touch/icon_volume_less_disable.bmp", "")\
    MACRO_NAME(TOUCH_ICON_READ_VOLUME_FOCUS,  "touch/icon_read_volume_focus.bmp", "")\
    MACRO_NAME(TOUCH_ICON_READ_WHITE,         "touch/icon_read_white.bmp", "touch/icon_read_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_READ_DISABLE,       "touch/icon_read_disable.bmp", "touch/icon_read_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_REFILL,             "touch/icon_refill.bmp", "touch/icon_refill@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_REFILL_WHITE,       "touch/icon_refill_white.bmp", "touch/icon_refill_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_REFILL_DISABLE,     "touch/icon_refill_disable.bmp", "touch/icon_refill_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_REFILL_QUIT,        "touch/icon_refill_quit.bmp", "touch/icon_refill_quit@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_REFILL_QUIT_WHITE,  "touch/icon_refill_quit_white.bmp", "touch/icon_refill_quit_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE,             "touch/icon_rotate.bmp", "touch/icon_rotate@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE01,           "touch/icon_rotate01.bmp", "touch/icon_rotate01@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE_WHITE,       "touch/icon_rotate_white.bmp", "touch/icon_rotate_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE01_WHITE,     "touch/icon_rotate01_white.bmp", "touch/icon_rotate01_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE02,           "touch/icon_rotate02.bmp", "touch/icon_rotate02@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE02_WHITE,     "touch/icon_rotate02_white.bmp", "touch/icon_rotate02_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE03,           "touch/icon_rotate03.bmp", "touch/icon_rotate03@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE03_WHITE,     "touch/icon_rotate03_white.bmp", "touch/icon_rotate03_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE04,           "touch/icon_rotate04.bmp", "touch/icon_rotate04@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_ROTATE04_WHITE,     "touch/icon_rotate04_white.bmp", "touch/icon_rotate04_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_SLID_DOWN,          "touch/icon_slid_down.bmp", "")\
    MACRO_NAME(TOUCH_ICON_SLID_UP,            "touch/icon_slid_up.bmp", "")\
    MACRO_NAME(TOUCH_ICON_SLID_LEFT,          "touch/icon_slid_left.bmp", "")\
    MACRO_NAME(TOUCH_ICON_SLID_RIGHT,         "touch/icon_slid_right.bmp", "")\
    MACRO_NAME(TOUCH_ICON_SEARCH,             "touch/2013/icon_search_on.bmp", "touch/2013/icon_search_on@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_SEARCH_WHITE,       "touch/icon_search_white.bmp", "touch/icon_search_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_SEARCH_DISABLE,       "touch/icon_search_disable.bmp", "touch/icon_search_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_SEARCH_OFF,       "touch/2013/icon_search_off.bmp", "touch/2013/icon_search_off@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_SELECT,			  "touch/icon_selected.bmp", "touch/icon_selected@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_MENU_CHECK,          "touch/icon_menu_check.bmp", "touch/icon_menu_check@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_SETTING,            "touch/icon_setting.bmp", "touch/icon_setting@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_SETTING_WHITE,      "touch/icon_setting_white.bmp", "touch/icon_setting_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_SETTING_DISABLE,    "touch/icon_setting_disable.bmp", "touch/icon_setting_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TEXT,               "touch/icon_text.bmp", "touch/icon_text@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TEXT_WHITE,         "touch/icon_text_white.bmp", "touch/icon_text_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TEXT_DISABLE,       "touch/icon_text_disable.bmp", "touch/icon_text_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_INFOMATION,         "touch/icon_Information.bmp", "touch/icon_Information@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_INFOMATION_WHITE,   "touch/icon_Information_white.bmp", "touch/icon_Information_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_FONTSIZE_DECRE,     "touch/icon_textsize01_normal.bmp", "touch/icon_textsize01_normal@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_FONTSIZE_DECRE_WHITE,"touch/icon_textsize01_pressed.bmp", "touch/icon_textsize01_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_FONTSIZE_DECRE_DISABLE,"touch/icon_textsize01_disable.bmp", "touch/icon_textsize01_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_FONTSIZE_INCRE,     "touch/icon_textsize02_normal.bmp", "touch/icon_textsize02_normal@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_FONTSIZE_INCRE_WHITE,"touch/icon_textsize02_pressed.bmp", "touch/icon_textsize02_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_FONTSIZE_INCRE_DISABLE,"touch/icon_textsize02_disable.bmp", "touch/icon_textsize02_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TRIM,               "touch/icon_trim.bmp", "touch/icon_trim@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TRIM_QUIT,          "touch/icon_trim_quit.bmp", "touch/icon_trim_quit@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TRIM_QUIT_WHITE,    "touch/icon_trim_quit_white.bmp", "touch/icon_trim_quit_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TRIM_WHITE,         "touch/icon_trim_white.bmp", "touch/icon_trim_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TRIM_DISABLE,       "touch/icon_trim_disable.bmp", "touch/icon_trim_disable@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_OPTION_NORMAL,      "touch/option_normal.bmp", "touch/option_normal@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_OPTION_SELECTED,    "touch/option_selected.bmp", "touch/option_selected@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_1,         "touch/pdf_01.bmp", "touch/pdf_01@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_1_PRESSED, "touch/pdf_01_pressed.bmp", "touch/pdf_01_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_2,         "touch/pdf_02.bmp", "touch/pdf_02@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_2_PRESSED, "touch/pdf_02_pressed.bmp", "touch/pdf_02_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_3,         "touch/pdf_03.bmp", "touch/pdf_03@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_3_PRESSED, "touch/pdf_03_pressed.bmp", "touch/pdf_03_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_4,         "touch/pdf_04.bmp", "touch/pdf_04@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_4_PRESSED, "touch/pdf_04_pressed.bmp", "touch/pdf_04_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_6,         "touch/pic_AA_06.bmp", "touch/pic_AA_06@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_6_PRESSED, "touch/pic_AA_06_pressed.bmp", "touch/pic_AA_06_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_7,         "touch/pic_AA_07.bmp", "touch/pic_AA_07@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PDF_ORDER_7_PRESSED, "touch/pic_AA_07_pressed.bmp", "touch/pic_AA_07_pressed@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_STATUS_BATTERY,     "touch/status_battery.bmp", "touch/status_battery@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_STATUS_BATTERY1,    "touch/status_battery1.bmp", "touch/status_battery1@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_STATUS_DUOKAN,      "touch/status_duokan.bmp", "touch/status_duokan@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_STATUS_WIFI,        "touch/status_wifi.bmp", "touch/status_wifi@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_WIFI_LEVEL,         "touch/wifi_level.bmp", "touch/wifi_level@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TYPE_EPUB,          "touch/type_epub.bmp", "touch/type_epub@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TYPE_FOLDER,        "touch/type_folder.bmp", "touch/type_folder@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TYPE_MOBI,          "touch/type_mobi.bmp", "touch/type_mobi@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TYPE_PDF,           "touch/type_pdf.bmp", "touch/type_pdf@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TYPE_PIC,           "touch/type_pic.bmp", "touch/type_pic@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TYPE_TXT,           "touch/type_txt.bmp", "touch/type_txt@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TYPE_ZIP,           "touch/type_zip.bmp", "touch/type_zip@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_TYPE_DUOKAN,        "touch/type_duokan.bmp", "touch/type_duokan@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BTN_UPLEFT,            "touch/button_normal_01.bmp", "touch/button_normal_01@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BTN_UPLEFT_FOCUS,      "touch/button_black_01.bmp", "touch/button_black_01@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BTN_UPRIGHT,           "touch/button_normal_02.bmp", "touch/button_normal_02@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BTN_UPRIGHT_FOCUS,     "touch/button_black_02.bmp", "touch/button_black_02@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BTN_RIGHTDOWN,         "touch/button_normal_03.bmp", "touch/button_normal_03@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BTN_RIGHTDOWN_FOCUS,   "touch/button_black_03.bmp", "touch/button_black_03@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BTN_LEFTDOWN,          "touch/button_normal_04.bmp", "touch/button_normal_04@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BTN_LEFTDOWN_FOCUS,    "touch/button_black_04.bmp", "touch/button_black_04@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_BTN_CLOSE,			 "touch/button_close.bmp", "touch/button_close@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_DOWNLOAD,				 "touch/download.bmp", "touch/download@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_LOGIN,				 "touch/login.bmp", "touch/login@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PAUSE,				 "touch/pause.bmp", "touch/pause@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_SUBSCRIBE,			 "touch/subscribe.bmp", "")\
    MACRO_NAME(TOUCH_ICON_SUBSCRIBE_DISABLE,	 "touch/subscribe_disable.bmp", "")\
    MACRO_NAME(TOUCH_NEW_BOOK,                   "touch/new_black.bmp", "touch/new_black@kp.bmp")\
    MACRO_NAME(TOUCH_ARROW_BLACK,                "touch/arrow_black.bmp", "touch/arrow_black@kp.bmp")\
    MACRO_NAME(TOUCH_ARROW_WHITE,                "touch/arrow_white.bmp", "touch/arrow_white@kp.bmp")\
    MACRO_NAME(TOUCH_SYS_TITLE,                  "touch/text_syssetting.bmp", "touch/text_syssetting@kp.bmp")\
    MACRO_NAME(TOUCH_BTN_BACK_NORMAL,            "touch/button_back_normal.bmp", "touch/button_back_normal@kp.bmp")\
    MACRO_NAME(TOUCH_BTN_BACK_PRESSED,           "touch/button_back_black.bmp", "touch/button_back_black@kp.bmp")\
    MACRO_NAME(TOUCH_BTN_CLOSE,                  "touch/button_close.bmp", "touch/button_close@kp.bmp")\
    MACRO_NAME(TOUCH_MAINICON_BOOKSHELF1,        "touch/mainicon_bookshefl.bmp", "touch/mainicon_bookshefl@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_STATUSBAR00,           "touch/icon_statusbar00.bmp", "touch/icon_statusbar00@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_STATUSBAR00_WHITE,     "touch/icon_statusbar00_white.bmp", "touch/icon_statusbar00_white@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_STATUSBAR01,           "touch/icon_statusbar01.bmp", "touch/icon_statusbar01@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_STATUSBAR01_WHITE,     "touch/icon_statusbar01_white.bmp", "touch/icon_statusbar01_white@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_BG_01,                 "touch/menu_bg_01.bmp", "touch/menu_bg_01@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_BG_02,                 "touch/menu_bg_02.bmp", "touch/menu_bg_02@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_BG_03,                 "touch/menu_bg_03.bmp", "touch/menu_bg_03@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_BG_04,                 "touch/menu_bg_04.bmp", "touch/menu_bg_04@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_BG_01_BLACK,           "touch/menu_bg_01_black.bmp", "touch/menu_bg_01_black@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_BG_02_BLACK,           "touch/menu_bg_02_black.bmp", "touch/menu_bg_02_black@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_BG_03_BLACK,           "touch/menu_bg_03_black.bmp", "touch/menu_bg_03_black@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_BG_04_BLACK,           "touch/menu_bg_04_black.bmp", "touch/menu_bg_04_black@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_GRAYBG_01,             "touch/menu_graybg_01.bmp", "touch/menu_graybg_01@kp.bmp")\
    MACRO_NAME(TOUCH_MENU_GRAYBG_02,             "touch/menu_graybg_02.bmp", "touch/menu_graybg_02@kp.bmp")\
    MACRO_NAME(TOUCH_SWITCH_OFF,                 "touch/switch_off.bmp", "touch/switch_off@kp.bmp")\
    MACRO_NAME(TOUCH_SWITCH_ON,                  "touch/switch_on.bmp", "touch/switch_on@kp.bmp")\
    MACRO_NAME(TOUCH_UP_ARROW,                   "touch/closebar.bmp", "touch/closebar@kp.bmp")\
    MACRO_NAME(TOUCH_BACK,						 "touch/fanhui.bmp", "")\
    MACRO_NAME(TOUCH_BACK_PRESSED,				 "touch/fanhui_pass.bmp", "")\
    MACRO_NAME(TOUCH_BOOKSHELF,					 "touch/home.bmp", "toucn/home@kp.bmp")\
    MACRO_NAME(TOUCH_BOOKSHELF_PRESSED,			 "touch/home.bmp", "touch/home@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_NEXTPAGE,				 "touch/icon_nextpage.bmp", "")\
    MACRO_NAME(TOUCH_ICON_PREPAGE,				 "touch/icon_prepage.bmp", "")\
    MACRO_NAME(TOUCH_ICON_STAR,					 "touch/icon_star.bmp", "")\
    MACRO_NAME(TOUCH_ICON_STAR_LEVEL,			 "touch/icon_star_level.bmp", "")\
    MACRO_NAME(TOUCH_ICON_COMMENT,               "sharedicon/icon_comments.bmp", "sharedicon/icon_comments@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_DIGEST,                "touch/icon_mark.bmp", "")\
    MACRO_NAME(TOUCH_ICON_BOOKMARK_SMALL,        "touch/icon_shuqian.bmp", "")\
    MACRO_NAME(TOUCH_ICON_FAMILIAR_TO_TRADITIONAL,        "touch/icon_change.bmp", "touch/icon_change@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_PUSH_MESSAGE,          "touch/icon_message.bmp", "touch/icon_message@kp.bmp")\
    MACRO_NAME(TOUCH_ICON_WEB_BROWSER,           "touch/icon-browser.bmp", "touch/icon-browser@kp.bmp")\
    MACRO_NAME(KPW_ICON_FRONTLIGHT,              "touch/icon_frontlight@kp.bmp", "")\
    MACRO_NAME(KPW_ICON_FRONTLIGHT_BIG,          "touch/icon_frontlight_big@kp.bmp", "")\
    MACRO_NAME(KPW_ICON_FRONTLIGHT_BAR_BLACK,    "touch/icon_frontlight_bar_black@kp.bmp", "")\
    MACRO_NAME(KPW_ICON_FRONTLIGHT_BAR_WHITE,    "touch/icon_frontlight_bar_white@kp.bmp", "")\
    MACRO_NAME(KPW_ICON_FRONTLIGHT_ADD,          "touch/icon_frontlight_add@kp.bmp", "")\
    MACRO_NAME(KPW_ICON_FRONTLIGHT_SUB,          "touch/icon_frontlight_sub@kp.bmp", "")\
    MACRO_NAME(TOUCH_HOME,        "touch/home.bmp", "touch/home@kp.bmp")\
    MACRO_NAME(TOUCH_STORE,        "touch/store.bmp", "touch/store@kp.bmp")\
    MACRO_NAME(TOUCH_ACCOUNT,        "touch/account.bmp", "touch/account@kp.bmp")\
    MACRO_NAME(TOUCH_SETTING,        "touch/setting.bmp", "touch/setting@kp.bmp")\
    MACRO_NAME(TOUCH_FAVOUR,        "touch/icon_favour.bmp", "touch/icon_favour@kp.bmp")\
    MACRO_NAME(TOUCH_FAVOURED,      "touch/icon_favoured.bmp", "touch/icon_favoured@kp.bmp")\
    MACRO_NAME(TOUCH_DUOKAN_LOGO,     "touch/2013/icon_weibo_duokan.bmp", "touch/2013/icon_weibo_duokan@kp.bmp")\
    MACRO_NAME(TOUCH_SINA_LOGO,     "touch/2013/icon_weibo_sina.bmp", "touch/2013/icon_weibo_sina@kp.bmp")\
    MACRO_NAME(TOUCH_WEIBO_GO,     "touch/2013/icon_weibo_go.bmp", "touch/2013/icon_weibo_go@kp.bmp")\
    MACRO_NAME(TOUCH_WEIBO_SHARE,     "touch/2013/button_weibo.bmp", "touch/2013/button_weibo@kp.bmp")\
	MACRO_NAME(ICON_CLOCK,     "icon-clock.png", "")\
	MACRO_NAME(ICON_BLOCK,     "icon-block.png", "")\
	MACRO_NAME(ICON_COVER_FORMAT_DUOKAN,     "icon_cover_format_duokan.png", "icon_cover_format_duokan@kp.png")\
	MACRO_NAME(ICON_COVER_FORMAT_EPUB,     "icon_cover_format_epub.png", "icon_cover_format_epub@kp.png")\
	MACRO_NAME(ICON_COVER_FORMAT_TXT,     "icon_cover_format_txt.png", "icon_cover_format_txt@kp.png")\
	MACRO_NAME(ICON_COVER_FORMAT_PDF,     "icon_cover_format_pdf.png", "icon_cover_format_pdf@kp.png")\
	MACRO_NAME(ICON_COVER_FORMAT_MOBI,     "icon_cover_format_mobi.png", "icon_cover_format_mobi@kp.png")\
	MACRO_NAME(ICON_COVER_FORMAT_ZIP,     "icon_cover_format_zip.png", "icon_cover_format_zip@kp.png")\
	MACRO_NAME(ICON_COVER_FORMAT_RAR,     "icon_cover_format_rar.png", "icon_cover_format_rar@kp.png")\
	MACRO_NAME(ICON_COVER_FORMAT_LOADING,     "icon_cover_format_loading.png", "icon_cover_format_loading@kp.png")\
    MACRO_NAME(ICON_TOUCH_MENU_ARROW, "touch/icon_menu_down_arrow.bmp", "touch/icon_menu_down_arrow@kp.bmp")\
    MACRO_NAME(ICON_BUTTON_BG, "sharedicon/icon_button.bmp", "sharedicon/icon_button@kp.bmp")\
    MACRO_NAME(ICON_TRANSMIT, "sharedicon/icon_transmit.bmp", "sharedicon/icon_transmit@kp.bmp")\
    MACRO_NAME(ICON_ZOOMOUT, "sharedicon/icon_zoomout.bmp", "sharedicon/icon_zoomout@kp.bmp")\
    MACRO_NAME(ICON_FOLDER_EXPAND_PLUS, "icon_folder_expand_plus.bmp", "icon_folder_expand_plus@kp.bmp")\
    MACRO_NAME(ICON_FOLDER_EXPAND_MINUS, "icon_folder_expand_minus.bmp", "icon_folder_expand_minus@kp.bmp")\
    MACRO_NAME(ICON_REFRESH, "sharedicon/icon_refresh.bmp", "sharedicon/icon_refresh@kp.bmp")\
    MACRO_NAME(ICON_CLOSE, "sharedicon/icon_closepic.bmp", "sharedicon/icon_closepic@kp.bmp")\
    MACRO_NAME(ICON_I_COMMENT, "sharedicon/icon_i_comment.bmp", "sharedicon/icon_i_comment@kp.bmp")\
    MACRO_NAME(ICON_I_TRIANGLE, "sharedicon/icon_i_triangle.bmp", "sharedicon/icon_i_triangle@kp.bmp")\
	MACRO_NAME(ICON_GO_LEFT, "touch/button-go-left.bmp", "touch/button-go-left@kp.bmp")\
	MACRO_NAME(ICON_GO_RIGHT, "touch/button-go-right.bmp", "touch/button-go-right@kp.bmp")\
	MACRO_NAME(ICON_GO_TO_LEFT, "touch/button-go-to-left.bmp", "touch/button-go-to-left@kp.bmp")\
	MACRO_NAME(ICON_GO_TO_RIGHT, "touch/button-go-to-right.bmp", "touch/button-go-to-right@kp.bmp")\
	MACRO_NAME(ICON_GO_TO_LEFT_GREY, "touch/button-go-to-left-grey.bmp", "touch/button-go-to-left-grey@kp.bmp")\
	MACRO_NAME(ICON_NUMBERINPUTIME_BTN, "touch/button-password.bmp", "touch/button-password@kp.bmp")\
	MACRO_NAME(ICON_NUMBERINPUTIME_BTN_PRESS, "touch/button-password-press.bmp", "touch/button-password-press@kp.bmp")\
	MACRO_NAME(ICON_NUMBERINPUTIME_BTN_DELETE, "touch/button-password-delete.bmp", "touch/button-password-delete@kp.bmp")\
	MACRO_NAME(ICON_NUMBERINPUTIME_BTN_DELETE_PRESS, "touch/button-password-press-delete.bmp", "touch/button-password-press-delete@kp.bmp")\




enum
{
#define IMAGE_LIST_INDEX_ENUM(CommandId, ImageNameOther, ImageNameKP) \
            IMAGE_##CommandId,
    IMAGE_LIST(IMAGE_LIST_INDEX_ENUM)
#undef IMAGE_LIST_INDEX_ENUM
    IMAGE_COUNT
};
/*
enum
{
    IMAGE_MAIN_LOGO,
    ......
    IMAGE_MSGBOX_ICON_UPDA,
    ......
    IMAGE_COUNT
}
*/

class ImageManager
{
public:
    static SPtr<ITpImage> GetImage(INT32 iImgId);
    static SPtr<ITpImage> GetStretchedImage(INT32 iImgId, INT32 iWidth, INT32 iHeight);
    static std::string GetImagePath(INT32 iImgId);

    static SPtr<ITpImage> GetImage(const std::string& imageName);

    static void FreeImages();

private:
    static SPtr<ITpImage> GetCachedImage(const std::string& imageName);

    static SPtr<ITpImage> GetCachedStretchedImage(INT32 iImgId, INT32 iWidth, INT32 iHeight);
    static void FreeCachedImages();
    static SPtr<ITpImage> FindCachedImage(const std::string& rstrImageId);
    static void AddCachedImage(const std::string& rstrImageId, SPtr<ITpImage> spImage);

private:
    ImageManager();
    static std::vector<std::string> s_imageIds;
    static std::vector<SPtr<ITpImage> > s_images;
    ////////////////////Method Section/////////////////////////
};
#endif
