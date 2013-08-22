#ifndef __DKREADER_ARCHIVEPARSER_CZIPSTREAM_H__
#define __DKREADER_ARCHIVEPARSER_CZIPSTREAM_H__

#include "SequentialStream.h"
#include "dkBuffer.h"
#include "zip.h"

namespace DkFormat
{
    class CZipStream : public IDKSequentialStream
    {
    public:
        CZipStream(const char *pFileName, const char *pPassword, int iFileIndex);
        virtual ~CZipStream(void);

        STDMETHOD(Read)(PBYTE8 pbBuffer, INT iCount, PINT piReadCount);
        STDMETHOD(ReadByte)(PINT piByte);

        STDMETHOD(Seek)(LONG lOffset, SeekMode origin, PLONG plPos);
        STDMETHOD(GetCanRead)(PBOOL pfCanRead);
        STDMETHOD(GetCanSeek)(PBOOL pfCanSeek);
        STDMETHOD(GetCanWrite)(PBOOL pfCanWrite);
        STDMETHOD(GetLength)(PLONG plLength);
        STDMETHOD(Tell)(PLONG plPos);

        STDMETHOD(Close)();
        STDMETHOD(Flush)();

        STDMETHOD(Write)(PBYTE8 pbBuffer, INT iCount, PINT piWriteCount);
        STDMETHOD(WriteByte)(BYTE8 bByte);

    private:
        struct zip *m_pstZip;
        struct zs_file *m_pFile;
        DkStreamMode m_streamMode;
    };
}

#endif // __DKREADER_ARCHIVEPARSER_CZIPSTREAM_H__
