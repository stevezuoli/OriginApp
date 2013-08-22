#ifndef __DKREADER_ARCHIVEPARSER_CZIPHANDLER_H__
#define __DKREADER_ARCHIVEPARSER_CZIPHANDLER_H__

#include "ArchiveParser/ArchiveParser.h"

struct zip;

class CZipHandler : public IArchiverParser
{
public:
    CZipHandler(const char *pFilePathName);
    virtual ~CZipHandler();

    STDMETHOD(IsEncryption)(ArchiveEncryption *pEncryption);
    STDMETHOD(SetPassword)(const char *pPassword);
    STDMETHOD(GetFileNum)(int *piFileNum);
    STDMETHOD(GetFileNameByIndex)(int iIndex, char **ppFileName);
    STDMETHOD(GetIndexByName)(const char *pFilePath, int *piIndex);
    STDMETHOD(ReleaseFileName)(char *pFileName);
    STDMETHOD(CheckIsFileByIndex)(int iIndex, bool *pbIsFile);
    STDMETHOD(GetFileSizeByIndex)(int iIndex, int *piFileSize);
    STDMETHOD(GetFileContentByIndex)(int iIndex, unsigned char *pFileData, int iDataSize);
    STDMETHOD(GetFileStreamByIndex)(int iIndex, IDkStream **ppStream);
    STDMETHOD(GetFileStreamByIndex)(int iIndex, DkFormat::IDKSequentialStream **ppStream);

private:
    bool              m_bInitialized;
    ArchiveEncryption m_eEncryption;
    zip *             m_pZip;
    int               m_iFileNum;
    char *            m_pFilePathName;
    char              m_szPassword[256];
};

#endif // __DKREADER_ARCHIVEPARSER_CZIPHANDLER_H__

