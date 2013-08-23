/**@file dk_File.h
 *
 * @brief 多看系统底层文件扫盘及基本处理接口
 *
 * @version 1.0
 *
 * @author wh
 * @date 2011-4-12
 * \n
 * @b 版权信息：
 *
 * - 北京多看科技有限公司 Copyright (C) 2011 - 2012
 *
 * _______________________________________________________________________
 */

#ifndef _DK_FILELIST_H_
#define _DK_FILELIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <vector>

#include "dkBaseType.h"
#include <string>
#include <tr1/memory>

using namespace std;
using namespace std::tr1;

#define INVALIDCHAPTER  -2

//TODO:PATH_MAX too long how to deal with?
#define FILEPATHMAXLEN      PATH_MAX            // 允许最长文件全路径
#define FILENAMEMAXLEN      NAME_MAX            // 允许最长文件名
#define FILEARTISTMAXLEN    256                 // 允许最长作者名
#define FILEPASSWORDMAXLEN  32                  // 允许最长文件密码
#define SERIALIZEDURLMAX    256

// 定义系统文件类型
enum DkFormatCategory
{
    DFC_Unknown = 0,    // 未知
    DFC_Book,           // 书籍
    DFC_Picture,        // 图片
    DFC_Music,          // 音频
    DFC_Video,          // 视频
    DFC_Package,        // 文件包
    DFC_Other           // 其他
};

#define FILE_FORMAT_LIST(MACRO_NAME)                            \
    MACRO_NAME(Book, Text           )                           \
    MACRO_NAME(Book, PortableDocumentFormat)                    \
    MACRO_NAME(Book, ElectronicPublishing)                      \
    MACRO_NAME(Book, MobiPocket)                                \
    MACRO_NAME(Book, Djvu)                                      \
    MACRO_NAME(Book, CompiledHelpManual)                        \
    MACRO_NAME(Book, WordDocument)                              \
    MACRO_NAME(Book, HyperText)                                 \
    MACRO_NAME(Book, Prc)                                       \
    MACRO_NAME(Book, Pdb)                                       \
    MACRO_NAME(Book, UniversalMobileDocument)                   \
    MACRO_NAME(Book, Serialized)                                   \
    \
    MACRO_NAME(Picture, Bitmap)                                 \
    MACRO_NAME(Picture, Jpeg)                                   \
    MACRO_NAME(Picture, GraphicsInterchangeFormat)              \
    MACRO_NAME(Picture, PortableNetworkGraphics)                \
    MACRO_NAME(Picture, TaggedImageFile)                        \
    \
    MACRO_NAME(Music, MpegAudioLayer3)                          \
    MACRO_NAME(Music, WindowsMediaAudio)                        \
    MACRO_NAME(Music, Wave)                                     \
    \
    MACRO_NAME(Video, RealMediaVariableBitrate)                 \
    MACRO_NAME(Video, RealMedia)                                \
    \
    MACRO_NAME(Book, ZipFile)                               \
    MACRO_NAME(Book, RoshalArchive)                         \
    \
    MACRO_NAME(Picture, PNG)                        \
    MACRO_NAME(Picture, TIF)                        \
    MACRO_NAME(Picture, Gif)                        \

#define FILE_EXT_LIST(MACRO_NAME)                               \
    MACRO_NAME(".txt",  DFC_Book,  Text)                        \
    MACRO_NAME(".zip",  DFC_Book,  ZipFile)                     \
    MACRO_NAME(".rar",  DFC_Book,  RoshalArchive)               \
    MACRO_NAME(".pdf",  DFC_Book,  PortableDocumentFormat)      \
    MACRO_NAME(".mobi", DFC_Book,  MobiPocket)                  \
    MACRO_NAME(".mp3",  DFC_Music, MpegAudioLayer3)             \
    MACRO_NAME(".epub", DFC_Book,  ElectronicPublishing)        \
    MACRO_NAME(DKLITE_SERIALIZEDBOOKPATH, DFC_Book, Serialized) \



//bmp, gif, png, jpg, jpeg, jpe, tif, tiff
#define FILE_EXT_LIST_ZIP(MACRO_NAME)                                \
    MACRO_NAME(".txt",  DFC_Book, Text)                            \
    MACRO_NAME(".pdf",  DFC_Book, PortableDocumentFormat)                            \
    MACRO_NAME(".jpg",  DFC_Picture, Jpeg)                    \
    MACRO_NAME(".bmp",  DFC_Picture, Bitmap)                \
    MACRO_NAME(".gif",  DFC_Picture, Gif)                    \
    MACRO_NAME(".png",  DFC_Picture, PNG)                \
    MACRO_NAME(".jpeg",  DFC_Picture, Jpeg)                    \
    MACRO_NAME(".jpe",  DFC_Picture, Jpeg)                \
    MACRO_NAME(".tif",  DFC_Picture, TIF)                    \
    MACRO_NAME(".tiff",  DFC_Picture, TIF)                \


#define DEFINE_FILE_FORMAT_ENUM(category, format)                \
    DFF_##format ,

enum DkFileFormat
{
    DFF_Unknown = 0,
    FILE_FORMAT_LIST(DEFINE_FILE_FORMAT_ENUM)
    DFF_Count
};


#define DEFINE_FILE_FORMAT_CATEGORY_MAPPING(category, format)               \
    DFC_##category ,

const DkFormatCategory g_rgkFormatCategory[]=
{
    DFC_Unknown,
    FILE_FORMAT_LIST(DEFINE_FILE_FORMAT_CATEGORY_MAPPING)
};

#define DKLITE_SERIALIZEDBOOKPATH  "/mnt/us/Serialized/"

#define DKLITE_MUSICFILEPATH  "/mnt/us/music/"

#define FILE_WORK_DIR(MACRO_NAME)                               \
    MACRO_NAME("/mnt/us/documents/",  KINDLE_DOCUMENTS)      \
    MACRO_NAME("/mnt/us/DK_Documents/",  DKLITE_DOCUMENTS)    \
    MACRO_NAME("/mnt/us/DK_Download/",  DKLITE_DOWNLOAD)    \
    MACRO_NAME("/mnt/us/DK_News/",  DKLITE_News)    \
    MACRO_NAME("/mnt/us/DK_BookStore/",  DKLITE_BookStore)    \
    MACRO_NAME(DKLITE_MUSICFILEPATH,  DKLITE_MUSIC) 




#define DEFINE_FILE_WORKDIRVATEGORY_MAPPING(dirpath, Category)               \
    Category,

enum FileWorkDirCategory
{
    UnknownDir = 0,
    FILE_WORK_DIR(DEFINE_FILE_WORKDIRVATEGORY_MAPPING)
    DirCount
};

#define DEFINE_FILE_SCANDIR_MAPPING(path, category)           \
{path,category},



typedef struct FILE_WORK_INFO
{
    const char* dirPath;
    FileWorkDirCategory Category;
}DKFILE_WORK_INFO;

const DKFILE_WORK_INFO g_rgFileWorkList[] =
{
    FILE_WORK_DIR(DEFINE_FILE_SCANDIR_MAPPING)
};

#define FILE_SCANDIR_SIZE (sizeof(g_rgFileWorkList)/sizeof(DKFILE_WORK_INFO))

// 定义系统文件来源
typedef enum FileSource
{
    UnknowSource =0,    //未知
    UsbCopy,            //usb拷贝
    WEBSERVER,          //网络
    DUOKAN_BOOKSTORE    // 多看书城
} DK_FileSource;


// 扫描磁盘后得到的文件信息
struct DirInfo
{
    char                        FilePath[FILEPATHMAXLEN];   //文件的全路径名
    DkFileFormat                FileFormat;                 //文件格式
    DkFormatCategory            FileCategory;               //文件类型
    int                         FileSize;
    bool                        isDKBookStoreBook;
    struct DirInfo*         next;
};

struct FileListItem
{
    unsigned int uFileId;
    char filepath[FILEPATHMAXLEN];
    DkFormatCategory eFileCategory;
    char pExtension[16];
};


struct BookFileList
{
    FileListItem* pFileList;
    unsigned int uLength;
    int iBookId;

    BookFileList()
    {
        pFileList = NULL;
        uLength = 0;
        iBookId = -1;
    }

    virtual ~BookFileList()
    {
        if(pFileList)
        {
            delete[] pFileList;
        }
        uLength = 0;
        iBookId = -1;
    }
};

// 文件基本信息
class CDKFile
{
public:
    CDKFile();
    virtual ~CDKFile();
public:
    std::string GetCoverImagePath() const;

    bool IsDuoKanBook() const;
    int GetFileID() const;
    int GetFileSize() const;
    int GetFilePlayProcess() const;
    long int GetFileLastReadTime() const;
    long GetFileReadingOrder() const;
    long GetFileAddOrder() const;
    const char* GetGbkName() const;
    const char* GetFilePath() const;
    const char* GetFileName() const;
    const char* GetFileArtist() const;
    const char* GetFileImage() const;
    const char* GetFileAbstract() const;
    const char* GetFilePassword() const;
    DkFormatCategory GetFileCategory() const;
    DkFileFormat GetFileFormat() const;
    DK_FileSource GetFileSource() const;
    bool GetSync() const;
    bool GetCrash() const;
	const char* GetBookID() const;

    void SetFileID(int id);
    void SetFileSize(int size);
    void SetFilePlayProcess(int process);
    void SetFileLastReadTime(long int time);
    void SetFileReadingOrder(long _lReadingOrder);
    void SetFileAddOrder(long _lAddOrder);
    int SetFilePath(const char* Path);
    int SetFileName(const char* Name);
    int SetFileArtist(const char* Artist);
    int SetFileImage(const char* Image);
    int SetFileAbstract(const char* Abstract);
    void SetFileCategory(DkFormatCategory Category);
    void SetFileFormat(DkFileFormat Format);
    void SetFileSource(DK_FileSource Source);
    void SetFilePassword(const char* password);
    void SetFileSync(bool bSync);
    void SetFileCrash(bool bCrash);
	void SetBookID(const char* bookID);
    void SyncFile();

    int GetPageCount() const;
    void SetPageCount(int pageCount);

    // 书籍特有
    virtual void SetIsTrialBook(bool isTrailBook);

    virtual bool GetIsTrialBook();
    virtual std::string GetBookRevision() { return m_revision; }
    void SetBookRevision(const char* bookRevision) { m_revision = bookRevision; }

private:
    int m_id;
    int m_size;
    int m_playProgress;
    mutable int m_pageCount;
    long int m_lastReadTime;
    long m_readingOrder;
    long m_addOrder;
    DkFormatCategory m_category;
    DkFileFormat m_format;
    DK_FileSource m_source;

    bool m_bSync;
    bool m_bCrash;

    string m_bookID;
    string m_gbkName;
    string m_path;
    string m_name;
    string m_artist;
    string m_password;
    string m_revision;

    PCHAR m_image;
    PCHAR m_abstract;
};

typedef shared_ptr<CDKFile> PCDKFile;

//书籍信息
class CDKBook : public CDKFile
{
public:
    CDKBook();
    ~CDKBook();

    void SetTotalChapterNum(int num);
    int  GetTotalChapterNum();

    void SetIsTrialBook(bool isTrailBook);
    bool GetIsTrialBook();

    std::string GetBookRevision();
    void SetBookRevision(const char* bookRevision);

private:
    int  m_CurChapterNum;    //下载的章节数
    int  m_TotalChapterNum;  //总的章节数
    bool m_isTrialBook;
    std::string m_bookRevision;
};

class FileMatcher
{
public:
    virtual bool operator()(const CDKFile* file) const = 0;
    virtual ~FileMatcher(){};
};

class FilePathMatcher: public FileMatcher
{
public:
    FilePathMatcher(const char* path)
        : m_path(path)
    {
    }
    virtual ~FilePathMatcher(){};

    virtual bool operator()(const CDKFile* file) const
    {
        if (NULL == m_path)
        {
            return false;
        }
        return strcmp(file->GetFilePath(), m_path) == 0;
    }

private:
    const char* m_path;
};

class FileIDMatcher: public FileMatcher
{
public:
    FileIDMatcher(int id)
        : m_id(id)
    {
    }
    virtual ~FileIDMatcher(){};

    virtual bool operator()(const CDKFile* file) const
    {
        return file->GetFileID() == m_id;
    }

private:
    int m_id;
};

class BookIdMatcher: public FileMatcher
{
public:
    BookIdMatcher(const char* bookId)
        : m_bookId(bookId)
    {
    }
    virtual ~BookIdMatcher(){};

    virtual bool operator()(const CDKFile* file) const
    {
        return file->GetBookID() == m_bookId;
    }

private:
    string m_bookId;
};

class FileCategoryMatcher: public FileMatcher
{
public:
    FileCategoryMatcher(DkFormatCategory category)
        : m_category(category)
    {
    }
    virtual ~FileCategoryMatcher(){};

    virtual bool operator()(const CDKFile* file) const
    {
        return file->GetFileCategory() == m_category;
    }

private:
    DkFormatCategory m_category;
};

class FileKeywordMatcher: public FileMatcher
{
public:
    FileKeywordMatcher(const char* keyword);
    virtual ~FileKeywordMatcher(){};
    virtual bool operator()(const CDKFile* file) const;
private:
    const char* m_keyword;
    bool m_matchByFirstLetters;
    std::wstring  m_wKeyWord;

};

class FileComparer
{
public:
    enum CompareType
    {
        CT_Name,
        CT_AddingOrder,
        CT_ReadingOrder
    };

    FileComparer(CompareType type)
        : m_type(type)
    {
    }

    bool operator()(const CDKFile* lhs, const CDKFile* rhs) const;
private:
    CompareType m_type;
};

typedef std::vector<PCDKFile> DKFileList;

//媒体信息
class CDKMedia:public CDKFile
{

public:
    CDKMedia();
    ~CDKMedia();
};
typedef CDKMedia * PCDKMedia;


class DK_FileFactory{
public:
    static CDKMedia*    CreateMediaFileInfo();
    static CDKBook*     CreateBookFileInfo();
    static CDKBook*     CreateBookFileInfoFromDownload(PCCH path, PCCH author, DK_FileSource internetSource, PCCH abstract, PCCH password, int filesize);
    static CDKBook*     CreateBookFileInfoFromPush(PCCH path, int filesize);

private:
    DK_FileFactory(){};
    ~DK_FileFactory(){};
};

//磁盘扫描相关函数接口
//TODO:LOCK
DirInfo* ScanFileOnDisk(DirInfo* *DirList, const char* Path, DkFileFormat (*IsScanFileType)(const char* ), bool fSearchSubFolder);
DirInfo* ScanFileInFolder(DirInfo* *DirList, const char* Path, DkFileFormat (*IsScanFileType)(const char* ));
DirInfo* AddFileToDirList(DirInfo* DirList, LPSTR FilePath, DkFileFormat (*IsScanFileType)(const char* ));
DirInfo* SerachDirNodebyPath(DirInfo* DirList, const char* FilePath,DkFormatCategory fileCategory);
//文件检测
DkFileFormat GetFileFormatbyExtName(const char* filename);

DkFileFormat GetFileFormatbyExtNameForZipFile(const char* filename);
DkFormatCategory GetFileCategoryForZipFile(const char* filename);

const char *GetExtNameByFormat(DkFileFormat eFormat);
DkFormatCategory GetFileCategory(const char* filename);
bool IsFileWorkDir(const char* dirPath);
const char* GetFileCommonPath(const char* pFileAllPath);

#endif /*_DK_FILELIST_H_*/
