////////////////////////////////////////////////////////////////////////
// TPDef.h
// Contact: wzh
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#ifndef TPDEF_H_
#define TPDEF_H_

#ifdef PLATFORM_KINDLE
#include <linux/input.h>
#endif

// Redirect DK's keycode to linux's
#define KEY_LPAGEUP     KEY_PAGEDOWN
#define KEY_LPAGEDOWN   KEY_PAGEUP

#define KEY_SYM         KEY_RIGHTMETA
#define KEY_FONT        KEY_F20
#define KEY_RPAGEUP     KEY_F23
#define KEY_RPAGEDOWN   KEY_F21
#define KEY_OKAY        KEY_F24
#define KEY_DEL         KEY_BACKSPACE

#define ALT_DOWN        0x00010000
#define SHIFT_DOWN      0x00020000
#define KEY_SYM_DOWN_K4NT      0x00040000
#define KEY_PTRSC       (KEY_SPACE | SHIFT_DOWN)
#define KEY_LFASTPAGEUP  (KEY_LPAGEUP | SHIFT_DOWN)
#define KEY_RFASTPAGEUP  (KEY_RPAGEUP | SHIFT_DOWN)
#define KEY_LFASTPAGEDOWN   (KEY_LPAGEDOWN | SHIFT_DOWN)
#define KEY_RFASTPAGEDOWN   (KEY_RPAGEDOWN | SHIFT_DOWN)
#define KEY_PTRSC_K4NT       (KEY_MENU | KEY_SYM_DOWN_K4NT)

#define LONGPRESS  0x00030000
//Define the long press key value
#define KEY_ENTER_LP    (KEY_ENTER | LONGPRESS)
#define KEY_BACK_LP     (KEY_BACK | LONGPRESS)
#define KEY_MENU_LP     (KEY_MENU | LONGPRESS)
#define KEY_HOME_LP     (KEY_HOME | LONGPRESS)
#define KEY_UP_LP       (KEY_UP | LONGPRESS)
#define KEY_DOWN_LP     (KEY_DOWN | LONGPRESS)
#define KEY_LEFT_LP     (KEY_LEFT | LONGPRESS)
#define KEY_RIGHT_LP    (KEY_RIGHT | LONGPRESS)
#define KEY_0_LP        (KEY_0 | LONGPRESS)
#define KEY_1_LP        (KEY_1 | LONGPRESS)
#define KEY_2_LP        (KEY_2 | LONGPRESS)
#define KEY_3_LP        (KEY_3 | LONGPRESS)
#define KEY_4_LP        (KEY_4 | LONGPRESS)
#define KEY_5_LP        (KEY_5 | LONGPRESS)
#define KEY_6_LP        (KEY_6 | LONGPRESS)
#define KEY_7_LP        (KEY_7 | LONGPRESS)
#define KEY_8_LP        (KEY_8 | LONGPRESS)
#define KEY_9_LP        (KEY_9 | LONGPRESS)
#define KEY_A_LP        (KEY_A | LONGPRESS)
#define KEY_B_LP        (KEY_B | LONGPRESS)
#define KEY_C_LP        (KEY_C | LONGPRESS)
#define KEY_D_LP        (KEY_D | LONGPRESS)
#define KEY_E_LP        (KEY_E | LONGPRESS)
#define KEY_F_LP        (KEY_F | LONGPRESS)
#define KEY_G_LP        (KEY_G | LONGPRESS)
#define KEY_H_LP        (KEY_H | LONGPRESS)
#define KEY_I_LP        (KEY_I | LONGPRESS)
#define KEY_J_LP        (KEY_J | LONGPRESS)
#define KEY_K_LP        (KEY_K | LONGPRESS)
#define KEY_L_LP        (KEY_L | LONGPRESS)
#define KEY_M_LP        (KEY_M | LONGPRESS)
#define KEY_N_LP        (KEY_N | LONGPRESS)
#define KEY_O_LP        (KEY_O | LONGPRESS)
#define KEY_P_LP        (KEY_P | LONGPRESS)
#define KEY_Q_LP        (KEY_Q | LONGPRESS)
#define KEY_R_LP        (KEY_R | LONGPRESS)
#define KEY_S_LP        (KEY_S | LONGPRESS)
#define KEY_T_LP        (KEY_T | LONGPRESS)
#define KEY_U_LP        (KEY_U | LONGPRESS)
#define KEY_V_LP        (KEY_V | LONGPRESS)
#define KEY_W_LP        (KEY_W | LONGPRESS)
#define KEY_X_LP        (KEY_X | LONGPRESS)
#define KEY_Y_LP        (KEY_Y | LONGPRESS)
#define KEY_Z_LP        (KEY_Z | LONGPRESS)
#define KEY_LEFTSHIFT_LP    (KEY_LEFTSHIFT | LONGPRESS)
#define KEY_LEFTALT_LP      (KEY_LEFTALT | LONGPRESS)
#define KEY_SPACE_LP        (KEY_SPACE | LONGPRESS)
#define KEY_DOT_LP          (KEY_DOT | LONGPRESS)
#define KEY_SLASH_LP        (KEY_SLASH | LONGPRESS)
#define KEY_VOLUMEUP_LP     (KEY_VOLUMEUP | LONGPRESS)
#define KEY_VOLUMEDOWN_LP   (KEY_VOLUMEDOWN | LONGPRESS)
#define KEY_LPAGEUP_LP      (KEY_LPAGEUP | LONGPRESS)
#define KEY_LPAGEDOWN_LP    (KEY_LPAGEDOWN | LONGPRESS)
#define KEY_SYM_LP          (KEY_SYM | LONGPRESS)
#define KEY_FONT_LP         (KEY_FONT | LONGPRESS)
#define KEY_RPAGEUP_LP      (KEY_RPAGEUP | LONGPRESS)
#define KEY_RPAGEDOWN_LP    (KEY_RPAGEDOWN | LONGPRESS)
#define KEY_OKAY_LP         (KEY_OKAY | LONGPRESS)
#define KEY_DEL_LP          (KEY_DEL | LONGPRESS)



const char  NOTVISIBLEKEY = -1;
char VirtualKeyToChar(int iVirtualKey);
bool IsAlpha(int iVirtualKey);
bool IsDigit(int iVirtualKey);

/* We reuse following keys from Linux:
#define KEY_ENTER   28
#define   KEY_BACK  158
#define KEY_MENU    139
#define KEY_HOME    102
#define KEY_UP  103
#define KEY_DOWN    108
#define KEY_LEFT    105
#define KEY_RIGHT   106
#define KEY_0   11
#define KEY_1   2
#define KEY_2   3
#define KEY_3   4
#define KEY_4   5
#define KEY_5   6
#define KEY_6   7
#define KEY_7   8
#define KEY_8   9
#define KEY_9   10
#define KEY_A   30
#define KEY_B   48
#define KEY_C   46
#define KEY_D   32
#define KEY_E   18
#define KEY_F   33
#define KEY_G   34
#define KEY_H   35
#define KEY_I   23
#define KEY_J   36
#define KEY_K   37
#define KEY_L   38
#define KEY_M   50
#define KEY_N   49
#define KEY_O   24
#define KEY_P   25
#define KEY_Q   16
#define KEY_R   19
#define KEY_S   31
#define KEY_T   20
#define KEY_U   22
#define KEY_V   47
#define KEY_W   17
#define KEY_X   45
#define KEY_Y   21
#define KEY_Z   44
#define KEY_LEFTSHIFT   42
#define KEY_LEFTALT 56
#define KEY_SPACE   57
#define KEY_DOT 52
#define KEY_SLASH   53
#define KEY_VOLUMEUP    115
#define KEY_VOLUMEDOWN  114
*/



enum
{
    ACTION_ACTIVITY = 1,
    ACTION_FRAME,
    ACTION_ELEMENT,
    ACTION_MENUITEM,
};

enum
{
    CONDITION_ALWAYS = 1,
    CONDITION_IF,
    CONDITION_ELSEIF,
    CONDITION_ELSE,
};

enum
{
    CONDITION_EQUAL = 1,
    CONDITION_NOT_EQUAL,
    CONDITION_GREATER_THAN,
    CONDITION_GREATER_THAN_OR_EQUAL,
    CONDITION_LESS_THAN,
    CONDITION_LESS_THAN_OR_EQUAL,
};


enum StrokeStyle
{
    SOLID_STROKE = 1,
    DOTTED_STROKE
};


enum EKeyboardEventType
{
     DK_KEYTYPE_UP,
    DK_KEYTYPE_DOWN,
    DK_KEYTYPE_REPEAT,
};

enum SCREEN_ROTATE
{
    SCREEN_NORMAL = 1,
    SCREEN_LEFT_ROTATE,
    SCREEN_RIGHT_ROTATE
};

enum HorizontalAlignMode
{
    ALIGN_LEFT = 1,
    ALIGN_CENTER,
    ALIGN_RIGHT,
	ALIGN_JUSTIFY
};

enum VerticalAlignMode
{
    VALIGN_TOP = 1,
    VALIGN_CENTER,
    VALIGN_BOTTOM
};

enum LayoutMode
{
    LAYOUT_RIGHT_ALIGN = 0x1,
    LAYOUT_BOTTOM_ALIGN = 0x2,
    LAYOUT_RIGHT_EXTEND = 0x4,
    LAYOUT_BOTTOM_EXTEND = 0x8
};

enum MoveDirection
{
    DIR_LEFT,
    DIR_TOP,
    DIR_RIGHT,
    DIR_DOWN
};


enum ScrollOrientation
{
    HSCROLL = 1,
    VSCROLL
};

enum AudioPlaybackStyle
{
    AUDIO_NOUI = 0,
    AUDIO_STATUSICON = 2,
    AUDIO_PROGRESSBAR = 4,
    AUDIO_TIMETEXT = 8,
    AUDIO_DEFAULT = 14
};

enum AudioPlaybackState
{
    STOPPED = 1,
    PLAYING,
    PAUSED,
    LOADING,
    NOTAVALIABLE
};

enum ListBoxItemStyle
{
    LIST_ITEM_NONE = 1,
    LIST_ITEM_CHECK,
    LIST_ITEM_RADIO
};

int GetTPKeyboardEventType(int eventType);



#endif /*TPDEF_H_*/

