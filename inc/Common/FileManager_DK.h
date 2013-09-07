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

#ifndef _DK_FILEMANAGER_H_
#define _DK_FILEMANAGER_H_

#include "Common/File_DK.h"
#include "FileSorts_DK.h"
#include "BookInfoManager.h"
#include "Bookmark.h"
#include "DKXManager/BaseType/CT_RefPos.h"
#include "DKXManager/DKX/DKXManager.h"
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"


using namespace DkFormat;

#define ManualMaxFilenameLen 32

typedef enum FileDkpOperate
{
    UNKNOWOPERATE = 0
    ,GETFILEINFO
    ,SAVEFILEINFO
    ,GETFILETAG
    ,SAVEFILETAG
    ,GETSERIALIZEDBOOKINFO
    ,SAVESERIALIZEDBOOKINFO
}DK_FileDkpOperate;

class FileListChangedArgs: public EventArgs
{
public:
    FileListChangedArgs()
    {
    };
    virtual ~FileListChangedArgs()
    {
    }
    virtual EventArgs* Clone() const
    {
        return new FileListChangedArgs(*this);
    }

};
// 系统文件管理
class CDKFileManager: public EventSet
{
    CDKFileManager();
    ~CDKFileManager();
public:
    static const char* EventFileListChanged;

    void FireFileListChangedEvent();

    bool HasBookmark(DkFormatCategory category, int iBookId);

    int GetFileNum() const;
    int GetBookCurSortNum() const { return m_sortedFiles.size(); }

    DK_FileSorts    GetBookSort() const;
    void SetBookSortType(DK_FileSorts sort);
    PCDKFile GetBookbyListIndex(int index) const;

    int GetMeidaCurSortNum() const;
    DK_FileSorts    GetMeidaSort() const;
    PCDKFile        GetMeidabyListIndex(int index);
    void SetMediaSortType(DK_FileSorts sort);

    void SearchBook(const char* KeyWord);

    static CDKFileManager * GetFileManager();

    int GetAllBookNumsUnderDir(const char* path) const;
    int GetAllBookNumsUnderDir(const std::string& path) const
    {
        return GetAllBookNumsUnderDir(path.c_str());
    }

    int AddFile(PCDKFile pNewFile);
    bool RemoveFileNode(PCDKFile pRemoveNode);
    int AddDownloadBookFile(PCCH path, PCCH author, DK_FileSource internetSource, PCCH abstract, PCCH password, int filesize);
    bool DelFile(PCDKFile pDelFile);
    bool DelFile(const char* path);
    bool DelFile(const std::string& path)
    {
        return DelFile(path.c_str());
    }
    bool DelFile(int fileId);
    int LoadFileToFileManger(const char* Path, bool scanSubFolder);
    int ReLoadFileToFileManger(const char* Path, bool scanSubFolder);
    int SortFile(DkFormatCategory category);

    void UnloadAllFile();

    int GetFileIdByPath(const char* path);
    int GetFileIdByPath(const std::string& path)
    {
        return GetFileIdByPath(path.c_str());
    }
    PCDKFile GetFileById(int Id);
    PCDKFile GetFileByBookId(const char* bookId);
    PCDKFile GetFileByPath(const char* path);
    PCDKFile GetFileByPath(const std::string& path)
    {
        return GetFileByPath(path.c_str());
    }
    PCDKFile GetFileByName(const std::string& name, int64_t size);

    bool GetFileIDList(std::vector<int>* fileIDList);


    HRESULT FileDkpProxy(PCDKFile pCurFile,DK_FileDkpOperate OperateType);
    int CalcProgress(const ICT_ReadingDataItem * const _pclsReadingItem);
    HRESULT ConvertDKPToDKX(const char* _pchFilepath);

    void SetReadingOrder(long _lReadingOrder) { m_lReadingOrder = _lReadingOrder; }
    long  GetReadingOrder() const { return m_lReadingOrder; }
    void ReadingOrderPlus();
    void SetMetaData(bool isOpen);
    bool IsInSearch() const;
    std::vector<std::string> GetDuoKanBookIds() const;

    int RenameFile(PCDKFile file, const char* newName);
    int RenameFile(PCDKFile file, const std::string& newName)
    {
        return RenameFile(file, newName.c_str());
    }
    bool RenameFolder(const char* source, const char* dest);
    bool RenameFolder(const std::string& source, const std::string& dest)
    {
        return RenameFolder(source.c_str(), dest.c_str());
    }
    string GetBookID(const char* bookPath);

protected:
    static bool SortReadbyRecentlyTime(PCDKFile pSource1,PCDKFile pSource2);

    HRESULT GetFileInfoFromDkx(PCDKFile pCurBook);
    HRESULT GetFileInfoFromDkp(PCDKFile pCurBook,IBookInfoProxy *pFileInfo);
    HRESULT SaveFileInfoToDkp(PCDKFile pCurBook,IBookInfoProxy *pFileInfo);

    int GetChapterCount(const char* filePath);
    

    HRESULT ConvertBookInfoToDKX(IBookInfoEntity *dkpEntity, const char* filePath);
    HRESULT ConvertSerialBookInfoToDKX(IBookInfoEntity *dkpEntity, const char* filePath);
    HRESULT ConvertBookMarkToDKX(IBookInfoEntity *dkpEntity, const char* filePath);
    

    HRESULT InitFileDKX(std::string _strFilePath);
    /*
    *从老版本的dkp的locator中获取阅读进度，存入指定的对象。该方法会释放给定的locator对象
    */
    HRESULT GetProgressFromLocator(IBookLocator *_pLocatorAtZero, CT_RefPos &_RefPos, string &_clsFileFormat);

private:
    bool IsPushedBook(const string& strFilePath);
    bool IsDuokanBook(const string& strFilePath);

    int m_iMaxFileId;

    //书籍相关
    DKFileList m_files;
    DKFileList m_sortedFiles;
    DK_FileSorts m_BookSort;

    //媒体相关
    DKFileList m_sortedMediaFiles;
    DK_FileSorts m_MediaSort;

    //此处管理全局的ReadingOrder，其他使用该值的地方都使用该值调用,AddOrder的值使用ReadingOrder即可
    int m_lReadingOrder;
    bool m_inSearch;
};
#endif /*_DK_FILEMANGER_H_*/

