
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


#include "FileManager_DK.h"
#include "interface.h"
#include "Utility.h"
#include "dkWinError.h"
#include <time.h>
#include "SQM.h"
#include "../inc/Common/SystemSetting_DK.h"
#include "TextLocatorImpl.h"
#include "KernelType.h"
#include "DKEAPI.h"
#include "../inc/Common/SystemSetting_DK.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "BookReader/EpubBookReader.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/LocalCategoryManager.h"
#include "Utility/PathManager.h"
#include "Common/IFileChapterInfo.h"
#include "Common/FileContentsFactory.h"
#include "Model/IFileContentsModel.h"
#include "I18n/StringManager.h"
#include <memory>
#include "Utility/SystemUtil.h"
#include "Utility/FileSystem.h"
#include "Utility/Misc.h"
#include "Utility/StringUtil.h"
#include "DkFilterFactory.h"

using dk::utility::SystemUtil;
using dk::utility::StringUtil;
using dk::utility::PathManager;
using namespace dk::bookstore;

//#define FILEMANAGER_DEBUG

using namespace DkFormat;

const char* CDKFileManager::EventFileListChanged = "EventFileListChanged";
CDKFileManager::CDKFileManager()
               : m_iMaxFileId(0)

{
    m_BookSort = SystemSettingInfo::GetInstance()->GetBookSortType();
    if(UnknowSort == m_BookSort)
    {
        SetBookSortType(RecentlyReadTime);
    }

    m_MediaSort = SystemSettingInfo::GetInstance()->GetMediaSortType();
    if(UnknowSort == m_MediaSort)
    {
        SetMediaSortType(RecentlyReadTime);
    } 
    m_lReadingOrder = SystemSettingInfo::GetInstance()->GetInitReadingOrder();
    m_inSearch = false;
}

CDKFileManager::~CDKFileManager()
{
    //书籍相关
    UnloadAllFile();
}

void CDKFileManager::FireFileListChangedEvent()
{
    FileListChangedArgs args;
    FireEvent(EventFileListChanged, args);
}

CDKFileManager * CDKFileManager::GetFileManager()
{
    static CDKFileManager s_fileManager;
    return &s_fileManager;
}


void CDKFileManager::SearchBook(const char* keyword)
{
    m_inSearch = true;
    m_sortedFiles.clear();
    DKFileList tmpFiles;
    FileKeywordMatcher matcher(keyword); 
    copy_if(m_files.begin(), m_files.end(), std::back_inserter(tmpFiles), matcher);

    FileCategoryMatcher categoryMatcher(DFC_Book);
    copy_if(tmpFiles.begin(), tmpFiles.end(), std::back_inserter(m_sortedFiles), categoryMatcher);
    return;
}

int CDKFileManager::GetFileNum()const
{
    return m_files.size();
}

DK_FileSorts    CDKFileManager::GetBookSort()const
{
    return (m_BookSort);
}

DK_FileSorts    CDKFileManager::GetMeidaSort()const
{
    return (m_MediaSort);
}

/*
*Make sure that only this method can change the value of m_BookSort!!!! If you want to change its value
*temporarily, please backup it firstly, and then restore it finally!!!!
*/
void CDKFileManager::SetBookSortType(DK_FileSorts sort)
{
    if(m_BookSort != sort)
    {
        m_BookSort = sort;
        SystemSettingInfo::GetInstance()->SetBookSortType(sort);
    }
}

/*
*Make sure that only this method can change the value of m_BookSort!!!! If you want to change its value
*temporarily, please backup it firstly, and then restore it finally!!!!
*/
void CDKFileManager::SetMediaSortType(DK_FileSorts sort)
{
    if(m_MediaSort != sort)
    {
        m_MediaSort = sort;
        SystemSettingInfo::GetInstance()->SetMediaSortType(sort);
    }
}



bool CDKFileManager::IsPushedBook(const string& strFilePath)
{
    return StringUtil::StartWith(strFilePath.c_str(), PathManager::GetNewsPath().c_str());
}

bool CDKFileManager::IsDuokanBook(const string& strFilePath)
{
    return StringUtil::StartWith(strFilePath.c_str(), PathManager::GetBookStorePath().c_str());
}

int CDKFileManager::AddFile(PCDKFile pNewFile)
{
    if(pNewFile == NULL || pNewFile->GetFilePath() == NULL)
    {
        return -1;
    }

    const char*   pszFilePath = pNewFile->GetFilePath();
    DkFormatCategory FileCategory = GetFileCategory(pszFilePath);

    switch (FileCategory)
    {
        case DFC_Book:
        case DFC_Picture:
        case DFC_Music:
        case DFC_Video:
            break;
        default:
            return -1;//break, and return error
    }
    pNewFile->SetFileCategory(FileCategory);
    pNewFile->SetFileID(++m_iMaxFileId);
    pNewFile->SetFileFormat(GetFileFormatbyExtName(pszFilePath));
    
    if(StringUtil::IsNullOrEmpty(pNewFile->GetFileName()))
    {
        pNewFile->SetFileName(PathManager::GetFileName(pszFilePath).c_str());
    }

    m_files.push_back(pNewFile);
    //      更新索引

    ReadingOrderPlus();
    DKXManager* _pclsDKXManager = DKXManager::GetInstance();
    const int newBookReadingOrder = (IsPushedBook(pszFilePath) || IsDuokanBook(pszFilePath)) ? m_lReadingOrder : -1;
    string bookID = GetBookID(pszFilePath);
    if(_pclsDKXManager)
    {
        _pclsDKXManager->SetBookCreateTime(pszFilePath, CUtility::TransferTimeToString(time(NULL)));
        _pclsDKXManager->SetBookReadingOrder(pszFilePath, newBookReadingOrder);
        _pclsDKXManager->SetBookAddOrder(pszFilePath, m_lReadingOrder);
        _pclsDKXManager->SetBookRevision(pszFilePath, pNewFile->GetBookRevision().c_str());
        _pclsDKXManager->SetBookID(pszFilePath, bookID.c_str());
        _pclsDKXManager->SetSync(pszFilePath, false);
    }
    pNewFile->SetFileReadingOrder(newBookReadingOrder);
    pNewFile->SetFileAddOrder(m_lReadingOrder);
    pNewFile->SetBookID(bookID.c_str());

    FileDkpProxy(pNewFile,SAVEFILEINFO);
    
    SortFile(DFC_Book);
    return pNewFile->GetFileID();
}

int CDKFileManager::AddDownloadBookFile(PCCH path, PCCH author, DK_FileSource internetSource, PCCH abstract, PCCH password, int filesize)
{
    PCDKFile pBookFile = new CDKBook();
    if(pBookFile)
    {
        pBookFile->SetFilePath(path);
        pBookFile->SetFileArtist(author);
        pBookFile->SetFileSource(internetSource);
        pBookFile->SetFileAbstract(abstract);
        pBookFile->SetFilePassword(password);
        pBookFile->SetFileSize(filesize);
    }
    return AddFile(pBookFile);//TODO: merge the Addfile code, ?addfile????new
}

HRESULT CDKFileManager::GetProgressFromLocator(IBookLocator *_pLocatorAtZero, CT_RefPos &_RefPos, string &_strFilePath)
{
    if(!_pLocatorAtZero || _strFilePath.empty())
    {
        return E_POINTER;
    }
    
    CTextLocatorImpl *_pclsTextLocator = (CTextLocatorImpl *)_pLocatorAtZero;
    if(!_pclsTextLocator)
    {
        return E_POINTER;
    }
    int _iBlockIndex = -1;
    if(FAILED(_pclsTextLocator->GetBlockId(_iBlockIndex)))
    {
        SafeDeletePointerEx(_pLocatorAtZero);
        return E_FAIL;
    }
    int _iBlockOffset = -1;    
    if(FAILED(_pclsTextLocator->GetBlockOffset(_iBlockOffset)))
    {
        SafeDeletePointerEx(_pLocatorAtZero);
        return E_FAIL;
    }
    if(-1 == _iBlockIndex || -1 == _iBlockOffset)
    {
        SafeDeletePointerEx(_pLocatorAtZero);
        return E_FAIL;
    }

    DkFileFormat _clsFileFormat = GetFileFormatbyExtName(_strFilePath.c_str());

    _RefPos.SetChapterIndex(0);
    if(DFF_Text == _clsFileFormat)
    {
        _RefPos.SetParaIndex(_iBlockOffset);
        _RefPos.SetAtomIndex(_iBlockIndex);
    }
    else if(DFF_ElectronicPublishing == _clsFileFormat)
    {
        //Get paragraph _iBlockIndex 's content and convert them into utf-8
        //Get char offset for input _iBlockOffset
        EpubBookReader *_pBookReader = new EpubBookReader();
        if(!_pBookReader || !_pBookReader->OpenDocument(_strFilePath))
        {
            SafeDeletePointerEx(_pBookReader);
            return false;
        }
        IDKEBook * _pDKEBook = _pBookReader->GetDKEBooK();
        if (!_pDKEBook)
        {
            SafeDeletePointerEx(_pBookReader);
            return E_FAIL;
        }
            
        long _lCount = _pDKEBook->GetChapterCount();
        DK_WCHAR* pDKWCHAR = NULL;
        if(_iBlockIndex >= _lCount)
        {
            _iBlockIndex = _lCount - 1;
        }
        
        if(DK_FAILED(_pDKEBook->GetChapterText(_iBlockIndex, &pDKWCHAR)))
        {
            _pDKEBook->FreeChapterText(pDKWCHAR);
            SafeDeletePointerEx(_pBookReader);
            return E_FAIL;
        }
        DK_CHAR* _pchar = EncodingUtil::WCharToChar(pDKWCHAR);
        std::auto_ptr<CharIter> iter(EncodingUtil::CreateCharIter((const DK_BYTE*)_pchar, strlen(_pchar), EncodingUtil::UTF8));
        DK_UINT _uCharOffset = iter->AdvanceBytes(_iBlockOffset);
        _RefPos.SetChapterIndex(_iBlockIndex);
        _RefPos.SetParaIndex(0);
        _RefPos.SetAtomIndex(_uCharOffset);

        _pDKEBook->FreeChapterText(pDKWCHAR);
        SafeFreePointer(_pchar);
        SafeDeletePointerEx(_pBookReader);
    }
    else if(DFF_ZipFile == _clsFileFormat || DFF_RoshalArchive == _clsFileFormat)//存储当前进度
    {
        _RefPos.SetChapterIndex(_iBlockIndex);
        _RefPos.SetParaIndex(0); 
        _RefPos.SetAtomIndex(0);
    }
    else
    {
        _RefPos.SetChapterIndex(_iBlockIndex);
        _RefPos.SetParaIndex(0); 
        _RefPos.SetAtomIndex(0);
    }

    SafeDeletePointerEx(_pLocatorAtZero);

    return S_OK;

}


HRESULT  CDKFileManager::ConvertDKPToDKX(const char* filePath)
{
    if(!filePath || !filePath[0])
    {
        return E_POINTER;
    }

    static long int lBooknumber = 0;
    static long int lCostTime = 0;
    lBooknumber++;
    time_t tBeginTime = time(NULL);

    string _strBookPath(filePath);
    if(!access(PathManager::BookFileToDkxFile(filePath).c_str(), F_OK))//如果DKX文件存在，则不需要转换
    {
        _strBookPath.assign(filePath);
        _strBookPath.append(".dkp2");
        unlink(_strBookPath.c_str());
        return S_OK;
    }

    _strBookPath.assign(filePath);
    if(access(_strBookPath.append(".dkp2").c_str(), F_OK)) //如果DKP文件不存在，则新生成DKX文件
    {
        InitFileDKX(string(filePath));
        return S_OK;
    }
    
    IBookInfoEntity *dkpEntity     = NULL;
    HRESULT hr = CBookInfoManager::GetBookInfoEntity(filePath, &dkpEntity, false);
    if(FAILED(hr) || !dkpEntity )//没有DKP文件，则为该文件生成DKX文件
    {
        DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertDKPToDKX NULL dkp entity!!!! for file %s", filePath);
        SafeDeletePointerEx(dkpEntity);
        InitFileDKX(string(filePath));
        return hr;
    }

    ConvertBookInfoToDKX(dkpEntity, filePath);
    
    ConvertBookMarkToDKX(dkpEntity, filePath);
    
    DKXManager *pclsDkx = DKXManager::GetInstance();
    if(pclsDkx)
    {
        DebugPrintf(DLC_XU_KAI,"Begin to Save DKX for : %s",filePath);
        pclsDkx->SaveCurDkx();
    }
    //Delete the old dkp file
    _strBookPath.assign(filePath);
    _strBookPath.append(".dkp2");
    if(unlink(_strBookPath.c_str()))
    {
        DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertDKPToDKX Failed to delete old DKP file: %s", _strBookPath.c_str());
    }

    SafeDeletePointerEx(dkpEntity);
    time_t tEndTime = time(NULL);
    long lCost = (long)difftime(tEndTime, tBeginTime);
    lCostTime += lCost;
    DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertDKPToDKX have convert %s successfully with %ld seconds, total seconds=%d, total books=%d", filePath, lCost, lCostTime, lBooknumber);
    return hr;
}

HRESULT  CDKFileManager::ConvertBookInfoToDKX(IBookInfoEntity *dkpEntity, const char* filePath)
{
    if(!dkpEntity || !filePath)
    {
        return E_POINTER;
    }

    DKXManager* _pclsDKXManager = DKXManager::GetInstance();
    if(!_pclsDKXManager)
    {
        return E_POINTER;
    }
    string _strBookPath(filePath);

    IBookInfoProxy *_pclsBookInfo = NULL;
    HRESULT hr = dkpEntity->GetInfoProxy(&_pclsBookInfo);
    if(FAILED(hr) || !_pclsBookInfo)
    {
        SafeDeletePointerEx(_pclsBookInfo);
        return  hr;
    }

    char _BookUtfName[FILENAMEMAXLEN] = {0};
    char *_pcBookName = NULL;

    memset(_BookUtfName,0,FILENAMEMAXLEN);
    if(!FAILED(_pclsBookInfo->GetBookName(&_pcBookName)))
    {
        sprintf(_BookUtfName,"%s",_pcBookName);
        SafeDeleteArrayEx(_pcBookName);
    }
    if(!_BookUtfName[0])
    {
        const char* pchar = strrchr(filePath,'/');
        if(pchar)
        {
            pchar++;
            sprintf(_BookUtfName,"%s",pchar);
        }
    }
    //  book name utf to gbk
    _pclsDKXManager->SetBookName(_strBookPath, string(_BookUtfName));
    char *_pcAuthorName = NULL;
    if(FAILED(_pclsBookInfo->GetAuthorName(&_pcAuthorName)))
    {
        DebugPrintf(DLC_LIUJT, "Failed convert Book author %s for file %s",_pcAuthorName, _strBookPath.c_str());
        _pcAuthorName = NULL;
    }
    else if(_pcAuthorName)
    {
        DebugPrintf(DLC_LIUJT, "convert Book author %s for file %s",_pcAuthorName, _strBookPath.c_str());
        _pclsDKXManager->SetBookAuthor(_strBookPath, string(_pcAuthorName));
        SafeDeleteArrayEx(_pcAuthorName);
    }
    DkBookSource _pBookSource(DBS_UNKNOWN);
    if(SUCCEEDED(_pclsBookInfo->GetBookSource(&_pBookSource)))
    {
        char _szSource[10] = {0};
        DK_FileSource _cslSource = _pBookSource == DBS_FROM_NETWORK ? WEBSERVER : UsbCopy;
        snprintf(_szSource, sizeof(_szSource), "%d", (int)_cslSource);
        _pclsDKXManager->SetBookSource(_strBookPath, string(_szSource));
    }

    //File Format
    DkFileFormat _clsFileFormat = GetFileFormatbyExtName(_strBookPath.c_str());
    char _szFormat[5] = {0};
    snprintf(_szFormat, sizeof(_szFormat), "%d", (int)_clsFileFormat);
    _pclsDKXManager->SetBookFileType(_strBookPath, string(_szFormat));

    char *_pcSummary = NULL;
    if(FAILED(_pclsBookInfo->GetSummary(&_pcSummary)))
    {
        _pcSummary = NULL;
    }
    else if(_pcSummary)
    {
        _pclsDKXManager->SetBookAbstract(_strBookPath, string(_pcSummary));
        SafeDeleteArrayEx(_pcSummary);
    }

    unsigned int _uSize = 0;
    _pclsBookInfo->GetFileSize(_uSize);

    if(!_uSize)
    {
        struct stat statbuf;
        if(lstat((_strBookPath.c_str()),&statbuf) == 0)
        {
            _uSize = statbuf.st_size;
        }
    }

    char _szSize[40] = {0};
    snprintf(_szSize, sizeof(_szSize), "%d", _uSize);
    _pclsDKXManager->SetBookSize(_strBookPath, string(_szSize));

    IBookmark *_pBookmark = NULL;
    
    ICT_ReadingDataItem* _pclsReadingItem = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);
    if(!_pclsReadingItem)
    {
        return E_POINTER;
    }
    
    hr = _pclsBookInfo->GetReadingBookmark(&_pBookmark);

    if (SUCCEEDED(hr) && _pBookmark != NULL)
    {
        int _iLocatorCount  = 0;
        _pBookmark->GetLocatorCount(&_iLocatorCount);

        /*EBookmarkType _clsMarkType = DK_BOOK_SUMMARY;
        _pBookmark->GetType(_clsMarkType);*/

        time_t _tCreationTime;
        _pBookmark->GetTime(_tCreationTime);
        IBookLocator* _pclsLocator = NULL;
        BookLocatorType eType = FILE_LOCATOR_BASE;
        if(FAILED(_pBookmark->GetLocator(&_pclsLocator, 0)) || !_pclsLocator || FAILED(_pclsLocator->GetType(eType)) || eType != FILE_LOCATOR_BASE)
        {
            SafeDeletePointerEx(_pclsLocator);
            SafeDeletePointerEx(_pBookmark);
            return hr;
        }
        CT_RefPos _clsStartRefPos;
        if(FAILED(GetProgressFromLocator(_pclsLocator, _clsStartRefPos, _strBookPath)))
        {
            SafeDeletePointerEx(_pclsLocator);
            SafeDeletePointerEx(_pBookmark);
            return hr;            
        }
        _pclsReadingItem->SetCreateTime(CUtility::TransferTimeToString(_tCreationTime));
        _pclsReadingItem->SetLastModifyTime(CUtility::TransferTimeToString(_tCreationTime));
    
        _pclsReadingItem->SetBeginPos(_clsStartRefPos);
    }
    else
    {
        time_t _tCreationTime = time(NULL);
        _pclsReadingItem->SetCreateTime(CUtility::TransferTimeToString(_tCreationTime));
        _pclsReadingItem->SetLastModifyTime(CUtility::TransferTimeToString(_tCreationTime));
    }

    CT_RefPos _clsEndRefPos;
        
    if(DFF_Text == _clsFileFormat)//如果为Txt，booksize即为最大偏移，用来计算百分比！
    {
        _clsEndRefPos.SetAtomIndex(_uSize);
    }
    else if(DFF_ZipFile == _clsFileFormat || DFF_RoshalArchive == _clsFileFormat)
    {
        _clsEndRefPos.SetChapterIndex(GetChapterCount(_strBookPath.c_str()));//Zip/Rar中图片总数
        DebugPrintf(DLC_LIUJT, "Get Chapter Count=%d for %s", _clsEndRefPos.GetChapterIndex(), _strBookPath.c_str());
    }

    DebugPrintf(DLC_LIUJT, "xxxxxxxxxx_clsEndRefPos.GetAtomIndex()=%d, format =%d, uSize=%d", _clsEndRefPos.GetAtomIndex(), _clsFileFormat, _uSize);
    _pclsReadingItem->SetEndPos(_clsEndRefPos);

    _pclsDKXManager->SetProgress(_strBookPath, _pclsReadingItem);

    DKXReadingDataItemFactory::DestoryReadingDataItem(_pclsReadingItem);
    SafeDeletePointerEx(_pBookmark);

    time_t   _lastreadtime = 0;
    if(FAILED(_pclsBookInfo->GetRecentOpenTime(_lastreadtime)))
    {
        _lastreadtime = 0;
    }
    
    _pclsDKXManager->SetBookLastReadTime(_strBookPath, CUtility::TransferTimeToString(_lastreadtime));
    _pclsDKXManager->SetBookReadingOrder(_strBookPath, _pclsDKXManager->FetchBookReadingOrder(_strBookPath));
    DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertBookInfoToDKX, in dkp, last read time=%s", CUtility::TransferTimeToString(_lastreadtime).c_str());

    char *_pcCurBookPwd = NULL;
    if(SUCCEEDED(_pclsBookInfo->GetPassword(&_pcCurBookPwd)))
    {
        _pclsDKXManager->SetBookPassword(_strBookPath, string(_pcCurBookPwd));
        SafeDeleteArrayEx(_pcCurBookPwd);
    }
    SafeDeletePointerEx(_pclsBookInfo);

    //TODO:::PDF的分栏模式参数也需要从DKP中读出到DKX中！！！！

#if !DONTSAVEDKX
    _pclsDKXManager->SaveCurDkx();
#endif
    DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertBookInfoToDKX End");
    return S_OK;
}

HRESULT  CDKFileManager::ConvertBookMarkToDKX(IBookInfoEntity *dkpEntity, const char* filePath)
{
    DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertBookMarkToDKX Enter");
    if(!dkpEntity || !filePath)
    {
        return E_POINTER;
    }

    DKXManager* _pclsDKXManager = DKXManager::GetInstance();
    if(!_pclsDKXManager)
    {
        return E_POINTER;
    }
    string _strBookPath(filePath);

    IBookmarkProxy *_pBookmarkProxy = NULL;
    HRESULT hr = dkpEntity->GetBookmarkProxy(&_pBookmarkProxy);
    if (FAILED(hr) || !_pBookmarkProxy)
    {
        SafeDeletePointerEx(_pBookmarkProxy);
        return hr;
    }
    vector<IBookmark *> _vBookmarkList;
    _pBookmarkProxy->GetBookmarkList(_vBookmarkList);
    unsigned int _uMarkCount = _vBookmarkList.size();
    DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertBookMarkToDKX bookmark count=%u", _uMarkCount);
    for(unsigned int i = 0; i < _uMarkCount; i++)
    {
        IBookmark * _pMark = _vBookmarkList[i];
        if(!_pMark)
        {
            continue;
        }
        int _iLocatorCount  = 0;
        hr = _pMark->GetLocatorCount(&_iLocatorCount);

        EBookmarkType _clsMarkType = DK_BOOK_SUMMARY;
        hr = _pMark->GetType(_clsMarkType);

        time_t _tCreationTime;
        hr = _pMark->GetTime(_tCreationTime);
        IBookLocator* _pLocatorAtZero = NULL;
        BookLocatorType eType = FILE_LOCATOR_BASE;
        if(FAILED(_pMark->GetLocator(&_pLocatorAtZero, 0)) || !_pLocatorAtZero || FAILED(_pLocatorAtZero->GetType(eType)) || eType != FILE_LOCATOR_BASE)
        {
            SafeDeletePointerEx(_pLocatorAtZero);
            SafeDeletePointerEx(_pBookmarkProxy);
            continue;
        }
        DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertBookMarkToDKX Mark type=%d, locatorcount=%d", _clsMarkType, _iLocatorCount);
        switch (_clsMarkType)
        {
            case DK_BOOK_MARK:
            {//书签
                if(1 == _iLocatorCount)
                {
                    CT_RefPos _clsPos;
                    if(FAILED(GetProgressFromLocator(_pLocatorAtZero, _clsPos, _strBookPath)))
                    {
                        break;
                    }

                    ICT_ReadingDataItem* _pclsReadingItem = DKXReadingDataItemFactory::CreateReadingDataItem();
                     if(!_pclsReadingItem)
                     {
                        break;
                     }
                     _pclsReadingItem->SetCreateTime(CUtility::TransferTimeToString(_tCreationTime));
                    _pclsReadingItem->SetLastModifyTime(CUtility::TransferTimeToString(_tCreationTime));
                    _pclsReadingItem->SetBeginPos(_clsPos);
                    _pclsReadingItem->SetEndPos(_clsPos);
                    _pclsDKXManager->AddBookmark(_strBookPath, _pclsReadingItem);
#if !DONTSAVEDKX
                    _pclsDKXManager->SaveCurDkx();
#endif
                    DKXReadingDataItemFactory::DestoryReadingDataItem(_pclsReadingItem);
                }
                break;
            }
            case DK_BOOK_SUMMARY://书摘
            case DK_BOOK_COMMENT://读者注释
            {
                if(2 == _iLocatorCount)
                {
                    CT_RefPos _clsStartPos, _clsEndPos;
                    if(FAILED(GetProgressFromLocator(_pLocatorAtZero, _clsStartPos, _strBookPath)))
                    {
                        break;
                    }

                    IBookLocator* _pSecondLocator = NULL;
                    BookLocatorType eType = FILE_LOCATOR_BASE;
                    if(FAILED(_pMark->GetLocator(&_pSecondLocator, 1)) || !_pSecondLocator || FAILED(_pSecondLocator->GetType(eType)) || eType != FILE_LOCATOR_BASE)
                    {
                        SafeDeletePointerEx(_pSecondLocator);
                        SafeDeletePointerEx(_pBookmarkProxy);
                        DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertBookMarkToDKX bookmark[%u] continue for null second locator", i);                        
                        break;
                    }
                    if(FAILED(GetProgressFromLocator(_pSecondLocator, _clsEndPos, _strBookPath)))
                    {
                        break;
                    }

                    ICT_ReadingDataItem* _pclsReadingItem = NULL;
                    if(DK_BOOK_COMMENT == _clsMarkType)
                    {
                        _pclsReadingItem =  DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::DIGEST);
                        char* _pcComment = NULL;
                        _pMark->GetComment(&_pcComment);
                        int _iCommentLength = 0;
                        _pMark->GetCommentLength(&_iCommentLength);
                        if(_pcComment)
                        {
                            _pclsReadingItem->SetUserContent(string(_pcComment));
                        }
                        DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertBookMarkToDKX comment[%s] index[%d, %d, %d] to [%d, %d, %d] for %s"
                        , _pcComment
                        , _clsStartPos.GetChapterIndex()
                        , _clsStartPos.GetParaIndex()
                        , _clsStartPos.GetAtomIndex()
                        , _clsEndPos.GetChapterIndex()
                        , _clsEndPos.GetParaIndex()
                        , _clsEndPos.GetAtomIndex()
                        , filePath);
                    }
                    else
                    {
                        _pclsReadingItem =  DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::COMMENT);
                        DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertBookMarkToDKX digest index[%d, %d, %d] to [%d, %d, %d] for %s"
                        , _clsStartPos.GetChapterIndex()
                        , _clsStartPos.GetParaIndex()
                        , _clsStartPos.GetAtomIndex()
                        , _clsEndPos.GetChapterIndex()
                        , _clsEndPos.GetParaIndex()
                        , _clsEndPos.GetAtomIndex()
                        , filePath);
                    }
                    _pclsReadingItem->SetCreateTime(CUtility::TransferTimeToString(_tCreationTime));
                    _pclsReadingItem->SetLastModifyTime(CUtility::TransferTimeToString(_tCreationTime));
                    _pclsReadingItem->SetBeginPos(_clsStartPos);
                    _pclsReadingItem->SetEndPos(_clsEndPos);
                    _pclsDKXManager->AddBookmark(_strBookPath, _pclsReadingItem);
#if !DONTSAVEDKX
                    _pclsDKXManager->SaveCurDkx();
#endif
                }
                break;
            }
        }
    }

    SafeDeletePointerEx(_pBookmarkProxy);
    DebugPrintf(DLC_LIUJT, "CDKFileManager::ConvertBookMarkToDKX End");

    return S_OK;
}

HRESULT  CDKFileManager::InitFileDKX(std::string _strFilePath)
{
    DebugPrintf(DLC_LIUJT, "CDKFileManager::InitFileDKX Entering for %s", _strFilePath.c_str());
    if(_strFilePath.empty())
    {
        return S_OK;
    }
    string::size_type _pos = _strFilePath.rfind('/');
    if(_pos == string::npos)
    {
        return E_FAIL;
    }
    string _strFilename = _strFilePath.substr(_pos+1);
    _pos = _strFilePath.rfind('.');
    if(_pos != string::npos)
    {
        _strFilename = _strFilename.substr(0, _pos);
    }

    DKXManager* _pclsDKXManager = DKXManager::GetInstance();
    if(!_pclsDKXManager)
    {
        return E_POINTER;
    }
    int _iFileSize = 0;
    time_t _tCreationTime = 0;
    struct stat statbuf;
    if(lstat((_strFilePath.c_str()),&statbuf) == 0)
    {
        _iFileSize = statbuf.st_size;
        _tCreationTime = statbuf.st_atime;
    }

    char _szSize[40] = {0};
    snprintf(_szSize, sizeof(_szSize), "%d", _iFileSize);
    //设置初始进度为0
    CT_RefPos _clsStartRefPos;
    ICT_ReadingDataItem* _pclsReadingItem = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);
    if(!_pclsReadingItem)
    {
        return E_POINTER;
    }
    _pclsReadingItem->SetCreateTime(CUtility::TransferTimeToString(_tCreationTime));
    _pclsReadingItem->SetLastModifyTime(CUtility::TransferTimeToString(_tCreationTime));
    _pclsReadingItem->SetBeginPos(_clsStartRefPos);


    CT_RefPos _clsEndRefPos;
    DkFileFormat _clsFileFormat = GetFileFormatbyExtName(_strFilePath.c_str());
    if(DFF_Text == _clsFileFormat)//如果为Txt，booksize即为最大偏移，用来计算百分比！
    {
        _clsEndRefPos.SetAtomIndex(_iFileSize);
    }
    
    _pclsReadingItem->SetEndPos(_clsEndRefPos);

    _pclsDKXManager->SetBookSize(_strFilePath, string(_szSize));
    _pclsDKXManager->SetBookName(_strFilePath, _strFilename);

    DK_FileSource _pBookSource(UsbCopy);
    char _szSource[10] = {0};
    snprintf(_szSource, sizeof(_szSource), "%d", (int)_pBookSource);
    _pclsDKXManager->SetBookSource(_strFilePath, _szSource );
    
    //设置format
    memset(_szSource, 0, sizeof(_szSource));
    snprintf(_szSource, sizeof(_szSource), "%d", (int)_clsFileFormat);
    _pclsDKXManager->SetBookFileType(_strFilePath, _szSource );

    _pclsDKXManager->SetBookCreateTime(_strFilePath, CUtility::TransferTimeToString(_tCreationTime));
    _pclsDKXManager->SetProgress(_strFilePath, _pclsReadingItem);
    SafeDeletePointerEx(_pclsReadingItem);

    DebugPrintf(DLC_LIUJT, "CDKFileManager::InitFileDKX save size=%d, bookname=%s and createtion time %s to dkx", _iFileSize, _strFilename.c_str(), CUtility::TransferTimeToString(_tCreationTime).c_str());
    return _pclsDKXManager->SaveCurDkx() ? S_OK : E_FAIL;
}

int  CDKFileManager::GetChapterCount(const char* filePath)
{
    if(!filePath || !filePath[0])
    {
        return 0;
    }

    IFileContentsModel *pCurBookContent = NULL;
    if(S_FALSE == CFileContentsFactory::CreateFileContentsInstance(filePath,&pCurBookContent))
    {
        SafeDeletePointerEx(pCurBookContent);
        return 0;
    }

    if(!pCurBookContent)
    {
        return 0;
    }

    vector < CFileChapterInfo > vChapterList;
    if(!pCurBookContent->GetChapterList(vChapterList))
    {
        SafeDeletePointerEx(pCurBookContent);
        return 0;
    }

    return vChapterList.size();
}


int CDKFileManager::GetFileIdByPath(const char* path)
{
    FilePathMatcher matcher(path);
    DK_AUTO(pos, std::find_if(m_files.begin(), m_files.end(), matcher));
    if (pos != m_files.end())
    {
        return (*pos)->GetFileID();
    }
    return -1;
}

PCDKFile CDKFileManager::GetFileByPath(const char* path)
{
    int fileId = GetFileIdByPath(path);
    if (-1 == fileId)
    {
        return NULL;
    }
    return GetFileById(fileId);
}

PCDKFile CDKFileManager::GetFileById(int id)
{
    FileIDMatcher matcher(id);
    DK_AUTO(pos, std::find_if(m_files.begin(), m_files.end(), matcher));
    if (pos != m_files.end())
    {
        return (*pos);
    }
    return NULL;
}

PCDKFile CDKFileManager::GetFileByBookId(const char* bookId)
{
    BookIdMatcher matcher(bookId);
    DK_AUTO(pos, std::find_if(m_files.begin(), m_files.end(), matcher));
    if (pos != m_files.end())
    {
        return (*pos);
    }
    return NULL;
}

bool CDKFileManager::GetFileIDList(std::vector<int>* fileIDList)
{
    if(!fileIDList)
    {
        return false;
    }
    for (DK_AUTO(cur, m_files.begin()); cur != m_files.end(); ++cur)
    {
        fileIDList->push_back((*cur)->GetFileID());
    }
    return true;
}


bool CDKFileManager::SortReadbyRecentlyTime(PCDKFile pSource1,PCDKFile pSource2)
{
    if(NULL == pSource1 || NULL == pSource2)
    {
        return false;
    }
    if(pSource1->GetFileLastReadTime() < pSource2->GetFileLastReadTime())
    {
        return true;
    }
    return false;
}


bool CDKFileManager::RemoveFileNode(PCDKFile pRemoveNode)
{
    DK_AUTO(pos, std::find(m_sortedFiles.begin(), m_sortedFiles.end(), pRemoveNode));
    if (pos != m_sortedFiles.end())
    {
        m_sortedFiles.erase(pos);
    }
    pos = std::find(m_files.begin(), m_files.end(), pRemoveNode);
    if (pos == m_files.end())
    {
        return false;
    }
    delete *pos;
    m_files.erase(pos);
    return true;
}

bool CDKFileManager::DelFile(const char* path)
{
    int fileId = GetFileIdByPath(path);
    if (-1 == fileId)
    {
        return false;
    }
    return DelFile(fileId);
}

bool CDKFileManager::DelFile(int fileId)
{
    if (fileId < 0)
    {
        return false;
    }
    CDKFile* file = GetFileById(fileId);
    if (NULL == file)
    {
        return false;
    }
    return DelFile(file);
}

bool CDKFileManager::DelFile(PCDKFile toDel)
{
    if (NULL == toDel)
    {
        return false;
    }
    bool duokanBook = toDel->IsDuoKanBook();
    if (duokanBook)
    {
        LocalCategoryManager::RemoveBookFromCategory(
                GetBookID(toDel->GetFilePath()));
    }
    CBookInfoManager::DeleteBookInfoEntity(toDel->GetFilePath());
    const char* pcFilePath = toDel->GetFilePath();
    if(pcFilePath)
    {
        if(FileSystem::DeleteFile(pcFilePath))
        {
            FileSystem::DeleteFile(PathManager::BookFileToDkxFile(pcFilePath));
            FileSystem::DeleteFile(PathManager::BookFileToDkptFile(pcFilePath));
            FileSystem::DeleteFile(PathManager::BookFileToCoverImageFile(pcFilePath));
            FileSystem::DeleteFolder(PathManager::BookFileToDir(pcFilePath));
            if (duokanBook)
            {
                FileSystem::DeleteFile(PathManager::EpubFileToInfoFile(pcFilePath));
            }
        }
    }
    RemoveFileNode(toDel);
    return true;
}

int CDKFileManager::LoadFileToFileManger(const char* strScanPath)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "Begin load file to manager: %s", strScanPath);
    DirInfo* DirList = NULL;
    DirInfo* pTemp   = NULL;
    int         filecount = 0;
    if(strScanPath == NULL)
    {
        return 1;
    }
    pTemp = ScanFileOnDisk(&DirList, strScanPath, GetFileFormatbyExtName, true);
    if( NULL == pTemp)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "No Any Book at %s ", strScanPath);
        return 1;
    }

    //加载书籍信息
    while(pTemp)
    {
        CDKFile* p = NULL;
        switch(pTemp->FileCategory)
        {
        case DFC_Book:
        case DFC_Picture:
            p = new CDKBook();
            break;
        case DFC_Music:
        case DFC_Video:
            p = new CDKMedia();
            break;
        case DFC_Unknown:
        case DFC_Other:
        case DFC_Package:
        default:
            p = NULL;
            break;
        }
        if(p == NULL)
        {
            pTemp=pTemp->next;
            continue;
        }

        p->SetFilePath(pTemp->FilePath);
        p->SetFileFormat(pTemp->FileFormat);
        p->SetFileCategory(pTemp->FileCategory);
        p->SetFileID(filecount);
        p->SetFileSize(pTemp->FileSize);

        string bookName = p->GetFilePath();
        size_t pos = bookName.rfind('/');
        if (pos != string::npos)
        {
            string path = bookName.substr(0,pos);
            if (path == dk::utility::PathManager::GetBookStorePath())
            {
                p->SetFileSource(DUOKAN_BOOKSTORE);
                bookName = bookName.substr(pos + 1);
                pos = bookName.rfind('.');
                string bookId = "";
                if (pos != string::npos)
                {
                    bookId = bookName.substr(0,pos);
                }
                bool isTrail = dk::bookstore::BookStoreInfoManager::GetInstance()->IsLocalFileTrial(bookId.c_str());
                p->SetIsTrialBook(isTrail);
            }
        }
        
        //从DKP中获取书籍更多信息
        if(pTemp->FileCategory != DFC_Music)
        {
            if(FileDkpProxy(p,GETFILEINFO) != S_OK)
            {
                delete p;
                p = NULL;
                pTemp=pTemp->next;
                continue;
            }
        }
        m_files.push_back(p);
        pTemp=pTemp->next;
        filecount++;
    }
    //释放扫盘链表
    pTemp = DirList;
    int scannedFileCount = 0;
    while(DirList)
    {
        ++scannedFileCount;
        pTemp = DirList->next;
        free(DirList);
        DirList=pTemp;
    }
    DebugPrintf(DLC_DIAGNOSTIC, "scanned %s found files %d", strScanPath, scannedFileCount);
    m_iMaxFileId = filecount;
    
    DK_FileSorts eCurFileSort = GetBookSort();
    if (eCurFileSort != RecentlyReadTime)
    {
        DebugPrintf(DLC_CDKFILEMANAGER, "%s, %d, %s, %s eCurFileSort != RecentlyReadTime", __FILE__,  __LINE__, "CDKFileManager", __FUNCTION__);
        SetBookSortType(RecentlyReadTime);
        SortFile(DFC_Book);
        SetBookSortType(eCurFileSort);
    }  
            
    DebugPrintf(DLC_CDKFILEMANAGER, "%s, %d, %s, %s end", __FILE__,  __LINE__, "CDKFileManager", __FUNCTION__);
    return 0;
}



int CDKFileManager::ReLoadFileToFileManger(const char* strScanPath)
{
    if(NULL == strScanPath)
    {
        return 1;
    }
    UnloadAllFile();
    DebugPrintf(DLC_LIUJT, "CDKFileManager::ReLoadFileToFileManger for foloder=%s", strScanPath);
    LoadFileToFileManger(strScanPath);
    SortFile(DFC_Book);
    return 0;
}

HRESULT CDKFileManager::SaveFileInfoToDkp(PCDKFile pCurBook,IBookInfoProxy *pFileInfo)
{
   DebugPrintf(DLC_LIUJT, "CDKFileManager::SaveFileInfoToDkp Entering");
    if(NULL == pCurBook)
    {
        return E_POINTER;
    }

    DKXManager* _pclsDKXManager = DKXManager::GetInstance();
    if(!_pclsDKXManager)
    {
        return E_POINTER;
    }

    const char* pFilePath = pCurBook->GetFilePath();
    if(NULL == pFilePath)
    {
        return E_POINTER;
    }
    DebugPrintf(DLC_LIUJT, "CDKFileManager::SaveFileInfoToDkp Entering for %s",pFilePath);
    string _strFilePath = string(pFilePath);

    const char* pTempPointer = pCurBook->GetFileName();
    if(pTempPointer)
    {
        DebugPrintf(DLC_XU_KAI,"pCurBook->GetFileName() : %s",pTempPointer);
        _pclsDKXManager->SetBookName(_strFilePath, string(pTempPointer));
    }

    pTempPointer = pCurBook->GetFileArtist();
    if(pTempPointer)
    {
        DebugPrintf(DLC_XU_KAI,"pCurBook->GetFileArtist() : %s",pTempPointer);
        _pclsDKXManager->SetBookAuthor(_strFilePath, string(pTempPointer));
    }

    
    //File Format
    DkFileFormat _clsFileFormat = pCurBook->GetFileFormat();
    char _szFormat[5] = {0};
    snprintf(_szFormat, sizeof(_szFormat), "%d", (int)_clsFileFormat);
    _pclsDKXManager->SetBookFileType(_strFilePath, string(_szFormat));

    
    pTempPointer = pCurBook->GetFileAbstract();
    if(pTempPointer)
    {
        _pclsDKXManager->SetBookAbstract(_strFilePath, string(pTempPointer));
    }
    
    pTempPointer = pCurBook->GetFilePassword();
    if(pTempPointer)
    {
        DebugPrintf(DLC_XU_KAI,"pCurBook->GetFilePassword() : %s",pTempPointer);
        _pclsDKXManager->SetBookPassword(_strFilePath, string(pTempPointer));
    }

    DK_FileSource _fileSource = pCurBook->GetFileSource();
    char _szSource[5] = {0};
    snprintf(_szSource, sizeof(_szSource), "%d", (int)_fileSource);
    _pclsDKXManager->SetBookSource(_strFilePath, string(_szSource));

    time_t _lastreadtime = pCurBook->GetFileLastReadTime();
    
    _pclsDKXManager->SetBookLastReadTime(_strFilePath, CUtility::TransferTimeToString(_lastreadtime));//最近阅读时间
    _pclsDKXManager->SetBookReadingOrder(_strFilePath, _pclsDKXManager->FetchBookReadingOrder(_strFilePath));
    _pclsDKXManager->SetBookAddOrder(_strFilePath, _pclsDKXManager->FetchBookAddOrder(_strFilePath));

    int _iBookSize = pCurBook->GetFileSize();
    char _szSize[40] = {0};
    snprintf(_szSize, sizeof(_szSize), "%d", _iBookSize);
    _pclsDKXManager->SetBookSize(_strFilePath, string(_szSize));//文件大小
    _pclsDKXManager->SaveCurDkx();

    return S_OK;
}

HRESULT    CDKFileManager::GetFileInfoFromDkx(PCDKFile pCurBook)
{
    time_t tBeginTime = time(NULL);
    static long lBooknumber = 0;
    static long lCostTime = 0;
    lBooknumber++;
    if(!pCurBook)
    {
        return E_POINTER;
    }
    
    const char* _pcCurBookPath = pCurBook->GetFilePath();
    if(!_pcCurBookPath)
    {
         return E_POINTER;
    }
    DebugPrintf(DLC_LIUJT, "CDKFileManager::GetFileInfoFromDkx for file=%s", _pcCurBookPath);
    string _strFilePath(_pcCurBookPath);
    DKXManager* _pclsDKXManager = DKXManager::GetInstance();
    if(!_pclsDKXManager)
    {
        return E_POINTER;
    }
    bool bNeedWriteDKX = false;

    //get book name and author
    string _strBookName = _pclsDKXManager->FetchBookName(_strFilePath);
    string _strAuthor = _pclsDKXManager->FetchBookAuthor(_strFilePath);
    bool bSync = _pclsDKXManager->FetchSync(_strFilePath);
    pCurBook->SetFileSync(bSync);

    string flieName;
    const char* pchar = strrchr(_pcCurBookPath,'/');
    if(pchar)
    {
        pchar++;
        flieName.assign(pchar);
    }
    
    if (dk::utility::PathManager::IsUnderBookStore(_strFilePath.c_str()))
    {
        string bookId = InterceptFileName(_strFilePath);
        dk::bookstore::BookStoreInfoManager *pManager =  dk::bookstore::BookStoreInfoManager::GetInstance();
        string tmpName = pManager->GetLocalFileTitle(bookId.c_str());
        string tmpAuthor = pManager->GetLocalFileAuthor(bookId.c_str());
        if (!tmpName.empty())
        {
            _strBookName = tmpName;
            _pclsDKXManager->SetBookName(_strFilePath, tmpName);
            bNeedWriteDKX = true;
        }
        
        if (!tmpAuthor.empty())
        {
            _strAuthor = tmpAuthor;
            _pclsDKXManager->SetBookAuthor(_strFilePath, tmpAuthor);
            bNeedWriteDKX = true;
        }
        
        //只有当前没有同步过，且不需要同步时才设置该值
        if(!bSync)
        {
            bool needSync = _strBookName.empty() || _strAuthor.empty();
            if(!needSync)
            {
                _pclsDKXManager->SetSync(_strFilePath, !needSync);
                pCurBook->SetFileSync(true);
                bNeedWriteDKX = true;
            }
        }
    }
    else
    {
        if(!SystemSettingInfo::GetInstance()->GetMetadataOpen())
        {
            _strBookName = flieName;
            _strAuthor = "";
        }
    }

    if( _strBookName.empty())
    {
        _strBookName = flieName;
    }

    DebugPrintf(DLC_LIUJT, "CDKFileManager::GetFileInfoFromDkx for file=%s, bookname=%s", _pcCurBookPath, _strBookName.c_str());

    if(_pclsDKXManager->FetchBookName(_strFilePath).empty())
    {
        //设置bookName
        _pclsDKXManager->SetBookName(_strFilePath, _strBookName);
        bNeedWriteDKX = true;
    }
    pCurBook->SetFileName(_strBookName.c_str());

    const char* pcAuthor = StringManager::GetPCSTRById(BOOK_AUTHOR);
    string::size_type tAuthor = _strAuthor.find(pcAuthor);//TODO：这段代码以后应去掉，因为下载过程已添加代码过滤掉
    if(string::npos != tAuthor)
    {
        _strAuthor = _strAuthor.replace(tAuthor, tAuthor+strlen(pcAuthor), "");
    }
    pCurBook->SetFileArtist(_strAuthor.c_str());
    
    string _strAbstract = _pclsDKXManager->FetchBookAbstract(_strFilePath);
    pCurBook->SetFileAbstract(_strAbstract.c_str());

    string _strLastReadTime = _pclsDKXManager->FetchBookLastReadTime(_strFilePath);
    time_t lastReadTime = CUtility::TransferStringToTime(_strLastReadTime);
    pCurBook->SetFileLastReadTime(lastReadTime);
    
    string _strFileSize = _pclsDKXManager->FetchBookSize(_strFilePath);
    int _iFileSize = atoi(_strFileSize.c_str());
    if(_iFileSize <= 0)
    {
        struct stat statbuf;
        if(lstat((_strFilePath.c_str()),&statbuf) == 0)
        {
            _iFileSize = statbuf.st_size;
        }
        if(_iFileSize)
        {
            char _szSize[40] = {0};
            snprintf(_szSize, sizeof(_szSize), "%d", _iFileSize);
            _pclsDKXManager->SetBookSize(_strFilePath, string(_szSize));
            _pclsDKXManager->SetBookCreateTime(_strFilePath, CUtility::TransferTimeToString(time(NULL)));

            ReadingOrderPlus();
            const int newBookReadingOrder = (IsPushedBook(_strFilePath) || IsDuokanBook(_strFilePath)) ? m_lReadingOrder : -1;
            _pclsDKXManager->SetBookReadingOrder(_strFilePath, newBookReadingOrder);
            _pclsDKXManager->SetBookAddOrder(_strFilePath, m_lReadingOrder);
            bNeedWriteDKX = true;
        }
    }
    pCurBook->SetFileSize(_iFileSize);
    pCurBook->SetFileReadingOrder(_pclsDKXManager->FetchBookReadingOrder(_strFilePath));
    pCurBook->SetFileAddOrder(_pclsDKXManager->FetchBookAddOrder(_strFilePath));

    string _strFormat = _pclsDKXManager->FetchBookFileType(_strFilePath);
    DkFileFormat _format = DkFileFormat(atoi(_strFormat.c_str()));
    if(!_format)
    {
        _format = GetFileFormatbyExtName(_strFilePath.c_str());
        char _szFormat[5] = {0};
        snprintf(_szFormat, sizeof(_szFormat), "%d", (int)_format);
        _pclsDKXManager->SetBookFileType(_strFilePath, string(_szFormat));
        bNeedWriteDKX = true;
    }
    pCurBook->SetFileFormat(_format);

    string bookID = _pclsDKXManager->FetchBookID(_strFilePath);
    if(bookID.empty())
    {
        //设置bookID
        bookID = GetBookID(_strFilePath.c_str());
        _pclsDKXManager->SetBookID(_strFilePath, bookID.c_str());
        bNeedWriteDKX = true;
    }
    pCurBook->SetBookID(bookID.c_str());
    
    //get progress bar
    int _iProgress = 0;
    ICT_ReadingDataItem *_pclsProgressItem = DKXReadingDataItemFactory::CreateReadingDataItem();
    if(_pclsProgressItem)
    {
        _pclsDKXManager->FetchProgress(_strFilePath, _pclsProgressItem);
        if(DFF_Text == _format)
        {
            DebugPrintf(DLC_LIUJT, "_pclsProgressItem->GetEndPos().GetAtomIndex()111=%d  for %s", _pclsProgressItem->GetEndPos().GetAtomIndex(), _strFilePath.c_str());
            if(0 == _pclsProgressItem->GetEndPos().GetAtomIndex())//此处为了Fix之前代码不完整的BUG：在扫书过程中没有将TXT的大小作为进度的EndPos设置！！！
            {
                CT_RefPos _clsEndPos = _pclsProgressItem->GetEndPos();
                _clsEndPos.SetAtomIndex(_iFileSize);
                _pclsProgressItem->SetEndPos(_clsEndPos);
                bNeedWriteDKX = true;
                _pclsDKXManager->SetProgress(_strFilePath, _pclsProgressItem);
            }
            DebugPrintf(DLC_LIUJT, "_pclsProgressItem->GetEndPos().GetAtomIndex()222=%d  for %s", _pclsProgressItem->GetEndPos().GetAtomIndex(), _strFilePath.c_str());
        }
        _iProgress = _pclsProgressItem->GetPercent();
        if (_iProgress < 0)
        {
            _iProgress = CalcProgress(_pclsProgressItem);
        }
    }
    SafeDeletePointerEx(_pclsProgressItem);
    pCurBook->SetFilePlayProcess(_iProgress);
    
    string _strSource = _pclsDKXManager->FetchBookSource(_strFilePath);
    int _iSource = atoi(_strSource.c_str());
    DK_FileSource _BookSource(UnknowSource); //TODO:: 需要将DK_FileSource和DkBookSource统一。目前存在DKX中的是DkBookSource
    if(1 == _iSource)
    {
        _BookSource = WEBSERVER;
    }
    else if (2 == _iSource)
    {
        _BookSource = UsbCopy;
    }
    else
    {
        _BookSource = (DK_FileSource)_iSource;
    }
    pCurBook->SetFileSource(_BookSource);
    
    string _strPassword = _pclsDKXManager->FetchBookPassword(_strFilePath);
    pCurBook->SetFilePassword(_strPassword.c_str());
    if(bNeedWriteDKX)
    {
        DebugPrintf(DLC_JUGH, "CDKFileManager::GetFileInfoFromDkX---bNeedWriteDKX = true %d", pCurBook->GetFileAddOrder());
        _pclsDKXManager->SaveCurDkx();
    }

    time_t tEndTime = time(NULL);
    long lCost = (long)difftime(tEndTime, tBeginTime);
    lCostTime += lCost;
    DebugPrintf(DLC_LIUJT, "CDKFileManager::GetFileInfoFromDkp Get info %s successfully with %ld seconds, total seconds=%d, total books=%d", _strFilePath.c_str(), lCost, lCostTime, lBooknumber);
    return S_OK;
}

int CDKFileManager::CalcProgress(const ICT_ReadingDataItem * const _pclsReadingItem)
{
    int _iProgress = 0;
    if(!_pclsReadingItem)
    {
        return _iProgress;
    }

    CT_RefPos _clsStartPos = _pclsReadingItem->GetBeginPos(), _clsEndPos = _pclsReadingItem->GetEndPos();

    if(_clsStartPos.CompareTo(_clsEndPos) > 0) //如果start比end还大，则返回0进度。
    {
        return _iProgress;
    }
    int iStartChapter =  _clsStartPos.GetChapterIndex(), iStartPara =  _clsStartPos.GetParaIndex(), iStartAtom = _clsStartPos.GetAtomIndex();
    int iEndChapter =  _clsEndPos.GetChapterIndex(), iEndPara =  _clsEndPos.GetParaIndex(), iEndAtom = _clsEndPos.GetAtomIndex();
    

    if(iEndChapter)
    {
        if(iStartChapter < iEndChapter)
        {
            _iProgress = iStartChapter*100/iEndChapter;
        }
        else//章索引相同，需要累加段和元素索引所占百分比
        {
            _iProgress = (iEndChapter-1)*100/iEndChapter;//根据章索引得到的百分比
            if (iEndPara)
            {
                if(iStartPara < iEndPara)
                {
                    _iProgress += (iStartPara*100/(iEndChapter*iEndPara));
                }
                else//段索引相同，需要累加元素索引所占百分比
                {
                    _iProgress += ((iEndPara-1)*100/(iEndChapter*iEndPara));
                    if(iEndAtom && (iStartAtom < iEndAtom))
                    {
                        _iProgress += iStartAtom*100/(iEndChapter*iEndPara*iEndAtom);
                    }
                    else//元素索引也相同，则进度为100%
                    {
                        _iProgress = 100;
                    }

                }
            }
        }
    }
    else if(iEndPara)
    {
        if( iStartPara < iEndPara)
        {
            _iProgress = iStartPara*100/iEndPara;
        }
        else//段索引相同，则需要同时考虑累加元素索引所占百分比
        {
            _iProgress = (iEndPara-1)*100/iEndPara;
            if(iEndAtom && (iStartAtom < iEndAtom))
            {
                _iProgress += iStartAtom*100/(iEndPara*iEndAtom);
            }
            else//元素索引相同，则进度为100%
            {
                _iProgress = 100;
            }
        }
    }
    else if(_clsEndPos.GetAtomIndex())
    {
        _iProgress = _clsStartPos.GetAtomIndex()*100/_clsEndPos.GetAtomIndex();
    }
    return _iProgress;
}

HRESULT    CDKFileManager::GetFileInfoFromDkp(PCDKFile pCurBook,IBookInfoProxy *pFileInfo)
{
    char   BookUtfName[FILENAMEMAXLEN] = {0};
    char *pBookName = NULL;

    if(NULL == pCurBook || NULL == pFileInfo )
    {
        return E_POINTER;
    }
    const char* pCurBookPath = pCurBook->GetFilePath();
    if(NULL == pCurBookPath)
    {
         return E_POINTER;
    }
    memset(BookUtfName,0,FILENAMEMAXLEN);
    if(!FAILED(pFileInfo->GetBookName(&pBookName)))
    {
        sprintf(BookUtfName,"%s",pBookName);
        delete []pBookName;
    }
    if( 0 == BookUtfName[0])
    {
        const char* pchar = strrchr(pCurBookPath,'/');
        if(pchar)
        {
            pchar++;
            sprintf(BookUtfName,"%s",pchar);
        }
    }
    //book name utf to gbk

    pCurBook->SetFileName(BookUtfName);


    //get book author
    char *pAuthorName = NULL;
    if(FAILED(pFileInfo->GetAuthorName(&pAuthorName)))
    {
        pAuthorName = NULL;
    }
    else if(pAuthorName)
    {
        pCurBook->SetFileArtist(pAuthorName);
        delete []pAuthorName;
    }
    char *pImageCover = NULL;
    if(FAILED(pFileInfo->GetCoverPagePath(&pImageCover)))
    {
        pImageCover = NULL;
    }
    else if(pImageCover)
    {

        pCurBook->SetFileImage(pImageCover);
        delete []pImageCover;
    }

    unsigned int   Progress = 0;
    if(FAILED(pFileInfo->GetReadingProgress((unsigned int *)(&Progress))))
    {
        Progress = 0;
    }
    pCurBook->SetFilePlayProcess(Progress);

    time_t   lastreadtime = 0;
    if(FAILED(pFileInfo->GetRecentOpenTime(lastreadtime)))
    {
        lastreadtime = 0;
    }
    if (0 == lastreadtime)
    {
        const int newBookReadingOrder = -1;
        pCurBook->SetFileReadingOrder(newBookReadingOrder);
    }
    pCurBook->SetFileLastReadTime(lastreadtime);
    //pFileInfo->SetFileSize(pCurBook->GetFileSize());


    char *CurBookPwd = NULL;
    if(SUCCEEDED(pFileInfo->GetPassword(&CurBookPwd)))
    {
        pCurBook->SetFilePassword(CurBookPwd);
        delete []CurBookPwd;
    }
    return S_OK;
}


HRESULT CDKFileManager::FileDkpProxy(PCDKFile pCurFile,DK_FileDkpOperate OperateType)
{
    
    HRESULT hr = E_FAIL;
    if(NULL == pCurFile)
    {
        return E_POINTER;
    }
    const char* pCurBookPath = pCurFile->GetFilePath();
    if(NULL == pCurBookPath)
    {
        return E_POINTER;
    }
    IBookInfoEntity *pDkpEntity     = NULL;
    IBookInfoProxy *pBookInfo         = NULL;
    
    bool bOldDKPExists = true;

    string _strBookPath(pCurBookPath);
    _strBookPath.append(".dkp2");

    if(access(_strBookPath.c_str(), F_OK) )//如果DKP2文件不存在
    {
        bOldDKPExists = false;
    }

    if(bOldDKPExists)
    {
        hr = CBookInfoManager::GetBookInfoEntity(pCurBookPath, &pDkpEntity, true);
        if(FAILED(hr) || NULL == pDkpEntity )
        {
            if(pDkpEntity)
            {
                delete pDkpEntity;
                pDkpEntity = NULL;
            }
            bOldDKPExists = false;
        }
    }
    switch(OperateType)
    {
    case GETFILEINFO:
        {
            if(bOldDKPExists && pDkpEntity)
            {
                hr = pDkpEntity->GetInfoProxy(&pBookInfo);
                if(SUCCEEDED(hr) && NULL != pBookInfo)
                {
                    hr = GetFileInfoFromDkp(pCurFile,pBookInfo);
                }
            }
            else
            {
                hr = GetFileInfoFromDkx(pCurFile);
            }
        }
        break;
    case SAVEFILEINFO:
        {
            hr = SaveFileInfoToDkp(pCurFile,pBookInfo);
        }
        break;
    default:
        break;
    }

    if(pBookInfo)
    {
        delete pBookInfo;
        pBookInfo = NULL;
    }


    if(pDkpEntity)
    {
        delete pDkpEntity;
        pDkpEntity = NULL;
    }

    return hr;
}

int CDKFileManager::SortFile(DkFormatCategory category)
{
    DebugPrintf(DLC_CDKFILEMANAGER, "%s, %d, %s, %s start", __FILE__,  __LINE__, "CDKFileManager", __FUNCTION__);
    CDKFileSorts FileSort;
    m_sortedFiles.clear();
    switch(category)
    {
    case DFC_Book:
    case DFC_Picture:
        {
            FileSort.SetFileSorts(m_BookSort);
            FileSort.SetFileListAndNum(m_files);
            FileSort.SetFileCategory(category);
            FileSort.SortsFile();        
            m_sortedFiles  = FileSort.GetFileSortResult();
            DKXManager* _pclsManager = DKXManager::GetInstance();
            if ((RecentlyReadTime == m_BookSort || RecentlyAdd == m_BookSort) && !m_sortedFiles.empty() && _pclsManager)
            {
                const int maxReadingOrder = (RecentlyReadTime == m_BookSort) ? 
                                             _pclsManager->FetchBookReadingOrder(m_sortedFiles[0]->GetFilePath()) :
                                             _pclsManager->FetchBookAddOrder(m_sortedFiles[0]->GetFilePath());
                if ((m_lReadingOrder < 0) || (maxReadingOrder > m_lReadingOrder))
                {
                    m_lReadingOrder = maxReadingOrder;
                    SystemSettingInfo::GetInstance()->SetInitReadingOrder(m_lReadingOrder);
                }
            }
        }    
        break;
    case DFC_Music:
    case DFC_Video:
        FileSort.SetFileSorts(m_MediaSort);
        FileSort.SetFileListAndNum(m_files);
        FileSort.SetFileCategory(category);
        FileSort.SortsFile();
        m_sortedMediaFiles = FileSort.GetFileSortResult();
        break;
    case DFC_Unknown:
    case DFC_Other:
    case DFC_Package:
    default:
        break;
    }
    return 0;
}

void CDKFileManager::ReadingOrderPlus()
{
    m_lReadingOrder++;
    // signed到最大后再加溢出就小于0了,需要特殊处理一下
    if (m_lReadingOrder < 0 && !m_sortedFiles.empty())
    {
        DKXManager* dkxManager = DKXManager::GetInstance(); 
        if(dkxManager)
        {
            DK_FileSorts savedFileSort = GetBookSort();
            if (savedFileSort != RecentlyReadTime)
            {
                SetBookSortType(RecentlyReadTime);
                SortFile(DFC_Book);
            }
            long lNewOrder = 1;
            for (int i = m_sortedFiles.size() - 1; i >= 0; --i, ++lNewOrder)
            {
                dkxManager->SetBookReadingOrder(m_sortedFiles[i]->GetFilePath(), lNewOrder);
                dkxManager->SaveCurDkx();
                m_sortedFiles[i]->SetFileReadingOrder(lNewOrder);
            }
            
            SetBookSortType(RecentlyAdd);
            SortFile(DFC_Book);
            lNewOrder = 1;
            for (int i = m_sortedFiles.size() - 1; i >= 0; --i, ++lNewOrder)
            {
                dkxManager->SetBookAddOrder(m_sortedFiles[i]->GetFilePath(), lNewOrder);
                dkxManager->SaveCurDkx();
                m_sortedFiles[i]->SetFileAddOrder(lNewOrder);
            }
            
            m_lReadingOrder = m_sortedFiles.size() + 2;
            if (savedFileSort != RecentlyAdd)
            {
                SetBookSortType(savedFileSort);
                SortFile(DFC_Book);
            }
        }
    }
}

void CDKFileManager::UnloadAllFile()
{
    for (DK_AUTO(cur, m_files.begin()); cur != m_files.end(); ++cur)
    {
        delete *cur;
    }
    m_files.clear();
    m_sortedFiles.clear();
    m_sortedMediaFiles.clear();
}


bool CDKFileManager::HasBookmark(DkFormatCategory category,int iBookId)
{
    bool bRet = false;
    if(DFC_Book == category)
    {
        CDKFileManager *pFileManager = CDKFileManager::GetFileManager();

        PCDKFile pFile = pFileManager->GetFileById(iBookId);
        if (NULL == pFile)
        {
            DebugPrintf(DLC_CDKFILEMANAGER, "%s, %d, %s, %s NULL == pFile", __FILE__,  __LINE__, "CDKFileManager", __FUNCTION__);
            return bRet;
        }

        const char* pBookPath = pFile->GetFilePath();
        if(NULL == pBookPath)
        {
            DebugPrintf(DLC_CDKFILEMANAGER, "%s, %d, %s, %s NULL == pBookPath", __FILE__,  __LINE__, "CDKFileManager", __FUNCTION__);
            return bRet;
        }

        DebugPrintf(DLC_CDKFILEMANAGER, "%s, %d, %s, %s default", __FILE__,  __LINE__, "CDKFileManager", __FUNCTION__);
        DKXManager* _pclsDKXManager = DKXManager::GetInstance();
        bRet = _pclsDKXManager && (_pclsDKXManager->FetchBookMarkNumber(string(pBookPath)) > 0);
    }
    DebugPrintf(DLC_CDKFILEMANAGER, "%s, %d, %s, %s end: %d", __FILE__,  __LINE__, "CDKFileManager", __FUNCTION__, bRet);
    return bRet;
}

//如果元数据提取关闭的话，只显示文件名，非书城书不显示作者
void CDKFileManager::SetMetaData(bool isOpen)
{
    DKXManager* _pclsDKXManager = DKXManager::GetInstance();
    for (DK_AUTO(cur, m_files.begin()); cur != m_files.end(); ++cur)
    {
        CDKFile* tempFile = *cur;
        //非EPUB MOBI文件不处理
        if(DFF_ElectronicPublishing == tempFile->GetFileFormat() || DFF_MobiPocket == tempFile->GetFileFormat())
        {
            string strBookPath = tempFile->GetFilePath();
            //书城中的书也不做处理
            if(!dk::utility::PathManager::IsUnderBookStore(strBookPath.c_str()))
            {
                if(isOpen)
                {
                    string strAuthor = _pclsDKXManager->FetchBookAuthor(strBookPath);
                    string strBookName = _pclsDKXManager->FetchBookName(strBookPath);
                    if(strBookName.empty())
                    {
                        const char* pchar = std::strrchr(strBookPath.c_str(),'/');
                        string bookName;
                        if(pchar)
                        {
                            pchar++;
                            strBookName.assign(pchar);
                        }
                    }
                    tempFile->SetFileName(strBookName.c_str());
                    tempFile->SetFileArtist(strAuthor.c_str());
                }
                else
                {
                    tempFile->SetFileArtist("");
                    const char* pchar = std::strrchr(strBookPath.c_str(),'/');
                    string bookName;
                    if(pchar)
                    {
                        pchar++;
                        bookName.assign(pchar);
                    }
                    tempFile->SetFileName(bookName.c_str());
                }
            }
        }
    }
}

CDKFile* CDKFileManager::GetBookbyListIndex(int index) const
{
    if (index < 0 || index >= (int)m_sortedFiles.size())
    {
        return NULL;
    }
    return m_sortedFiles[index];
}

int CDKFileManager::GetAllBookNumsUnderDir(const char* path) const
{
    if (StringUtil::IsNullOrEmpty(path))
    {
        return 0;
    }

    std::string part1, part2;
    PathManager::SplitLastPath(path, &part1, &part2);
    if (PathManager::IsBookStorePath(part1.c_str()))
    {
        return LocalCategoryManager::GetBookCountByCategory(part2.c_str());
    }

    bool endWithSlash = StringUtil::EndWith(path, "/");
    int pathLen = strlen(path);
    int result = 0;
    for (size_t i = 0; i < m_files.size(); ++i)
    {
        const char* filePath = m_files[i]->GetFilePath();
        if (strncmp(path, filePath, pathLen) == 0
                && (endWithSlash || filePath[pathLen] == '/'))
        {
            ++result;
        }
    }
    return result;
}

std::vector<std::string> CDKFileManager::GetDuoKanBookIds() const
{
    std::vector<std::string> results;
    for (size_t i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i]->IsDuoKanBook())
        {
            string s1, s2;
            PathManager::SplitLastPath(
                    PathManager::GetFileNameWithoutExt(m_files[i]->GetFilePath()).c_str(),
                        &s1, &s2);

            results.push_back(s2);
        }
    }
    return results;
}

// return new file id
int    CDKFileManager::RenameFile(PCDKFile pFile, const char* newName)
{
    if(!pFile || !pFile->GetFilePath() || StringUtil::IsNullOrEmpty(newName))
    {
        return -1;
    }
    const char* oldName = pFile->GetFileName();
    if(!oldName)
    {
        return -1;
    }
    if(!strcmp(oldName, newName))
    {
        return pFile->GetFileID();
    }
    const char* oldFilePath = pFile->GetFilePath();
    //如果用户未打开过该文件，则可能只含有DKP，而没有DKX！！！此处先进行转换！
    ConvertDKPToDKX(oldFilePath);

    DkFormatCategory FileCategory = GetFileCategory(oldFilePath);
    switch(FileCategory)
    {
        case DFC_Book:
        case DFC_Picture:
            break;
        case DFC_Music:
        case DFC_Video:
            break;
        case DFC_Unknown:
        case DFC_Other:
        case DFC_Package:
        default:
            return -1;//break, and return error
    }
    std::string path1, path2;
    PathManager::SplitLastPath(oldFilePath, &path1, &path2);
    std::string newFilePath = PathManager::ConcatPath(path1.c_str(), newName);
    newFilePath += GetExtNameByFormat(pFile->GetFileFormat());

    if (!PathManager::RenameFile(oldFilePath, newFilePath.c_str()))
    {
        return -1;
    }

    std::string oldDkxFile = PathManager::BookFileToDkxFile(oldFilePath);
    std::string newDkxFile = PathManager::BookFileToDkxFile(newFilePath);
    PathManager::RenameFile(oldDkxFile, newDkxFile);

    std::string oldDkptFile = PathManager::BookFileToDkptFile(oldFilePath);
    std::string newDkptFile = PathManager::BookFileToDkptFile(newFilePath);
    PathManager::RenameFile(oldDkptFile, newDkptFile);

    std::string oldCoverFile = PathManager::BookFileToCoverImageFile(oldFilePath);
    std::string newCoverFile = PathManager::BookFileToCoverImageFile(newFilePath);
    PathManager::RenameFile(oldCoverFile, newCoverFile);

    pFile->SetFilePath(newFilePath.c_str());
    pFile->SetFileName(newName);
    return pFile->GetFileID();
}

string CDKFileManager::GetBookID(const char* bookPath)
{
    string localBookID = "";
    if(IsDuokanBook(bookPath))
    {
        std::string baseName = PathManager::GetFileName(bookPath);
        // 4: "xx.epub"
        if (baseName.size() > 5)
        {
            localBookID = baseName.substr(0, baseName.size() - 5);
        }
    }
    else
    {
        char bookID[33] = {0};
        if(DKR_OK == DkFilterFactory::GetLocalBookID(bookPath, bookID))
        {
            localBookID = (string)bookID;
        }
    }
    return localBookID;
}

bool CDKFileManager::RenameFolder(const char* source, const char* dest)
{
    if(StringUtil::IsNullOrEmpty(source) || StringUtil::IsNullOrEmpty(dest))
    {
        return false;
    }
    
    if(PathManager::RenameFile(source, dest))
    {
        ReLoadFileToFileManger(PathManager::GetRootPath().c_str());
        return true;
    }
    return false;
}
