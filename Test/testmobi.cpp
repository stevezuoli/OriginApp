#include "KernelType.h"
#include "DKEAPI.h"
#include "KernelMacro.h"
#include "KernelString.h"
#include "KernelLog.h"
#include <dlfcn.h>
#include "BookReader/IBookReader.h"
#include "BookReader/MobiBookReader.h"
#include <time.h>

static const string pSaveDir("/DuoKan/testmobi/bmp");

static DK_BOOL WriteImageToBmp2(const DK_BYTE* pBuf, DK_INT w, DK_INT h, const DK_CHAR* path)
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
    DK_SIZE_T strip = width;

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
    infoHeader.biBitCount = 8;
    infoHeader.biSizeImage = strip * height;

    FILE* fp = fopen(path, "wb+");
    if (DK_NULL == fp)
    {
        return DK_FALSE;
    }
    fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
    fwrite(&infoHeader, sizeof(infoHeader), 1, fp);
    
    //灰度图像，有颜色表，写入文件  
    DK_RGBQUAD colortable[256];
    for (int i=0;i<256;++i)   
    {   
        colortable[i].rgbBlue=i;
        colortable[i].rgbGreen=i;
        colortable[i].rgbRed=i;
        colortable[i].rgbReserved=0;
    }
    DK_RGBQUAD* pColorTable1=colortable;
    fwrite(pColorTable1, sizeof(DK_RGBQUAD),256, fp);
    
    const DK_BYTE* data = pBuf;
    for (DK_SIZE_T i = 0; i < height; ++i)
    {
        DK_BYTE* p = (DK_BYTE*)&data[strip * i];
        fwrite(p, strip, 1, fp);
    }
    fclose(fp);
    return DK_TRUE;
}

static DK_BOOL SaveOutputDevAsBmp2(const DK_IMAGE* pImage, const DK_CHAR* path)
{
    DK_ASSERT(DK_NULL != pImage);
    DK_ASSERT(DK_NULL != path);
    DK_ASSERT(1 == pImage->iColorChannels);

    return WriteImageToBmp2(pImage->pbData, pImage->iWidth, pImage->iHeight, path);
}

static void TestNextPrevPage(IBookReader* pBookReader)
{
    DEBUG_POS;
    pBookReader->GotoNextPage();
    DEBUG_POS;
    string dir = pSaveDir+"GotoNextPage1.bmp";
    SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str());      
    DEBUG_POS;
    pBookReader->GotoNextPage();
    DEBUG_POS;
    dir = pSaveDir+"GotoNextPage2.bmp";
    SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str());      
    DEBUG_POS;
    pBookReader->GotoPrevPage();
    DEBUG_POS;
    dir = pSaveDir+"GotoPrevPage1.bmp";
    SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str());      
    DEBUG_POS;
}

void TestMobiKernel()
{
    DEBUG_POS;
    IBookReader* pBookReader = new MobiBookReader();
    DEBUG_POS;
    IBMPProcessor* pProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
    if (DK_NULL != pProcessor)
    {
        DEBUG_POS;
        pBookReader->AddBMPProcessor(pProcessor);
    }
    DEBUG_POS;
    DK_LAYOUTSETTING _layout;
	pBookReader->SetLayoutSettings(_layout);
    DEBUG_POS;	
    int readerPageWidth = 600 - 2 * 30;
	int readerPageHeight =  800 - 2 * 30;
	pBookReader->SetPageSize(readerPageWidth, readerPageHeight);
    DEBUG_POS;	
    DK_ASSERT(DK_NULL != pBookReader);
    string dir;
    if (pBookReader->OpenDocument(string("/mnt/us/documents/test2.mobi")))
    {
        DEBUG_POS;        
        while (DK_TRUE)
        {
            DK_INT iPageCount = pBookReader->GetTotalPageNumber();
            if (iPageCount > 0)
            {
                pBookReader->MoveToPage(iPageCount - 1);      
                DEBUG_POS;
                dir = pSaveDir+"FinalPage.bmp";
                SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str());        
                DEBUG_POS; 
                break;
            }
            sleep(5);
        }
        DEBUG_POS;
       
        DEBUG_POS;
        pBookReader->MoveToPage(6);        
        DEBUG_POS;
        dir = pSaveDir+"1.bmp";
        SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str());      
        DEBUG_POS;
        pBookReader->GotoPrevPage();           
        DEBUG_POS;
        dir = pSaveDir+"2.bmp";
        SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str());    
        DEBUG_POS;
        pBookReader->GotoNextPage();           
        DEBUG_POS;
        dir = pSaveDir+"3.bmp";
        SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str());   
        DEBUG_POS;
        pBookReader->MoveToChapter(6);        
        DEBUG_POS;
        dir = pSaveDir+"4.bmp";
        SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str()); 
        DEBUG_POS;        
        pBookReader->MoveToPage(6);          
        DEBUG_POS;
        dir = pSaveDir+"5.bmp";
        SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str());      
        DEBUG_POS;
        
        DEBUG_POS;        
        pBookReader->MoveToChapter(4);          
        DEBUG_POS;
        dir = pSaveDir+"6.bmp";
        SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str()); 
        DEBUG_POS;
        pBookReader->GotoNextPage();       
        DEBUG_POS;
        dir = pSaveDir+"7.bmp";
        SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str()); 
        DEBUG_POS;
        pBookReader->GotoPrevPage();       
        DEBUG_POS;
        dir = pSaveDir+"8.bmp";
        SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str()); 
        DEBUG_POS;
        pBookReader->GotoNextPage();       
        DEBUG_POS;
        dir = pSaveDir+"9.bmp";
        SaveOutputDevAsBmp2(pBookReader->GetPageBMP(), dir.c_str()); 
        DEBUG_POS;
        pBookReader->CloseDocument();
        DEBUG_POS;
    }
    else
    {
        DEBUG_POS;
        printf("OpenDocument ERROR!!!\n");   
    }
    DEBUG_POS;
}

    
