#ifndef __UTILITY_PATHMANAGER_H__
#define __UTILITY_PATHMANAGER_H__

#include <string>
#include <vector>

namespace dk
{
namespace utility
{

class PathManager
{
public:
    static bool Init();
    static std::string GetRootPath();
    static std::string GetTempPath();
    static std::string GetCheatFilePath();
    static std::string GetCoverImagePath();
    static std::string GetBookStorePath();
    static std::string GetBookStoreLocalCategoryFile();
    static std::string GetNewsPath();
    static std::string GetCommentExportPath();
    static std::string GetScreenSaverCfgPath();
    static std::string GetScreenSaverDir();
    static std::string GetLanguagePath();
    static std::string GetInteractiveImagePath();
    static std::string GetBookWaitUploadNotePath();
    static std::string GetSystemConfigPath();
    static std::string GetBookStoreCorrectionPath();
    static std::string GetBookStoreCartPath();
    static std::string GetBookStoreFavouritesPath();
    static std::string GetResourcePath();
    static std::string GetXiaomiAccountPath();
    static std::string GetEvernotePath();
    static std::string GetEvernoteInternalPath();
    static std::string GetEvernoteTokenPath();
    static std::string GetEvernoteUserPath();
	static std::string GetMigrateResultPath();
    static std::string GetMigratePromptTimePath();
	static std::string GetAccountMappingPath();
	static std::string GetReadingHistoryPath();
	static std::string GetMiCloudServiceTokenPath();
    static std::string GetDownloadFolderPath();

    static bool IsRootPath(const char* path);
    static bool IsRootPath(const std::string& path)
    {
        return IsRootPath(path.c_str());
    }
    static bool IsBookStorePath(const char* path);
    static bool IsBookStorePath(const std::string& path)
    {
        return IsBookStorePath(path.c_str());
    }
    static bool ShouldBlockPath(const char* path, const char* itemName);
    static bool ShouldBlockPath(const std::string& path, const std::string& itemName)
    {
        return ShouldBlockPath(path.c_str(), itemName.c_str());
    }

    static bool IsDocumentsPath(const char* path);
    static bool IsNewsPath(const char* path);
    static bool IsValidDirName(const char* path);

    static std::string GetFileNameWithoutExt(const char* filename);
    static std::string GetFileExt(const char* filename);

    static bool IsUnderBookStore(const char* path);
    static bool IsUnderBookStore(const std::string& path)
    {
        return IsUnderBookStore(path.c_str());
    }
    static bool IsUnderNews(const char* path);
    static bool IsUnderNews(const std::string& path)
    {
        return IsUnderNews(path.c_str());
    }

    static std::string ConcatPath(const char* dir, const char* filename);
    static std::string ConcatPath(const std::string& dir, const std::string& filename)
    {
        return ConcatPath(dir.c_str(), filename.c_str());
    }
    static std::string GetFileName(const char* src);
    static std::string GetFileName(const std::string& src)
    {
        return GetFileName(src.c_str());
    }
    static std::vector<std::string> GetFilesInPath(const char* path);
    static std::vector<std::string> GetDirsInPath(const char* path);

    static std::string GetBaseUrl(const char* url);
    static std::string NormalizeCoverImageUrl(
            const char* imageUrl,
            int dstWidth,
            int dstHeight);
    static std::string NormalizeCoverImageUrl(
            const std::string& imageUrl,
            int dstWidth,
            int dstHeight)
    {
        return NormalizeCoverImageUrl(imageUrl.c_str(), dstWidth, dstHeight);
    }

    static std::string CoverImageUrlToFile(const char* coverImageUrl);
    static std::string CoverImageUrlToFile(const std::string& coverImageUrl)
    {
        return CoverImageUrlToFile(coverImageUrl.c_str());
    }
    static std::string BookIdToEpubFile(const std::string& bookId)
    {
        return BookIdToEpubFile(bookId.c_str());
    }
    static std::string BookIdToEpubFile(const char* bookId);
    static std::string BookIdToInfoFile(const std::string& bookId)
    {
        return BookIdToInfoFile(bookId.c_str());
    }
    static std::string BookIdToInfoFile(const char* bookId);
    static bool IsFileExisting(const char* file);
    static bool IsFileExisting(const std::string& file)
    {
        return IsFileExisting(file.c_str());
    }
    static std::string EpubFileToInfoFile(const char* epubFile);

    static std::string BookFileToCoverImageFile(const char* bookFile);
    static std::string BookFileToCoverImageFile(const std::string& bookFile)
    {
        return BookFileToCoverImageFile(bookFile.c_str());
    }

    static std::string BookFileToDkxFile(const char* bookFile);
    static std::string BookFileToDkxFile(const std::string& bookFile)
    {
        return BookFileToDkxFile(bookFile.c_str());
    }

    static std::string BookFileToDkptFile(const char* bookFile);
    static std::string BookFileToDkptFile(const std::string& bookFile)
    {
        return BookFileToDkptFile(bookFile.c_str());
    }

    static std::vector<std::string> GetTopNormalBookFolders();
    static std::vector<std::string> GetTopSpecialBookFolders();
    static bool IsTopSpecialBookFolders(const char* path);
    static bool IsTopSpecialBookFolders(const std::string& path)
    {
        return IsTopSpecialBookFolders(path.c_str());
    }

    static bool MakeDirectoryRecursive(const char* dir);
    static bool MakeDirectoryRecursive(const std::string& dir)
    {
        return MakeDirectoryRecursive(dir.c_str());
    }
    static std::string NormalizePath(const char* path);
    static std::string GetDirectoryDisplayName(const char* path);
    static std::string GetDirectoryDisplayName(const std::string& path)
    {
        return GetDirectoryDisplayName(path.c_str());
    }
    static const char* GetUserInfoFile();
    // for /aaaa/bbbb/// /aaaa/bbbb/ /aaaa/bbbb return /aaaa  bbbb
    static void SplitLastPath(const char* path, std::string* part1, std::string* part2);
    static bool RenameFile(const char* oldPath, const char* newPath);
    static bool RenameFile(const std::string& oldPath, const std::string& newPath)
    {
        return RenameFile(oldPath.c_str(), newPath.c_str());
    }
    static std::string BookFileToDir(const char* bookFile)
    {
        return BookFileToDir(std::string(bookFile));
    }
    static std::string BookFileToDir(const std::string& bookFile);

    static bool MakeDirIfEvernoteDirNotExisting();
private:
    static bool MakeSureBookDirExisting(const char* bookFile);
    PathManager();
    static std::string BookExtFileToDir(const char* bookFile, const char* ext);
};

} // namespace utility
} // namespace dk
#endif

