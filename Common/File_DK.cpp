/**@file dk_list.c
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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include "dkBaseType.h"
#include "interface.h"
#include "Common/File_DK.h"
#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "DKXManager/DKX/DKXManager.h"
#include "Utility/StringUtil.h"
#include "Utility/RenderUtil.h"
#include "Utility/CharUtil.h"
#include "Utility/ColorManager.h"
#include "BookReader/IBookReader.h"
#include "BookReader/PageTable.h"
#include "DkStreamFactory.h"
#include "DKRAPI.h"
#include "Common/WindowsMetrics.h"
#include "Common/BookCoverLoader.h"
#include "Common/FileManager_DK.h"
#include "KernelVersion.h"
#include <memory>

using namespace WindowsMetrics;
using namespace dk::utility;

#define EXCEPTION_PATH "/mnt/us/DK_System"

CDKFile::CDKFile()
        : m_id(0)
        , m_size(0)
        , m_playProgress(0)
        , m_lastReadTime(0)
        , m_readingOrder(0)
        , m_addOrder(0)
        , m_path()
        , m_name()
        , m_artist()
        , m_password()
        , m_category(DFC_Unknown)
        , m_format(DFF_Unknown)
        , m_source(UnknowSource)
        , m_image(NULL)
        , m_abstract(NULL)
        , m_bSync(false)
        , m_pageCount(-1)
{
}

CDKFile::~CDKFile()
{
    if(m_image)
    {
        free(m_image);
        m_image =   NULL;
    }

    if(m_abstract)
    {
        free(m_abstract);
        m_abstract = NULL;
    }
}

bool CDKFile::IsDuoKanBook() const
{
    return StringUtil::StartWith(GetFilePath(), PathManager::GetBookStorePath().c_str()) 
        && StringUtil::EndWith(GetFilePath(), ".epub")
        && PathManager::IsFileExisting(PathManager::EpubFileToInfoFile(GetFilePath()).c_str());
}

std::string CDKFile::GetCoverImagePath() const
{
    std::string result;
    DkFileFormat fileFormat = GetFileFormat();
    if (DFF_ElectronicPublishing  != fileFormat
            && DFF_MobiPocket != fileFormat
            && DFF_PortableDocumentFormat != fileFormat)
    {
        return result;
    }

    result = PathManager::BookFileToCoverImageFile(GetFilePath());
    if (PathManager::IsFileExisting(result.c_str()) &&
        BookCoverLoader::GetInstance()->IsValidCoverImage(result.c_str()))
    {
        return result;
    }
    BookCoverLoader::GetInstance()->AddBook(GetFilePath());

    return "";
}

int CDKFile::GetFileID()const
{
    return m_id;
}

int CDKFile::GetFileSize()const
{
    return m_size;
}

int CDKFile::GetFilePlayProcess()const
{
    return m_playProgress;
}

long int CDKFile::GetFileLastReadTime()const
{
    return m_lastReadTime;
}

long CDKFile::GetFileAddOrder()const
{
    return m_addOrder;
}

long CDKFile::GetFileReadingOrder() const
{
    return m_readingOrder;
}

const char*  CDKFile::GetFilePath() const
{
    return m_path;
}

const char* CDKFile::GetGbkName() const
{
    return m_gbkName.c_str();
}

const char*  CDKFile::GetFileName() const
{
    return m_name;
}

const char*  CDKFile::GetFileArtist() const
{
    return m_artist;
}

const char*  CDKFile::GetFilePassword() const
{
    return m_password;
}

const char*  CDKFile::GetFileImage() const
{
    return m_image;
}

const char*  CDKFile::GetFileAbstract() const
{
    return m_abstract;
}

DkFormatCategory CDKFile::GetFileCategory() const
{
    return m_category;
}

DkFileFormat CDKFile::GetFileFormat() const
{
    return m_format;
}

DK_FileSource   CDKFile::GetFileSource() const
{
    return m_source;
}


bool CDKFile::GetSync() const
{
	return m_bSync;
}

bool CDKFile::GetCrash() const
{
	return m_bCrash;
}

const char* CDKFile::GetBookID() const
{
	return m_bookID.c_str();
}

void CDKFile::SetFileID(int id)
{
    m_id = id;
    return;
}

void CDKFile::SetFileSize(int size)
{
    if(size > 0)
    {
        m_size = size;
    }
    return;
}

void CDKFile::SetFilePlayProcess(int process)
{
    if(process >= 0)
    {
        m_playProgress = process;
    }
    return;
}

void CDKFile::SetFileLastReadTime(long int time)
{

    if(time > 0)
    {
        m_lastReadTime = time;
    }
    return;
}

void CDKFile::SetFileReadingOrder(long _lReadingOrder)
{
    m_readingOrder = _lReadingOrder;
}

void CDKFile::SetFileAddOrder(long _lAddOrder)
{
    if (_lAddOrder >= 0)
        m_addOrder = _lAddOrder;
}

int CDKFile::SetFilePath(const char* Path)
{
    int ret = 1;
    if(Path)
    {
        int iLen = strlen(Path);
        int iCopyLen = ((iLen >= FILEPATHMAXLEN)?FILEPATHMAXLEN - 1:iLen);
        strncpy(m_path,Path,iCopyLen);
        m_path[iCopyLen] = '\0';
        return 0;
    }
    return ret;
}


void CDKFile::SetFilePassword(const char* password)
{

    if(password)
    {
        int iLen = strlen(password);
        int iCopyLen = ((iLen >= FILEPASSWORDMAXLEN) ? FILEPASSWORDMAXLEN - 1 : iLen);
        strncpy(m_password, password, iCopyLen);
        m_password[iCopyLen] = '\0';
    }
}

void CDKFile::SetFileSync(bool bSync)
{
	m_bSync = bSync;
}

void CDKFile::SyncFile()
{
	if(m_bSync)
	{
		return;
	}
	if(!SystemSettingInfo::GetInstance()->GetMetadataOpen() && !IsDuoKanBook())
	{
		return;
	}
	if (DFF_ElectronicPublishing != m_format &&
        DFF_MobiPocket != m_format &&
        DFF_PortableDocumentFormat != m_format &&
        DFF_Text != m_format)
	{
		return;
	}

	DKXManager* pclsDKXManager = DKXManager::GetInstance();
	if(pclsDKXManager->FetchSync(m_path))
	{
		SetFileSync(true);
		return;
	}

	if(pclsDKXManager->FetchCrash(m_path))
	{
		SetFileCrash(true);
		return;
	}
	pclsDKXManager->SetCrash(m_path, true);
	pclsDKXManager->SaveCurDkx();

	string strTitle;
	string strAuthor;
	std::auto_ptr<IBookReader> bookReader(IBookReader::CreateBookReaderFromFormat(m_format));
    if (bookReader.get() && bookReader->OpenDocument(m_path))
    {
        BOOK_INFO_DATA bookInfo = bookReader->GetBookInfo();
        strAuthor = bookInfo.author;
        strTitle = bookInfo.title;
    }

	if(!StringUtil::Trim(strTitle.c_str()).empty())
	{
		pclsDKXManager->SetBookName(m_path, strTitle);
		SetFileName(strTitle.c_str());
	}
	if(!StringUtil::Trim(strAuthor.c_str()).empty())
	{
		pclsDKXManager->SetBookAuthor(m_path, strAuthor);
		SetFileArtist(strAuthor.c_str());
	}
	pclsDKXManager->SetSync(m_path, true);
	SetFileSync(true);
	pclsDKXManager->SetCrash(m_path, false);
	pclsDKXManager->SaveCurDkx();
}

void CDKFile::SetFileCrash(bool bCrash)
{
	m_bCrash = bCrash;
}

static bool IsValidBookId(const char* bookId)
{
    if (NULL == bookId)
    {
        return false;
    }
    if (strlen(bookId) != 32)
    {
        return false;
    }
    char c = bookId[0];
    if ('0' <= c && c <= '9')
    {
        return true;
    }
    if ('a' <= c && c <= 'w' && c != 'q')
    {
        return true;
    }
    return false;
}

void CDKFile::SetBookID(const char* bookID)
{
    if (!IsValidBookId(bookID))
    {
        return;
    }
	m_bookID = bookID;
}

int CDKFile::SetFileName(const char* Name)
{
    int ret = 1;
    if(Name)
    {
        int iLen = strlen(Name);
        int iCopyLen = ((iLen >= FILENAMEMAXLEN)?FILENAMEMAXLEN - 1:iLen);
        strncpy(m_name,Name,iCopyLen);
        m_name[iCopyLen] = '\0';
        if(GetFileFormatbyExtName(m_name)!= DFF_Unknown)
        {
            char *pchar = strrchr(m_name,'.');
            if(pchar)
            {
                *pchar = '\0';
            }
        }
        m_gbkName = EncodeUtil::UTF8ToGBKString(m_name);
        return 0;
    }
    return ret;
}

int CDKFile::SetFileArtist(const char* Artist)
{

    int ret = 1;
    if(Artist)
    {
        int iLen = strlen(Artist);
        int iCopyLen = ((iLen >= FILEARTISTMAXLEN)?FILEARTISTMAXLEN - 1:iLen);
        strncpy(m_artist,Artist,iCopyLen);
        m_artist[iCopyLen] =  '\0';
        return 0;
    }
    return ret;
}

int CDKFile::SetFileImage(const char* Image)
{
    int ret = 1;
    if(Image)
    {
        int len = strlen(Image);
        if(m_image)
        {
            free(m_image);
        }
        m_image =(PCHAR)malloc(len + 1);
        if(m_image)
        {
            memcpy(m_image,Image,len);
            m_image[len] = '\0';
            ret = 0;
        }
    }
    return ret;
}

int CDKFile::SetFileAbstract(const char* pcAbstract)
{

    int ret = 1;
    if(pcAbstract)
    {
        int len = strlen(pcAbstract);
        if(m_abstract)
        {
            free(m_abstract);
        }
        m_abstract =(PCHAR)malloc(len + 1);
        if(m_abstract)
        {
            memcpy(m_abstract,pcAbstract,len);
            m_abstract[len] = '\0';
            ret = 0;
        }
    }
    return ret;
}



void CDKFile::SetFileCategory(DkFormatCategory category)
{
    m_category = category;
}

void CDKFile::SetFileFormat(DkFileFormat Format)
{
    m_format = Format;
}

void CDKFile::SetFileSource(DK_FileSource Source)
{
    m_source = Source;
}

void CDKFile::SetIsTrialBook(bool isTrailBook)
{

}

bool CDKFile::GetIsTrialBook()
{
    return FALSE;
}

CDKBook::CDKBook()
        : m_isTrialBook(FALSE)
{

}

CDKBook::~CDKBook()
{

}


void CDKBook::SetIsTrialBook(bool isTrailBook)
{
    m_isTrialBook = isTrailBook;
}

bool CDKBook::GetIsTrialBook()
{
    return m_isTrialBook;
}

std::string CDKBook::GetBookRevision()
{
    return m_bookRevision;
}

void CDKBook::SetBookRevision(const char* bookRevision)
{
    if (NULL != bookRevision)
    {
        m_bookRevision = bookRevision;
    }
}

CDKMedia::CDKMedia()
{

}

CDKMedia::~CDKMedia()
{

}

/** 扫描磁盘
* @param[in] Path：     扫描路径
* @param[in] DirList:   存放扫描结果链表头结点
* @param[in] IsScanFileType：过滤规则
* @param[out] mode：是否扫描子目录  值为1扫描子目录
* @return 扫描链表头结点
*/
DirInfo* ScanFileOnDisk(DirInfo* *DirList, const char* Path, DkFileFormat (*IsScanFileType)(const char* ), bool fSearchSubFolder)
{

    //DebugPrintf(DLC_DIAGNOSTIC, "ScanFileOnDisk on %s", Path); 
    DIR     *dir_ptr = NULL;
    struct stat statbuf;
    struct dirent *entryPtr = NULL;
    struct dirent *direntp = NULL;
    string DirPath("");
    char   TempPath[FILEPATHMAXLEN - FILENAMEMAXLEN]={0};
    if(NULL == Path)
    {
        return NULL;
    }
    direntp = (struct dirent *)malloc(sizeof(struct dirent) + FILEPATHMAXLEN);
    if(NULL == direntp)
    {
        DebugLog(DLC_ERROR,"do_lsRoot malloc error");
        return  NULL;
    }

//  防止路径拼接错误
    int PathLen = strlen(Path);
    strncpy(TempPath,Path,PathLen);
    if(PathLen && Path[PathLen -1] == '/')
    {
        TempPath[PathLen -1] = '\0';
    }

    dir_ptr = opendir(Path);
    if(dir_ptr == NULL)
    {
        DebugPrintf(DLC_ERROR, "open err %s",Path);
        if(direntp)
        {
            free(direntp);
            direntp = NULL;
        }
        return NULL;
    }
    else
    {
        //      chdir(Path);
        while (( readdir_r( dir_ptr, direntp, &entryPtr) == 0 ) && entryPtr != NULL )
        {
            if(strlen( direntp->d_name) > FILENAMEMAXLEN)
            {
                DebugPrintf(DLC_TRACE, "direntp->d_name  too long > %d",FILENAMEMAXLEN);
                continue;
            }

            DirPath = string(TempPath) + string("/") + string(direntp->d_name);
            if(DirPath.c_str() && lstat(DirPath.c_str(),&statbuf) < 0)
            {
                DebugPrintf(DLC_ERROR, "lstat error");
                perror("error");
                continue;
            }

            if(S_ISDIR(statbuf.st_mode))//是目录
            {
                if (('.' == direntp->d_name[0]) || !PathManager::IsValidDirName(direntp->d_name))
                {
                    continue;
                }
                if(fSearchSubFolder)
                {
                    DirPath += string("/");
                    if(DirPath.c_str() && !IsFileWorkDir(DirPath.c_str()))
                    {
                        continue;
                    }
                    *DirList  = ScanFileOnDisk(DirList,DirPath.c_str(),IsScanFileType, fSearchSubFolder);
                }
                else
                {
                    *DirList = AddFileToDirList(*DirList,(LPSTR)DirPath.c_str(), IsScanFileType);
                    continue;
                }
            }
            else
            {
                if(!IsFileWorkDir(DirPath.c_str()))
                {
                    continue;
                }
                if(IsMacOSFileInfo(DirPath.c_str()))
                {
                    continue;
                }
                //              strncpy(DirPath+strlen(DirPath),direntp->d_name,strlen(direntp->d_name)+strlen(DirPath)>FILEPATHMAXLEN ? FILEPATHMAXLEN-strlen(DirPath):strlen(direntp->d_name));
                *DirList = AddFileToDirList(*DirList,(LPSTR)DirPath.c_str(), IsScanFileType);
            }
        }
        //      chdir("..");
        closedir(dir_ptr);
    }
    if(direntp)
    {
        free(direntp);
        direntp = NULL;
    }
    return *DirList;
}

DirInfo* ScanFileInFolder(DirInfo* *DirList, const char* Path, DkFileFormat (*IsScanFileType)(const char* ))
{
    DIR     *dir_ptr = NULL;
    struct stat statbuf;
    struct dirent *entryPtr = NULL;
    struct dirent *direntp = NULL;
    string DirPath("");
    char   TempPath[FILEPATHMAXLEN - FILENAMEMAXLEN]={0};
    if(NULL == Path)
    {
        return NULL;
    }
    direntp = (struct dirent *)malloc(sizeof(struct dirent) + FILEPATHMAXLEN);
    if(NULL == direntp)
    {
        DebugLog(DLC_ERROR,"do_lsRoot malloc error");
        return  NULL;
    }

//  防止路径拼接错误
    int PathLen = strlen(Path);
    strncpy(TempPath,Path,PathLen);
    if(PathLen && Path[PathLen -1] == '/')
    {
        TempPath[PathLen -1] = '\0';
    }

    dir_ptr = opendir(Path);
    if(dir_ptr == NULL)
    {
        DebugPrintf(DLC_ERROR, "open err %s",Path);
        if(direntp)
        {
            free(direntp);
            direntp = NULL;
        }
        return NULL;
    }
    else
    {
        //      chdir(Path);
        while (( readdir_r( dir_ptr, direntp, &entryPtr) == 0 ) && entryPtr != NULL )
        {
            if(strlen( direntp->d_name) > FILENAMEMAXLEN)
            {
                DebugPrintf(DLC_TRACE, "direntp->d_name  too long > %d",FILENAMEMAXLEN);
                continue;
            }

            DirPath = string(TempPath) + string("/") + string(direntp->d_name);
            if(DirPath.c_str() && lstat(DirPath.c_str(),&statbuf) < 0)
            {
                DebugPrintf(DLC_ERROR, "lstat error");
                perror("error");
                continue;
            }

            if(S_ISDIR(statbuf.st_mode))//是目录
            {
                    continue;
            }
            else
            {
                if(IsMacOSFileInfo(DirPath.c_str()))
                {
                    continue;
                }
 //              strncpy(DirPath+strlen(DirPath),direntp->d_name,strlen(direntp->d_name)+strlen(DirPath)>FILEPATHMAXLEN ? FILEPATHMAXLEN-strlen(DirPath):strlen(direntp->d_name));
                *DirList = AddFileToDirList(*DirList,(LPSTR)DirPath.c_str(), IsScanFileType);
            }
        }
        //      chdir("..");
        closedir(dir_ptr);
    }
    if(direntp)
    {
        free(direntp);
        direntp = NULL;
    }
    return *DirList;
}


/** 配合扫盘函数使用，扫描出的节点添加到DirList链表中
* @param[in] DirList:   存放扫描结果链表头结点
* @param[in] IsScanFileType：过滤规则
* @param[in] FilePath 扫描得到的文件
* @return 扫描链表头结点
*/
DirInfo* AddFileToDirList(DirInfo* DirList, LPSTR FilePath, DkFileFormat (*IsScanFileType)(const char* ))
{

    char * pchar = NULL;
    struct stat statbuf;
    DirInfo* pTemp = NULL;
    DkFileFormat fileformat = DFF_Unknown;
    if(NULL == FilePath || 0 == FilePath[0])
    {
        DebugLog(DLC_ERROR, "input param error!");
        return DirList;
    }
    fileformat = IsScanFileType(FilePath);
    if(fileformat)
    {
        DkFormatCategory fileCategory = GetFileCategory(FilePath);
        if(fileCategory == DFC_Picture)
        {
            pchar = strrchr(FilePath,'/');
            if(pchar)
            {
                *pchar = '\0';
            }
            if(SerachDirNodebyPath(DirList,FilePath,fileCategory))
            {
                return DirList;
            }
        }
        pTemp=(DirInfo*)malloc(sizeof(DirInfo));
        if(pTemp ==NULL)
        {
            DebugLog(DLC_ERROR, "malloc err");
            return DirList;
        }
        memset(pTemp,0,sizeof(DirInfo));
        strncpy(pTemp->FilePath,FilePath,(strlen(FilePath) >= FILEPATHMAXLEN ? FILEPATHMAXLEN - 1:strlen(FilePath)));
        if(lstat((pTemp->FilePath),&statbuf) == 0)
        {
            pTemp->FileSize = statbuf.st_size;
        }
        else
        {
            pTemp->FileSize = 0;
        }
        pTemp->FileFormat = fileformat;
        pTemp->FileCategory   = fileCategory;
        pTemp->next = DirList;
        DirList = pTemp;
    }
    return DirList;
}

//过滤图片是要使用。图片只记录其文件夹名，为防止多次记录，需要check
DirInfo* SerachDirNodebyPath(DirInfo* DirList, const char* FilePath,DkFormatCategory fileCategory)
{
    DirInfo* CurNode = DirList;
    if(NULL == FilePath || NULL == DirList)
    {
        return NULL;
    }
    while(CurNode)
    {
        if((fileCategory == CurNode->FileCategory) && (strcmp(CurNode->FilePath,FilePath) == 0))
        {
            return CurNode;
        }
        CurNode = CurNode->next;
    }
    return NULL;
}



struct FILE_EXT_INFO
{
    const char* kszExt;
    DkFormatCategory category;
    DkFileFormat format;
};

#define DEFINE_FILE_FORMAT_EXT_MAPPING(ext, category, format)           \
{ext, category, DFF_##format},

FILE_EXT_INFO g_rgFileExtInfo[] =
{
    FILE_EXT_LIST(DEFINE_FILE_FORMAT_EXT_MAPPING)
};


FILE_EXT_INFO g_rgZipFileExtInfo[] =
{
    FILE_EXT_LIST_ZIP(DEFINE_FILE_FORMAT_EXT_MAPPING)
};

#define FILE_EXT_COUNT  (sizeof(g_rgFileExtInfo)/sizeof(FILE_EXT_INFO))
#define FILE_EXT_COUNT_ZIP  (sizeof(g_rgZipFileExtInfo)/sizeof(FILE_EXT_INFO))

/** 检测文件是否为系统支持的格式，扫描时过滤使用
* @param[in] filename 文件全路径
* @return 文件格式
*/
DkFileFormat GetFileFormatbyExtName(const char* filename)
{
    const char* pchar = NULL;
    if(filename)
    {
        pchar = strrchr(filename,'.');
        if (pchar)
        {
            for (unsigned int iLoop = 0; iLoop < FILE_EXT_COUNT; iLoop++)
            {
                if (!strcasecmp(pchar, g_rgFileExtInfo[iLoop].kszExt))
                {
                    return g_rgFileExtInfo[iLoop].format;
                }
            }
        }
        for (unsigned int iLoop = 0; iLoop < FILE_EXT_COUNT; iLoop++)
        {
            if (strncmp(g_rgFileExtInfo[iLoop].kszExt,filename,strlen(g_rgFileExtInfo[iLoop].kszExt)) == 0)
            {
                return g_rgFileExtInfo[iLoop].format;
            }
        }
    }
    return DFF_Unknown;
}




//TODO 现在图片只有在压缩文件中暂时支持，以后会同GetFileFormatbyExtName
DkFileFormat GetFileFormatbyExtNameForZipFile(const char* filename)
{
    const char *pchar = NULL;
    if(filename)
    {
        pchar = strrchr(filename,'.');
        if (pchar)
        {
            for (unsigned int iLoop = 0; iLoop < FILE_EXT_COUNT_ZIP; iLoop++)
            {
                if (!strcasecmp(pchar, g_rgZipFileExtInfo[iLoop].kszExt))
                {
                    return g_rgZipFileExtInfo[iLoop].format;
                }
            }
        }
    }
    return DFF_Unknown;
}

//TODO 现在图片只有在压缩文件中暂时支持，以后会同GetFileFormatbyExtName
DkFormatCategory GetFileCategoryForZipFile(const char* filename)
{
    const char* pchar = NULL;
    if(filename)
    {
        pchar = strrchr(filename,'.');
        if (pchar)
        {
            for (unsigned int iLoop = 0; iLoop < FILE_EXT_COUNT_ZIP; iLoop++)
            {
                if (!strcasecmp(pchar, g_rgZipFileExtInfo[iLoop].kszExt))
                {

                    return g_rgZipFileExtInfo[iLoop].category;
                }
            }
        }
    }
    return DFC_Unknown;
}

/** 获取格式的扩展名
* @param[in] eFormat 文件格式
* @return 文件扩展名
*/
const char *GetExtNameByFormat(DkFileFormat eFormat)
{
    for (unsigned int iLoop = 0; iLoop < FILE_EXT_COUNT; iLoop++)
    {
        if (g_rgFileExtInfo[iLoop].format == eFormat)
        {
            return g_rgFileExtInfo[iLoop].kszExt;
        }
    }

    return NULL;
}

/** 得到文件所属的类型
* @param[in] filename 文件全路径
* @return 文件格式
*/
DkFormatCategory GetFileCategory(const char* filename)
{
    const char* pchar = NULL;
    if(filename)
    {
        pchar = strrchr(filename,'.');
        if (pchar)
        {
            for (unsigned int iLoop = 0; iLoop < FILE_EXT_COUNT; iLoop++)
            {
                if (!strcasecmp(pchar, g_rgFileExtInfo[iLoop].kszExt))
                {

                    return g_rgFileExtInfo[iLoop].category;
                }
            }
        }

        for (unsigned int iLoop = 0; iLoop < FILE_EXT_COUNT; iLoop++)
        {
            if (strncmp(g_rgFileExtInfo[iLoop].kszExt,filename,strlen(g_rgFileExtInfo[iLoop].kszExt)) == 0)
            {
                return g_rgFileExtInfo[iLoop].category;
            }
        }
    }
    return DFC_Unknown;
}


bool IsFileWorkDir(const char* dirPath)
{
    bool bIsWorkDir = false;
    if(NULL == dirPath)
    {
        return bIsWorkDir;
    }


    for(unsigned int i = 0; i < FILE_SCANDIR_SIZE; i++)
    {
        if(strncmp(g_rgFileWorkList[i].dirPath,dirPath,strlen(g_rgFileWorkList[i].dirPath)) == 0)
        {
            bIsWorkDir = true;
            break;
        }
    }
    return bIsWorkDir;
}


const char* GetFileCommonPath(const char* pFileAllPath)
{
    if(NULL == pFileAllPath)
    {
        return NULL;
    }

    const char* pCommonPath = NULL;
    for(unsigned int i = 0; i < FILE_SCANDIR_SIZE; i++)
    {
        if(strstr(pFileAllPath,g_rgFileWorkList[i].dirPath))
        {
            pCommonPath = pFileAllPath + strlen(g_rgFileWorkList[i].dirPath);
            break;
        }
    }
    return pCommonPath;
}


CDKMedia*    DK_FileFactory::CreateMediaFileInfo()
{
    return NULL;
}

CDKBook*     DK_FileFactory::CreateBookFileInfo()
{
    return NULL;
}

CDKBook*     DK_FileFactory::CreateBookFileInfoFromPush(PCCH path, int filesize)
{
    CDKBook* pBookFile = new CDKBook();
    if (pBookFile)
    {
        pBookFile->SetFilePath(path);
        pBookFile->SetFileSource(WEBSERVER);
        pBookFile->SetFileSize(filesize);
    }

    return pBookFile;
}
CDKBook*     DK_FileFactory::CreateBookFileInfoFromDownload(PCCH path, PCCH author, DK_FileSource internetSource, PCCH abstract, PCCH password, int filesize)
{
    CDKBook* pBookFile = new CDKBook();
    if (pBookFile)
    {
        pBookFile->SetFilePath(path);
        pBookFile->SetFileArtist(author);
        pBookFile->SetFileSource(internetSource);
        pBookFile->SetFileAbstract(abstract);
        pBookFile->SetFilePassword(password);
        pBookFile->SetFileSize(filesize);
    }

    return  pBookFile;
}


FileKeywordMatcher::FileKeywordMatcher(const char* keyword)
        : m_keyword(keyword)
        , m_matchByFirstLetters(false)
{
    if (NULL != keyword)
    {
        m_wKeyWord = StringUtil::ToLower(EncodeUtil::ToWString(keyword).c_str());
        while (('a' <= *keyword && *keyword <= 'z')
            || ('A' <= *keyword && *keyword <= 'Z'))
        {
            ++keyword;
        }
        if (0 == *keyword)
        {
            m_matchByFirstLetters = true;
        }
    }
}

bool FileKeywordMatcher::operator()(const PCDKFile file) const
{
    if (m_wKeyWord.empty())
    {
        return false;
    }
    // 7 hardcode for /mnt/us
    std::wstring wPath;
    if (!file->IsDuoKanBook())
    {
        CDKFileManager* fileManager = CDKFileManager::GetFileManager();
        if (fileManager->GetBookSort() == DIRECTORY)
        {
            wPath = StringUtil::ToLower(EncodeUtil::ToWString(file->GetFilePath()).c_str() + 7);
        }
        else
        {
            wPath = StringUtil::ToLower(EncodeUtil::ToWString(PathManager::GetFileName(file->GetFilePath())).c_str());
        }
        const wchar_t* lastDot = wcsrchr(wPath.c_str(), L'.');
        if (NULL == lastDot)
        {
            return false;
        }
        wPath = wPath.substr(0, lastDot - wPath.c_str());
    }
    
    DK_AUTO(pos, wPath.find(m_wKeyWord));

    if (pos != std::wstring::npos)
    {
        return true;
    }
    std::wstring wName = StringUtil::ToLower(EncodeUtil::ToWString(file->GetFileName()).c_str());
    if (wName.find(m_wKeyWord) != std::wstring::npos)
    {
        return true;
    }
    if (!m_matchByFirstLetters)
    {
        return false;
    }
	if(StringUtil::MatchByFirstLetter(wPath, m_wKeyWord))
	{
		return true;
	}
	if(StringUtil::MatchByFirstLetter(wName, m_wKeyWord))
	{
		return true;
	}
    return false;
}

int CDKFile::GetPageCount() const
{
    if (-1 != m_pageCount)
    {
        return m_pageCount;
    }
    CPageTable pageTable(GetFilePath(), DK_GetKernelVersion(), -1, false);
    std::string md5 = SystemSettingInfo::GetInstance()->
                            GetDuokanBookMD5(-1, -1);

    PageTable pageTableData = pageTable.GetMatchedOrFirstPageTable(md5);
    m_pageCount = pageTableData.IsNull() ? -1 : pageTableData.pageCount;
    return m_pageCount;
}

void CDKFile::SetPageCount(int pageCount)
{
    m_pageCount = pageCount;
}

///////////////////////////////////////////////////////////////////////////////
//
// class FileComparer
//
///////////////////////////////////////////////////////////////////////////////
bool FileComparer::operator()(const PCDKFile lhs, const PCDKFile rhs) const
{
    switch (m_type)
    {
        case CT_Name:
            return CharUtil::StringCompareByDisplay(lhs->GetGbkName(),
                    rhs->GetGbkName()) < 0;
        case CT_AddingOrder:
            return lhs->GetFileAddOrder() > rhs->GetFileAddOrder();
        case CT_ReadingOrder:
            return lhs->GetFileReadingOrder() > rhs->GetFileReadingOrder();
        default:
            return false;
    }
}
