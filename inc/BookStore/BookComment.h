#ifndef __BOOKSTORE_BOOKCOMMENT_H__
#define __BOOKSTORE_BOOKCOMMENT_H__

#include "BookStore/BasicObject.h"
#include "BookStore/CommentReply.h"

namespace dk
{
namespace bookstore
{
namespace model
{

class BookComment;
typedef std::tr1::shared_ptr<BookComment> BookCommentSPtr;
typedef std::vector<BookCommentSPtr> BookCommentList;

/*
 *

 http://172.27.11.77:14000/get_book_comments?device_id=D001&user_id=asdf&start_index=1&count=1000&book_id=ffd11b9aa2fc11e1904e00163e005692
{"count": 3, "total": 3, "result": 0, 
"comments": [
{"publish_time": "2012-08-09", "useful": 0, "score": 8, "title": "wierqwerqwerklqweqwe", "nick_name": "sdsadfas", "comment_id": "502329154f4ea029d1000002", "content": "saaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "reply_count": 4, "useless": 0, "device_type": "ios-iTouch", 
"reply": 
[
{"nick_name": "sssss", "content": "ssssssssssssssssssssssssssssssssssssss", "publish_time": "2012-08-09", "reply_id": "50232cda4f4ea029d100000b", "device_type": "Kindle"},
{"nick_name": "sssssssssssss", "content": "ttttttttttttttttttttttttttttttttttttttttttt", "publish_time": "2012-08-09", "reply_id": "502329634f4ea029d1000008", "device_type": "Kindle"},
{"nick_name": "22222222", "content": "2222222222222222222", "publish_time": "2012-08-09", "reply_id": "5023293f4f4ea029d1000006", "device_type": "Kindle"}, 
{"nick_name": "1111", "content": "11111111111111111", "publish_time": "2012-08-09", "reply_id": "502329384f4ea029d1000005", "device_type": "Kindle"}
], "total": 3},
{"publish_time": "2012-08-09", "useful": 0, "title": "wierqwerqwerklqweqwe", "nick_name": "sdsadfas", "comment_id": "502329124f4ea029d1000001", "content": "saaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "score": 8, "useless": 0, "device_type": "ios-iTouch", "reply": []}, 
{"publish_time": "2012-08-09", "useful": 0, "title": "wierqwerqwerklqweqwe", "nick_name": "sdsadfas", "comment_id": "5023290e4f4ea029d1000000", "content": "saaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "score": 8, "useless": 0, "device_type": "ios-iTouch", "reply": []}], "more": false}

{"count": 7, "total": 7, "result": 0, "comments": 
[
{
"publish_time": "2012-07-30",
"useful": 0,
"title": "\u8fd9\u771f\u662f\u4e00\u672c\u597d\u4e66\u554a\u771f\u7684\u4e48",
"nick_name": "cool",
"content": "\u4e00\u70b9\u90fd\u4e0d\u8272\u60c5",
"score": 4,
"useless": 0,
"device_type": "ios-iTouch", 
"reply": {
"50168f364f4ea0282e000024": 
{
"nick_name": "cool",
"content": "ok ,
hello world",
"publish_time": "2012-07-30",
"device_type": "Kindle"},
"50167f1e4f4ea0282e000021": {"nick_name": "cool", "content": "\u8fc7\u6ee4\u4e0d\u5728\u6c5f\u5de5\u8fc7\u53bb\u4e2d\u4e2d\u4e2d\u4e2d\u6587", "publish_time": "2012-07-30", "device_type": "Kindle"}
},
"total": 2, "id": "50167e894f4ea0282e000020"},
{"publish_time": "2012-07-27", "useful": 0, "title": "asdfassssssssssssss", "nick_name": "asdfasdf", "content": "asfdasfdasdfasdf", "score": 5, "useless": 0, "device_type": "ios-iTouch", "reply": {}, "id": "501196e24f4ea023c4000000"}, {"publish_time": "2012-07-27", "useful": 1, "title": "asfdasfdasdfasdfasdf", "nick_name": "asdfasdfssss", "content": "asdfasdf", "score": 4, "useless": 1, "device_type": "ios-iTouch", "reply": {}, "id": "501196a14f4ea022ce000007"}, {"publish_time": "2012-07-27", "useful": 0, "title": "asfdasfdasdfasdfasdf", "nick_name": "asdfasdfssss", "content": "asdfasdf", "score": 4, "useless": 0, "device_type": "ios-iTouch", "reply": {}, "id": "5011969f4f4ea022ce000006"}, {"publish_time": "2012-07-27", "useful": 0, "title": "asfdasfdasdfasdfasdf", "nick_name": "asdfasdfssss", "content": "asdfasdf", "score": 4, "useless": 0, "device_type": "ios-iTouch", "reply": {}, "id": "5011969d4f4ea022ce000005"}, {"publish_time": "2012-07-27", "useful": 0, "title": "asfdasfdasdfasdfasdf", "nick_name": "asdfasdfssss", "content": "asdfasdf", "score": 4, "useless": 0, "device_type": "ios-iTouch", "reply": {}, "id": "5011969c4f4ea022ce000004"}, {"publish_time": "2012-07-27", "useful": 0, "title": "asfdasfdasdfasdfasdf", "nick_name": "asdfasdf", "content": "asdfasdf", "score": 4, "useless": 0, "device_type": "ios-iTouch", "reply": {}, "id": "501196984f4ea022ce000003"}], "more": false}
 http://172.27.11.77:14000/add_book_comment?
 book_id=ffd11b9aa2fc11e1904e00163e005692&
 content=%E7%9C%9F%E7%9A%84%E4%B8%8D%E9%AA%97%E4%BD%A0%EF%BC%8C%E6%88%91%E6%B2%A1%E7%9C%8B%E8%BF%87%E7%9E%8E%E7%8C%9C%E7%9A%84%2Caaaaaa&
 device_id=D001&
 score=4&
 title=%E6%8D%AE%E8%AF%B4%E4%B8%8D%E9%94%99%E7%9A%84
 &user_id=aaa
 */
class BookComment: public BasicObject
{
private:
    BookComment();
public:
    static const int MAX_STARS = 5;
    static BookComment* CreateBookComment(const JsonObject* jsonObject, ObjectType objectType);
    static BookCommentList FromBasicObjectList(const BasicObjectList& basicObjectList);
    virtual ObjectType GetObjectType() const
    {
        return m_objectType;
    }
    virtual BasicObject* Clone() const
    {
        return new BookComment(*this);
    }

    virtual ~BookComment(){};

    std::string GetCommentId() const
    {
        return m_commentId;
    }

    std::string GetPublishTime() const
    {
        return m_publishTime;
    }

    int GetUsefulCount() const
    {
        return m_usefulCount;
    }

    int GetUselessCount() const
    {
        return m_uselessCount;
    }

    int GetPublishUTCTime() const
    {
        return m_publishUTCTime;
    }

    std::string GetContent() const
    {
        return m_content;
    }

    std::string GetTitle() const
    {
        return m_title;
    }

    int GetReplyCount() const
    {
        return m_replyCount;
    }

    // 该评论用户给这本书打的分，2,4,6,8,10,对应1到5星
    int GetScore() const
    {
        return m_score;
    }

    std::string GetCommenterNick() const
    {
        return m_commenterNick;
    }
    std::string GetDeviceType() const
    {
        return m_deviceType;
    }
    std::string GetBookId() const
    {
        return m_bookId;
    }
    std::string GetBookName() const
    {
        return m_bookName;
    }
    std::string GetBookCoverUrl() const
    {
        return m_bookCoverUrl;
    }

    void SetBookId(const char* bookId)
    {
        m_bookId = bookId;
    }

    const CommentReplyList& GetReplyList() const
    {
        return m_replyList;
    }

private:
    bool Init(const JsonObject* jsonObj, ObjectType objectType);
    //bookId and bookName, bookcover是MyComments所有的
    std::string m_bookId;
    std::string m_bookName;
    std::string m_commenterNick;
    std::string m_title;
    std::string m_publishTime;
    std::string m_commentId;
    std::string m_content;
    std::string m_bookCoverUrl;
    int m_usefulCount;
    int m_uselessCount;
    std::string m_deviceType;
    int m_score;
    int m_replyCount;
    int m_publishUTCTime;
    CommentReplyList m_replyList;
    ObjectType m_objectType;
};

} // namespace model
} // namespace bookstore
} // namespace dk
#endif
