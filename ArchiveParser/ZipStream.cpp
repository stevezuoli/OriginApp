#include <string.h>
#include "ZipStream.h"
#include "dkWinError.h"

using namespace DkFormat;


CZipStream::CZipStream(const char *pFileName, const char *pPassword, int iFileIndex)
    : m_pstZip(NULL)
    , m_pFile(NULL)
    , m_streamMode(DSM_READ_ONLY)
{
    if (NULL == pFileName || iFileIndex < 0)
    {
        return;
    }

    int iErrorStatus = 0;
    m_pstZip = zip_open(pFileName, 0, &iErrorStatus);
    if (NULL == m_pstZip)
    {
        return;
    }

    if (pPassword && 0 != pPassword[0])
    {
        m_pFile = zs_fopen_index_encrypted(m_pstZip, (zip_uint64_t)iFileIndex, 0, pPassword);
    }
    else
    {
        m_pFile = zs_fopen_index_encrypted(m_pstZip, (zip_uint64_t)iFileIndex, 0, NULL);
    }
}

CZipStream::~CZipStream(void)
{
    if (m_pFile)
    {
        zs_fclose(m_pFile);
        m_pFile = NULL;
    }

    if (m_pstZip)
    {
        zip_close(m_pstZip);
        m_pstZip = NULL;
    }
}

HRESULT CZipStream::Read(PBYTE8 pbBuffer, INT iCount,PINT piReadCount)
{
    if (NULL == pbBuffer || iCount < 1)
    {
        return E_INVALIDARG;
    }

    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }

    if (DSM_WRITE_ONLY == m_streamMode)
    {
        return E_FAIL;
    }

    zip_uint64_t ret = 0;
    ret = zs_fread(m_pFile, pbBuffer, (zip_uint64_t)iCount);
    if (piReadCount)
    {
        *piReadCount = ret;
    }

    return S_OK;
}

HRESULT CZipStream::ReadByte(PINT piByte)
{
    if (NULL == piByte)
    {
        return E_FAIL;
    }

    BYTE8 tmp = 0;

    HRESULT hr = Read(&tmp, 1, NULL);
    *piByte = tmp;
    return hr;
}

HRESULT CZipStream::Seek(LONG lOffset, SeekMode origin, PLONG plPos)
{
    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }

    int tmpOrigin = 0;
    switch (origin)
    {
    case DK_SEEK_SET:
        tmpOrigin = ZS_SEEK_SET;
        break;
    case DK_SEEK_CUR:
        tmpOrigin = ZS_SEEK_CUR;
        break;
    case DK_SEEK_END:
        tmpOrigin = ZS_SEEK_END;
        break;
    default:
        return E_INVALIDARG;
    }

    int iRet = 0;
    iRet = zs_fseek(m_pFile, (zip_int64_t)lOffset, tmpOrigin);
    if (1 != iRet)
    {
        return E_FAIL;
    }

    if (plPos)
    {
        Tell(plPos);
    }

    return S_OK;
}

HRESULT CZipStream::GetCanRead(PBOOL pfCanRead)
{
    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }
    if (NULL == pfCanRead)
    {
        return E_INVALIDARG;
    }

    *pfCanRead = TRUE;
    return S_OK;
}

HRESULT CZipStream::GetCanSeek(PBOOL pfCanSeek)
{
    if (NULL == pfCanSeek)
    {
        return E_INVALIDARG;
    }

    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }

    *pfCanSeek = TRUE;
    return S_OK;
}

HRESULT CZipStream::GetCanWrite(PBOOL pfCanWrite)
{
    if (NULL == pfCanWrite)
    {
        return E_INVALIDARG;
    }

    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }

    *pfCanWrite = FALSE;
    return S_OK;
}

HRESULT CZipStream::GetLength(PLONG plLength)
{
    if (NULL == plLength)
    {
        return E_INVALIDARG;
    }

    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }

    *plLength = zs_fgetlen(m_pFile);
    return S_OK;
}

HRESULT CZipStream::Tell(PLONG plPos)
{
    if (NULL == plPos)
    {
        return E_INVALIDARG;
    }

    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }

    *plPos = (LONG)zs_ftell(m_pFile);
    return S_OK;
}

HRESULT CZipStream::Close()
{
    if (m_pFile)
    {
        zs_fclose(m_pFile);
        m_pFile = NULL;
    }

    if (m_pstZip)
    {
        zip_close(m_pstZip);
        m_pstZip = NULL;
    }

    return S_OK;
}

HRESULT CZipStream::Flush()
{
    return E_NOTIMPL;
}

HRESULT CZipStream::Write(PBYTE8 pbBuffer, INT iCount,PINT piWriteCount)
{
    return E_NOTIMPL;
}

HRESULT CZipStream::WriteByte(BYTE8 bByte)
{
    return E_NOTIMPL;
}

