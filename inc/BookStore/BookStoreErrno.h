#ifndef __BOOKSTORE_BOOKSTOREERRNO_H__
#define __BOOKSTORE_BOOKSTOREERRNO_H__

namespace dk
{
namespace bookstore
{


enum BookStoreErrno
{
    BSE_UNKNOWN = -1,
    BSE_OK = 0,

    // Login Error
    BSE_DUOKAN_ACCOUNT_REGISTER_CLOSED = 64,
    BSE_DUOKAN_ACCOUNT_MIGRATED = 65,
    BSE_DUOKAN_ACCOUNT_LOGIN_CLOSED = 74,
    BSE_LOGIN_EXPIRED = 1001,
    BSE_LOGIN_NO_LOGIN = 1002,
    BSE_LOGIN_RE_LOGIN = 1003,

    // Purchase
    BSE_BOOK_PURCHASED = 10008,
    BSE_COMMENT_NO_COMMENT = 40002,
    BSE_COMMENT_REPEATED_VOTE = 40007,
    REDEEM_TOO_LONG_WORDS = 50001,
    REDEEM_WRONG_DEST_EMAIL = 50002,
    REDEEM_ILLIGEL_CODE = 50003,
    REDEEM_INVALID_CODE = 50004,
    REDEEM_CAN_NOT_USE_FREE_BOOK = 50005,
    REDEEM_BOOK_PURCHASED = 50006,
    REDEEM_BOOK_NOT_EXIST = 50007,

    // Cart and Favourites
    CART_IS_FULL = 100001,
    FAVOURITES_IS_FULL = 100001,


};

inline bool IsLoginError(int errorCode)
{
    switch (errorCode)
    {
        case BSE_LOGIN_EXPIRED:
        case BSE_LOGIN_NO_LOGIN:
        case BSE_LOGIN_RE_LOGIN:
            return true;
        default:
            return false;
    };
}

} // namespace bookstore
} // namespace dk
#endif
