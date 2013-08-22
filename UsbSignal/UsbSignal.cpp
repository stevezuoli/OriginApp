#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>


#define USB_CONFIGURED          41
#define USB_UNCONFIGURED        42
#define USB_PLUGOUT             43
#define USB_PLUGIN              44


#define USB_MSG_CONFIGURED          "usbConfigured"
#define USB_MSG_UNCONFIGURED        "usbUnconfigured"
#define USB_MSG_PLUGOUT             "usbPlugOut"
#define USB_MSG_USBPLUGIN           "usbPlugIn"


void SendMessage(char *lpMsg)
{
    char szCmd[64] = {0};
    int  nSend = 1;
    if(strcmp(lpMsg, USB_MSG_CONFIGURED) == 0)
    {
        sprintf(szCmd, "killall -%d KindleApp", USB_CONFIGURED);
    }
    else if(strcmp(lpMsg, USB_MSG_UNCONFIGURED) == 0)
    {
        sprintf(szCmd, "killall -%d KindleApp", USB_UNCONFIGURED);
    }
    else if(strcmp(lpMsg, USB_MSG_PLUGOUT) == 0)
    {
        sprintf(szCmd, "killall -%d KindleApp", USB_PLUGOUT);
    }
    else if(strcmp(lpMsg, USB_MSG_USBPLUGIN) == 0)
    {
        sprintf(szCmd, "killall -%d KindleApp", USB_PLUGIN);
    }
    else
    {
        nSend = 0;
    }
    if(nSend)
    {
        //printf("UsbSignal %s\n", lpMsg);
        system(szCmd);
    }
}
void *ThreadUsbSignal(void *lparam)
{
    char szCmd[128] = {0};
    sprintf(szCmd, "lipc-wait-event -s 0 com.lab126.hal  %s", (char*)lparam);
    while(1)
    {
        if(system(szCmd) != -1)
        {
            SendMessage((char*)lparam);
        }
    }
    return NULL;
}

pthread_t CreateThread(const char *lparam)
{
     pthread_attr_t threadAttr;
     pthread_t threadid;
     pthread_attr_init(&threadAttr);
     pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
     pthread_attr_setstacksize(&threadAttr, PTHREAD_STACK_MIN);
     pthread_create(&threadid, &threadAttr, ThreadUsbSignal, (void*)lparam);
     pthread_attr_destroy(&threadAttr);
     return threadid;
}


int main(int argc, char *argv[])
{
#if 0
    CreateThread(USB_MSG_CONFIGURED);
    CreateThread(USB_MSG_UNCONFIGURED);
    CreateThread(USB_MSG_PLUGOUT);
    CreateThread(USB_MSG_USBPLUGIN);
#endif
    while(1)
    {
        sleep(1000000000);
    }

    return 1;
}
