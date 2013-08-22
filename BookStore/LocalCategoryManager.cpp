#include "BookStore/LocalCategoryManager.h"
#include "Utility/ConfigFile.h"
#include "Utility/PathManager.h"
#include "Utility/EncodeUtil.h"
#include "Utility/StringUtil.h"
#include <string.h>
#include <algorithm>
#include "KernelDef.h"
#include "../Common/FileManager_DK.h"
#include "I18n/StringManager.h"

using namespace dk::utility;

namespace dk
{
namespace bookstore
{

class CategoryNode
{
private:
    std::string m_name; // utf8
    std::string m_gbkName;
    std::vector<std::string> m_books;
    static const char* s_seperators;

public:
    CategoryNode()
    {
    }

    CategoryNode(const char* utf8Name)
    {
        SetUtf8Name(utf8Name);
    }

    void SetUtf8Name(const char* utf8Name)
    {
        m_name = utf8Name;
        m_gbkName = EncodeUtil::UTF8ToGBKString(utf8Name);
    }

    void AddBooks(const char* bookIdString)
    {
        std::vector<std::string> bookIds = StringUtil::Split(bookIdString, s_seperators); 
        for (DK_AUTO(cur, bookIds.begin()); cur != bookIds.end(); ++cur)
        {
            if (PathManager::IsFileExisting(
                        PathManager::BookIdToEpubFile(*cur)))
            {
                AddBook(cur->c_str());
            }
        }
    }

    bool AddBook(const char* bookId)
    {
        if (std::find(m_books.begin(), m_books.end(), bookId) == m_books.end())
        {
            m_books.push_back(bookId);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool RemoveBook(const char* bookId)
    {
        DK_AUTO(pos, std::find(m_books.begin(), m_books.end(), bookId));
        if (pos != m_books.end())
        {
            m_books.erase(pos);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator<(const CategoryNode& rhs) const
    {
        return strcasecmp(m_gbkName.c_str(), rhs.m_gbkName.c_str()) < 0;
    }

    bool operator==(const char* utf8Name) const
    {
        return strcasecmp(m_name.c_str(), utf8Name) == 0;
    }

    const char* GetGbkName() const
    {
        return m_gbkName.c_str();
    }

    const char* GetUtf8Name() const
    {
        return m_name.c_str();
    }

    std::string GetBookIdsString() const
    {
        return StringUtil::Join(m_books, ",");
    }
    
    const std::vector<std::string> &GetBookIds() const
    {
        return m_books;
    }

    bool HasBookId(const char* bookId) const
    {
        if (StringUtil::IsNullOrEmpty(bookId))
        {
            return false;
        }
        return std::find(m_books.begin(), m_books.end(), bookId) != m_books.end();
    }

};

const char* CategoryNode::s_seperators = " ,\t";

static std::vector<CategoryNode> s_allCategoryNodes;
static std::vector<std::string> s_allCategories;

static void BuildCategoriesFromNodes()
{
    s_allCategories.clear();
    for (DK_AUTO(cur, s_allCategoryNodes.begin()); cur != s_allCategoryNodes.end(); ++cur)
    {
        s_allCategories.push_back(cur->GetUtf8Name());
    }
}

static void SaveCategoryInfo()
{
    ConfigFile config;
    for (DK_AUTO(cur, s_allCategoryNodes.begin()); cur != s_allCategoryNodes.end(); ++cur)
    {
        config.SetString(cur->GetUtf8Name(), cur->GetBookIdsString().c_str());
    }
    config.SaveToFile(PathManager::GetBookStoreLocalCategoryFile().c_str());
}

void LocalCategoryManager::Init()
{
    static bool s_init = false;
    if (!s_init)
    {
        s_init = true;

        ConfigFile config;
        config.LoadFromFile(PathManager::GetBookStoreLocalCategoryFile().c_str());
        std::vector<std::string> allUtf8Categories = config.GetAllKeys();
        std::string bookIds;
        for (DK_AUTO(cur, allUtf8Categories.begin()); cur != allUtf8Categories.end(); ++cur)
        {
            CategoryNode node;
            node.SetUtf8Name(cur->c_str());
            bookIds.clear();
            config.GetString(cur->c_str(), &bookIds);
            node.AddBooks(bookIds.c_str());
            s_allCategoryNodes.push_back(node);
        }
        std::sort(s_allCategoryNodes.begin(), s_allCategoryNodes.end());
        BuildCategoriesFromNodes();
    }
}

std::vector<std::string> LocalCategoryManager::GetAllCategories()
{
    return s_allCategories;
}

bool LocalCategoryManager::AddCategory(const char* category, std::string* errorMsg)
{
    if (std::find(s_allCategories.begin(), s_allCategories.end(), category) != s_allCategories.end())
    {
        *errorMsg = StringManager::GetPCSTRById(CATEGORY_EXISTING);
        return false;
    }
    CategoryNode node(category);
    s_allCategoryNodes.push_back(node);
    std::sort(s_allCategoryNodes.begin(), s_allCategoryNodes.end());
    BuildCategoriesFromNodes();
    SaveCategoryInfo();
    return true;
}

bool LocalCategoryManager::RenameCategory(const char* category, const char* newName, std::string* errorMsg)
{
    DK_AUTO(pos, std::find(s_allCategoryNodes.begin(), s_allCategoryNodes.end(), category));
    if (pos == s_allCategoryNodes.end())
    {
        return false;
    }

    // check whether the new category has existed
    if (std::find(s_allCategories.begin(), s_allCategories.end(), newName) != s_allCategories.end())
    {
        *errorMsg = StringManager::GetPCSTRById(CATEGORY_EXISTING);
        return false;
    }

    (*pos).SetUtf8Name(newName);
    std::sort(s_allCategoryNodes.begin(), s_allCategoryNodes.end());
    BuildCategoriesFromNodes();
    SaveCategoryInfo();
    return true;
}

bool LocalCategoryManager::RemoveCategory(const char* category)
{
    DK_AUTO(pos, std::find(s_allCategoryNodes.begin(), s_allCategoryNodes.end(), category));
    if (pos == s_allCategoryNodes.end())
    {
        return false;
    }
    s_allCategoryNodes.erase(pos);
    BuildCategoriesFromNodes();
    SaveCategoryInfo();
    return true;
}

bool LocalCategoryManager::AddBookToCategory(const char* category, const char* bookId)
{
    if (StringUtil::IsNullOrEmpty(category) || StringUtil::IsNullOrEmpty(bookId))
    {
        return false;
    }
    for (DK_AUTO(cur, s_allCategoryNodes.begin()); cur != s_allCategoryNodes.end(); ++cur)
    {
        if (cur->HasBookId(bookId))
        {
            return false;
        }
    }
    DK_AUTO(pos, std::find(s_allCategoryNodes.begin(), s_allCategoryNodes.end(), category));
    if (pos == s_allCategoryNodes.end())
    {
        return false;
    }
    if (!pos->AddBook(bookId))
    {
        return false;
    }
    SaveCategoryInfo();
    return true;
}

bool LocalCategoryManager::RemoveBookFromCategory(const char* category, const char* bookId)
{
    if (StringUtil::IsNullOrEmpty(category) || StringUtil::IsNullOrEmpty(bookId))
    {
        return false;
    }
    DK_AUTO(pos, std::find(s_allCategoryNodes.begin(), s_allCategoryNodes.end(), category));
    if (pos == s_allCategoryNodes.end())
    {
        return false;
    }
    if (!pos->RemoveBook(bookId))
    {
        return false;
    }
    SaveCategoryInfo();
    return true;
}

std::string LocalCategoryManager::GetCategoryOfBookId(const char* bookId)
{
    for (DK_AUTO(cur, s_allCategoryNodes.begin()); cur != s_allCategoryNodes.end(); ++cur)
    {
        if (cur->HasBookId(bookId))
        {
            return cur->GetUtf8Name();
        }
    }
    return "";
}

std::vector<std::string> LocalCategoryManager::GetBookIdsByCategory(const char* category)
{
    DK_AUTO(pos, std::find(s_allCategoryNodes.begin(), s_allCategoryNodes.end(), category));
    if (pos == s_allCategoryNodes.end())
    {
        return std::vector<std::string>();
    }
    return pos->GetBookIds();
}

size_t LocalCategoryManager::GetBookCountByCategory(const char* category)
{
    DK_AUTO(pos, std::find(s_allCategoryNodes.begin(), s_allCategoryNodes.end(), category));
    if (pos == s_allCategoryNodes.end())
    {
        return 0;
    }
    return pos->GetBookIds().size();
}

std::vector<std::string> LocalCategoryManager::GetBookIdsWithNoCategory()
{
    std::vector<std::string> bookIds = CDKFileManager::GetFileManager()->GetDuoKanBookIds();
    std::vector<std::string> bookIdWithCategory;
    for (DK_AUTO(cur, s_allCategoryNodes.begin()); cur != s_allCategoryNodes.end(); ++cur)
    {
        bookIdWithCategory.insert(bookIdWithCategory.end(), cur->GetBookIds().begin(), cur->GetBookIds().end());
    } 

    std::vector<std::string> results;
    for (DK_AUTO(cur, bookIds.begin()); cur != bookIds.end(); ++cur)
    {
        if (std::find(bookIdWithCategory.begin(), bookIdWithCategory.end(), *cur) == bookIdWithCategory.end())
        {
            results.push_back(*cur);
        }
    }
    return results;
}

bool LocalCategoryManager::RemoveBookFromCategory(const char* bookId)
{
    if (NULL == bookId)
    {
        return false;
    }
    std::string category = GetCategoryOfBookId(bookId);
    if (category.empty())
    {
        return false;
    }
    return RemoveBookFromCategory(category.c_str(), bookId);
}
}
}
