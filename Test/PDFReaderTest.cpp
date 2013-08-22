#include "KernelType.h"
#include "DKEAPI.h"
#include "KernelMacro.h"
#include <dlfcn.h>
#include "BookReader/IBookReader.h"
#include "BookReader/PdfBookReader.h"
#include "FontManager/DKFont.h"
//#include <gtest/gtest.h>

static DK_BOOL SaveImageToBmp(const DK_IMAGE* pImage, const DK_CHAR* pFilePathName)
{
	DK_ASSERT(pImage);
	DK_ASSERT(pFilePathName);
	DK_ASSERT(pFilePathName[0]);
    
    DK_SIZE_T width = (DK_SIZE_T)(pImage->iWidth);
    DK_SIZE_T height = (DK_SIZE_T)(pImage->iHeight);
    DK_SIZE_T strip = (DK_SIZE_T)(pImage->iStrideWidth);

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
    infoHeader.biBitCount = pImage->iColorChannels * 8;
    infoHeader.biSizeImage = strip * height;

    FILE* fp = fopen(pFilePathName, "wb+");
    if (DK_NULL == fp)
    {
        return DK_FALSE;
    }

    if (infoHeader.biBitCount < 16)
    {
        fileHeader.bfOffBits += 1 << infoHeader.biBitCount;
    }
	
    fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
    fwrite(&infoHeader, sizeof(infoHeader), 1, fp);
    
    //灰度图像，有颜色表，写入文件
    DK_INT colorTableSize = 0;
	if (infoHeader.biBitCount < 16)
	{
		colorTableSize = 1 << infoHeader.biBitCount;
		DK_RGBQUAD* pColorTable = new DK_RGBQUAD[colorTableSize];
		DK_ASSERT(pColorTable);
	    for (DK_INT i = 0; i<colorTableSize; i++)   
	    {   
	        pColorTable[i].rgbBlue = i;
	        pColorTable[i].rgbGreen = i;
	        pColorTable[i].rgbRed = i;
	        pColorTable[i].rgbReserved = 0;
	    }
		
	    fwrite(pColorTable, sizeof(DK_RGBQUAD), colorTableSize, fp);
		delete pColorTable;
	}
	
    const DK_BYTE* data = pImage->pbData;
    for (DK_SIZE_T i = 0; i < height; ++i)
    {
        DK_BYTE* p = (DK_BYTE*)&data[strip * i];
        fwrite(p, strip, 1, fp);
    }
	
    fclose(fp);
    return DK_TRUE;

}

void PDFReaderTest()
{	
	PdfModeController modeController;
	printf("PDFReaderTest begin \n");
	PdfBookReader* pReader = new PdfBookReader;	
	pReader->OpenDocument(string("MyTest.pdf"));
	pReader->SetPageSize(600, 800);
	// 模式设置
	modeController.m_iRotation = 270;
	modeController.m_eReadingMode = PDF_RM_NormalPage;
	//modeController.m_eReadingMode = PDF_RM_Split4Page;
	//modeController.m_eReadingMode = PDF_RM_Split2Page;
	//modeController.m_eReadingMode = PDF_RM_ZoomPage;
	//modeController.m_eReadingMode = PDF_RM_Rearrange;
	modeController.m_dWidth = 0.5;
	modeController.m_dHeight = 0.5;
	modeController.m_uCurSubPage = 0;
	modeController.m_uCurPageStartPos = 0;
	modeController.m_uSubPageOrder = 0;
	modeController.m_dUserZoom = 1.5;

	
	modeController.m_eTurnPageMode = PDF_TPM_SinglePage;
	//modeController.m_eTurnPageMode = PDF_TPM_SinglePage;

	//切边设置
	modeController.m_eCuttingEdgeMode = PDF_CEM_Normal;
	//modeController.m_eCuttingEdgeMode = PDF_CEM_Smart;
	/*
	modeController.m_eCuttingEdgeMode = PDF_CEM_Custom;
	modeController.m_dLeftEdge = 0.0;
	modeController.m_dRightEdge = 0.0;
	modeController.m_dTopEdge = 0.0;
	modeController.m_dBottomEdge = 0.0;
	*/

	
	pReader->SetPdfModeController(&modeController);
	DK_INT pageCount = pReader->GetTotalPageNumber();
	printf("Total Page:%d \n", pageCount);

    const DK_IMAGE* pImage = DK_NULL;
    DK_CHAR rgFilePathName[1024];
    DK_INT i = 0;
    do
	{
        sprintf(rgFilePathName, "%05d.bmp", i++);
		printf("count:%d \n", i);
		pImage = pReader->GetPageBMP();
		SaveImageToBmp(pImage, rgFilePathName);
		if (i > 7)
		{
			break;
		}
	} while(BOOKREADER_GOTOPAGE_SUCCESS == pReader->GotoNextPage());

	SafeDeletePointer(pReader);
	printf("PDFReaderTest end \n");
}




    
