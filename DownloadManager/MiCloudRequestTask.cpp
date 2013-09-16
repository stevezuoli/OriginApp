#include "DownloadManager/MiCloudRequestTask.h"
#include "Utility/EncodeUtil.h"
#include "Utility/PathManager.h"
#include "Utility/JsonObject.h"
#include "XiaoMi/MiCloudDownloadTaskBuilder.h"
#include "XiaoMi/XiaoMiSecureRequest.h"
#include "XiaoMi/XiaoMiConstants.h"
#include "XiaoMi/XiaoMiServiceFactory.h"

using namespace dk::utility;
using namespace xiaomi;
namespace dk
{
MiCloudCreateFileRequestTask::MiCloudCreateFileRequestTask(const string& dirId,
        const string& filePath,
        const string& displayName)
    : m_dirId(dirId)
    , m_filePath(filePath)
    , m_displayName(displayName)
{
}

bool MiCloudCreateFileRequestTask::PrepareData()
{
    string name = PathManager::GetFileName(m_filePath.c_str());
    string sha1 = EncodeUtil::SHA_160(m_filePath.c_str());
    string retry = "0";
    string data = GenerateSignatureData();

    m_postData.clear();

    map<string, string> params = MiCloudDownloadTaskBuilder::GenerateDefaultParams();
    params["parent_id"] = m_dirId;
    params["name"] = name;
    params["sha1"] = sha1;
    params["retry"] = retry;
    params["data"] = data;

    m_postData = SecureRequest::EncryptParams(m_method, m_url, params, XiaoMiServiceFactory::GetMiCloudService()->GetSecurity());
    m_cookieData = XiaoMiServiceFactory::GetMiCloudService()->GetCookies();

    return true;
}

std::string MiCloudCreateFileRequestTask::GenerateSignatureData()
{
    vector<string> shaVecs, md5Vecs;
    vector<int> sizeVecs;

    bool getVecsSucceeded = EncodeUtil::CalcFileBlockInfos(m_filePath.c_str(), XiaoMiConstants::GetBlockSize(), &shaVecs, &md5Vecs, &sizeVecs);
    if (!getVecsSucceeded 
            || shaVecs.size() != md5Vecs.size()
            || shaVecs.size() != sizeVecs.size()
            || md5Vecs.size() != sizeVecs.size())
    {
        return "";
    }

    int arrayLen =  shaVecs.size();
    JsonObjectSPtr dataJsonObj = JsonObject::CreateJsonObject();
    JsonObjectSPtr kssJsonObj = JsonObject::CreateJsonObject();
    JsonObjectSPtr blockInfosObj = JsonObject::CreateJsonArrayObject();
    for (int i = 0; i < arrayLen; ++i)
    {
        JsonObjectSPtr blockInfoObj = JsonObject::CreateJsonObject();
        blockInfoObj->AddStringValue("sha1", shaVecs[i]);
        blockInfoObj->AddStringValue("md5", md5Vecs[i]);
        blockInfoObj->AddIntValue("size", sizeVecs[i]);

        blockInfosObj->AddJsonObjectArray(blockInfoObj);
    }

    kssJsonObj->AddJsonObject("block_infos", blockInfosObj);
    dataJsonObj->AddJsonObject("kss", kssJsonObj);

    return dataJsonObj->GetJsonString();
}

}
