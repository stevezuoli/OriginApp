
#ifdef __cplusplus
extern "C"
{
#endif
#include "./Fcitx/ime.h"
#include "./Fcitx/InputWindow.h"
#include "./Fcitx/PYmain.h"

#ifdef __cplusplus
}
#endif

#include "stdlib.h"
#include "string.h"
#include "Utility.h"
#include "string"
#include "interface.h"
#include "IME/IMEFcitx.h"
#include "Utility/EncodeUtil.h"

using namespace dk::utility;

using namespace std;
typedef enum _tag_ImeState
{
    IME_STATE_NOSTATE           = 0,
    IME_STATE_PINYIN            = 1,
    IME_STATE_PINYIN_NO_OPTION  = 2,
    IME_STATE_LEGEND            = 3,
    IME_STATE_UNKNOWN           = 4
}ImeState;

extern char g_szAppPath[];

#define IME_DATA_PATH "res/py/"

extern char respath[];
extern int iCursorPos;
extern char strStringGet[];
extern unsigned long            uMessageDown ;
extern MESSAGE                  messageDown[32];
extern unsigned long            uMessageUp;
extern MESSAGE                  messageUp[32];
extern int             iMaxCandWord;//一次查找出来的最多字符组数
extern int             iCandPageCount;
extern int             iCurrentCandPage;
extern int             iCandWordCount;
extern INT8            iIMIndex;
extern int             bUseGBKT;
SINGLETON_CPP(IMEFcitx)

IMEFcitx::IMEFcitx()
{
}

IMEFcitx::~IMEFcitx()
{
}

void IMEFcitx::SetPinyin()
{
    bUseGBKT = false;
    if(0 != iIMIndex)
    {
        iIMIndex = 0;
        SwitchIM(0);
        ResetInput ();
    }
}
void IMEFcitx::SetWubi()
{
    bUseGBKT = false;
    if(1 != iIMIndex)
    {
        iIMIndex = 1;
        SwitchIM(1);
        ResetInput ();
    }
}
void IMEFcitx::SetPinyinFanti()
{
    bUseGBKT = true;
    if(0 != iIMIndex)
    {
        iIMIndex = 0;
        SwitchIM(0);
        ResetInput ();
    }
}
void IMEFcitx::SetCangjie()
{
    bUseGBKT = false;
    if(2 != iIMIndex)
    {
        iIMIndex = 2;
        SwitchIM(2);
        ResetInput ();
    }
}

eProcessResult IMEFcitx::ProcessKeys(char iASCIICode)
{
    iCurrentCandPage = 0;
    if(
        (iASCIICode >= '0' && iASCIICode <= '9') 
        || (iASCIICode >= 'a' && iASCIICode <= 'z')
        || (iASCIICode >= 'A' && iASCIICode <= 'Z')
        )
    {
        if(iASCIICode >= 'A' && iASCIICode <= 'Z')
        {
            if(0 == uMessageUp)
            {
                uMessageDown = 1;
                messageDown[0].strMsg[0] = iASCIICode;
                messageDown[0].strMsg[1] = 0;
                messageDown[0].type = MSG_FIRSTCAND;
                return PROCESSED_WITH_RESULT;
            }
            else
            {
                iASCIICode = iASCIICode - 'A' + 'a';
            }
        }
        ProcessKey(iASCIICode,0,1);
        DebugPrintf(DLC_CX,"iMaxCandWord:%d iCandPageCount:%d iCurrentCandPage:%d iCandWordCount:%d",iMaxCandWord,iCandPageCount,iCurrentCandPage,iCandWordCount);
        if(strStringGet[0] == 0)
        {
            return PROCESSED_NO_RESULT;
        }
        else
        {
            for(unsigned int i= 0;i< uMessageDown;i++)
            {
                memset(messageDown[i].strMsg, 0, sizeof(messageDown[i].strMsg));
            }
            messageDown[0].type = MSG_FIRSTCAND;
            strcpy(messageDown[0].strMsg,strStringGet);
            strStringGet[0] = 0;
            iCurrentCandPage = 0;
            uMessageDown = 1;
            return PROCESSED_WITH_RESULT;
        }
    }
    else if(iASCIICode == 8)
    {
        if(iCursorPos !=0)
        {
            ProcessKey(iASCIICode,0,1);
            return PROCESSED_NO_RESULT;
        }
        else
        {
            return CAN_NOT_PROCESS;
        }
    }
    return CAN_NOT_PROCESS;
}

std::string IMEFcitx::GetInputString()
{
    string str;
    for(unsigned int i = 0 ;i<uMessageUp;i++)
    {
        str.append(messageUp[i].strMsg);
        if(str[str.size() - 1] == ' ')
        {
            str.resize(str.size()-1);
        }
    }
    return EncodeUtil::GBKToUTF8String(str);
}

int IMEFcitx::ChangePageAndGetCurIndex(int Index)
{
    int lastpage = iCurrentCandPage;
    int curStartIndex = iCurrentCandPage * iMaxCandWord;
    int curEndIndex = curStartIndex + iMaxCandWord - 1;
    DebugPrintf(DLC_CX,"GetStringByIndex  iCurrentCandPage:%d iMaxCandWord: %d iCandPageCount %d  iCandWordCount %d",iCurrentCandPage,iMaxCandWord,iCandPageCount,iCandWordCount);
    while(Index > curEndIndex)
    {
        if(iCurrentCandPage >= iCandPageCount)
        {
            return -1;
        }
        ProcessKey('=',0,1);
        if(iCurrentCandPage == lastpage)
        {
            return -1;
        }
        lastpage = iCurrentCandPage;
        curStartIndex = iCurrentCandPage * iMaxCandWord;
        curEndIndex = curStartIndex + iMaxCandWord - 1;     
    }
    while(Index < curStartIndex)
    {
        if(iCurrentCandPage == 0)
        {
            return -1;
        }
        ProcessKey('-',0,1);
        lastpage = iCurrentCandPage;
        curStartIndex = iCurrentCandPage * iMaxCandWord;
    }
    Index = Index - curStartIndex;
    return Index;
}

eProcessResult IMEFcitx::SelectIndex(int iIndex)
{
    iIndex = ChangePageAndGetCurIndex( iIndex);
    if(iIndex == -1)
    {
        return CAN_NOT_PROCESS;
    }
    char ch=0;
    if(iIndex < 9)
    {
        ch = '1'+iIndex;
    }
    else if(iIndex == 9)
    {
        ch = '0';
    }
    return this->ProcessKeys(ch);
}

std::string IMEFcitx::GetStringByIndex(unsigned int iIndex)
{
    int count =0;
    int Index = this->ChangePageAndGetCurIndex(iIndex);
    if(Index == -1)
    {
        return "";
    }
    for(unsigned int i=0; i<uMessageDown; i++)
    {
        if(messageDown[i].type == MSG_FIRSTCAND || messageDown[i].type == MSG_OTHER || messageDown[i].type == MSG_TIPS)
        {
            if(count == Index)
            {
                return EncodeUtil::GBKToUTF8String(messageDown[i].strMsg);
            }
            else if(count <Index)
            {
                count ++;
            }
            else
            {
                return "";
            }
        }
    }
    return "";
}

unsigned int IMEFcitx::SetInputIndex(unsigned int CurIndex)
{
    return 0;
}
unsigned int IMEFcitx::GetInputIndex()
{
    return 0;
}
void IMEFcitx::Reset()
{
    ResetInput();
    for(unsigned int i=0;i<uMessageUp;i++)
    {
        messageUp[i].strMsg[0] = 0;
    }
    uMessageUp = 0;
    for(unsigned int i=0;i<uMessageDown;i++)
    {
        messageDown[i].strMsg[0] = 0;
    }
    uMessageDown = 0;
    strStringGet[0] = 0;
}
void IMEFcitx::Init()
{
    static BOOL fFirst = FALSE;
    if(fFirst == FALSE)
    {
        fFirst = TRUE;
        sprintf(respath,"%s%s",g_szAppPath,IME_DATA_PATH);
        main_PY();
        uMessageUp = 0;
        uMessageDown = 0;
        this->Reset();
        iMaxCandWord = 10;
    }
}

