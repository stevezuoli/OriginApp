#include "BookStore/BookInfo.h"
#include "Common/LockObject.h"
#include "Utility/StringUtil.h"
#include "Utility/EncodeUtil.h"
#include "KernelDef.h"
#include <map>

using namespace std;
using namespace dk::common;
using namespace dk::utility;

namespace dk
{

namespace bookstore
{

namespace model
{

bool BookCategory::Init(JsonObjectSPtr jsonObject)
{
    if (jsonObject == 0)
    {
        return false;
    }

    jsonObject->GetStringValue("category_id", &id_);
    jsonObject->GetStringValue("label", &label_);
    jsonObject->GetIntValue("sid", &sid_);
    return true;
}

BookInfo::BookInfo()
    : m_subType(OT_BOOK)
    , m_weight(0)
    , m_price(0)
    , m_discountPrice(-1)
    , m_paperPrice(0)
    , m_size(0)
    , m_wordCount(0)
    , m_hasExpireTime(false)
    , m_hasChangeLog(false)
    , m_scoring(-1)
    , m_scoreCount(-1)
    , m_isDrm(1)
    , m_PurchasedUTCtime(0)
{
}

model::BookInfoSPtr BookInfo::ParseBookInfo(const char* jsonString)
{
    JsonObjectSPtr jsonObj = JsonObject::CreateFromString(jsonString);
    if (!jsonObj)
    {
        return model::BookInfoSPtr();
    }
    int result;
    if (!jsonObj->GetIntValue("result", &result) || result != 0) 
    {
        return model::BookInfoSPtr();
    }

    JsonObjectSPtr bookObj = jsonObj->GetSubObject("book");
    if (bookObj)
    {
        model::BookInfoSPtr bookInfo(model::BookInfo::CreateBookInfo(bookObj.get()));
        return bookInfo;
    }
    else
    {
        return model::BookInfoSPtr();
    }
}

std::vector<std::string> ParseStringList(const JsonObject* jsonObj,
                                         const std::string& key_word,
                                         const std::string& seperator)
{
    std::vector<std::string> results;
    std::string raw_string;
    if (jsonObj->GetStringValue(key_word.c_str(), &raw_string) && !raw_string.empty())
    {
        results = StringUtil::Split(raw_string.c_str(), seperator.c_str());
    }
    return results;
}

bool BookInfo::Init(const JsonObject* jsonObj)
{
    m_platforms = ParseStringList(jsonObj, "platforms", "\n");
	jsonObj->GetIntValue("time", &m_PurchasedUTCtime);
    jsonObj->GetStringValue("updated", &m_updateTime);
    jsonObj->GetStringValue("issued", &m_issuedTime);
    jsonObj->GetIntValue("weight", &m_weight);
    double tempPrice;
    if (jsonObj->GetDoubleValue("price", &tempPrice))
    {
        m_price = (int)(tempPrice * 100);
    }
    if (jsonObj->GetDoubleValue("new_price", &tempPrice))
    {
        m_discountPrice = (int)(tempPrice * 100);
    }
    if (jsonObj->GetDoubleValue("paper_price", &tempPrice))
    {
        m_paperPrice = (int)(tempPrice * 100);
    }

    jsonObj->GetStringValue("summary", &m_summary);
    jsonObj->GetStringValue("authors", &m_authors);
    jsonObj->GetStringValue("editors", &m_editors);
    jsonObj->GetStringValue("translators", &m_translators);
    jsonObj->GetStringValue("identifier", &m_identifier);
    jsonObj->GetStringValue("author", &m_authors);
    char enter1 = '\r';
    char enter2 = '\n';
    string replaceString = " ";
    m_authors = StringUtil::ReplaceString(m_authors, enter1, replaceString.c_str());
    m_authors = StringUtil::ReplaceString(m_authors, enter2, replaceString.c_str());
    m_editors = StringUtil::ReplaceString(m_editors, enter1, replaceString.c_str());
    m_editors = StringUtil::ReplaceString(m_editors, enter2, replaceString.c_str());
    m_translators = StringUtil::ReplaceString(m_translators, enter1, replaceString.c_str());
    m_translators = StringUtil::ReplaceString(m_translators, enter2, replaceString.c_str());

    jsonObj->GetStringValue("epub", &m_fileUrl);
    jsonObj->GetStringValue("bookurl", &m_fileUrl);
    jsonObj->GetStringValue("rights", &m_rights);
    jsonObj->GetStringValue("title", &m_title);
    jsonObj->GetStringValue("cover", &m_coverUrl);
    jsonObj->GetStringValue("content", &m_content);
    jsonObj->GetStringValue("trial", &m_trialUrl);
    jsonObj->GetStringValue("book_id", &m_bookId);
    jsonObj->GetStringValue("iap", &m_iap);
    jsonObj->GetStringValue("revision", &m_revision);
    jsonObj->GetIntValue("epub_size", &m_size);
    jsonObj->GetIntValue("word_count", &m_wordCount);
    jsonObj->GetStringValue("order_id", &m_orderId);
    jsonObj->GetIntValue("is_drm", &m_isDrm);
    jsonObj->GetDoubleValue("score", &m_scoring);
    jsonObj->GetIntValue("score_count", &m_scoreCount);
    jsonObj->GetBooleanValue("has_change_log", &m_hasChangeLog);
    jsonObj->GetStringValue("kernel", &m_lowestKernelVersion);
    jsonObj->GetStringValue("afs", &m_afs);

    double expireTime;
    if (jsonObj->GetDoubleValue("expire", &expireTime))
    {
        time_t tm = (time_t)expireTime;
        if (NULL != localtime_r(&tm, &m_expireTime))
        {
            m_hasExpireTime = true;
        }
    }

    m_tagList = ParseStringList(jsonObj, "tags", "\n");

    std::string catalogue;
    if (jsonObj->GetStringValue("toc", &catalogue) && !catalogue.empty())
    {
        string newCata = catalogue;
        string para("\n");
        string space(" ");
        char preview('-');
        size_t startIndex = 0;
        while(true)
        {
            size_t found = catalogue.find(para.c_str(), startIndex);
            if (string::npos == found)
            {
                break;
            }
            string para = catalogue.substr(startIndex, found - startIndex);
            size_t spaceIndex = para.find(space.c_str(), 0);
            if (string::npos != spaceIndex)
            {
                string view = para.substr(spaceIndex + 1, para.length() - spaceIndex);
                if (!view.empty())
                {
                    if (para[spaceIndex - 1] == preview)
                    {
                        const int endIndex = m_catalogueLst.size() - 1;
                        m_catalogueLst[endIndex]->SetPreview(view);
                    }
                    else
                    {
                        CatalogueInfoSPtr catalogueInfo(CatalogueInfo::CreateCatalogueInfo(view.c_str(), spaceIndex));
                        if (catalogueInfo)
                        {
                            m_catalogueLst.push_back(catalogueInfo);
                        }
                    }
                }
            }
            startIndex = ++found;
        }
    }

    JsonObjectSPtr categories = jsonObj->GetSubObject("categories");
    if (categories != 0)
    {
        int cat_size = categories->GetArrayLength();
        for (int i = 0; i < cat_size; ++i)
        {
            JsonObjectSPtr book_category = categories->GetElementByIndex(i);
            if (book_category != 0)
            {
                BookCategory book_cat_instance;
                if (book_cat_instance.Init(book_category))
                {
                    m_bookCategories[book_cat_instance.id_] = book_cat_instance;
                }
            }
        }
    }

    JsonObjectSPtr related = jsonObj->GetSubObject("related");
    if (related)
    {
        int relatedSize = related->GetArrayLength();
        for (int i = 0; i < relatedSize; ++i)
        {
            JsonObjectSPtr relatedBookJson = related->GetElementByIndex(i);
            if (relatedBookJson)
            {
                model::BookInfo* relatedBook = CreateBookInfo(relatedBookJson.get());
                if (NULL != relatedBook)
                {
                    AddRelatedBook(model::BookInfoSPtr(relatedBook));
                }
            }
        }
    }
    return true;
}

static std::map<std::string, std::string> s_bookIdToTitle;
static LockObject s_lock;

std::string BookInfo::BookIdToTitle(const char* bookId)
{
    LockScope lockScope(s_lock);
    DK_AUTO(pos, s_bookIdToTitle.find(bookId));
    return pos != s_bookIdToTitle.end() ? pos->second : "";
}

BookInfo* BookInfo::CreateBookInfo(const JsonObject* jsonObject, ObjectType subType)
{
    BookInfo* bookInfo = new BookInfo();
    if (NULL != bookInfo && bookInfo->Init(jsonObject))
    {
        bookInfo->m_subType = subType;
        LockScope lockScope(s_lock);
        s_bookIdToTitle[bookInfo->GetBookId()] = bookInfo->GetTitle();
        return bookInfo;
    }
    if (bookInfo)
    {
        delete bookInfo;
    }
    return NULL;
}

bool BookInfo::IsReadable() const
{
    for (int i = 0; i < m_platforms.size(); i++)
    {
        if (m_platforms[i].compare("Kindle") == 0)
        {
            return true;
        }
    }
    return false;
}

BookInfoList BookInfo::FromBasicObjectList(const BasicObjectList& basicObjectList)
{
    BookInfoList result;
    for (size_t i = 0; i < basicObjectList.size(); ++i)
    {
        if (basicObjectList[i] &&
			(basicObjectList[i]->GetObjectType() == OT_BOOK || 
			basicObjectList[i]->GetObjectType() == OT_BOOK_IN_CART))
        {
            result.push_back(BookInfoSPtr((BookInfo*)(basicObjectList[i]->Clone())));
        }
    }
    return result;
}

BookInfoKeywordMatcher::BookInfoKeywordMatcher(const char* keyword)
        : m_keyword(keyword)
{
    if (NULL != keyword)
    {
        m_wKeyWord = StringUtil::ToLower(EncodeUtil::ToWString(keyword).c_str());
        while (('a' <= *keyword && *keyword <= 'z')
            || ('A' <= *keyword && *keyword <= 'Z'))
        {
            ++keyword;
        }
    }
}

bool BookInfoKeywordMatcher::operator()(const BookInfoSPtr bookInfo) const
{
    if (m_wKeyWord.empty() && bookInfo.get())
    {
        return false;
    }
    std::wstring bookName = StringUtil::ToLower(EncodeUtil::ToWString(bookInfo->GetTitle()).c_str());
    if (bookName.find(m_wKeyWord) != std::wstring::npos)
    {
        return true;
    }
	std::wstring bookAuthor = StringUtil::ToLower(EncodeUtil::ToWString(bookInfo->GetAuthors()).c_str());
	if (bookAuthor.find(m_wKeyWord) != std::wstring::npos)
    {
        return true;
    }
	if(StringUtil::MatchByFirstLetter(bookName, m_wKeyWord))
	{
		return true;
	}
    return false;
}

} // namespace model
} // namespace bookstore
} // namespace dk
