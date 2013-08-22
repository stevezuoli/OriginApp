#ifndef __BACKGROUNDLOADER_H__
#define __BACKGROUNDLOADER_H__

#include <pthread.h>
#include "dkBaseType.h"

class CBackgroundLoader
{
public:    
    static void Start();
    static void CheckFontsCache();
    static void WaitForCheckingFontsCache();
    static void ResetInitializeStatus ();
    
private:
    static void* LoaderThreadFunc(void * p);
};

#endif /*__BACKGROUNDLOADER_H__*/

