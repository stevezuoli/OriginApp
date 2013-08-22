#include "BookStore/BookNoteSummary.h"
#include "Utility/JsonObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{
bool BookNoteSummary::Init(const JsonObject* jsonObject)
{
    if (!jsonObject)
    {
        return false;
    }

    jsonObject->GetIntValue("comment_count", &m_commentCount);
    jsonObject->GetIntValue("version", &m_version);
    jsonObject->GetIntValue("bookmark_count", &m_bookMarkCount);
    jsonObject->GetStringValue("lastupdatetime", &m_lastUpdateTime);
    jsonObject->GetStringValue("book_title", &m_bookTitle);
    jsonObject->GetStringValue("author", &m_bookAuthor);
    jsonObject->GetStringValue("editor", &m_bookEditor);
    jsonObject->GetStringValue("cover_url", &m_bookCoverUrl);
    return true;
}

BookNoteSummary* BookNoteSummary::CreateBookNoteSummary(const JsonObject* jsonObject)
{
    BookNoteSummary* pBookNoteSummary = new BookNoteSummary();
    if (NULL != pBookNoteSummary && pBookNoteSummary->Init(jsonObject))
    {
        return pBookNoteSummary;
    }

    SafeDeletePointer(pBookNoteSummary);
    return NULL;
}

BookNoteSummarySPtrList BookNoteSummary::FromBasicObjectList(const BasicObjectList& basicObjectList)
{
    BookNoteSummarySPtrList result;
    for (size_t i = 0; i < basicObjectList.size(); ++i)
    {
        if (basicObjectList[i] && basicObjectList[i]->GetObjectType() == OT_BOOKNOTESUMMARY)
        {
            result.push_back(BookNoteSummarySPtr((BookNoteSummary*)(basicObjectList[i]->Clone())));
        }
    }
    return result;
}

bool BookNoteSummary::BookNoteCompare(const BasicObjectSPtr lhs, const BasicObjectSPtr rhs)
{
    return ((BookNoteSummary*)lhs.get())->GetLastUpdateTime().compare(((BookNoteSummary*)rhs.get())->GetLastUpdateTime()) > 0;
}
}//model
}//bookstore
}//dk
