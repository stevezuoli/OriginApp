#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "DKXManager/DKX/DKXManager.h"
#include "KernelDef.h"
#include "interface.h"
#include "Utility/StringUtil.h"

using namespace dk::utility;


SINGLETON_CPP(DKXManager)
/*********************************************************************
*                     Interface on BookInfo                          *
*********************************************************************/
bool DKXManager::SetBookCreateTime(const string &_strBookPath,const string &_strCreateTime)
{
    if(!InitCurDkx(_strBookPath))
    {
        return "";
    }
    
    return m_pCurDkx->SetCreateTime(_strCreateTime);
}

bool DKXManager::SetBookRevision(const string &_strBookPath, const char* bookRevision)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    
    m_pCurDkx->SetBookRevision(bookRevision);
    return true;
}

std::string DKXManager::GetBookRevision(const string& bookPath)
{
    if(!InitCurDkx(bookPath))
    {
        return "";
    }
    
    return m_pCurDkx->GetBookRevision();
}

bool DKXManager::SetBookAddOrder(const string &_strBookPath, long _lAddOrder)
{
    
    return m_pCurDkx->SetAddOrder(_lAddOrder);
}

long DKXManager::FetchBookAddOrder(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return -1;
    }
    return m_pCurDkx->GetAddOrder();
}

bool DKXManager::SetBookLastReadTime(const string &_strBookPath,const string &_strLastReadTime)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    
    return m_pCurDkx->SetLastReadTime(_strLastReadTime);
}


string DKXManager::FetchBookLastReadTime(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return "";
    }
    return m_pCurDkx->GetLastReadTime();
}

bool DKXManager::SetBookReadingOrder(const string &_strBookPath, long _lReadingOrder)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->SetReadingOrder(_lReadingOrder);
}

long DKXManager::FetchBookReadingOrder(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return -1;
    }
    
    return m_pCurDkx->GetReadingOrder();
}

string DKXManager::FetchBookFileType(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return "";
    }
    return m_pCurDkx->GetFileType();
}

bool DKXManager::SetBookFileType(const string &_strBookPath, const string &_strFileType)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    m_pCurDkx->SetFileType(_strFileType);
    return true;
}

string DKXManager::FetchBookName(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return "";
    }
    return m_pCurDkx->GetBookName();
}
bool DKXManager::SetBookName(const string &_strBookPath, const string &_strBookName)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->SetBookName(_strBookName);
}
    
string DKXManager::FetchBookAuthor(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return "";
    }
    return m_pCurDkx->GetBookAuthor();
}
bool DKXManager::SetBookAuthor(const string &_strBookPath, const string &_strAuthor)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->SetBookAuthor(_strAuthor);
}

bool DKXManager::FetchSync(const string &_strBookPath)
{
	if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
	return m_pCurDkx->GetSync();
}

bool DKXManager::SetSync(const string &_strBookPath, bool isSync)
{
	if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    m_pCurDkx->SetSync(isSync);
	return true;
}

bool DKXManager::FetchCrash(const string &_strBookPath)
{
	if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->GetCrash();
}

bool DKXManager::SetCrash(const string &_strBookPath, bool isCrash)
{
	if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    m_pCurDkx->SetCrash(isCrash);
	return true;
}

bool DKXManager::SetBookID(const string& bookPath, const char* bookID)
{
	if(!InitCurDkx(bookPath))
    {
        return false;
    }
    m_pCurDkx->SetBookID(bookID);
	return true;
}

string DKXManager::FetchBookID(const string& bookPath)
{
	if(!InitCurDkx(bookPath))
    {
        return "";
    }
    return m_pCurDkx->GetBookID();
}

string DKXManager::FetchBookSource(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return "";
    }
    return m_pCurDkx->GetBookSource();
}
bool DKXManager::SetBookSource(const string &_strBookPath, const string &_strSource)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->SetBookSource(_strSource);
}

string DKXManager::FetchBookAbstract(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return "";
    }
    return m_pCurDkx->GetBookAbstract();
}

bool DKXManager::SetBookAbstract(const string &_strBookPath, const string &_strAbstract)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->SetBookAbstract(_strAbstract);
}

string DKXManager::FetchBookPassword(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return "";
    }
    return m_pCurDkx->GetBookPassword();
}

bool DKXManager::SetBookPassword(const string &_strBookPath, const string &_strPassword)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->SetBookPassword(_strPassword);
}

string DKXManager::FetchBookSize(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return "";
    }
    return m_pCurDkx->GetFileSize();
}

bool DKXManager::SetBookSize(const string &_strBookPath, const string &_strSize)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->SetFileSize(_strSize);
}


std::vector<TxtTocItem> DKXManager::FetchTxtToc(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        std::vector<TxtTocItem> vTxtToc;
        vTxtToc.clear();
        return vTxtToc;
    }
    return m_pCurDkx->GetTxtToc();

}

bool DKXManager::SetTxtToc(const string &_strBookPath,const std::vector<TxtTocItem> &_vTxtToc)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->SetTxtToc(_vTxtToc);
}

bool DKXManager::FetchTocExtractedState(const string &_strBookPath)
{
	if(!InitCurDkx(_strBookPath))
	{
		return false;
	}
	return m_pCurDkx->GetTocExtractedState();
}

bool DKXManager::SetTocExtractedState(const string &_strBookPath,bool _bHasExtracted)
{
	if(!InitCurDkx(_strBookPath))
	{
		return false;
	}
	return m_pCurDkx->SetTocExtractedState(_bHasExtracted);
}


/*********************************************************************
*                     Interface on ReadingData                       *
*********************************************************************/

int DKXManager::FetchBookMarkNumber(const string &_strBookPath)
{
    if(!InitCurDkx(_strBookPath))
    {
        return 0;
    }
    return m_pCurDkx->GetBookmarkCount();
}

bool DKXManager::AddBookmark(const string &_strBookPath, const ICT_ReadingDataItem *_pclsUserDataItem)
{
    if(!InitCurDkx(_strBookPath) || NULL == _pclsUserDataItem)
    {
        return false;
    }
    return m_pCurDkx->AddBookmark(_pclsUserDataItem);
}

bool DKXManager::RemoveBookmarkByIndex(const string &_strBookPath, int _iIndex)
{
    if(!InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->RemoveBookmarkByIndex(_iIndex);
}

bool DKXManager::FetchAllBookmarks(const string &_strBookPath, vector<ICT_ReadingDataItem *> *_pvBookmarks)
{
	if(!InitCurDkx(_strBookPath) || NULL == _pvBookmarks)
	{
		return false;
	}
    return m_pCurDkx->GetAllBookmarks(_pvBookmarks);
}

bool DKXManager::FetchBookmarkByIndex(
        const string &_strBookPath, 
        int _iIndex,
        ICT_ReadingDataItem *_pclsUserDataItem)
{
    if(NULL == _pclsUserDataItem || !InitCurDkx(_strBookPath))
    {
        return false;
    }
    return m_pCurDkx->GetBookmarkByIndex(
            _iIndex,
           _pclsUserDataItem);
}




bool DKXManager::SetProgress(const string &_strBookPath, const ICT_ReadingDataItem *_pclsUserDataItem)
{
    if(!InitCurDkx(_strBookPath) || NULL == _pclsUserDataItem)
    {
        return false;
    }
    return m_pCurDkx->SetProgress(_pclsUserDataItem);
}

bool DKXManager::FetchProgress(const string &_strBookPath,ICT_ReadingDataItem *_pclsUserDataItem)
{
    if(NULL == _pclsUserDataItem || !InitCurDkx(_strBookPath))
    {
        return false;
    }
    m_pCurDkx->GetProgress(_pclsUserDataItem);
    return true;
}

/*********************************************************************
*                     Interface on Setting Data                       *
*********************************************************************/

/**********************  PDF 专有参数    ************************/
    
/**********************  非PDF 专有参数    ************************/

bool DKXManager::TestIsThisDkx(string _strFilePath)
{
    return true;
}
bool DKXManager::SaveCurDkx()
{
    if(m_pCurDkx)
    {
        m_pCurDkx->Serialize(m_strCurFilePath);
        SafeDeletePointer(m_pCurDkx);
        return true;
    }
    return false;
}


bool DKXManager::InitCurDkx(string _strFilePath)
{
    if(_strFilePath.empty())
    {
        return false;
    }
    if(_strFilePath == m_strCurFilePath && m_pCurDkx)
    {
        return true;
    }
    SafeDeletePointer(m_pCurDkx);
    m_pCurDkx = new DKXBlock(_strFilePath);
    if(!m_pCurDkx)
    {
        return false;
    }
    m_strCurFilePath = _strFilePath;
    return true;
}


int DKXManager::GetDataVersion(const string& bookPath)
{
    if(!InitCurDkx(bookPath))
    {
        return false;
    }
    return m_pCurDkx->GetDataVersion();
}

bool DKXManager::SetDataVersion(const string& bookPath, int dataVersion)
{
    if(!InitCurDkx(bookPath))
    {
        return false;
    }
    return m_pCurDkx->SetDataVersion(dataVersion);
}

bool DKXManager::MergeCloudBookmarks(const string& bookPath, const ReadingBookInfo& readingBookInfo, int cloudDataVersion)
{
    if(!InitCurDkx(bookPath))
    {
        return false;
    }
    return m_pCurDkx->MergeCloudBookmarks(readingBookInfo, cloudDataVersion);
}

void DKXManager::SetReadingDataModified(const string& bookPath, bool modified)
{
    if(!InitCurDkx(bookPath))
    {
        return;
    }
    m_pCurDkx->SetReadingDataModified(modified);
}

bool DKXManager::IsReadingDataModified(const string& bookPath)
{
    if(!InitCurDkx(bookPath))
    {
        return false;
    }
    return m_pCurDkx->IsReadingDataModified();
}

void DKXManager::MarkReadingDataAsUploaded(const string& bookPath)
{
    if (!InitCurDkx(bookPath))
    {
        return;
    }
    m_pCurDkx->MarkReadingDataAsUploaded();
}

DKXBlock* DKXManager::CreateDKXBlock(const char* path)
{
    if (StringUtil::IsNullOrEmpty(path))
    {
        return NULL;
    }
    return new DKXBlock(path);
}

void DKXManager::DestoryDKXBlock(DKXBlock* block)
{
    delete block;
}


void DKXManager::Reset()
{
    m_strCurFilePath.clear();
    if (NULL != m_pCurDkx)
    {
        delete m_pCurDkx;
        m_pCurDkx = NULL;
    }
}
