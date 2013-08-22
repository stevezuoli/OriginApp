#include "../inc/Common/ConfigInfo.h"
#include "drivers/DeviceInfo.h"
#include "KernelDef.h"
#include "Utility/PathManager.h"
#include "Utility/ConfigFile.h"

using dk::utility::ConfigFile;
using dk::utility::PathManager;

extern char g_szAppPath[PATH_MAX];
#define INIFILE_PATH "/mnt/us/DK_System/xKindle"

#define DEFINE_CONFIGINFO_KEY_CATEGORY_MAPPING(Index, kt_default, kp_default)\
    {#Index, kt_default},
keymap g_cgKeyInfoTouch[CKI_COUNT] = 
{
    CONFIG_KEY_LIST(DEFINE_CONFIGINFO_KEY_CATEGORY_MAPPING)
};
#undef DEFINE_CONFIGINFO_KEY_CATEGORY_MAPPING

#define DEFINE_CONFIGINFO_KEY_CATEGORY_MAPPING(Index, kt_default, kp_default)\
    {#Index, kp_default},
keymap g_cgKeyInfoPaperWhite[CKI_COUNT] = 
{
    CONFIG_KEY_LIST(DEFINE_CONFIGINFO_KEY_CATEGORY_MAPPING)
};
#undef DEFINE_CONFIGINFO_KEY_CATEGORY_MAPPING

keymap* g_cgKeyInfo = NULL;
void InitConfigMap()
{
    if (NULL != g_cgKeyInfo)
    {
        return;
    }
    if (DeviceInfo::IsKPW())
    {
        g_cgKeyInfo = g_cgKeyInfoPaperWhite;
    }
    else
    {
        g_cgKeyInfo = g_cgKeyInfoTouch;
    }
}
void LoadConfigInfo()
{
	InitConfigMap();
	ConfigFile configInfo;
	configInfo.LoadFromFile(PathManager::GetSystemConfigPath().c_str());
	string result("");
	for(int i = 0; i < CKI_COUNT; i++)
	{
		if(configInfo.GetString(g_cgKeyInfo[i].keyname, &result))
		{
			snprintf(g_cgKeyInfo[i].keyvalue, DK_DIM(g_cgKeyInfo[i].keyvalue), "%s", result.c_str());
		}
	}
}

const char* Config::GetConfigValueString(int index)
{
    if (NULL == g_cgKeyInfo || index < 0 || index >= CKI_COUNT)
    {
        return NULL;
    }
    return g_cgKeyInfo[index].keyvalue;
}


const char* Config::GetConfigName(int index)
{
    if (NULL == g_cgKeyInfo || index < 0 || index >= CKI_COUNT)
    {
        return NULL;
    }
    return g_cgKeyInfo[index].keyname;
}

bool Config::SetConfigValueInt(int index, int value)
{
	if (NULL == g_cgKeyInfo || index < 0 || index >= CKI_COUNT)
    {
        return NULL;
    }
	snprintf(g_cgKeyInfo[index].keyvalue, DK_DIM(g_cgKeyInfo[index].keyvalue), "%d", value);
	ConfigFile configInfo;
	configInfo.LoadFromFile(PathManager::GetSystemConfigPath().c_str());
	configInfo.SetString(g_cgKeyInfo[index].keyname, g_cgKeyInfo[index].keyvalue);
	configInfo.SaveToFile(PathManager::GetSystemConfigPath().c_str());
    return true;
}

bool Config::SetConfigValueString(int index, const char* s)
{
	if (NULL == g_cgKeyInfo || index < 0 || index >= CKI_COUNT)
    {
        return NULL;
    }
	snprintf(g_cgKeyInfo[index].keyvalue, DK_DIM(g_cgKeyInfo[index].keyvalue), "%s", s);
	ConfigFile configInfo;
	configInfo.LoadFromFile(PathManager::GetSystemConfigPath().c_str());
	configInfo.SetString(g_cgKeyInfo[index].keyname, g_cgKeyInfo[index].keyvalue);
	configInfo.SaveToFile(PathManager::GetSystemConfigPath().c_str());
    return true;
}
