#include "BookStore/BookScore.h"
#include <string.h>
#include "Utility/JsonObject.h"


namespace dk
{
namespace bookstore
{
namespace model
{

BookScore::BookScore()
    : m_score(0)
    , m_commentCount(0)
{
    memset(m_stars, 0, sizeof(m_stars));
}

BookScore* BookScore::CreateBookScore(const JsonObject* jsonObject)
{
    BookScore* commentSummary = new BookScore();
    if (NULL != commentSummary && commentSummary->Init(jsonObject))
    {
        return commentSummary;
    }
    delete commentSummary;
    return NULL;
}

bool BookScore::Init(const JsonObject* jsonObject)
{
    if (NULL == jsonObject)
    {
        return NULL;
    }
    jsonObject->GetIntValue("one_star", &m_stars[0]);
    jsonObject->GetIntValue("two_stars", &m_stars[1]);
    jsonObject->GetIntValue("three_stars", &m_stars[2]);
    jsonObject->GetIntValue("four_stars", &m_stars[3]);
    jsonObject->GetIntValue("five_stars", &m_stars[4]);
    jsonObject->GetDoubleValue("score", &m_score);
    jsonObject->GetIntValue("total", &m_commentCount);
    return true;
}

} // namespace model
} // namespace bookstore
} // namespace dk
