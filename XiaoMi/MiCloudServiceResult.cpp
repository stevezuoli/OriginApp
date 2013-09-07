#include "XiaoMi/MiCloudServiceResult.h"
#include "KernelDef.h"

namespace xiaomi
{
bool MiCloudServiceResultBase::Parse(const char* result)
{
    JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result);
    if (jsonObj)
    {
        jsonObj->GetStringValue("result", &m_result);
        jsonObj->GetStringValue("description", &m_description);
        jsonObj->GetIntValue("code", &m_code);
        if (m_code != 0)
        {
            jsonObj->GetStringValue("reason", &m_errorReason); 
        }
        else
        {
            m_dataJsonObj = jsonObj->GetSubObject("data");
        }

        return true;
    }

    return false;
}

MiCloudServiceResultGetQuota* MiCloudServiceResultGetQuota::CreateResult(const char* result)
{
    MiCloudServiceResultGetQuota* miCloudServiceResultGetQuota = new MiCloudServiceResultGetQuota;
    if (miCloudServiceResultGetQuota && miCloudServiceResultGetQuota->Init(result))
    {
        return miCloudServiceResultGetQuota;
    }

    SafeDeletePointer(miCloudServiceResultGetQuota);
    return NULL;
}

bool MiCloudServiceResultGetQuota::Init(const char* result)
{
    if (MiCloudServiceResultBase::Parse(result))
    {
        if (m_dataJsonObj)
        {
            m_dataJsonObj->GetInt64Value("total", &m_total);
            m_dataJsonObj->GetInt64Value("available", &m_available);
            m_dataJsonObj->GetInt64Value("ns_used", &m_nsUsed);
        }
        return true;
    }

    return false;
}

MiCloudServiceResultGetChildren* MiCloudServiceResultGetChildren::CreateResult(const char* result)
{
    MiCloudServiceResultGetChildren* miCloudServiceResultGetChildren = new MiCloudServiceResultGetChildren;
    if (miCloudServiceResultGetChildren && miCloudServiceResultGetChildren->Init(result))
    {
        return miCloudServiceResultGetChildren;
    }

    SafeDeletePointer(miCloudServiceResultGetChildren);
    return NULL;
}

bool MiCloudServiceResultGetChildren::Init(const char* result)
{
    if(MiCloudServiceResultBase::Parse(result))
    {
        if (m_dataJsonObj)
        {
            JsonObjectSPtr fileInfosObj = m_dataJsonObj->GetSubObject("list");
            if (fileInfosObj)
            {
                int arraySize = fileInfosObj->GetArrayLength();
                for (int i = 0; i < arraySize; ++i)
                {
                    string type;
                    JsonObjectSPtr fileInfoObj = fileInfosObj->GetElementByIndex(i);
                    m_fileList.push_back(MiCloudFileSPtr(MiCloudFile::CreateFromJsonObj(fileInfoObj.get())));
                }
            }
        }
        return true;
    }

    return false;
}

MiCloudServiceResultCreateFile* MiCloudServiceResultCreateFile::CreateResult(const char* result)
{
    MiCloudServiceResultCreateFile* miCloudServiceResultCreateFile = new MiCloudServiceResultCreateFile;
    if (miCloudServiceResultCreateFile && miCloudServiceResultCreateFile->Init(result))
    {
        return miCloudServiceResultCreateFile;
    }

    SafeDeletePointer(miCloudServiceResultCreateFile);
    return NULL;
}

bool MiCloudServiceResultCreateFile::Init(const char* result)
{
    if(MiCloudServiceResultBase::Parse(result))
    {
        if (m_dataJsonObj)
        {
            JsonObjectSPtr infoObj = m_dataJsonObj->GetSubObject("info");
            JsonObjectSPtr kssObj = m_dataJsonObj->GetSubObject("kss");
            //no need to upload to kss
            if (infoObj)
            {
                m_fileInfo.reset(MiCloudFile::CreateFromJsonObj(infoObj.get()));
            }
            else if (kssObj)
            {
                m_kssStrInfo = kssObj->GetJsonString();
                m_kssInfo.reset(KSSUploadInfo::CreateFromJsonObj(m_dataJsonObj.get()));
            }
        }

        return true;
    }

    return false;
}

MiCloudServiceResultCreateDir* MiCloudServiceResultCreateDir::CreateResult(const char* result)
{
    MiCloudServiceResultCreateDir* miCloudServiceResultCreateDir = new MiCloudServiceResultCreateDir;
    if (miCloudServiceResultCreateDir && miCloudServiceResultCreateDir->Init(result))
    {
        return miCloudServiceResultCreateDir;
    }

    SafeDeletePointer(miCloudServiceResultCreateDir);

    return NULL;
}

bool MiCloudServiceResultCreateDir::Init(const char* result)
{
    if (MiCloudServiceResultBase::Parse(result))
    {
        if (m_dataJsonObj)
        {
            JsonObjectSPtr infoObj = m_dataJsonObj->GetSubObject("info");
            if (infoObj)
            {
                m_dirInfo.reset(MiCloudFile::CreateFromJsonObj(infoObj.get()));
            }
        }

        return true;
    }

    return false;
}

MiCloudServiceResultDeleteDir* MiCloudServiceResultDeleteDir::CreateResult(const char* result)
{
    MiCloudServiceResultDeleteDir* miCloudServiceResultDeleteDir = new MiCloudServiceResultDeleteDir;
    if (miCloudServiceResultDeleteDir && miCloudServiceResultDeleteDir->Init(result))
    {
        return miCloudServiceResultDeleteDir;
    }

    SafeDeletePointer(miCloudServiceResultDeleteDir);
    return NULL;
}

bool MiCloudServiceResultDeleteDir::Init(const char* result)
{
    if (MiCloudServiceResultBase::Parse(result))
    {
        return true;
    }

    return false;
}

MiCloudServiceResultDeleteFile* MiCloudServiceResultDeleteFile::CreateResult(const char* result)
{
    MiCloudServiceResultDeleteFile* miCloudServiceResultDeleteFile = new MiCloudServiceResultDeleteFile;
    if (miCloudServiceResultDeleteFile && miCloudServiceResultDeleteFile->Init(result))
    {
        return miCloudServiceResultDeleteFile;
    }

    SafeDeletePointer(miCloudServiceResultDeleteFile);
    return NULL;
}

bool MiCloudServiceResultDeleteFile::Init(const char* result)
{
    if (MiCloudServiceResultBase::Parse(result))
    {
        return true;
    }

    return false;
}

MiCloudServiceResultRequestDownload* MiCloudServiceResultRequestDownload::CreateResult(const char* result)
{
    MiCloudServiceResultRequestDownload* miCloudServiceResultRequestDownload = new MiCloudServiceResultRequestDownload;
    if (miCloudServiceResultRequestDownload && miCloudServiceResultRequestDownload->Init(result))
    {
        return miCloudServiceResultRequestDownload;
    }

    SafeDeletePointer(miCloudServiceResultRequestDownload);
    return NULL;
}


bool MiCloudServiceResultRequestDownload::Init(const char* result)
{
    if (MiCloudServiceResultBase::Parse(result))
    {
        if (m_dataJsonObj)
        {
            JsonObjectSPtr infoObj = m_dataJsonObj->GetSubObject("kss");
            if (infoObj)
            {
                m_kssStrInfo = infoObj->GetJsonString();

                return true;
            }
        }
    }

    return false;
}

MiCloudServiceResultGetInfo* MiCloudServiceResultGetInfo::CreateResult(const char* result)
{
    MiCloudServiceResultGetInfo* miCloudServiceResultGetInfo = new MiCloudServiceResultGetInfo;
    if (miCloudServiceResultGetInfo && miCloudServiceResultGetInfo->Init(result))
    {
        return miCloudServiceResultGetInfo;
    }

    SafeDeletePointer(miCloudServiceResultGetInfo);
    return NULL;
}

bool MiCloudServiceResultGetInfo::Init(const char* result)
{
    if (MiCloudServiceResultBase::Parse(result))
    {
        if (m_dataJsonObj)
        {
            JsonObjectSPtr infoObj = m_dataJsonObj->GetSubObject("info");
            if (infoObj)
            {
                m_fileInfo.reset(MiCloudFile::CreateFromJsonObj(infoObj.get()));
            }
        }

        return true;
    }

    return false;
}
}//xiaomi
