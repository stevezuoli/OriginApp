#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>

#include "interface.h"
#include "Common/DebugConsole.h"

struct termios saved_attributes;

void reset_input_mode (void)
{
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode (void)
{
    struct termios tattr;

    /* Make sure stdin is a terminal. */
    if (!isatty (STDIN_FILENO))
    {
        fprintf (stderr, "Not a terminal.\n");
        exit (EXIT_FAILURE);
    }

    /* Save the terminal attributes so we can restore them later. */
    tcgetattr (STDIN_FILENO, &saved_attributes);
    atexit (reset_input_mode);

    /* Set the funny terminal modes. */
    tcgetattr (STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
    tattr.c_cc[VMIN] = 1;
    tattr.c_cc[VTIME] = 0;
    tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

int foo()
{
    char c;
    while (1)
    {
        read (STDIN_FILENO, &c, 1);
        if (c == '\004') /* C-d */
            break;
        else
            DebugPrintf(DLC_VERBOSE, "char received: %d", c);
    }
    reset_input_mode ();
    return EXIT_SUCCESS;

    while(1)
    {
        DebugLog(DLC_VERBOSE, "---------Waiting for getchar()");
        int ich=getchar();
        DebugPrintf(DLC_VERBOSE, "------------------ ch=%d", ich);
        sleep(1);
    }
    return EXIT_SUCCESS;
}

int set_nonblock_flag(int desc, int value)
{
    int oldflags = fcntl (desc, F_GETFL, 0);

    /* If reading the flags failed, return error indication now. */
    if (oldflags == -1)
        return -1;

    /* Set just the flag we want to set. */
    if (value != 0)
        oldflags |= O_NONBLOCK;
    else
        oldflags &= ~O_NONBLOCK;

    /* Store modified flag word in the descriptor. */
    return fcntl (desc, F_SETFL, oldflags);
}

void ProcessConsoleInput()
{
    DebugLog(DLC_DBG_CONSOLE, "ENTER ProcessConsoleInput()");

    int iRet=set_nonblock_flag(STDIN_FILENO, 1);
    DebugPrintf(DLC_DBG_CONSOLE, "set_nonblock_flag(STDIN_FILENO, 1) returns %d", iRet);
    char ch(0);
    ssize_t cRet=read(STDIN_FILENO, &ch, 1);
    DebugPrintf(DLC_DBG_CONSOLE, "iRet=%d, ch: %d", cRet, ch);
    set_nonblock_flag(STDIN_FILENO, 0);

    if ( 12 == ch)    // Ctrl+L
    {
        int iTemp, iType;

        BOOL fHasOn(FALSE);
        for ( iTemp = 0; iTemp < DLC_Count; iTemp ++ )
        {
            if ( IsDebugDeviceEnabled( iTemp ) )
            {
                printf("( %s ) : ON \n", GetDebugDeviceName( iTemp ));
                fHasOn = TRUE;
            }
        }
        if (!fHasOn)
        {
            printf("ALL Logs are OFF!\n");
        }

        printf("\nSupported Debug Log Categories are: \n" );
        printf("\t-2:ALL OFF\t\t-1:ALL ON\n");
        for ( iTemp = 0; iTemp < DLC_Count; iTemp ++ )
        {
            printf("\t%02d: %s", iTemp + 1, GetDebugDeviceName( iTemp ) );
            if (iTemp & 1)
            {
                printf("\n");
            }
            else
            {
                printf("\t");
            }
        }

        do {
            printf("\nPlease input the category you want to change: ( 0 to exit) \n" );
            scanf("%d", &iType);
            DebugPrintf(DLC_DBG_CONSOLE, "iType = %d, DLC_Count=%d", iType, DLC_Count);
        }
        while (iType < -2 || iType > DLC_Count);

        if (iType)
        {
            if (-2 == iType)
            {
                // ALL OFF
                for ( iTemp = 0; iTemp < DLC_Count; iTemp++ )
                {
                    EnableDebugDevice( iTemp, FALSE );
                }
                printf("ALL debug logs are OFF...\n");
            }
            else if (-1 == iType)
            {
                // ALL ON
                for ( iTemp = 0; iTemp < DLC_Count; iTemp++ )
                {
                    EnableDebugDevice( iTemp, TRUE );
                }
                printf("ALL debug logs are ON...\n");
            }
            else
            {

                iType--;
                EnableDebugDevice( iType, !IsDebugDeviceEnabled( iType ) );
                printf("( %s ) Debug Log Status: %s!\n",
                    GetDebugDeviceName( iType ),
                    IsDebugDeviceEnabled( iType ) ? "ON" : "OFF"
                    );
            }
        }
        else
        {
            printf("Exiting...\n" );
        }
    }

    DebugLog(DLC_DBG_CONSOLE, "LEAVE ProcessConsoleInput()");
}

