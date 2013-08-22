#ifndef __DKREADER_ARCHIVEPARSER_CRARSTREAM_H__
#define __DKREADER_ARCHIVEPARSER_CRARSTREAM_H__

#include "SequentialStream.h"
#include "dkBuffer.h"
#include "rar.hpp"
#include "dll.hpp"

namespace DkFormat
{
    class CRarStream : public IDKSequentialStream
    {
    public:
        CRarStream(const char *pFileName, const char *pPassword, int iFileIndex);
        virtual ~CRarStream(void);

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
        HANDLE m_pFile;
        DkStreamMode m_streamMode;
    };
}

#endif // __DKREADER_ARCHIVEPARSER_CRARSTREAM_H__
