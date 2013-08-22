#ifndef _ARCHIVEFACTORY_H_
#define _ARCHIVEFACTORY_H_

#include "ArchiveParser/ArchiveParser.h"

namespace DkFormat
{
class CArchiveFactory
{
public:
    static HRESULT CreateArchiverInstance(const char *lpszFileName, IArchiverParser **ppInstance);
};
}
#endif // _ARCHIVEFACTORY_H_
