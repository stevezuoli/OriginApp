//#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../../DkReader/Common/DKLogger.h"

#include "FontManager/DKFont.h"
#include "Framework/KernelGear.h"
#include "BookReader/EpubBookReader.h"
#include "Utility.h"

bool WriteImageToBmp(const DK_BYTE* pBuf, DK_INT w, DK_INT h, const DK_CHAR* path)
{
    assert(pBuf);
    assert(w > 0);
    assert(h > 0);
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

void TestEPUBBookReader ()
{
	IBookReader* pReader = new EpubBookReader ();
	if (!pReader->OpenDocument ("./test.epub"))
	{
		assert (false);
	}
	
    pReader->SetPageSize(600, 800);

    ICT_ReadingDataItem* pClsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::BOOKMARK);
    if(NULL == pClsReadingProgress)
    {
        return;
    }
    
    CT_RefPos clsBeginPos;
    clsBeginPos.SetChapterIndex(0);
    clsBeginPos.SetParaIndex(0);
    clsBeginPos.SetAtomIndex(0);

    CT_RefPos clsEndPos;
    clsEndPos.SetChapterIndex(1);
    clsEndPos.SetParaIndex(0);
    clsEndPos.SetAtomIndex(0);

    pClsReadingProgress->SetBeginPos(clsBeginPos);
    pClsReadingProgress->SetEndPos(clsEndPos);
	
	if (!pReader->GotoBookMark (pClsReadingProgress))
	{
		assert (false);
	}

	SafeDeletePointer (pClsReadingProgress);

	int pageNum(0);
	while (BOOKREADER_GOTOPAGE_SUCCESS == pReader->GotoNextPage ())
	{
		printf ("goto next page pageNum %d\n", pageNum++);
		DK_IMAGE* pImg;
		int i = 5;
		while (i)
		{
            pImg = pReader->GetPageBMP();
			if (pImg)
			{
                WriteImageToBmp (pImg->pbData, pImg->iWidth, pImg->iHeight, "./testepub.bmp");
				break;
			}

			i--;
			sleep (1);
		}
	}

	pReader->CloseDocument ();
	SafeDeletePointer (pReader);
}

void PDFReaderTest();

int main(int argc, char* argv[])
{
    printf("main start\n");
    DKLogger::InitLogger ();

    // /mnt/hgfs/Linux/kindle-lite/Laddish/KindleApp/Test
    string fontPath = "/mnt/hgfs/Linux/kindle-lite/Laddish/KindleApp/res/sysfonts";
    g_pFontManager->AddFontsDir (fontPath);

    g_pFontManager->SetDefaultFontFaceNameForCharSet(DK_CHARSET_GB, L"WenQuanYi Micro Hei");
    g_pFontManager->SetSystemDefaultFontFaceName(L"WenQuanYi Micro Hei");

	InitializeKernel ();
	RegisterKernelFonts ();
	SetKernelDefaultFonts ();

	TestEPUBBookReader ();
    PDFReaderTest();
    UnRegisterKernelFonts();
	DestroyKernel ();
    if (g_pFontManager)
    {
        delete g_pFontManager;
        g_pFontManager = NULL;
    }
    printf("main finish\n");
}

