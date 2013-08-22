#include "DkRarStream.h"
#include "DkStreamOpenMode.h"

DkRarStream::DkRarStream(const char *pFileName, const char *pPassword, int iFileIndex)
    : m_pFileName(NULL)
    , m_pPassword(NULL)
    , m_iFileIndex(0)
    , m_pFile(DK_NULL)
    , m_streamType(FILTER_STREAM)
    , m_openMode(DK_ERROR_STREAM)
    , m_offset(0)
    , m_length(0)
    , m_canSeekBack(DK_TRUE)
{
    if (NULL == pFileName || 0 == *pFileName || iFileIndex < 0)
    {
        return;
    }

    unsigned int uFilePathNameLen = strlen(pFileName);
    m_pFileName = (char *)malloc(uFilePathNameLen + 1);
    if (NULL == m_pFileName)
    {
        return;
    }

    memcpy(m_pFileName, pFileName, uFilePathNameLen);
    m_pFileName[uFilePathNameLen] = 0;

    if (pPassword && *pPassword)
    {
        unsigned int uPasswordLen = strlen(pPassword);
        m_pPassword = (char *)malloc(uPasswordLen + 1);
        if (NULL == m_pPassword)
        {
            return;
        }

        memcpy(m_pPassword, pPassword, uPasswordLen);
        m_pPassword[uPasswordLen] = 0;
    }

    m_iFileIndex = iFileIndex;

    m_pFile = RS_OpenFile(pFileName, pPassword, iFileIndex);
    if (m_pFile)
    {
        m_openMode = DK_CLOSED;
        RS_Seek(m_pFile, 0, RS_SEEK_END);
        m_length = RS_Tell(m_pFile);
        RS_Seek(m_pFile, 0, RS_SEEK_SET);
    }
}

DkRarStream::~DkRarStream()
{
    if (m_pFile) { RS_CloseFile(m_pFile); }
    if (m_pPassword) { free(m_pPassword); }
    if (m_pFileName) { free(m_pFileName); }
}

IDkStream * DkRarStream::CreateCopy()
{
    return new DkRarStream(m_pFileName, m_pPassword, m_iFileIndex);
}

DK_ReturnCode DkRarStream::Destroy()
{
    return DKR_FAILED;
}

DK_ReturnCode DkRarStream::Open(OPEN_MODE openMode)
{
    // 拒绝非法的打开方式
    if (DK_READ_ONLY != openMode)
    {
        return DKR_IO_STREAM_ERR_MODE;
    }

    if (ErrMode(m_openMode))
    {
        return DKR_IO_STREAM_OPEN_FAIL;
    }

    if (IsOpen(m_openMode))
    {
        return DKR_IO_STREAM_OPENED;
    }

    m_openMode = DK_READ_ONLY;
    return DKR_OK;
}

DK_ReturnCode DkRarStream::Close()
{
    if (DK_OPEN_FAIL == m_openMode || DK_CLOSED == m_openMode)
    {
        return DKR_UNSUPPORTED;
    }

    m_openMode = DK_CLOSED;
    return DKR_OK;
}

DK_ReturnCode DkRarStream::Seek(DK_FILESIZE_T offset, SEEK_ORIGIN origin)
{
    if (!IsOpen(m_openMode))
    {
        return DKR_IO_STREAM_CLOSED;
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
        return DKR_IO_STREAM_ERR_SEEK_ORIGIN;
    }

    int iRet = 0;
    iRet = RS_Seek(m_pFile, (long)offset, tmpOrigin);
    if (1 != iRet)
    {
        return DKR_FAILED;
    }

    m_offset = RS_Tell(m_pFile);
    return DKR_OK;
}

DK_ReturnCode DkRarStream::Read(DK_VOID *pDest, DK_FILESIZE_T destLen, DK_FILESIZE_T *pReadLen)
{
    if (DK_NULL == pDest || destLen < 0)
    {
        return DKR_INVALIDINPARAM;
    }

    if (!IsOpen(m_openMode))
    {
        return DKR_IO_STREAM_CLOSED;
    }

    long ret = 0;
    ret = RS_Read(m_pFile, pDest, (long)destLen);
    m_offset = RS_Tell(m_pFile);
    if (pReadLen)
    {
        *pReadLen = ret;
        return ret? DKR_OK: DKR_FAILED;
    }
    else
    {
        return (destLen == (DK_FILESIZE_T)ret)? DKR_OK: DKR_FAILED;
    }
}

DK_INT DkRarStream::GetByte()
{
    DK_BYTE ret = 0;
    if (DKR_OK != Read(&ret, 1))
    {
        return -1;
    }

    return ret;
}

DK_INT DkRarStream::PeekByte()
{
    DK_INT ret = GetByte();
    RS_Seek(m_pFile, -1, RS_SEEK_CUR);
    m_offset--;
    return ret;
}

DK_ReturnCode DkRarStream::ReadAll(DK_BYTE **ppDest, DK_FILESIZE_T *pReadLen)
{
    if (DK_NULL == ppDest || DK_NULL ==pReadLen)
    {
        return DKR_INVALIDINPARAM;
    }

    if (DKR_OK != Open())
    {
        return DKR_FAILED;
    }

    *pReadLen = GetLength();
    if (*pReadLen)
    {
        *ppDest = DK_MALLOC_OBJ_N(DK_BYTE, (DK_SIZE_T)(*pReadLen + 1));
        if (DKR_OK != Read(*ppDest, *pReadLen))
        {
            SafeFreePointer(*ppDest);
            return DKR_FAILED;
        }

        (*ppDest)[*pReadLen] = 0;
    }

    Reset();
    return DKR_FAILED;
}

DK_ReturnCode DkRarStream::Write(const DK_BYTE *pSrc, DK_FILESIZE_T writeLen)
{
    return DKR_FAILED;
}

DK_ReturnCode DkRarStream::Flush()
{
    return DKR_FAILED;
}

DK_ReturnCode DkRarStream::Clear()
{
    return DKR_FAILED;
}
