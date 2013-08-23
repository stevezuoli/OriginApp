#ifndef __BOOKSTORE_BASICBOOKINFO_H__
#define __BOOKSTORE_BASICBOOKINFO_H__

#include "Utility/JsonObject.h"
#include "BookStore/BasicObject.h"
#include "BookStore/CatalogueInfo.h"
#include <time.h>
#include <vector>
#include <map>

namespace dk
{
namespace bookstore
{
    class OfflineBookStoreManager;
};
};

namespace dk
{
namespace bookstore
{

class OfflineBookStoreManager;

namespace model
{

class BookInfo;

typedef std::tr1::shared_ptr<BookInfo> BookInfoSPtr;
typedef std::vector<BookInfoSPtr> BookInfoList;
typedef std::map<std::string, BookInfoSPtr> BookInfoMap;

class BookInfoKeywordMatcher
{
public:
    BookInfoKeywordMatcher(const char* keyword);
    virtual ~BookInfoKeywordMatcher(){};
    virtual bool operator()(const BookInfoSPtr bookInfo) const;
private:
    const char* m_keyword;
    std::wstring  m_wKeyWord;
};

class BookCategory
{
public:
    BookCategory() {}
    virtual ~BookCategory() {}
    bool Init(JsonObjectSPtr jsonObject);

public:
    std::string id_;
    std::string label_;
    int         sid_;
};
typedef std::map<std::string, BookCategory> BookCategories;

class BookInfo: public BasicObject
{
private:
    BookInfo();

public:
    virtual ObjectType GetObjectType() const
    {
        return m_subType;
    }

    static BookInfo* CreateBookInfo(const JsonObject* jsonObject, ObjectType subType = OT_BOOK);
    static std::string BookIdToTitle(const char* bookId);

    static BookInfoList FromBasicObjectList(const BasicObjectList& basicObjectList);
    static BookInfoSPtr ParseBookInfo(const char* jsonString);

    virtual BasicObject* Clone() const {
        return new BookInfo(*this);
    }

    virtual ~BookInfo(){};

    void AddRelatedBook(BookInfoSPtr bookInfo) { m_relatedBooks.push_back(bookInfo); }
    void SetSubType(ObjectType subType) { m_subType = subType; }

    std::string GetBookId() const { return m_bookId; }
    std::string GetAuthors() const { return m_authors; }
    std::string GetEditors() const { return m_editors; }
    std::string GetTranslators() const { return m_translators; }
    std::string GetIdentifier() const { return m_identifier; }
    std::string GetAuthorsOrEditors() const
    {
        if (!m_authors.empty())
            return m_authors;
        else
            return m_editors;
    }

    std::string GetTitle() const { return m_title; }
    std::string GetSummary() const { return m_summary; }

    // price in cent
    int GetPrice() const { return m_price; }

    // price in discount, <0 means not existing
    int GetDiscountPrice() const { return m_discountPrice; }
    int GetPaperPrice() const { return m_paperPrice; }
    std::string GetCoverUrl() const { return m_coverUrl; }
    bool HasExpireTime() const { return m_hasExpireTime; }
    bool HasChangeLog() const { return m_hasChangeLog; }
    struct tm GetExpireTime() const { return m_expireTime; }

    int GetWeight() const { return m_weight; }
    std::string GetFileUrl() const { return m_fileUrl; }
    std::string& MutableFileUrl() { return m_fileUrl; }

    std::string GetTrialUrl() const { return m_trialUrl; }
    std::string GetCopyRights() const { return m_rights; }
    std::string GetRevision() const { return m_revision; }
    std::string& MutableRevision() { return m_revision; }

    std::string GetIap() const { return m_iap; }
    std::string GetContent() const { return m_content; }
    std::string GetLowestKernelVersion() const { return m_lowestKernelVersion; }
    const BookInfoList& GetRelatedBooks() const { return m_relatedBooks; }

    //CatalogueInfoList GetCatalogueList()
    const CatalogueInfoList& GetCatalogueList() const { return m_catalogueLst; }
    std::string GetUpdateTime() const { return m_updateTime; }
    std::string GetIssuedTime() const { return m_issuedTime; }
    int GetSize() const { return m_size; }
    int GetWordCount() const { return m_wordCount; }

    std::vector<std::string> GetTagList() const { return m_tagList; }
    bool IsDrm() const { return m_isDrm != 0; }
    std::string GetOrderId() const { return m_orderId; }
    std::string& MutableOrderId() { return m_orderId; }

	int GetPurchasedUTCTime() const{ return m_PurchasedUTCtime; }
    double GetScore() const { return m_scoring; }
    int GetScoreCount() const { return m_scoreCount; }

    const BookCategories& GetBookCategories() const { return m_bookCategories; }
    bool IsReadable() const;

private:
    bool Init(const JsonObject* jsonObj);

private:
    std::string m_issuedTime;
    std::string m_updateTime;
    ObjectType m_subType;
    int m_weight;
    int m_price;
    int m_discountPrice;
    int m_paperPrice;
    int m_size;
    int m_wordCount;
    std::string m_rights;
    std::string m_title;
    std::string m_coverUrl;
    std::string m_content;
    std::string m_trialUrl;
    std::string m_bookId;
    std::string m_iap;

    std::string m_fileUrl;
    std::string m_authors;
    std::string m_editors;
    std::string m_translators;
    std::string m_identifier;
    std::string m_summary;
    std::string m_revision;
    std::string m_lowestKernelVersion;
    std::string m_afs;
    struct tm m_expireTime;
    bool m_hasExpireTime;
    bool m_hasChangeLog;
    double m_scoring;
    int m_scoreCount;

    BookInfoList m_relatedBooks;
    CatalogueInfoList m_catalogueLst;
    BookCategories m_bookCategories;

    std::string m_orderId;
    int m_isDrm;
	int m_PurchasedUTCtime;
    std::vector<std::string> m_tagList;
    std::vector<std::string> m_platforms;

private:
    friend class dk::bookstore::OfflineBookStoreManager;
}; // class BookInfo


} // namespace model
} // namespace bookstore
} // namespace dk
#endif

