#ifndef __BOOKSTORE_BOOKSCORE_H__
#define __BOOKSTORE_BOOKSCORE_H__

#include "BookStore/BasicObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{

class BookScore;
typedef std::tr1::shared_ptr<BookScore> BookScoreSPtr;

/*
 *
 http://172.27.11.77:14000/get_book_summary?device_id=D001&user_id=asdfasfda&book_id=ffd11b9aa2fc11e1904e00163e005692
 {"five_stars": 2, "four_stars": 2, "score": 9.0, "result": 0, "three_stars": 0, "two_stars": 0, "total": 7, "one_star": 0}
http://172.27.11.77:14000/add_book_score
book_id=ffd11b9aa2fc11e1904e00163e005692&device_id=D004&score=5&user_id=aaa
{"result": 0}

 *
 */

class BookScore: public BasicObject
{
private:
    BookScore();
public:
    static const int MAX_STARS = 5;
    static BookScore* CreateBookScore(const JsonObject* jsonObject);
    virtual ObjectType GetObjectType() const
    {
        return OT_BOOK_SCORE;
    }
    virtual BasicObject* Clone() const
    {
        return new BookScore(*this);
    }
    virtual ~BookScore(){};
    // star level from 1 to 5
    int GetStarCount(int starLevel) const
    {
        if (starLevel > MAX_STARS || starLevel <= 0)
        {
            return -1;
        }
        return m_stars[starLevel - 1];
    }

    // from 0 to 10
    double GetScore() const
    {
        return m_score;
    }

    int GetTotalStarCount() const
    {
        int sum = 0;
        for (int starLevel = 1; starLevel <= MAX_STARS; ++starLevel)
        {
            sum += GetStarCount(starLevel);
        }
        return sum;
    }
    int GetCommentCount() const
    {
        return m_commentCount;
    }

private:
    bool Init(const JsonObject* jsonObj);
    double m_score;
    int m_stars[MAX_STARS];
    int m_commentCount;
};

} // namespace model
} // namespace bookstore
} // namespace dk
#endif
