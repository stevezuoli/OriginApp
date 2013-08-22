#ifndef __BOOKSTORE_BOOKSTORETYPES_H__
#define __BOOKSTORE_BOOKSTORETYPES_H__
namespace dk
{
namespace bookstore
{

enum FetchDataResult
{
    FDR_PENDING, // send network request 
    FDR_SUCCESSED, // data in local
    FDR_FAILED,
    FDR_EMPTY
};

enum FetchDataOrder
{
    FDO_DEFAULT, // send network request 
    FDO_LATEST, // data in local
};

enum BookStatus
{
    BS_UNKNOWN = 0,
    BS_UNPURCHASED = 0x001,
    BS_UNDOWNLOAD = 0x002,
    BS_DOWNLOADED = 0x004,
    BS_DOWNLOADING = 0x008,
    BS_NEEDSUPDATE = 0x010,
    BS_FREE = 0x020,
    BS_WAITING = 0x040,
};

enum TrialBookStatus
{
    TBS_UNKNOWN = 0,
    TBS_PURCHASED = 0x001,
    TBS_UNDOWNLOAD = 0x002,
    TBS_DOWNLOADED = 0x004,
    TBS_DOWNLOADING = 0x008,
    TBS_WAITING = 0x040,
};

enum ChartsRankType
{
    CRT_PAY,
    CRT_FREE,
    CRT_MONTHLY,
    CRT_FAVOURITE,
    CRT_DOUBAN,
    CRT_AMAZON,
    CRT_JINGDONG,
    CRT_DANGDANG,
    CRT_COUNTS
};

enum SpecialEventType
{
    SPECIAL_EVENT_INVALID = -1,
    SPECIAL_EVENT_ALL = 0
};

#define CART_SIZE_LIMIT 50
#define FAVOURITES_SIZE_LIMIT 200

} // namespace bookstore
} // namespace dk
#endif


