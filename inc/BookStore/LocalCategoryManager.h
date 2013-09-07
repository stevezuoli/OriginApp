#ifndef __BOOKSTORE_LOCALCATEGORYMANAGER_H__
#define __BOOKSTORE_LOCALCATEGORYMANAGER_H__

#include <vector>
#include <string>

namespace dk
{
namespace bookstore
{

class LocalCategoryManager
{
public:
    static void Init();

    static bool AddCategory(const char* category, std::string* errorMsg);
    static bool RemoveCategory(const char* category);
    static bool RenameCategory(const char* category, const char* newName, std::string* errorMsg);

    static bool AddBookToCategory(const char* category, const char* bookId);
    static bool RemoveBookFromCategory(const char* category, const char* bookId);

    static std::vector<std::string> GetAllCategories();
    static std::vector<std::string> GetBookIdsByCategory(const char* category);
    static std::vector<std::string> GetBookIdsByCategory(const std::string& category)
    {
        return GetBookIdsByCategory(category.c_str());
    }
    static std::vector<std::string> GetBookIdsWithNoCategory();
    static std::string GetCategoryOfBookId(const char* bookId);
    static std::string GetCategoryOfBookId(const std::string& bookId)
    {
        return GetCategoryOfBookId(bookId.c_str());
    }
    static size_t GetBookCountByCategory(const char* category);
    static size_t GetBookCountByCategory(const std::string& category)
    {
        return GetBookCountByCategory(category.c_str());
    }
    static bool RemoveBookFromCategory(const char* bookId);
    static bool RemoveBookFromCategory(const std::string& bookId)
    {
        return RemoveBookFromCategory(bookId.c_str());
    }

private:
    LocalCategoryManager();
};
};
};
#endif
