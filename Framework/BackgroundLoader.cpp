
#include <unistd.h>
#include <stdlib.h>
#include "Framework/BackgroundLoader.h"
#include "interface.h"
#include "Utility.h"
#include "IME/IMEManager.h"
#include "Utility/ThreadHelper.h"

using namespace DkFormat;

static BOOL fFontCacheInitialized = FALSE;

void* CBackgroundLoader::LoaderThreadFunc (void*)
{
    // init text layout
    CBackgroundLoader::CheckFontsCache();

    // init ime
    IMEManager::_init_ime_data(NULL);

    // Add more init work here...
    ThreadHelper::ThreadExit(NULL);
    return NULL;
}

void CBackgroundLoader::Start()
{
    pthread_t tid;
    ThreadHelper::CreateThread(&tid, CBackgroundLoader::LoaderThreadFunc, NULL, "CBackgroundLoader @ LoaderThreadFunc",true, THREAD_STACK_SIZE_KINDLE_DEFAULT);
}

void CBackgroundLoader::CheckFontsCache()
{
    DebugPrintf(DLC_ZHANGXB, "CBackgroundLoader::CheckFontsCache: entering");
    if (!fFontCacheInitialized)
    {
        //CFCFontManager::CheckFontsCache();
        //PangoContextManager::GetInstance()->Reset();
        fFontCacheInitialized = TRUE;
    }

    DebugPrintf(DLC_ZHANGXB, "CBackgroundLoader::CheckFontsCache: finish");
}

void CBackgroundLoader::ResetInitializeStatus ()
{
    fFontCacheInitialized = FALSE;
}

void CBackgroundLoader::WaitForCheckingFontsCache()
{
    DebugPrintf(DLC_ZHANGXB, "CBackgroundLoader::WaitForCheckingFontsCache: entering");

    while (!fFontCacheInitialized)
    {
        DebugPrintf(DLC_LIUJT, "CBackgroundLoader::WaitForCheckingFontsCache ing...");
        usleep(10000);
    }

    DebugPrintf(DLC_ZHANGXB, "CBackgroundLoader::WaitForCheckingFontsCache: finish");
}

