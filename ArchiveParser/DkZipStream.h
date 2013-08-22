#ifndef __DKREADER_ARCHIVEPARSER_DKZIPSTREAM_H__
#define __DKREADER_ARCHIVEPARSER_DKZIPSTREAM_H__

#include "DkStream.h"
#include "zip.h"

class DkZipStream : public IDkStream
{
public:
    DkZipStream(const char *pFileName, const char *pPassword, int iFileIndex);
    virtual ~DkZipStream();

    virtual TYPE          GetType() const { return m_streamType; }
    virtual IDkStream *   CreateCopy();
    virtual DK_ReturnCode Destroy();

    virtual DK_ReturnCode Open(OPEN_MODE openMode = DK_READ_ONLY);
    virtual DK_ReturnCode Close();
    virtual OPEN_MODE     OpenMode() const { return m_openMode; }
    virtual DK_ReturnCode GetStreamInfo(StreamInfo * /*pStreamInfo*/) const { return DKR_FAILED; }

    virtual DK_FILESIZE_T Tell() const { return m_offset; }
    virtual DK_FILESIZE_T GetLength() const { return m_length; }
    virtual DK_BOOL       IsEOF() const { return m_offset == m_length; }

    virtual DK_ReturnCode Seek(DK_FILESIZE_T offset, SEEK_ORIGIN origin);
    virtual DK_BOOL       CanSeekBack() const { return m_canSeekBack; }
    virtual DK_ReturnCode Reset() { return Seek(0, DK_SEEK_SET); }
    virtual DK_ReturnCode SkipByte() { return Seek(1, DK_SEEK_CUR); }

    virtual DK_ReturnCode Read(DK_VOID *pDest, DK_FILESIZE_T destLen, DK_FILESIZE_T *pReadLen = DK_NULL);
    virtual DK_INT        GetByte();
    virtual DK_INT        PeekByte();
    virtual DK_ReturnCode ReadAll(DK_BYTE **ppDest, DK_FILESIZE_T *pReadLen);

    virtual DK_ReturnCode Write(const DK_BYTE *pSrc, DK_FILESIZE_T writeLen);
    virtual DK_ReturnCode PutChar(const DK_BYTE ch) { return Write(&ch, 1); }
    virtual DK_ReturnCode Flush();
    virtual DK_ReturnCode Clear();
protected:
    char *      m_pFileName;
    char *      m_pPassword;
    int         m_iFileIndex;
    zip *       m_pZip;
    zs_file *   m_pFile;
    TYPE        m_streamType;
    OPEN_MODE   m_openMode;
    DK_UINT     m_offset;
    DK_UINT     m_length;
    DK_BOOL     m_canSeekBack;
};

#endif // __DKREADER_ARCHIVEPARSER_DKZIPSTREAM_H__
