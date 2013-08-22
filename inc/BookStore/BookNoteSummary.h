#ifndef __BOOKREADER_NOTESUMMARY_H
#define __BOOKREADER_NOTESUMMARY_H

#include <string>
#include <vector>
#include "BookStore/BasicObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{
class BookNoteSummary;
typedef std::tr1::shared_ptr<BookNoteSummary> BookNoteSummarySPtr;
typedef std::vector<BookNoteSummarySPtr> BookNoteSummarySPtrList;

class BookNoteSummary : public BasicObject
{
public:
    static BookNoteSummary* CreateBookNoteSummary(const JsonObject* jsonObject);
    static BookNoteSummarySPtrList FromBasicObjectList(const BasicObjectList& basicObjectList);
    static bool BookNoteCompare(const BasicObjectSPtr lhs, const BasicObjectSPtr rhs);

    virtual BasicObject* Clone() const
    {
        return new BookNoteSummary(*this);
    }

    virtual ObjectType GetObjectType() const
    {
        return OT_BOOKNOTESUMMARY;
    }

    BookNoteSummary()
        : m_commentCount(-1)
        , m_version(-1)
        , m_bookMarkCount(-1)
        , m_isDKStoreBook(false)
    {}

    std::string GetBookId() const
    {
        return m_bookId;
    }

    void SetBookId(const std::string& bookId)
    {
        m_bookId = bookId;
    }

    void SetDuoKanBook(bool isDKStoreBook)
    {
        m_isDKStoreBook = isDKStoreBook;
    }

    bool IsDuoKanBook() const
    {
        return m_isDKStoreBook;
    }
    int GetTotalCount() const
    {
        return m_commentCount + m_bookMarkCount;
    }
    int GetCommentCount() const
    {
        return m_commentCount;
    }
    std::string GetLastUpdateTime() const
    {
        return m_lastUpdateTime;
    }

    std::string GetBookTitle() const
    {
        return m_bookTitle;
    }

    std::string GetBookCoverUrl()
    {
        return m_bookCoverUrl;
    }

    std::string GetBookAuthor() const
    {
        if (m_bookAuthor.empty())
        {
            return m_bookEditor;
        }
        return m_bookAuthor;
    }

private:
    bool Init(const JsonObject* jsonObj);
    int m_commentCount;
    int m_version;
    int m_bookMarkCount;
    bool m_isDKStoreBook;
    std::string m_bookId;
    std::string m_lastUpdateTime;
    std::string m_bookTitle;
    std::string m_bookCoverUrl;
    std::string m_bookAuthor;
    std::string m_bookEditor;
};//BookNoteSummary
}
}
}
#endif//__BOOKREADER_NOTESUMMARY_H
