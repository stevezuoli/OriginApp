//===========================================================================
// Summary:
//     File Stream
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-09-05
// Author:
//     Zhai Guanghe(zhaigh@duokan.com)
//===========================================================================
#ifndef __KERNEL_COMMONLIBS_IOBASE_FILESTREAM_H__
#define __KERNEL_COMMONLIBS_IOBASE_FILESTREAM_H__

#include "DkStream.h"

class FileStream : public IDkStream
{
public:
    FileStream(const DK_CHAR *pFilePathName);
    virtual ~FileStream();

    virtual TYPE          GetType() const { return m_streamType; }
    virtual IDkStream *   CreateCopy();
    virtual DK_ReturnCode Destroy();

    virtual DK_ReturnCode Open(OPEN_MODE openMode = DK_READ_ONLY);
    virtual DK_ReturnCode Close();
    virtual OPEN_MODE     OpenMode() const { return m_openMode; }
    virtual DK_ReturnCode GetStreamInfo(StreamInfo * /*pStreamInfo*/) const { return DKR_FAILED; }

    virtual DK_FILESIZE_T Tell() const;
    virtual DK_FILESIZE_T GetLength() const { return m_length; }
    virtual DK_BOOL       IsEOF() const { return Tell() == m_length; }

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

public:
    const DK_CHAR* GetFilePathName() const { return m_pFilePathName; }

protected:
    TYPE          m_streamType;
    DK_CHAR *     m_pFilePathName;
    OPEN_MODE     m_openMode;
    FILE *        m_pFile;
    DK_FILESIZE_T m_length;
    DK_BOOL       m_canSeekBack;
};

#endif // __KERNEL_COMMONLIBS_IOBASE_FILESTREAM_H__
