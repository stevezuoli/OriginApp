#include "KernelType.h"
#include "DKEAPI.h"
#include "KernelMacro.h"
#include "KernelLog.h"
#include <dlfcn.h>  

static DK_BOOL WriteImageToBmp(const DK_BYTE* pBuf, DK_INT w, DK_INT h, const DK_CHAR* path)
{
    DK_ASSERT(pBuf);
    DK_ASSERT(w > 0);
    DK_ASSERT(h > 0);
    if (DK_NULL == pBuf || w < 1 || h < 1)
    {
        return DK_FALSE;
    }
    DK_SIZE_T width = (DK_SIZE_T)w;
    DK_SIZE_T height = (DK_SIZE_T)h;
    DK_SIZE_T strip = (DK_SIZE_T)width * 4;

    DK_BITMAPFILEHEADER fileHeader;
    DkZero(fileHeader);
    fileHeader.bfType = 0x4d42; // BM
    fileHeader.bfOffBits = sizeof(DK_BITMAPFILEHEADER) + sizeof(DK_BITMAPINFOHEADER);
    fileHeader.bfSize = strip * height + fileHeader.bfOffBits;


    DK_BITMAPINFOHEADER infoHeader;
    DkZero(infoHeader);
    infoHeader.biSize = sizeof(infoHeader);
    infoHeader.biWidth = width;
    infoHeader.biHeight = -(DK_LONG)height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 32;
    infoHeader.biSizeImage = strip * height;

    FILE* fp = fopen(path, "wb+");
    if (DK_NULL == fp)
    {
        return DK_FALSE;
    }
    fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
    fwrite(&infoHeader, sizeof(infoHeader), 1, fp);
    const DK_BYTE* data = pBuf;
    for (DK_SIZE_T i = 0; i < height; ++i)
    {
        DK_BYTE* p = (DK_BYTE*)&data[strip * i];
        fwrite(p, strip, 1, fp);
    }
    fclose(fp);
    return DK_TRUE;
}
static DK_BOOL SaveOutputDevAsBmp(const DK_BITMAPBUFFER_DEV* pDevice, const DK_CHAR* path)
{
    DK_ASSERT(DK_NULL != pDevice);
    DK_ASSERT(DK_NULL != path);
    DK_ASSERT(DK_PIXELFORMAT_RGB32 == pDevice->nPixelFormat);
    if (DK_NULL == pDevice || DK_NULL == path)
    {
        return DK_FALSE;
    }
    if (pDevice->nPixelFormat != DK_PIXELFORMAT_RGB32)
    {
        return DK_FALSE;
    }

    return WriteImageToBmp(pDevice->pbyData, pDevice->lWidth, pDevice->lHeight, path);
}

void TestEpub()
{
    void* epublib = dlopen("/DuoKan/libepublib.so", RTLD_LAZY |RTLD_DEEPBIND);
    DK_AUTO(g_DKE_Initialize, (bool(*)(const DK_WCHAR*)) dlsym(epublib, "DKE_Initialize"));
    DK_ASSERT(DK_NULL != epublib);
    g_DKE_Initialize(DK_NULL);
    DEBUG_POS;

    DK_AUTO(g_DKE_RegisterFont, (bool(*)(const DK_WCHAR*, const DK_WCHAR*)) dlsym(epublib, "DKE_RegisterFont"));
    DK_AUTO(g_DKE_SetDefaultFont, (bool(*)(const DK_WCHAR*, DK_CHARSET_TYPE)) dlsym(epublib, "DKE_SetDefaultFont"));
    const DK_WCHAR* faceName = L"wenquanyi";
    const DK_WCHAR* fontFile = L"/DuoKan/res/sysfonts/wqy-microhei.ttc";
    DEBUG_POS;

    g_DKE_RegisterFont(faceName, fontFile);
    g_DKE_SetDefaultFont(faceName, DK_CHARSET_GB);
    g_DKE_SetDefaultFont(faceName, DK_CHARSET_ANSI);
    DEBUG_POS;

    DK_AUTO(g_DKE_OpenDocument, (EPUBDOCHANDLE(*)(const DK_WCHAR* , DKE_CONTAINER_TYPE , const DK_WCHAR* ))dlsym(epublib, "DKE_OpenDocument"));;
    DEBUG_POS;

    IDKEBook* pBook = (IDKEBook*)g_DKE_OpenDocument(L"/DuoKan/test.epub",DKE_CONTAINER_ZIP, DK_NULL);
    DEBUG_POS;
    DK_ASSERT(DK_NULL != pBook);
    DEBUG_POS;
    DK_ReturnCode res = pBook->PrepareParseContent(DK_NULL);
    DK_ASSERT(DK_SUCCEEDED(res));

    DEBUG_POS;
    DKE_PARSER_OPTION option;
    DkZero(option);
    DK_UINT width = 600, height = 800;
    DK_BOX pageBox = DK_BOX(0, 0, width, height);
    DEBUG_POS;

    option.pageBox = pageBox;
    option.dpi = RenderConst::SCREEN_DPI;
    option.msType = DK_MEASURE_PIXEL;
    option.ptMode = DKE_PARSER_PAGETABLE_BOOK;
    res = pBook->ParseContent(option);
    DEBUG_POS;
    DK_ASSERT(DK_SUCCEEDED(res));
    DEBUG_POS;

    DK_BITMAPBUFFER_DEV dev;
    dev.lWidth = width;
    dev.lHeight = height;
    dev.nPixelFormat = DK_PIXELFORMAT_RGB32;//(DK_PIXELFORMAT)env->GetLongField(infodev, idbiPixelFormat);
    dev.lStride = width * 4;
    dev.nDPI = RenderConst::SCREEN_DPI;
    dev.pbyData = DK_MALLOCZ_OBJ_N(DK_BYTE, dev.lStride * height);
    MAKE_SCOPE_EXIT(devDataScope, dev.pbyData, &DK_FREE);

    DK_FLOWRENDEROPTION renderOption;
    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice = &dev;
    DK_FLOWRENDERRESULT renderResult;

    for (DK_INT i = 0; i < pBook->GetPageCount(); ++i)
    {
        if (i > 2)
            break;
        printf("page: %d\n", i);
        memset(dev.pbyData, 0xFF, dev.lStride * height);

        IDKEPage* pPage = pBook->GetPage(i);
        pPage->Render(renderOption, &renderResult);
        DK_CHAR buf[100];
        dk_snprintf(buf, sizeof(buf), "/DuoKan/epubtest%d.bmp", i);
        SaveOutputDevAsBmp(&dev, buf);
    }
} 

    
