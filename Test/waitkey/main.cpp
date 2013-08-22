
#include <linux/input.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/kd.h>
#include <pthread.h>
#include <signal.h>
#include  <iostream>

using namespace std;

static int ret_value = -1;

static int kbd_fd = 0;          // fd for main keyboard
static int kbd5_fd = 0;         // fd 五向键设备
static int kbd_volume_fd = 0;   // fd Kindle3音量键

int InitKeyboard(const char * pszDev)
{
    if(NULL == pszDev)
    {
        return -1;
    }
    int s32Fd = open(pszDev, O_RDONLY|O_NOCTTY);
    if(s32Fd>=0)
    {
		cout << "open successfully:" << pszDev << endl;
    }
    else
    {
		cout << "open failed:" << pszDev << endl;
    }

    if (s32Fd>=FD_SETSIZE)
    {
		cout << "add to keyboard fd set:" << pszDev << endl;
    }
    return s32Fd;
}

void InitAllKeys(void)
{
    kbd_fd = InitKeyboard("/dev/input/event0");
    kbd5_fd = InitKeyboard("/dev/input/event1");
    kbd_volume_fd = InitKeyboard("/dev/input/event2");
}

void CloseAllKeys(void)
{
    if (kbd_fd >= 0 )
        close(kbd_fd);
    if (kbd5_fd >= 0)
        close(kbd5_fd);
    if (kbd_volume_fd >= 0)
        close(kbd_volume_fd);
}

void* EventLoop(void*)
{

    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
	
    struct input_event iebuf;
    fd_set readfds;

    int iMaxfd = FD_SETSIZE;

    if (kbd_fd >kbd5_fd )
        iMaxfd = kbd_fd;
    else 
        iMaxfd = kbd5_fd;    
    
    if ( iMaxfd < kbd_volume_fd)
        iMaxfd = kbd_volume_fd;
    
    {
		pthread_testcancel();
        FD_ZERO(&readfds);
        
        if (kbd_fd >= 0 &&  kbd_fd < FD_SETSIZE)
            FD_SET(kbd_fd, &readfds);
        
        if (kbd5_fd >= 0 &&  kbd5_fd < FD_SETSIZE)
            FD_SET(kbd5_fd, &readfds);

        if (kbd_volume_fd >= 0 &&  kbd_volume_fd < FD_SETSIZE)
           FD_SET(kbd_volume_fd, &readfds);


        int iRet=select(iMaxfd+1, &readfds, NULL, NULL, NULL);
        if ( iRet > 0)
        {

            if (FD_ISSET(kbd_fd, &readfds))
            {
                read(kbd_fd, &iebuf, sizeof(iebuf));
            }
            else if (FD_ISSET(kbd5_fd, &readfds))
            {
                read(kbd5_fd, &iebuf, sizeof(iebuf));
            }
            else if (kbd_volume_fd > 0 && FD_ISSET(kbd_volume_fd, &readfds))
            {
                read(kbd_volume_fd, &iebuf, sizeof(iebuf));
            }
            else
            {
                cout << "no key \n" <<endl;
				return NULL;
            }

			cout << "iebuf.code=" << iebuf.code << " iebuf.type=" << iebuf.type << " iebuf.value=" << iebuf.value<<endl;

			ret_value = iebuf.code * iebuf.type  * iebuf.value;
        }
    }

	return NULL;
}

int main(int argc, char *argv[])
{  
	InitAllKeys();
	
	pthread_t tidp;
	pthread_create( &tidp, NULL , EventLoop, NULL);
	sleep(5);

	if (ret_value < 0)
		pthread_cancel(tidp);

	CloseAllKeys();
	
	return ret_value;
}

