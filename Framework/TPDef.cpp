#include "Public/Base/TPDef.h"

static bool isKeycodeMapIntialized = false;
static char keycodeMap[256];
static char alphaWithALT[256]; 
/*
* Translate Symbian's keyboard event type to Taipan's
*
* @return -1 cannot handle
*/

int GetTPKeyboardEventType(int eventType)
{
    switch (eventType)
    {
    case 0x00:
        return DK_KEYTYPE_UP;
        break;
    case 0x02:
        // FIXME: here is the key keep event, do we return repeat?
        return DK_KEYTYPE_REPEAT;
        break;
    case 0x01:
        return DK_KEYTYPE_DOWN;
        break;
    default:
        {
            return -1;
        }
    }

    return -1;
}

char VirtualKeyToChar(int iVirtualKey)
{
    if(!isKeycodeMapIntialized)
    {
        int mapSize = sizeof(keycodeMap)/sizeof(keycodeMap[0]);
        for (int i = 0; i < mapSize; i++)
        {
            // empty keycode map
            keycodeMap[i] = NOTVISIBLEKEY;
        }

        keycodeMap[KEY_A] = 'A';
        keycodeMap[KEY_B] = 'B';
        keycodeMap[KEY_C] = 'C';
        keycodeMap[KEY_D] = 'D';
        keycodeMap[KEY_E] = 'E';
        keycodeMap[KEY_F] = 'F';
        keycodeMap[KEY_G] = 'G';
        keycodeMap[KEY_H] = 'H';
        keycodeMap[KEY_I] = 'I';
        keycodeMap[KEY_J] = 'J';
        keycodeMap[KEY_K] = 'K';
        keycodeMap[KEY_L] = 'L';
        keycodeMap[KEY_M] = 'M';
        keycodeMap[KEY_N] = 'N';
        keycodeMap[KEY_O] = 'O';
        keycodeMap[KEY_P] = 'P';
        keycodeMap[KEY_Q] = 'Q';
        keycodeMap[KEY_R] = 'R';
        keycodeMap[KEY_S] = 'S';
        keycodeMap[KEY_T] = 'T';
        keycodeMap[KEY_U] = 'U';
        keycodeMap[KEY_V] = 'V';
        keycodeMap[KEY_W] = 'W';
        keycodeMap[KEY_X] = 'X';
        keycodeMap[KEY_Y] = 'Y';
        keycodeMap[KEY_Z] = 'Z';

        keycodeMap[KEY_0] = '0';
        keycodeMap[KEY_1] = '1';
        keycodeMap[KEY_2] = '2';
        keycodeMap[KEY_3] = '3';
        keycodeMap[KEY_4] = '4';
        keycodeMap[KEY_5] = '5';
        keycodeMap[KEY_6] = '6';
        keycodeMap[KEY_7] = '7';
        keycodeMap[KEY_8] = '8';
        keycodeMap[KEY_9] = '9';

        mapSize = sizeof(alphaWithALT)/sizeof(alphaWithALT[0]);

        for (int i = 0; i < mapSize; i++)
        {
            alphaWithALT[i] = NOTVISIBLEKEY;
        }

        alphaWithALT[(int)'Q'] = '!';
        alphaWithALT[(int)'W'] = '@';
        alphaWithALT[(int)'E'] = '#';
        alphaWithALT[(int)'R'] = '$';
        alphaWithALT[(int)'T'] = '%';
        alphaWithALT[(int)'Y'] = '^';
        alphaWithALT[(int)'U'] = '&';
        alphaWithALT[(int)'I'] = '*';
        alphaWithALT[(int)'O'] = '(';
        alphaWithALT[(int)'P'] = ')';

        alphaWithALT[(int)'q'] = '1';
        alphaWithALT[(int)'w'] = '2';
        alphaWithALT[(int)'e'] = '3';
        alphaWithALT[(int)'r'] = '4';
        alphaWithALT[(int)'t'] = '5';
        alphaWithALT[(int)'y'] = '6';
        alphaWithALT[(int)'u'] = '7';
        alphaWithALT[(int)'i'] = '8';
        alphaWithALT[(int)'o'] = '9';
        alphaWithALT[(int)'p'] = '0';

        isKeycodeMapIntialized = true;
    }

    int virtualKey = iVirtualKey & (~ALT_DOWN) & (~SHIFT_DOWN);
    if (virtualKey >= 256 || virtualKey < 0) 
    {
        return NOTVISIBLEKEY;
    }
    char value = keycodeMap[virtualKey];
    if (value == NOTVISIBLEKEY) 
    {
        return NOTVISIBLEKEY;
    }

    if(value >='0' && value <='9') 
    {
        return value;
    }

    if ((iVirtualKey & SHIFT_DOWN) == 0)    // Shift was not pressed, return lowercase letter
    {
        value += 32;
    }

    if ((iVirtualKey & ALT_DOWN) != 0)        //ALT was pressed, return special keys that doesn't exist on DK keyboard
    {
        return alphaWithALT[(int)value];
    }

    return value;


}

bool IsAlpha(int iVirtualKey)
{
    char cKey = VirtualKeyToChar(iVirtualKey);

    return (cKey >='A' && cKey <='Z') || (cKey >= 'a' && cKey <= 'z');
}

bool IsDigit(int iVirtualKey)
{
    char cKey = VirtualKeyToChar(iVirtualKey);
    return cKey >='0' && cKey <='9';
}


