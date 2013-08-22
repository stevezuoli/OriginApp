#include "CZipHandler.h"
#include <stdlib.h>
#include "Utility.h"
#include "ZipStream.h"
#include "DkZipStream.h"
#include "Utility/EncodeUtil.h"

using dk::utility::EncodeUtil;

CZipHandler::CZipHandler(const char *pFilePathName)
    : m_bInitialized(false)
    , m_eEncryption(ARCHIVE_NO_ENCRYPTION)
    , m_pZip(NULL)
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

    int iErrorStatus = 0;
    m_pZip = zip_open(m_pFilePathName, 0, &iErrorStatus);
    if (NULL == m_pZip)
    {
        return;
    }

    m_iFileNum = zip_get_num_files(m_pZip);
    if (m_iFileNum < 0)
    {
        return;
    }

    struct zip_stat stZipStat;
    for (int i = 0; i < m_iFileNum; i++)
    {
        if (0 == zip_stat_index(m_pZip, i, 0, &stZipStat) && 1 == stZipStat.encryption_method)
        {
            m_eEncryption = ARCHIVE_CONTENT_ENCRYPTION;
            break;
        }
    }

    m_bInitialized = true;
}

CZipHandler::~CZipHandler()
{
    if (m_pZip)
    {
        zip_close(m_pZip);
    }

    if (m_pFilePathName)
    {
        free(m_pFilePathName);
    }
}

HRESULT CZipHandler::IsEncryption(ArchiveEncryption *pEncryption)
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

HRESULT CZipHandler::SetPassword(const char *pPassword)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (NULL == pPassword || 0 == *pPassword)
    {
        return E_INVALIDARG;
    }

    strncpy(m_szPassword, pPassword, sizeof(m_szPassword) - 1);
    m_szPassword[sizeof(m_szPassword) - 1] = 0;

    zip_set_default_password(m_pZip, m_szPassword);
    return S_OK;
}

HRESULT CZipHandler::GetFileNum(int *piFileNum)
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

HRESULT CZipHandler::GetFileNameByIndex(int iIndex, char **ppFileName)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (NULL == ppFileName || iIndex < 0 || iIndex >= m_iFileNum)
    {
        return E_INVALIDARG;
    }

    *ppFileName = NULL;
    const char *pTmp = zip_get_name(m_pZip, iIndex, 0);
    if (NULL == pTmp)
    {
        return E_FAIL;
    }

    std::string utf8Name = EncodeUtil::GBKToUTF8String(pTmp);
    if (utf8Name.empty())
    {
        return E_FAIL;
    }


    *ppFileName = (char *)malloc(utf8Name.size() + 1);
    if (NULL == *ppFileName)
    {
        return E_FAIL;
    }

    memcpy(*ppFileName, utf8Name.c_str(), utf8Name.size() + 1);
    return S_OK;
}

HRESULT CZipHandler::GetIndexByName(const char *pFilePath, int *piIndex)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (NULL == pFilePath || NULL == piIndex)
    {
        return E_INVALIDARG;
    }

    std::string gbkString = EncodeUtil::UTF8ToGBKString(pFilePath);
    if (gbkString.empty())
    {
        return E_FAIL;
    }

    *piIndex = zip_name_locate(m_pZip, gbkString.c_str(), 0);
    return S_OK;
}

HRESULT CZipHandler::ReleaseFileName(char *pFileName)
{
    if (pFileName)
    {
        free(pFileName);
        return S_OK;
    }

    return E_FAIL;
}

HRESULT CZipHandler::CheckIsFileByIndex(int iIndex, bool *pbIsFile)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (iIndex < 0 || iIndex >= m_iFileNum || NULL == pbIsFile)
    {
        return E_INVALIDARG;
    }

    *pbIsFile = false;
    const char *pTmp = zip_get_name(m_pZip, iIndex, 0);
    if (NULL == pTmp)
    {
        return E_FAIL;
    }

    int iLen = strlen(pTmp);
    *pbIsFile = (pTmp[iLen - 1] != '/');
    return S_OK;
}

HRESULT CZipHandler::GetFileSizeByIndex(int iIndex, int *piFileSize)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (NULL == piFileSize || iIndex < 0 || iIndex >= m_iFileNum)
    {
        return E_INVALIDARG;
    }

    *piFileSize = -1;
    struct zip_stat stStat;
    if (zip_stat_index(m_pZip, iIndex, 0, &stStat))
    {
        return E_FAIL;
    }

    *piFileSize = (int)(stStat.size);
    return S_OK;
}

HRESULT CZipHandler::GetFileContentByIndex(int iIndex, unsigned char *pFileData, int iDataSize)
{
    if (!m_bInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    if (iIndex < 0 || iIndex >= m_iFileNum || NULL == pFileData || iDataSize <= 0)
    {
        return E_INVALIDARG;
    }

    struct zip_file *pZipFile = NULL;
    pZipFile = zip_fopen_index(m_pZip, iIndex, 0);
    if (NULL == pZipFile)
    {
        return E_FAIL;
    }

    int iReadLen = (int)zip_fread(pZipFile, pFileData, iDataSize);
    zip_fclose(pZipFile);
    if (iReadLen < iDataSize)
    {
        memset(pFileData + iReadLen, 0, iDataSize - iReadLen);
    }

    return S_OK;
}

HRESULT CZipHandler::GetFileStreamByIndex(int iIndex, IDkStream **ppStream)
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
    *ppStream = new DkZipStream(m_pFilePathName, m_szPassword, iIndex);
    if (NULL == *ppStream)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

HRESULT CZipHandler::GetFileStreamByIndex(int iIndex, DkFormat::IDKSequentialStream **ppStream)
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
    *ppStream = new DkFormat::CZipStream(m_pFilePathName, m_szPassword, iIndex);
    if (NULL == *ppStream)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}
