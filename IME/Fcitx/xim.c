/***************************************************************************
 *   Copyright (C) 2002~2005 by Yuking                                     *
 *   yuking_net@sohu.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stdafx.h"

//szj #include <X11/Xlib.h>
//#include "./X11/Xlib.h"

//szj #include <iconv.h>

#include <string.h>

#include "xim.h"
//#include "IC.h"
#include "tools.h"
#include "MainWindow.h"
#include "InputWindow.h"
#ifdef _ENABLE_TRAY
#include "TrayWindow.h"
#endif
#include "vk.h"
#include "ui.h"

CONNECT_ID     *connectIDsHead = (CONNECT_ID *) NULL;
ICID           *icidsHead = (ICID *) NULL;
//XIMS            ims;

//************************************************
CARD16          connect_id = 0;
CARD16          lastConnectID = 0;
//************************************************

//IC             *CurrentIC = NULL;
char            strLocale[201] = "zh_CN.GB18030,zh_CN.GB2312,zh_CN,zh_CN.GBK,zh_CN.UTF-8,zh_CN.UTF8,en_US.UTF-8,en_US.UTF8";

int        iClientCursorX = INPUTWND_STARTX;
int        iClientCursorY = INPUTWND_STARTY;

extern IM      *im;
extern INT8     iIMIndex;

extern Display *dpy;
extern int      iScreen;
extern Window   mainWindow;
extern Window   inputWindow;
extern Window   VKWindow;

extern int      iMainWindowX;
extern int      iMainWindowY;
extern int      iInputWindowX;
extern int      iInputWindowY;
extern unsigned long     iInputWindowHeight;
extern unsigned long     iInputWindowWidth;
extern Bool     bTrackCursor;
extern Bool     bCenterInputWindow;
extern Bool     bIsUtf8;
extern int      iCodeInputCount;
//szj extern iconv_t  convUTF8;
extern unsigned long     uMessageDown;
extern unsigned long     uMessageUp;
extern Bool     bVK;
extern Bool     bCorner;
extern HIDE_MAINWINDOW hideMainWindow;

//计算打字速度
extern Bool     bStartRecordType;
extern unsigned long     iHZInputed;

extern Bool     bShowInputWindowTriggering;

extern Bool     bUseGBKT;

#ifdef _DEBUG

extern char            strUserLocale[];
char            strXModifiers[100];
#endif

#if 0
static XIMStyle Styles[] = {
    XIMPreeditPosition | XIMStatusArea,        //OverTheSpot
    XIMPreeditPosition | XIMStatusNothing,        //OverTheSpot
    XIMPreeditPosition | XIMStatusNone,        //OverTheSpot
    XIMPreeditNothing | XIMStatusNothing,        //Root
    XIMPreeditNothing | XIMStatusNone,        //Root
/*    XIMPreeditCallbacks | XIMStatusCallbacks,    //OnTheSpot
    XIMPreeditCallbacks | XIMStatusArea,        //OnTheSpot
    XIMPreeditCallbacks | XIMStatusNothing,        //OnTheSpot
    XIMPreeditArea | XIMStatusArea,            //OffTheSpot
    XIMPreeditArea | XIMStatusNothing,        //OffTheSpot
    XIMPreeditArea | XIMStatusNone,            //OffTheSpot */
    0
};

#endif
XIMTriggerKey  *Trigger_Keys = (XIMTriggerKey *) NULL;
INT8            iTriggerKeyCount;

/* Supported Chinese Encodings */


//void SendHZtoClient (IMForwardEventStruct * call_data, char *strHZ)
void SendHZtoClient (void * call_data, char *strHZ)
{
#if 0

    XTextProperty   tp;
    IMCommitStruct  cms;
    char            strOutput[300];
    char           *ps;
    char           *pS2T = (char *) NULL;

#ifdef _DEBUG
    fprintf (stderr, "Sending %s  icid=%d connectid=%d\n", strHZ, CurrentIC->id, connect_id);
#endif

    if (bUseGBKT)
    pS2T = strHZ = ConvertGBKSimple2Tradition (strHZ);

    if (bIsUtf8) {
    size_t          l1, l2;

    ps = strOutput;
    l1 = strlen (strHZ);
    l2 = 299;
    l1 = iconv (convUTF8, (ICONV_CONST char **) (&strHZ), &l1, &ps, &l2);
    *ps = '\0';
    ps = strOutput;
    }
    else
    ps = strHZ;

    XmbTextListToTextProperty (dpy, (char **) &ps, 1, XCompoundTextStyle, &tp);

    memset (&cms, 0, sizeof (cms));
    cms.major_code = XIM_COMMIT;
    cms.icid = call_data->icid;
    cms.connect_id = call_data->connect_id;
    cms.flag = XimLookupChars;
    cms.commit_string = (char *) tp.value;
    IMCommitString (ims, (XPointer) & cms);

    XFree (tp.value);

    if (bUseGBKT)
    free (pS2T);

#endif

}

