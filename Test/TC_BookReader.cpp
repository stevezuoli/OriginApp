#include "BookReader/IBookReader.h"
#include "BookReader/TextBookReader.h"	
#include "BookReader/EpubBookReader.h"	 
#include "BookReader/MobiBookReader.h"	 
#include "BookReader/PdfBookReader.h"	  
#include "Common/File_DK.h"
#include <time.h>
#include <sys/stat.h>

const string gc_strSaveDir = "/mnt/us/DK_TC";
static int   gs_iPrintIndex = 0;

static DK_BOOL WriteImageToBmp(const DK_BYTE* pBuf, DK_INT w, DK_INT h, const DK_CHAR* path)
{
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

static DK_BOOL SaveOutputDevAsBmp(const DK_IMAGE* pImage, const DK_CHAR* path)
{
    if (NULL == pImage || NULL == path || 1 != pImage->iColorChannels)
    {
        return false;
    }

    return WriteImageToBmp(pImage->pbData, pImage->iWidth, pImage->iHeight, path);
}

static bool MkDir(string &strSaveDir)
{
    string strCmd = "mkdir " + strSaveDir;
    SystemUtil::ExecuteCommandstrCmd.c_str());
    return access(strSaveDir.c_str(),F_OK) == 0;
}

static bool TC_BookReader_OpenDocument(IBookReader * _pBookReader, const string &_strFilename)
{
    assert (NULL != _pBookReader);
    DEBUG_POS;
    if (!_pBookReader->OpenDocument(_strFilename))
	{
        DEBUG_POS;
		return false;
	}
    
    DEBUG_POS;    
    return true;    
}

static bool TC_BookReader_CloseDocument(IBookReader * _pBookReader)
{
    assert (NULL != _pBookReader);
    DEBUG_POS;
    return _pBookReader->CloseDocument();    
}

static bool TC_BookReader_SetPageSize(IBookReader * _pBookReader)
{
    assert (NULL != _pBookReader);
    int readerPageWidth = 600 - 2 * 30;
	int readerPageHeight =  800 - 2 * 30;
    DEBUG_POS;
    return _pBookReader->SetPageSize(readerPageWidth, readerPageHeight);
}

static bool TC_BookReader_GetCurrentPageIndex(IBookReader * _pBookReader, int _iPageCount)
{
    assert (NULL != _pBookReader && _iPageCount >= 0);
    
    int lstPageIndex[3] = {0, _iPageCount >> 1, _iPageCount - 1};
    for (int i = 0; i < 3; ++i)
    {
        if (BOOKREADER_GOTOPAGE_SUCCESS != _pBookReader->MoveToPage(lstPageIndex[i]) || _pBookReader->GetCurrentPageIndex() != lstPageIndex[i])
    	{
            DEBUG_POS;
    		return false;
    	}       
    }
    return true;
}

static bool TC_BookReader_GotoPrevPage(IBookReader * _pBookReader)
{
    assert (NULL != _pBookReader);
    DEBUG_POS;
    gs_iPrintIndex++;    
    char pSaveDir[256] = {0};
    sprintf(pSaveDir,"%s/%d_TC_BookReader_GotoPrevPage", gc_strSaveDir.c_str(), gs_iPrintIndex);
    string strSaveDir(pSaveDir);
    DEBUG_POS;
	if(!MkDir(strSaveDir))
	{
        DEBUG_POS;
		return false;
	}
    
    DEBUG_POS;    
    string strBefore = strSaveDir + "/Before.bmp";
    SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strBefore.c_str());

    if (BOOKREADER_GOTOPAGE_SUCCESS == _pBookReader->GotoPrevPage())
    {
        DEBUG_POS;    
        string strBefore = strSaveDir + "/After.bmp";
        SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strBefore.c_str());
        DEBUG_POS;
        return true;        
    }   
    DEBUG_POS;
    return false;
}

static bool TC_BookReader_GotoNextPage(IBookReader * _pBookReader)
{
    assert (NULL != _pBookReader);
    DEBUG_POS;
    gs_iPrintIndex++;    
    char pSaveDir[256] = {0};
    sprintf(pSaveDir, "%s/%d_TC_BookReader_GotoNextPage", gc_strSaveDir.c_str(), gs_iPrintIndex);
    string strSaveDir(pSaveDir);
    DEBUG_POS;
	if(!MkDir(strSaveDir))
	{
        DEBUG_POS;
		return false;
	}
    
    DEBUG_POS;    
    string strBefore = strSaveDir + "/Before.bmp";
    SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strBefore.c_str());

    if (BOOKREADER_GOTOPAGE_SUCCESS == _pBookReader->GotoNextPage())
    {
        DEBUG_POS;    
        strBefore = strSaveDir + "/After.bmp";
        SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strBefore.c_str());
        DEBUG_POS;
        return true;        
    }   
    DEBUG_POS;
    return false;
}

static bool TC_BookReader_GetTOC(IBookReader * _pBookReader)
{
#if 0
    assert (NULL != _pBookReader);
    gs_iPrintIndex++;    
    char pSaveDir[256] = {0};
    sprintf(pSaveDir,"%s/%d_TC_BookReader_GetTOC", gc_strSaveDir.c_str(), gs_iPrintIndex);
    string strSaveDir(pSaveDir);
    DEBUG_POS;
	if(!MkDir(strSaveDir))
	{
        DEBUG_POS;
		return false;
	}
    
    DEBUG_POS;
    int         iChapterCount = -1;
    DK_TOCINFO* pTocInfo      = _pBookReader->GetTOC(&iChapterCount);    
    if (NULL == pTocInfo || iChapterCount <= 0)
    {
        DEBUG_POS;
        return false;        
    }  
    
    string strBefore = strSaveDir + "/GetTOC.txt";
    FILE* fp = fopen(strBefore.c_str(), "wb+");
    DEBUG_POS;
    if (DK_NULL == fp)
    {
        DEBUG_POS;
        return false; 
    }
    
    DEBUG_POS;
    for (int i = 0; i < iChapterCount; ++i)
    {
        DEBUG_POS;
        if (NULL != pTocInfo[i].pcChapterName)
        {
            DEBUG_POS;
            char csTemp[256] = {0};
            sprintf(csTemp,"(%d,\t%d)\t", pTocInfo[i].lPageIndex, pTocInfo[i].lPageCount);
            fwrite(csTemp, dk_strlen(csTemp), 1, fp);
            DK_CHAR* strChapterName = EncodingUtil::WCharToChar(pTocInfo[i].pcChapterName);
            fwrite(strChapterName, dk_strlen(strChapterName), 1, fp);
            fwrite("\r\n", 2, 1, fp);
            DK_FREE(strChapterName);
        }
    }
    fclose(fp);
#endif 
    return true;
}

static bool TC_BookReader_MoveToChapter(IBookReader * _pBookReader)
{
    assert (NULL != _pBookReader);
    gs_iPrintIndex++;    
    char pSaveDir[256] = {0};
    sprintf(pSaveDir,"%s/%d_TC_BookReader_MoveToChapter", gc_strSaveDir.c_str(), gs_iPrintIndex);
    string strSaveDir(pSaveDir);
    DEBUG_POS;
	if(!MkDir(strSaveDir))
	{
        DEBUG_POS;
		return false;
	}
    
    int         iChapterCount = -1;
    DK_TOCINFO* pTocInfo      = NULL;/*_pBookReader->GetTOC(&iChapterCount);  */
    DEBUG_POS;
    if (NULL == pTocInfo || iChapterCount <= 0)
    {
        DEBUG_POS;
        return false;        
    }  
    
    DEBUG_POS;
    int lstChapterIndex[3] = {0, iChapterCount >> 1, iChapterCount-1};
    for (int i = 0; i < 3; ++i)
    {
        DEBUG_POS;
        char pTempDir[256] = {0};
        sprintf(pTempDir,"%s/MoveToChapter_%d.bmp", pSaveDir, lstChapterIndex[i]);
        
        if (BOOKREADER_GOTOPAGE_SUCCESS != _pBookReader->MoveToChapter(lstChapterIndex[i]))
        {
            DEBUG_POS;
            return false;        
        } 
        
        DEBUG_POS;    
        SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), pTempDir);
    }
    
    DEBUG_POS; 
    return true;
}

static bool TC_BookReader_MoveToPage(IBookReader * _pBookReader, int _iPageCount)
{
    assert (NULL != _pBookReader);

    gs_iPrintIndex++;
    char pSaveDir[256] = {0};
    sprintf(pSaveDir,"%s/%d_TC_BookReader_MoveToPage", gc_strSaveDir.c_str(), gs_iPrintIndex);
    string strSaveDir(pSaveDir);
    DEBUG_POS;
	if(!MkDir(strSaveDir))
	{
        DEBUG_POS;
		return false;
	}
    
    DEBUG_POS;
    int lstPageIndex[3] = {0, _iPageCount >> 1, _iPageCount-1};
    for (int i = 0; i < 3; ++i)
    {
        DEBUG_POS;
        char pTempDir[256] = {0};
        sprintf(pTempDir,"%s/MoveToPage_%d.bmp", pSaveDir, lstPageIndex[i]);
        
        if (BOOKREADER_GOTOPAGE_SUCCESS != _pBookReader->MoveToPage(lstPageIndex[i]))
        {
            DEBUG_POS;  
            return false;        
        }    
        SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), pTempDir);
    }
    DEBUG_POS;
    return true;
}

static bool TC_BookReader_GetCurrentPageText(IBookReader * _pBookReader)
{
    assert (NULL != _pBookReader);
    gs_iPrintIndex++;    
    char pSaveDir[256] = {0};
    sprintf(pSaveDir,"%s/%d_TC_BookReader_GetCurrentPageText", gc_strSaveDir.c_str(), gs_iPrintIndex);
    string strSaveDir(pSaveDir);
    DEBUG_POS;
	if(!MkDir(strSaveDir))
	{
        DEBUG_POS;
		return false;
	}
    
    DEBUG_POS;
    int iPageNumber = _pBookReader->GetTotalPageNumber();
	if(iPageNumber <= 0)
	{
        DEBUG_POS;
		return false;
	}
    
    int lstPageIndex[3] = {0, iPageNumber >> 1, iPageNumber - 1};
    for (int i = 0; i < 3; ++i)
    {
        int iPageIndex = lstPageIndex[i];
        char pTempDir[256] = {0};
        sprintf(pTempDir,"%s/GetCurrentPageText%d", pSaveDir, i);
        strSaveDir = pTempDir;

        if (BOOKREADER_GOTOPAGE_SUCCESS != _pBookReader->MoveToPage(iPageIndex))
        {
            DEBUG_POS;  
            return false;        
        }    
        string strTemp = strSaveDir + ".bmp";
        SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strTemp.c_str());
        DEBUG_POS; 

        string strPageText = _pBookReader->GetCurrentPageText();
        if (strPageText.length() > 0)
        {        
            strTemp = strSaveDir + ".txt";
            FILE* fp = fopen(strTemp.c_str(), "wb+");
            if (DK_NULL == fp)
            {
                DEBUG_POS;
                return false; 
            }        
            DEBUG_POS;
            
            fwrite(strPageText.c_str(), strPageText.length(), 1, fp);
            fclose(fp);       
        }   
        DEBUG_POS;
    }
    return true;
}

static bool TC_BookReader_GetTotalPageNumber(IBookReader * _pBookReader, int* _pPageCount)
{
    // 解析完再测试,放到第一个测试的位置
    assert (NULL != _pBookReader && NULL != _pPageCount);
    DEBUG_POS;
    int iTryTimes = 5;
    while (iTryTimes > 0)
    {
        DEBUG_POS;
        *_pPageCount = _pBookReader->GetTotalPageNumber();
        if (*_pPageCount >= 0)
        {
            DEBUG_POS;
            return true;        
        }   
        sleep(5);
        iTryTimes--;
        DEBUG_POS;
    }
    DEBUG_POS;
    return false;    
}

static bool TC_BookReader_ReParseContent(IBookReader* _pBookReader, int _iPageCount)
{
    assert (NULL != _pBookReader && _iPageCount > 0);
    gs_iPrintIndex++;    
    char pSaveDir[256] = {0};
    sprintf(pSaveDir,"%s/%d_TC_BookReader_ReParseContent", gc_strSaveDir.c_str(), gs_iPrintIndex);
    string strSaveDir(pSaveDir);
    DEBUG_POS;
	if(!MkDir(strSaveDir))
	{
        DEBUG_POS;
		return false;
	}
    
    DEBUG_POS;
    if (BOOKREADER_GOTOPAGE_SUCCESS != _pBookReader->MoveToPage(_iPageCount >> 1))
    {
        DEBUG_POS;
        return false;
    }
    
    DEBUG_POS;
    string strBefore = strSaveDir + "/ReParseContentBefore.bmp";
    SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strBefore.c_str());
    DEBUG_POS;

    if (BOOKREADER_GOTOPAGE_SUCCESS != _pBookReader->GotoPrevPage())
    {
        DEBUG_POS;
        return false;        
    }   
    DEBUG_POS;    
    strBefore = strSaveDir + "/ReParseContentBeforePrev.bmp";
    SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strBefore.c_str());
    DEBUG_POS;

    if (BOOKREADER_GOTOPAGE_SUCCESS != _pBookReader->GotoNextPage())
    {
        DEBUG_POS;
        return false;        
    }   
    DEBUG_POS;    
    strBefore = strSaveDir + "/ReParseContentBeforeNext.bmp";
    SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strBefore.c_str());
    DEBUG_POS;
    
    DK_LAYOUTSETTING clsLayoutSetting;
    clsLayoutSetting.dFontSize    = 27;
    clsLayoutSetting.dLineGap     = 2;
    clsLayoutSetting.dParaSpacing = 1;
    clsLayoutSetting.dTabStop     = 2;
    clsLayoutSetting.dIndent      = 1;
    if (!_pBookReader->SetLayoutSettings(clsLayoutSetting))
    {
        DEBUG_POS;
        return false;
    }
    DEBUG_POS;
    
    if (!_pBookReader->ReParseContent())
    {
        DEBUG_POS;
        return false;
    }
    
    DEBUG_POS; 
    string strAfter = strSaveDir + "/ReParseContentAfter.bmp";
    int iTryTimes = 5;
    while (iTryTimes >= 0)
    {
        DK_IMAGE* pImage = _pBookReader->GetPageBMP();
        if (NULL != pImage)
        {
            SaveOutputDevAsBmp(pImage, strAfter.c_str());
            DEBUG_POS;
            break;
        }
        sleep(5);
        iTryTimes--;
    }   
      
    if (iTryTimes < 0)
    {
        DEBUG_POS;
        return false;        
    }   
    DEBUG_POS;    

    if (BOOKREADER_GOTOPAGE_SUCCESS != _pBookReader->GotoPrevPage())
    {
        DEBUG_POS;
        return false;        
    }   
    DEBUG_POS;    
    strAfter = strSaveDir + "/ReParseContentAfterPrev.bmp";
    SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strAfter.c_str());
    DEBUG_POS;

    if (BOOKREADER_GOTOPAGE_SUCCESS != _pBookReader->GotoNextPage())
    {
        DEBUG_POS;
        return false;        
    }   
    DEBUG_POS;    
    strAfter = strSaveDir + "/ReParseContentAfterNext.bmp";
    SaveOutputDevAsBmp(_pBookReader->GetPageBMP(), strAfter.c_str());
    DEBUG_POS;
    
    return true;
}

void TestBookReader(DkFileFormat _format, string &_strFileDir)
{ 
    printf("\n/***********************************************************************\n"); 
    printf("/*************************TestEpubKernel Start**************************\n");  
    printf("/***********************************************************************\n\n"); 

    DEBUG_POS;
    IBookReader* pBookReader = NULL;
	DK_LAYOUTSETTING		  _layout;
	if ( DFF_Text == _format)
	{
		pBookReader = new TextBookReader();
        _layout.dFontSize = 24;
        _layout.dLineGap = 1.0;
        _layout.dParaSpacing = 0.01;
	}
	else if ( DFF_ElectronicPublishing == _format)
	{
		pBookReader = new EpubBookReader();
	}
	else if ( DFF_PortableDocumentFormat == _format)
	{
		pBookReader = new PdfBookReader();
	}
	else if ( DFF_MobiPocket == _format)
	{
		pBookReader = new MobiBookReader();
	}		 
    DK_ASSERT(DK_NULL != pBookReader);
    DEBUG_POS;
	pBookReader->SetLayoutSettings(_layout);
    DEBUG_POS;
    IBMPProcessor* pProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
    if (DK_NULL != pProcessor)
    {
        DEBUG_POS;
        pBookReader->AddBMPProcessor(pProcessor);
    }
    DEBUG_POS;
    if (!TC_BookReader_SetPageSize(pBookReader))
    {
        DEBUG_POS;
        printf("\n----------------SetPageSize ERROR----------------\n\n"); 
    }
    
    if (!TC_BookReader_OpenDocument(pBookReader, _strFileDir))
    {
        DEBUG_POS;
        printf("\n----------------OpenDocument ERROR----------------\n\n"); 
        return;
    }
    
    int  iPageCount = -1;
    if (!TC_BookReader_GetTotalPageNumber(pBookReader, &iPageCount))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_GetTotalPageNumber ERROR----------------\n\n"); 
        return;   
    }
    
    if (!TC_BookReader_GetTOC(pBookReader))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_GetTOC ERROR----------------\n\n");   
    }
    
    if (!TC_BookReader_GetCurrentPageIndex(pBookReader, iPageCount - 1))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_GetCurrentPageIndex ERROR----------------\n\n");   
    }
    
    if (!TC_BookReader_MoveToChapter(pBookReader))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_MoveToChapter ERROR----------------\n\n");   
    }
    
    if (!TC_BookReader_MoveToPage(pBookReader, iPageCount))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_MoveToPage ERROR----------------\n\n");   
    }
    
    if (!TC_BookReader_GetCurrentPageText(pBookReader))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_GetCurrentPageText ERROR----------------\n\n");   
    }
    
    if (!TC_BookReader_ReParseContent(pBookReader, iPageCount))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_ReParseContent ERROR----------------\n\n");   
    }
    
    if (!TC_BookReader_GotoNextPage(pBookReader))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_GotoNextPage ERROR----------------\n\n");   
    }
    
    if (!TC_BookReader_GotoPrevPage(pBookReader))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_GotoPrevPage ERROR----------------\n\n");   
    }  
    
    if (!TC_BookReader_CloseDocument(pBookReader))
    {
        DEBUG_POS;
        printf("\n----------------TC_BookReader_CloseDocument ERROR----------------\n\n");   
    } 
    DEBUG_POS;
    
    printf("\n/***********************************************************************\n"); 
    printf("/**************************TestEpubKernel End***************************\n");  
    printf("/***********************************************************************\n\n"); 
}

 
