#include "CRarHandler.h"
#include "Utility.h"
#include "RarStream.h"
#include "DkRarStream.h"

struct RARFileInfo
{
    char szFileName[256];
    int  iUnCompressFileSize;
    int  iFileAttr;
};

struct RarData
{
    unsigned char * pBuf;
    unsigned int    iBufSize; //可用空间的大小
    unsigned int    iDataLen; //用户使用的大小
};

static int RARCallbackProc(UINT nMsg, LPARAM lpUserData, LPARAM lParam1, LPARAM lParam2)
{
    switch(nMsg)
    {
    case UCM_CHANGEVOLUME:
        if (lParam2 == RAR_VOL_ASK)
        {
            return -1;// -1 mains no next volume.
        }

        break;
    case UCM_PROCESSDATA:
        if (lpUserData != 0)
        {
            if (0 == lParam1 || lParam2 < 0)
            {
                return 0;
            }

            RarData *pstTmp = (RarData *)lpUserData;
            memcpy(pstTmp->pBuf + pstTmp->iDataLen, (const void*)lParam1, lParam2);
            pstTmp->iDataLen += lParam2;
        }

        break;
    case UCM_NEEDPASSWORD:
        break;
    default:
        break;
    }

    return 0;
}

CRarHandler::CRarHandler(const char *pFilePathName)
    : m_bInitialized(false)
    , m_eEncryption(ARCHIVE_NO_ENCRYPTION)
    , m_iFileNum(0)
    , m_pFilePathName(NULL)
{
    m_szPassword[0] = 0;

    if (NULL == pFilePathName || 0 == *pFilePathName)
    {
        return;
    }

    unsigned int uFilePathNameLen = strlen(pFilePathName);
    m_pFilePathName = (char *)malloc(uFilePathNameLen + 1);
    if (NULL == m_pFilePathName)
    {
        return;
    }

    memcpy(m_pFilePathName, pFilePathName, uFilePathNameLen);
    m_pFilePathName[uFilePathNameLen] = 0;
    HandlerInit();
}

CRarHandler::~CRarHandler()
{
    if (m_pFilePathName)
    {
        free(m_pFilePathName);
    }

    for (unsigned int i = 0; i < m_vFileList.size(); ++i)
    {
        if (m_vFileList.at(i))
        {
            delete m_vFileList.at(i);
        }
    }

    m_vFileList.clear();
}

HRESULT CRarHandler::IsEncryption(ArchiveEncryption *pEncryption)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (NULL == pEncryption)
    {
        return E_INVALIDARG;
    }

    *pEncryption = m_eEncryption;
    return S_OK;
}

HRESULT CRarHandler::SetPassword(const char *pPassword)
{
    // 不检查m_bInitialized
    if (NULL == pPassword || 0 == *pPassword)
    {
        return E_INVALIDARG;
    }

    strncpy(m_szPassword, pPassword, sizeof(m_szPassword) - 1);
    m_szPassword[sizeof(m_szPassword) - 1] = 0;

    HandlerInit();
    return m_bInitialized? S_OK: E_FAIL;
}

HRESULT CRarHandler::GetFileNum(int *piFileNum)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (NULL == piFileNum)
    {
        return E_INVALIDARG;
    }

    *piFileNum = m_iFileNum;
    return S_OK;
}

HRESULT CRarHandler::GetFileNameByIndex(int iIndex, char **ppFileName)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (NULL == ppFileName || iIndex < 0 || iIndex >= m_iFileNum)
    {
        return E_INVALIDARG;
    }

    *ppFileName = (char *)strdup(m_vFileList.at(iIndex)->szFileName);
    return S_OK;
}

HRESULT CRarHandler::GetIndexByName(const char *pFilePath, int *piIndex)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (NULL == pFilePath || NULL == piIndex)
    {
        return E_INVALIDARG;
    }

    *piIndex = -1;
    for (int i = 0; i < m_iFileNum; i++)
    {
        if (0 == strcmp(m_vFileList.at(i)->szFileName, pFilePath))
        {
            *piIndex = i;
            return S_OK;
        }
    }

    return E_FAIL;
}

HRESULT CRarHandler::ReleaseFileName(char *pFileName)
{
    if (pFileName)
    {
        free(pFileName);
        return S_OK;
    }

    return E_FAIL;
}

HRESULT CRarHandler::CheckIsFileByIndex(int iIndex, bool *pbIsFile)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (iIndex < 0 || iIndex >= m_iFileNum || NULL == pbIsFile)
    {
        return E_INVALIDARG;
    }

    *pbIsFile = (m_vFileList.at(iIndex)->iFileAttr != 16);
    return S_OK;
}

HRESULT CRarHandler::GetFileSizeByIndex(int iIndex, int *piFileSize)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (NULL == piFileSize || iIndex < 0 || iIndex >= m_iFileNum)
    {
        return E_INVALIDARG;
    }

    *piFileSize = m_vFileList.at(iIndex)->iUnCompressFileSize;
    return S_OK;
}

HRESULT CRarHandler::GetFileContentByIndex(int iIndex, unsigned char *pFileData, int iDataSize)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (iIndex < 0 || iIndex >= m_iFileNum || NULL == pFileData || iDataSize <= 0)
    {
        return E_INVALIDARG;
    }

    RAROpenArchiveDataEx archiveDataEx;
    archiveDataEx.ArcName = m_pFilePathName;
    archiveDataEx.OpenMode = RAR_OM_EXTRACT;

    HANDLE hArchive = RAROpenArchiveEx(&archiveDataEx);
    if (NULL == hArchive || archiveDataEx.OpenResult)
    {
        if (hArchive)
        {
            RARCloseArchive(hArchive);
        }

        return E_FAIL;
    }

    RarData stData;
    stData.iBufSize = iDataSize;
    stData.pBuf = pFileData;
    stData.iDataLen = 0;
    RARSetCallback(hArchive, (UNRARCALLBACK)RARCallbackProc, (long int)(&stData));
    if (ARCHIVE_NO_ENCRYPTION != m_eEncryption)
    {
        if (0 != m_szPassword[0])
        {
            RARSetPassword(hArchive, m_szPassword);
        }
    }

    struct RARHeaderDataEx* pHeaderData = new RARHeaderDataEx;
    if (NULL == pHeaderData)
    {
        return E_OUTOFMEMORY;
    }

    memset(pHeaderData, 0, sizeof(*pHeaderData));
    LONG hr = RARReadHeaderEx(hArchive, pHeaderData);
    int index = 0;
    while (ERAR_END_ARCHIVE != hr)
    {
        if (ERAR_EREAD == hr)
        {
            break;
        }

        if (index == iIndex)
        {
            if ((UINT)iDataSize < pHeaderData->UnpSize)
            {
                break;
            }

            if (0 != RARProcessFileW(hArchive, RAR_TEST, NULL, NULL))
            {
                break;
            }

            RARCloseArchive(hArchive);
            delete pHeaderData;
            return S_OK;
        }

        RARProcessFileW(hArchive, RAR_SKIP, NULL, NULL);
        hr = RARReadHeaderEx(hArchive, pHeaderData);
        index++;
    }

    RARCloseArchive(hArchive);

    delete pHeaderData;
    return E_FAIL;
}

HRESULT CRarHandler::GetFileStreamByIndex(int iIndex, IDkStream **ppStream)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if ( iIndex < 0 || iIndex >= m_iFileNum || NULL == ppStream)
    {
        return E_INVALIDARG;
    }

    *ppStream = NULL;
    *ppStream = new DkRarStream(m_pFilePathName, m_szPassword, iIndex);
    if (NULL == *ppStream)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

HRESULT CRarHandler::GetFileStreamByIndex(int iIndex, DkFormat::IDKSequentialStream **ppStream)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if ( iIndex < 0 || iIndex >= m_iFileNum || NULL == ppStream)
    {
        return E_INVALIDARG;
    }

    *ppStream = NULL;
    *ppStream = new DkFormat::CRarStream(m_pFilePathName, m_szPassword, iIndex);
    if (NULL == *ppStream)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

void CRarHandler::HandlerInit()
{
    //new pstArchiveData 1
    struct RAROpenArchiveDataEx* pstArchiveData = new RAROpenArchiveDataEx();
    if (NULL == pstArchiveData)
    {
        return;
    }

    memset(pstArchiveData, 0, sizeof(RAROpenArchiveDataEx));
    pstArchiveData->ArcName = (char *)m_pFilePathName;
    pstArchiveData->OpenMode = RAR_OM_LIST;
    // 2 open hArchive
    HANDLE hArchive = RAROpenArchiveEx(pstArchiveData);
    if (NULL == hArchive || 0 != pstArchiveData->OpenResult)
    {
        // TODO: should do some investigation to find out whether we need to close
        // file handle when open exception occurs.
        if (hArchive)
        {
            RARCloseArchive(hArchive);
        }

        delete pstArchiveData;
        return;
    }

    if ((int)(pstArchiveData->Flags & 0x80) > 0)
    {
        m_eEncryption = ARCHIVE_NAME_ENCRYPTION;
    }

    //文件头加密处理
    if (m_eEncryption != ARCHIVE_NO_ENCRYPTION)
    {
        if (0 == m_szPassword[0])
        {
            RARCloseArchive(hArchive);
            delete pstArchiveData;
            return;
        }
        RARSetPassword(hArchive, m_szPassword);
        //RARSetCallback(hArchive, (UNRARCALLBACK)RARCallbackProc, (LPARAM)m_pPassword);
    }

    // 3 new pstHeaderData
    struct RARHeaderDataEx* pstHeaderData = new RARHeaderDataEx();
    if (NULL == pstHeaderData)
    {
        RARCloseArchive(hArchive);
        delete pstArchiveData;
        return;
    }

    memset(pstHeaderData, 0, sizeof(RARHeaderDataEx));
    HRESULT hr = RARReadHeaderEx(hArchive, pstHeaderData);
    // 读出所有的文件名
    int iUnicodeLen = 0;
    ULONG uUtf8Len = 0;
    RARFileInfo *pstFileInfo = NULL;
    while (0 == hr)
    {
        if (ERAR_EREAD == hr)
        {
            RARCloseArchive(hArchive);
            delete pstHeaderData;
            delete pstArchiveData;
            return;
        }

        if (m_eEncryption == ARCHIVE_NO_ENCRYPTION)
        {
            if (pstHeaderData->Flags & 0x4)
            {
                m_eEncryption = ARCHIVE_CONTENT_ENCRYPTION;
            }
        }

        iUnicodeLen = sizeof(pstHeaderData->FileNameW);
        pstFileInfo = new RARFileInfo;
        // 4 new pstFileInfo
        if (NULL == pstFileInfo)
        {
            RARCloseArchive(hArchive);
            delete pstHeaderData;
            delete pstArchiveData;
            return;
        }
        memset(pstFileInfo, 0, sizeof(*pstFileInfo));
        int iUtf8BufLen = sizeof(pstFileInfo->szFileName);

        // 转码
#ifdef _WIN32
        hr = DkFormat::CUtility::ConvertEncoding(DkFormat::DK_CHARSET_UTF8, DkFormat::DK_CHARSET_UTF16_LE, (const char *)pstHeaderData->FileNameW, \
            iUnicodeLen, pstFileInfo->szFileName, iUtf8BufLen, &uUtf8Len);
#else
        hr = DkFormat::CUtility::ConvertEncoding(DkFormat::DK_CHARSET_UTF8, DkFormat::DK_CHARSET_UTF32_LE, (const char *)pstHeaderData->FileNameW, \
            iUnicodeLen, pstFileInfo->szFileName, iUtf8BufLen, &uUtf8Len);
#endif
        if (FAILED(hr))
        {
            delete pstFileInfo;
            RARCloseArchive(hArchive);
            delete pstHeaderData;
            delete pstArchiveData;
            return;
        }

        pstFileInfo->szFileName[uUtf8Len] = 0;
        uUtf8Len = 0;
        pstFileInfo->iFileAttr = pstHeaderData->FileAttr;
        pstFileInfo->iUnCompressFileSize = pstHeaderData->UnpSize;
        m_vFileList.push_back(pstFileInfo);// pstFileInfo加入数组中,在析构释放
        pstFileInfo = NULL;
        if (0 != RARProcessFile(hArchive, RAR_SKIP, 0, 0))
        {
            RARCloseArchive(hArchive);
            delete pstHeaderData;
            delete pstArchiveData;
            return;
        }

        hr = RARReadHeaderEx(hArchive, pstHeaderData);
    }

    RARCloseArchive(hArchive);
    m_iFileNum = m_vFileList.size();
    if (ERAR_END_ARCHIVE == hr)
    {
        m_bInitialized = true;
    }

    delete pstHeaderData;
    delete pstArchiveData;
}
