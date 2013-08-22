#ifndef __UTILITY_READERHELPER_H__
#define __UTILITY_READERHELPER_H__

#include "BookStore/ReadingBookInfo.h"
#include "Common/File_DK.h"

class DKXBlock;
class ICT_ReadingDataItem;
using dk::bookstore::sync::ReadingBookInfo;
namespace dk
{
namespace utility
{
class ReaderUtil 
{
public:
    static bool GetAllBookmarksForUpgrade(DKXBlock* curDKXBlock, vector<ICT_ReadingDataItem*>* vBookmarks, DkFileFormat format);
    static bool GenerateReadingBookInfo(const char* bookId, ReadingBookInfo* readingDataInfo);

};//ReaderUtil
}//utility
}//dk
#endif//__UTILITY_READERHELPER_H__
