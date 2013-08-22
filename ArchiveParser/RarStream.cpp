#include <string.h>
#include "RarStream.h"
#include "dkWinError.h"

using namespace DkFormat;

CRarStream::CRarStream(const char *pFileName, const char *pPassword, int iFileIndex)
    : m_pFile(NULL)
    , m_streamMode(DSM_READ_ONLY)
{
    if (NULL == pFileName || iFileIndex < 0)
    {
        return;
    };

    m_pFile = RS_OpenFile(pFileName, pPassword, iFileIndex);
}


CRarStream::~CRarStream(void)
{
    if (m_pFile)
    {
        RS_CloseFile(m_pFile);
        m_pFile = NULL;
    }
}

HRESULT CRarStream::Read(PBYTE8 pbBuffer, INT iCount,PINT piReadCount)
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

    long ret = 0;
    ret = RS_Read(m_pFile, pbBuffer, iCount);
    if (piReadCount)
    {
        *piReadCount = ret;
    }

    return S_OK;
}

HRESULT CRarStream::ReadByte(PINT piByte)
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

HRESULT CRarStream::Seek(LONG lOffset, SeekMode origin, PLONG plPos)
{
    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }

    int tmpOrigin = 0;
    switch (origin)
    {
    case DK_SEEK_SET:
        tmpOrigin = RS_SEEK_SET;
        break;
    case DK_SEEK_CUR:
        tmpOrigin = RS_SEEK_CUR;
        break;
    case DK_SEEK_END:
        tmpOrigin = RS_SEEK_END;
        break;
    default:
        return E_INVALIDARG;
    }

    int iRet = 0;
    iRet = RS_Seek(m_pFile, lOffset, tmpOrigin);
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

HRESULT CRarStream::GetCanRead(PBOOL pfCanRead)
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

HRESULT CRarStream::GetCanSeek(PBOOL pfCanSeek)
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

HRESULT CRarStream::GetCanWrite(PBOOL pfCanWrite)
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

HRESULT CRarStream::GetLength(PLONG plLength)
{
    if (NULL == plLength)
    {
        return E_INVALIDARG;
    }

    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }

    Seek(0, DK_SEEK_END, plLength);
    return S_OK;
}

HRESULT CRarStream::Tell(PLONG plPos)
{
    if (NULL == plPos)
    {
        return E_INVALIDARG;
    }

    if (NULL == m_pFile)
    {
        return E_OBJ_NO_INIT;
    }

    *plPos = RS_Tell(m_pFile);
    return S_OK;
}

HRESULT CRarStream::Close()
{
    if (m_pFile)
    {
        RS_CloseFile(m_pFile);
        m_pFile = NULL;
    }

    return S_OK;
}

HRESULT CRarStream::Flush()
{
    return E_NOTIMPL;
}

HRESULT CRarStream::Write(PBYTE8 pbBuffer, INT iCount,PINT piWriteCount)
{
    return E_NOTIMPL;
}

HRESULT CRarStream::WriteByte(BYTE8 bByte)
{
    return E_NOTIMPL;
}

