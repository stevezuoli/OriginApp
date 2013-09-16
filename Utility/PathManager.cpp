#include "Utility/PathManager.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include "KernelDef.h"
#include "Utility/StringUtil.h"
#include "I18n/StringManager.h"
#include "Utility/FileSystem.h"

namespace dk
{
namespace utility
{
static const char* s_rootPath = "/mnt/us";
static const char* s_tempPath = "/mnt/us/system/temp";
static const char* s_cheatConfPath = "/mnt/us/DK_System/xKindle/cheat.conf";
static const char* s_coverImagePath = "/mnt/us/system/temp/image/covers";
static const char* s_bookStorePath = "/mnt/us/DK_BookStore";
static const char* s_bookStoreLocalCategoryFile = "/mnt/us/DK_BookStore/local_category.conf";
static const char* s_duokanDocument = "/mnt/us/DK_Documents";
static const char* s_duokanDownload = "/mnt/us/DK_Download";
static const char* s_kindleDocument = "/mnt/us/documents";
static const char* s_duokanNews = "/mnt/us/DK_News";
static const char* s_userInfoFile = "/mnt/us/system/userfile";
static const char* s_readingDataPath = "/mnt/us/DK_ReadingData";
static const char* s_screenSaverUpdatePath = "/mnt/us/system/ScreenSaverUpdateCfg.dat";
static const char* s_screenSaverPath = "/mnt/us/DK_System/xKindle/res/ScreenSaver/";
static const char* s_languageStringPath = "/mnt/us/DK_System/xKindle/res/language";
static const char* s_interactiveImagePath = "/mnt/us/DK_System/xKindle/interactiveimage.";
static const char* s_systemConfigPath = "/mnt/us/DK_System/xKindle/config.ini";
static const char* s_bookStoreCorrectionPath = "/mnt/us/DK_System/xKindle/corrections.xml";
static const char* s_bookWaitUploadNotePath = "/mnt/us/DK_System/xKindle/BookWithNewNotes.xml";
static const char* s_bookStoreCartPath = "/mnt/us/DK_System/xKindle/local_cart.xml";
static const char* s_bookStoreFavouritesPath = "/mnt/us/DK_System/xKindle/local_favourites.xml";
static const char* s_resourcePath = "/DuoKan";
static const char* s_evernotePath = "/mnt/us/DK_System/xKindle/evernote";
static const char* s_evernoteInternalPath = "/mnt/us/DK_System/xKindle/evernote_internal";
static const char* s_evernoteTokenPath = "/var/local/evernote/evernote_token.xml";
static const char* s_evernoteUserPath = "/var/local/evernote/evernote_user.xml";
static const char* s_xiaomiAccountFile = "/var/local/xiaomi/token.xml";
static const char* s_migrateResultPath = "/var/local/xiaomi/migration.xml";
static const char* s_migratePromptTimePath = "/mnt/us/system/migrateprompttime";
static const char* s_accountMappingPath = "/var/local/xiaomi/accountMapping.dk";
static const char* s_readingHistoryPath = "/mnt/us/system/readingHistory/";
static const char* s_micloudServiceTokenPath = "/var/local/xiaomi/miCloud.xml";
static const char* s_kssTempPath = "/mnt/us/DK_System/xKindle/kss";

bool PathManager::Init()
{
    return MakeDirectoryRecursive(GetTempPath())
        && MakeDirectoryRecursive(GetCoverImagePath())
        && MakeDirectoryRecursive(GetBookStorePath())
        && MakeDirectoryRecursive(GetKSSTempPath());
}

static bool IsPath(const char* path, const char* pat)
{
    if (NULL == path || NULL == pat)
    {
        return false;
    }
    size_t patLen = strlen(pat);
    if (strncasecmp(path, pat, patLen) != 0)
    {
        return false;
    }
    return path[patLen] == 0 || (path[patLen] == '/' && path[patLen + 1] == 0);
}

bool PathManager::IsRootPath(const char* path)
{
    return IsPath(path, s_rootPath);
}

bool PathManager::IsBookStorePath(const char* path)
{
    return IsPath(path, s_bookStorePath);
}

bool PathManager::IsDocumentsPath(const char* path)
{
    return IsBookStorePath(path)
        || IsNewsPath(path)
        || IsPath(path, s_duokanDocument)
        || IsPath(path, s_duokanDownload)
        || IsPath(path, s_kindleDocument);
}

bool PathManager::IsNewsPath(const char* path)
{
    return IsPath(path, s_duokanNews);
}

std::string PathManager::GetRootPath()
{
    return s_rootPath;
}

std::string PathManager::GetNewsPath()
{
    return s_duokanNews;
}

std::string PathManager::GetDownloadFolderPath()
{
    return s_duokanDownload;
}

std::string PathManager::GetTempPath()
{
    return s_tempPath;
}

std::string PathManager::GetCheatFilePath()
{
    return s_cheatConfPath;
}

std::string PathManager::GetCoverImagePath()
{
    return s_coverImagePath;
}

std::string PathManager::GetBookStorePath()
{
    return s_bookStorePath;
}

std::string PathManager::GetBookStoreLocalCategoryFile()
{
    return s_bookStoreLocalCategoryFile;
}

std::string PathManager::GetCommentExportPath()
{
    return s_readingDataPath;
}

std::string PathManager::GetBookStoreCorrectionPath()
{
    return s_bookStoreCorrectionPath;
}

std::string PathManager::GetBookStoreCartPath()
{
    return s_bookStoreCartPath;
}

std::string PathManager::GetBookStoreFavouritesPath()
{
    return s_bookStoreFavouritesPath;
}

std::string PathManager::GetResourcePath()
{
    return s_resourcePath;
}

std::string PathManager::GetXiaomiAccountPath()
{
    return s_xiaomiAccountFile;
}

std::string PathManager::GetScreenSaverCfgPath()
{
    return s_screenSaverUpdatePath;
}

std::string PathManager::GetScreenSaverDir()
{
    return s_screenSaverPath;
}

std::string PathManager::GetLanguagePath()
{
    return s_languageStringPath;
}

std::string PathManager::GetInteractiveImagePath()
{
    return s_interactiveImagePath;
}

std::string PathManager::GetSystemConfigPath()
{
	return s_systemConfigPath;
}

const char* PathManager::GetUserInfoFile()
{
    return s_userInfoFile;
}

std::string PathManager::GetBookWaitUploadNotePath()
{
    return s_bookWaitUploadNotePath;
}

std::string PathManager::GetEvernotePath()
{
    return s_evernotePath;
}

std::string PathManager::GetEvernoteInternalPath()
{
    return s_evernoteInternalPath;
}

std::string PathManager::GetEvernoteTokenPath()
{
    return s_evernoteTokenPath;
}

std::string PathManager::GetEvernoteUserPath()
{
    return s_evernoteUserPath;
}

std::string PathManager::GetMigrateResultPath()
{
	return s_migrateResultPath;
}

std::string PathManager::GetMigratePromptTimePath()
{
    return s_migratePromptTimePath;
}

std::string PathManager::GetAccountMappingPath()
{
	return s_accountMappingPath;
}

std::string PathManager::GetReadingHistoryPath()
{
	return s_readingHistoryPath;
}

std::string PathManager::GetMiCloudServiceTokenPath()
{
    return s_micloudServiceTokenPath;
}

std::string PathManager::GetKSSTempPath()
{
    return s_kssTempPath;
}

bool PathManager::MakeDirectoryRecursive(const char* dir)
{
    if (NULL == dir)
    {
        return false;
    }

    if (strlen(dir) >= PATH_MAX)
    {
        return false;
    }
    char buf[PATH_MAX];
    strncpy(buf, dir, DK_DIM(buf));

    char* sep =  strchr(buf, '/');
    struct stat st;
    while (sep)
    {
        if (sep != buf)
        {
            *sep = 0;

            if (0 != stat(buf, &st))
            {
                int err = mkdir(buf, S_IRWXU | S_IRWXG | S_IRWXO);
                if (0 != err && EEXIST != err)
                {
                    return false;
                }
            }
            *sep = '/';
        }
        if (*(sep + 1) == 0)
        {
            return true;
        }

        sep = strchr(sep + 1, '/');
    }
    return 0 == stat(buf, &st) || 0 == mkdir(buf, S_IRWXU | S_IRWXG | S_IRWXO);
}

std::string PathManager::GetFileName(const char* src)
{
    if (NULL == src)
    {
        return "";
    } 
    const char* result = strrchr(src, '/');
    if (NULL == result)
    {
        return "";
    }
    return result + 1;
}

std::string PathManager::ConcatPath(const char* dir, const char* filename)
{
    if (NULL == dir || NULL == filename)
    {
        return "";
    }
    int dirLen = strlen(dir);
    if (0 == dirLen)
    {
        return filename;
    }
    if (dir[dirLen - 1] == '/')
    {
        return std::string(dir) + filename;
    }
    else
    {
        return std::string(dir) + '/' + filename;
    }
}

std::string PathManager::GetBaseUrl(const char* url)
{
    string baseUrl = url;
    size_t pos = baseUrl.find('?');
    if (pos != string::npos)
    {
        return baseUrl.erase(pos);
    }
    return baseUrl;
}

std::string PathManager::CoverImageUrlToFile(const char*  coverImageUrl)
{
    return ConcatPath(GetCoverImagePath().c_str(), GetFileName(coverImageUrl).c_str());
}

std::string PathManager::BookIdToEpubFile(const char*  bookId)
{
    std::string fileName = bookId + std::string(".epub");
    return ConcatPath(GetBookStorePath().c_str(), fileName.c_str());
}

std::string PathManager::EpubFileToInfoFile(const char* epubFile)
{
    std::string fileName = epubFile;
    if (fileName.size() <= 5)
    {
        return "";
    }
   return fileName.substr(0, fileName.size() - 5) + ".info";
}


std::string PathManager::BookIdToInfoFile(const char*  bookId)
{
    return EpubFileToInfoFile(BookIdToEpubFile(bookId).c_str());
}


bool PathManager::IsFileExisting(const char* file)
{
    if (NULL == file)
    {
        return false;
    }
    struct stat st;
    return stat(file, &st) == 0;
}

std::string PathManager::NormalizePath(const char* path)
{
    if (NULL == path)
    {
        return "";
    }
    std::string normalizedPath;
    size_t pathLen = strlen(path);
    for (size_t i = 0; i < pathLen; ++i)
    {
        if ('/' == path[i])
        {
            ++i;
            while ('/' == path[i])
            {
                ++i;
            }
            --i;
        }
        normalizedPath.push_back(path[i]);
    }
	

	// °Ñ /mnt/base-us Ìæ»»Îª /mnt/us
    if (normalizedPath.find("/mnt/base-us") == 0)
    {
        normalizedPath.replace(0, strlen("/mnt/base-us"), "/mnt/us");
    }

    return normalizedPath;
}

bool PathManager::IsUnderBookStore(const char* path)
{
    std::string normalizedPath = NormalizePath(path);
    return normalizedPath.find(GetBookStorePath()) == 0;
}

bool PathManager::IsUnderNews(const char* path)
{
    std::string normalizedPath = NormalizePath(path);
    return normalizedPath.find(GetNewsPath()) == 0;
}

std::vector<std::string> PathManager::GetFilesInPath(const char* path)
{
    std::vector<std::string> files;
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    if((dp  = opendir(path)) != NULL)
    {
        while ((dirp = readdir(dp)) != NULL)
        {
            if (DT_REG == dirp->d_type)
            {
                files.push_back(dirp->d_name);
            }
        }
        closedir(dp);
    }
    return files;
}

std::vector<std::string> PathManager::GetFilesInPathRecursive(const char* path)
{
    DIR* dp = NULL;
    struct dirent* dirp = NULL;
    std::string filePath(path);
    std::vector<std::string> files;
    if ((dp = opendir(filePath.c_str())) != NULL)
    {
        while ((dirp = readdir(dp)) != NULL)
        {
            if (DT_REG == dirp->d_type)
            {
                files.push_back(dirp->d_name);
            }
            else if (DT_DIR == dirp->d_type 
                    && strcmp(dirp->d_name, ".") != 0
                    && strcmp(dirp->d_name, "..") != 0)
            {
                string childDirName(filePath + string(1, '/') + string(dirp->d_name));
                vector<string> childFiles = GetFilesInPathRecursive(childDirName.c_str());
                for (unsigned int i = 0; i < childFiles.size(); ++i)
                {
                    files.push_back(childDirName.append(1, '/').append(childFiles[i]));
                }
            }
            else
            {
                //DebugPrintf(DLC_DIAGNOSTIC, "type ignore. %s/%s\n", filePath.c_str(), dirp->d_name);
            }
        }

        closedir(dp);
    }
    else
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "opendir error: %s, %s", filePath.c_str(), strerror(errno));
    }

    return files;
}

bool PathManager::IsValidDirName(const char* path)
{
    return !StringUtil::IsNullOrEmpty(path) 
        && path[0] != '.' 
        && strcasestr(path, ".sdr") == NULL
        && strcasestr(path, ".dir") == NULL;
}

std::vector<std::string> PathManager::GetDirsInPath(const char* path)
{
    std::vector<std::string> dirs;
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    if((dp  = opendir(path)) != NULL)
    {
        while ((dirp = readdir(dp)) != NULL)
        {
            if (DT_DIR == dirp->d_type && IsValidDirName(dirp->d_name))
            {
                dirs.push_back(dirp->d_name);
            }
        }
        closedir(dp);
    }
    return dirs;
}

std::string PathManager::GetFileNameWithoutExt(const char* filename)
{
    if (NULL == filename)
    {
        return "";
    }
    const char* lastDot = strrchr(filename, '.');
    if (NULL == lastDot)
    {
        return filename;
    }
    else
    {
        return std::string(filename, lastDot - filename);
    }
}

std::string PathManager::GetFileExt(const char* filename)
{
    if (NULL == filename)
    {
        return "";
    }
    const char* lastDot = strrchr(filename, '.');
    if (NULL == lastDot)
    {
        return filename;
    }
    else
    {
        return lastDot + 1;
    }
}


std::string PathManager::BookFileToCoverImageFile(const char* file)
{
    if (StringUtil::EndWith(StringUtil::ToLower(file).c_str(), ".pdf"))
    {
        FileSystem::DeleteFile(std::string(file) + ".cover.bmp");
    }
    return BookExtFileToDir(file, "cover.bmp");
}

std::vector<std::string> PathManager::GetTopNormalBookFolders()
{
    std::vector<std::string> results;
    results.push_back(s_kindleDocument);
    results.push_back(s_duokanDocument);
    results.push_back(s_duokanDownload);
    //results.push_back(s_duokanNews);
    return results;
}

bool PathManager::IsTopSpecialBookFolders(const char* path)
{
    return IsBookStorePath(path) || IsNewsPath(path);
}

std::vector<std::string> PathManager::GetTopSpecialBookFolders()
{
    std::vector<std::string> results;
    results.push_back(s_bookStorePath);
    results.push_back(s_duokanNews);
    return results;
}

void PathManager::SplitLastPath(const char* path, std::string* part1, std::string* part2)
{
    if (StringUtil::IsNullOrEmpty(path))
    {
        return;
    }
    int pathLen = strlen(path);
    while (pathLen > 0 && path[pathLen - 1] == '/')
    {
        --pathLen;
    }
    if (0 == pathLen)
    {
        return;
    }
    int lastSlash = pathLen;
    while (lastSlash >= 0 && path[lastSlash] != '/')
    {
        --lastSlash;
    }
    *part2 = std::string(path + lastSlash + 1, pathLen - lastSlash - 1);
    if (lastSlash < 0)
    {
        return;
    }
    int prevNoneSlash = lastSlash;
    while (prevNoneSlash >= 0 && path[prevNoneSlash] == '/')
    {
        --prevNoneSlash;
    }
    *part1 = std::string(path, prevNoneSlash + 1);
}

std::string PathManager::GetDirectoryDisplayName(const char* path)
{
    if (StringUtil::IsNullOrEmpty(path))
    {
        return "";
    }
    if (IsBookStorePath(path))
    {
        return StringManager::GetPCSTRById(BOOKSTORE_DUOKAN_CHOICEST_BOOK);
    }
    if (IsNewsPath(path))
    {
        return StringManager::GetPCSTRById(DUOKAN_NEWS_DISPLAY_NAME);
    }
    std::string part1, part2;
    SplitLastPath(path, &part1, &part2);
    return part2;
}

bool PathManager::ShouldBlockPath(const char* path, const char* itemName)
{
    if (IsPath(path, s_kindleDocument) && strcasecmp(itemName, "dictionaries") == 0)
    {
        return true;
    }
    return false;
}

bool PathManager::MakeSureBookDirExisting(const char* bookFile)
{
    std::string dir = BookFileToDir(bookFile);
    return MakeDirectoryRecursive(dir);
}

std::string PathManager::BookExtFileToDir(const char* bookFile, const char* ext)
{
    if (!MakeSureBookDirExisting(bookFile))
    {
        return "";
    }
    std::string dir = BookFileToDir(bookFile);
    std::string oldFile = std::string(bookFile) + "."  + ext;
    std::string newFile = ConcatPath(dir, ext);
    RenameFile(oldFile, newFile);
    return newFile;
}

std::string PathManager::BookFileToDkxFile(const char* bookFile)
{
    return BookExtFileToDir(bookFile, "dkx");
}

std::string PathManager::BookFileToDkptFile(const char* bookFile)
{
    return BookExtFileToDir(bookFile, "dkpt");
}

bool PathManager::RenameFile(const char* oldPath, const char* newPath)
{
    if (StringUtil::IsNullOrEmpty(oldPath) || !IsFileExisting(oldPath))
    {
        return true;
    }
    return rename(oldPath, newPath) == 0;
}


std::string PathManager::NormalizeCoverImageUrl(
        const char* imageUrl,
        int dstWidth,
        int dstHeight)
{
    const char* bang = strchr(imageUrl, '!');
    if (NULL == bang)
    {
        return imageUrl;
    }
    ++bang;
    // image spec from http://dkmars/wiki/index.php?title=%E5%9B%BE%E5%BA%8A%E8%A7%84%E8%8C%83
    const char* postfix[] = {"l", "m", "e", "s", "ss", "t", "vt",  NULL};
    int imgSizes[][2] = {
        {768, 1024}, // l
        {384, 512}, // m
        {256, 342}, // e
        {192, 256}, // s
        {128, 172}, // ss
        {96, 128}, // t
        {48, 64}, // vt
    };
    int i = 0;
    for (; postfix[i]; ++i)
    {
        if (dstWidth > imgSizes[i][0] || dstHeight > imgSizes[i][1])
        {
            break;
        }
    }
    if (i > 0)
    {
        --i;
    }
    string result;
    result.reserve(bang - imageUrl + strlen(postfix[i]) + 1);
    result.append(imageUrl, bang);
    result.append(postfix[i]);
    return result;
}

std::string PathManager::BookFileToDir(const std::string& bookFile)
{
    return bookFile + ".dir";
}

bool PathManager::MakeDirIfEvernoteDirNotExisting()
{
    if (!IsFileExisting(s_evernotePath))
    {
        MakeDirectoryRecursive(s_evernotePath);
    }

    if (!IsFileExisting(s_evernoteInternalPath))
    {
        MakeDirectoryRecursive(s_evernoteInternalPath);
    }
    return true;
}
} // namespace utility

} // namespace dk
