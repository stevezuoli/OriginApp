#ifndef __DKKSS_MANAGER_H__
#define __DKKSS_MANAGER_H__

#include "LibKss/ILibKss/ILibKss.h"
#include "DKKSSMaster.h"
#include <tr1/memory>
#include <string>
#include <vector>
using namespace std;

class DKKSSManager
{
public:
    static DKKSSManager* GetInstance();
    void SetTempFolder(const string& tempFolder);
    ErrorNo Upload(const string& kssInfo,
        const string& fileName,
        FileProgressDelegate progress,
        string& fileMeta,
        vector<string>& commitMetas);

    ErrorNo Download(const string& kssInfo,
        const string& fileName,
        FileProgressDelegate progress);
    DKKSSManager();
    ~DKKSSManager();

private:
    bool Init();
    void Destory();

    DKKSSMaster::CKSSMaster* m_kssMaster;
};//DKKSSManager



#endif
