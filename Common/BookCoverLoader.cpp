#include "Common/BookCoverLoader.h"
#include "Common/File_DK.h"
#include "Utility/PathManager.h"
#include "Utility/RenderUtil.h"
#include "BookReader/IBookReader.h"
#include "Common/WindowsMetrics.h"
#include "DKXManager/ReadingInfo/CT_ReadingDataItemImpl.h"
#include "DKXManager/DKX/DKXManager.h"
#include "DkStreamFactory.h"
#include "DKRAPI.h"

using namespace dk::utility;
using namespace WindowsMetrics;

const char* BookCoverLoader::EventBookCoverLoaded = "EventBookCoverLoaded";
void BookCoverLoader::FireBookCoverLoadedEvent(
        const char* bookPath,
        const char* coverPath)
{
    BookCoverLoadedArgs args;
    args.bookPath = bookPath;
    args.coverPath = coverPath;
    FireEvent(EventBookCoverLoaded, args);
}

BookCoverLoader::BookCoverLoader()
    : m_minCoverWidth(GetWindowMetrics(UICoverViewItemInnerCoverWidthIndex))
    , m_minCoverHeight(GetWindowMetrics(UICoverViewItemInnerCoverHeightIndex))
{
}

BookCoverLoader* BookCoverLoader::GetInstance()
{
    static BookCoverLoader s_bookCoverLoader;
    return &s_bookCoverLoader;
}

bool IsValidImage(const char* imageFile, int minWidth, int minHeight)
{
    DK_AUTO(wImageFile, EncodeUtil::ToWString(imageFile));
    if (wImageFile.empty())
    {
        return false;
    }

    DK_UINT imgWidth  = 0;
    DK_UINT imgHeight = 0;
    DKR_AnalyseImageFile(wImageFile.c_str(), &imgWidth, &imgHeight);
    if (static_cast<int>(imgWidth) != minWidth && static_cast<int>(imgHeight) != minHeight)
    {
        return false;
    }
    return true;
}

void BookCoverLoader::AddBook(const char* book)
{
    m_toLoadBooks.Send(book);
}

std::string LoadCoverBmp(const char* filePath, int minWidth, int minHeight)
{
    DkFileFormat fileFormat = GetFileFormatbyExtName(filePath);
    std::string result;
    result = PathManager::BookFileToCoverImageFile(filePath);
    if (PathManager::IsFileExisting(result) &&
        IsValidImage(result.c_str(), minWidth, minHeight))
    {
        return result;
    }
    std::auto_ptr<IBookReader> bookReader(IBookReader::CreateBookReaderFromFormat(fileFormat));
    int smallWidth = minWidth;
    int smallHeight = minHeight;
    if (bookReader.get() && bookReader->OpenDocument(filePath))
    {
        if (DFF_PortableDocumentFormat == fileFormat)
        {
            DKXManager* pclsDKXManager = DKXManager::GetInstance();
            if (pclsDKXManager)
            {
                if(pclsDKXManager->FetchCrash(filePath))
                {
                    return "";
                }
                pclsDKXManager->SetCrash(filePath, true);
                pclsDKXManager->SaveCurDkx();
            }

            IBMPProcessor* pProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);

            bookReader->AddBMPProcessor(pProcessor);
    
            PdfModeController modelController; 
            bookReader->SetPdfModeController(&modelController);
            bookReader->SetScreenSize(smallWidth, smallHeight);
            bookReader->SetPageSize(0, 0, smallWidth, smallHeight);
            CT_ReadingDataItemImpl readingBookmark;
            CT_RefPos beginPos(0, 0, 0, 0);
            readingBookmark.SetBeginPos(beginPos);
            bookReader->GotoBookMark(&readingBookmark);
            DK_IMAGE* img = bookReader->GetPageBMP();
			if(NULL == img)
			{
				return "";
            }

            if (pclsDKXManager)
            {
                pclsDKXManager->SetCrash(filePath, false);
                pclsDKXManager->SaveCurDkx();
            }

            int imgWidth = img->iWidth;
            int imgHeight = img->iHeight;
            if (imgWidth <= smallWidth / 2 || imgHeight <= smallHeight / 2)
            {
                return "";
            }
            else
            {
                RenderUtil::SaveDKImage(*img, result.c_str());
                return result;
            }
        }
        else
        {
            BOOK_INFO_DATA bookInfo = bookReader->GetBookInfo();
            if (NULL != bookInfo.coverData)
            {
                std::auto_ptr<IDkStream> imgStream( 
                        DkStreamFactory::GetMemoryStream((void*)bookInfo.coverData,
                            bookInfo.coverDataLen,
                            bookInfo.coverDataLen));
                if (NULL == imgStream.get())
                {
                    return "";
                }
                imgStream->Open();
                DK_UINT width = 0, height = 0;
                DKR_AnalyseImageStream(imgStream.get(), &width, &height);
                if (width <= 0 || height <= 0)
                {
                    return "";
                }
                if (smallWidth * height > smallHeight * width)
                {
                    smallWidth = smallHeight * width / height;
                }
                else
                {
                    smallHeight = smallWidth * height / width;
                }

                DK_IMAGE imgSmallCover;
                DK_BITMAPBUFFER_DEV dev;
                if (!RenderUtil::CreateRenderImageAndDevice32(
                            smallWidth,
                            smallHeight,
                            DK_ARGBCOLOR(0xFF, 0xFF, 0xFF, 0xFF),
                            &imgSmallCover,
                            &dev))
                {
                    return "";
                }
                DK_FLOWRENDEROPTION renderOption;
                renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
                renderOption.pDevice = &dev;
                DKR_RenderImageStream(renderOption,
                        DK_BOX(0, 0, smallWidth, smallHeight),
                        DK_BOX(0, 0, width, height),
                        1,
                        imgStream.get());
                IBMPProcessor* grayProcessor = BMPProcessFactory::CreateInstance(
                        IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
                if(NULL == grayProcessor || !grayProcessor->Process(&imgSmallCover))
                {
                    DK_FREE(imgSmallCover.pbData);
                    return "";
                }
                if (RenderUtil::SaveDKImage(imgSmallCover, result.c_str()))
                {
                    DK_FREE(imgSmallCover.pbData);
                    return result;
                }
                DK_FREE(imgSmallCover.pbData);
            }
        }
    }

    return "";
}

void* BookCoverLoader::ThreadFunc(void* arg)
{
    BookCoverLoader* pThis = (BookCoverLoader*)arg;
    std::string book;
    while (pThis->m_toLoadBooks.Receive(&book))
    {
        std::string cover = LoadCoverBmp(book.c_str(), pThis->m_minCoverWidth, pThis->m_minCoverHeight);
        if (!cover.empty())
        {
            pThis->FireBookCoverLoadedEvent(book, cover);
        }
    }
    return NULL;
}

bool BookCoverLoader::IsValidCoverImage(const char* imageFile)
{
    return IsValidImage(imageFile, m_minCoverWidth, m_minCoverHeight);
}

void BookCoverLoader::Start()
{
    static bool s_init = false;
    if (s_init)
    {
        return;
    }
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, ThreadFunc, (void*)GetInstance());
    pthread_attr_destroy(&attr);
    s_init = true;
}
