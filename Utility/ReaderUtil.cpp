#include "Utility/ReaderUtil.h"
#include "DKXManager/DKX/DKXBlock.h"
#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"
#include "Common/FileManager_DK.h"
#include "KernelVersion.h"

namespace dk
{
namespace utility
{
bool ReaderUtil::GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem*>* vBookmarks, DkFileFormat format)
{
    switch (format)
    {
    case DFF_Text:
    case DFF_MobiPocket:
        if(curDKXBlock && vBookmarks)
        {
            if(curDKXBlock->GetAllBookmarks(vBookmarks))
            {
                for(size_t i = 0; i < vBookmarks->size(); i++)
                {
                    ICT_ReadingDataItem* readingData = vBookmarks->at(i);
                    CT_RefPos beginPos = readingData->GetBeginPos();
                    beginPos.SetChapterIndex(1);
                    CT_RefPos endPos = readingData->GetEndPos();
                    endPos.SetChapterIndex(1);
                    readingData->SetBeginPos(beginPos);
                    readingData->SetEndPos(endPos);
                }
                return true;
            }
        }
        return false;
    case DFF_ElectronicPublishing:
        if(curDKXBlock && vBookmarks)
        {
            if(curDKXBlock->GetAllBookmarks(vBookmarks))
            {
                return true;
            }
        }
        return false;
    case DFF_PortableDocumentFormat:
        if(curDKXBlock && vBookmarks)
        {
            if(curDKXBlock->GetAllBookmarks(vBookmarks))
            {
                for(size_t i = 0; i < vBookmarks->size(); i++)
                {
                    ICT_ReadingDataItem* readingData = vBookmarks->at(i);
                    CT_RefPos beginPos = readingData->GetBeginPos();
                    beginPos.SetChapterIndex(beginPos.GetChapterIndex() + 1);
                    CT_RefPos endPos = readingData->GetEndPos();
                    endPos.SetChapterIndex(endPos.GetChapterIndex() + 1);
                    readingData->SetBeginPos(beginPos);
                    readingData->SetEndPos(endPos);
                }
                return true;
            }
        }
         return false;
    default:
         return false;
    }
}

bool ReaderUtil::GenerateReadingBookInfo(const char* bookId, ReadingBookInfo* readingDataInfo)
{
    if (!bookId || !readingDataInfo)
    {
        return false;
    }

    PCDKFile fileInfo = CDKFileManager::GetFileManager()->GetFileByBookId(bookId);
    if (!fileInfo)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "ReaderUtil: Can NOT get file by BOOK ID");
        return false;
    }
    
    readingDataInfo->SetKernelVersion(DK_GetKernelVersion());
    readingDataInfo->SetVersion(ReadingBookInfo::CURRENT_VERSION);
    readingDataInfo->SetBookId(fileInfo->GetBookID());
    readingDataInfo->SetBookRevision(DKXManager::GetInstance()->GetBookRevision(fileInfo->GetFilePath()).c_str());

#ifdef KINDLE_FOR_TOUCH
        DKXBlock* dkxBlock= DKXManager::CreateDKXBlock(fileInfo->GetFilePath());
        if (NULL == dkxBlock)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "No dkxBlock or NoModified, dkxBlock: %x", dkxBlock);
            return false;
        }

        vector<ICT_ReadingDataItem *> vBookmarks;
        if(!dk::utility::ReaderUtil::GetAllBookmarksForUpgrade(dkxBlock, &vBookmarks, fileInfo->GetFileFormat()))
        {
            DebugPrintf(DLC_DIAGNOSTIC, "ReaderUtil::GetAllBookmarksForUpgrade Error!");
            return false;
        }

#else
        DKXManager* pDKXManager = DKXManager::GetInstance();
        const string filePath = fileInfo->GetFilePath();
        if (NULL == pDKXManager)
        {
            return false;
        }

        vector<ICT_ReadingDataItem *> vBookmarks;
        if(!dk::utility::ReaderUtil::GetAllBookmarksForUpgrade(filePath, &vBookmarks, fileInfo->GetFileFormat()))
        {
            DebugPrintf(DLC_DIAGNOSTIC, "ReaderUtil::GetAllBookmarksForUpgrade Error!");
            return false;
        }
#endif

        for (size_t i = 0; i < vBookmarks.size(); ++i)
        {
            ICT_ReadingDataItem* localReadingDataItem = vBookmarks[i];
            if (NULL == localReadingDataItem)
            {
                continue;
            }
            ReadingDataItem* readingDataItem = CreateReadingDataItemFromICTReadingDataItem(*localReadingDataItem);
            if (NULL != readingDataItem)
            {
                switch (readingDataItem->GetType())
                {
                    case ReadingDataItem::PROGRESS:
                        //readingProgressInfo.AddReadingDataItem(*readingDataItem);
                        break;
                    default:
                        readingDataInfo->AddReadingDataItem(*readingDataItem);
                        break;
                }
                delete readingDataItem;
            }
            DKXReadingDataItemFactory::DestoryReadingDataItem(localReadingDataItem);
        }
#ifdef KINDLE_FOR_TOUCH
        if (dkxBlock)
        {
            DKXManager::DestoryDKXBlock(dkxBlock);
        }
#endif

        return true;
}
}
}
