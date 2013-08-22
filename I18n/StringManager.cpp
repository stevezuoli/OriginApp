#include "I18n/StringManager.h"
#include "interface.h"
#include "Common/ConfigInfo.h"
#include "Utility/FileSystem.h" 
#include "Common/SystemSetting_DK.h"     
#include <fstream>
#include <sstream>
#include <iostream>
#include <string> 
#include <map>
#include <vector>
#include "Utility/StringUtil.h"
using namespace std;
using dk::utility::StringUtil;


#define LANGUAGE_FILE_PATH  "/mnt/us/DK_System/xKindle/res/language/"

static map<string,int> msi;

static string StringNames[STRING_COUNT]=
{
#define STRING_LIST_NAME_QUOTE(CommandId, StringName)\
            #CommandId,
    STRING_LIST(STRING_LIST_NAME_QUOTE)
#undef STRING_LIST_NAME_QUOTE
};

#define STRING_LIST_NAME(CommandId, StringName)\
            StringName,
static CString gpStringName[STRING_COUNT] = 
{
    STRING_LIST(STRING_LIST_NAME)
};

static CString gpStringNameDefault[STRING_COUNT] = 
{
    STRING_LIST(STRING_LIST_NAME)
};
#undef STRING_LIST_NAME

string gstrLayoutDemo = "";
//用于reload
StringManager* StringManager::s_pStringManager =  NULL;

StringManager::StringManager()
{
}

StringManager::~StringManager()
{
}

//int StringManager::CheckUtf8WordBytes(const char *_pWord)
//{
//	if(!_pWord)
//	{
//		return 0;
//	}
//	int i = 7;
//	while(((*_pWord) >> i) & 0x1)
//	{
//		i--;
//	}
//	return 7 - i;
//}

const string &StringManager::GetDemoString()
{
    static string empty;
	if(gstrLayoutDemo.empty())
	{
		ifstream fin(LAYOUT_DEMO_FILEPATH,ifstream::in | ifstream::binary);
		if(!fin.is_open())
		{
			return empty; 
		}

		string str_tmp = "";
		while(getline(fin,str_tmp))
		{
			gstrLayoutDemo += str_tmp;
			gstrLayoutDemo += '\n';
		}
	}
    return gstrLayoutDemo;
}

CString StringManager::GetStringById(SOURCESTRINGID stringId) //避免越界
{
    if (stringId >= 0 && stringId < STRING_COUNT)
    {
        return gpStringName[stringId];
    }
    return CString();
}

LPCSTR StringManager::GetPCSTRById(SOURCESTRINGID stringId) //避免越界
{
    if (stringId >= 0 && stringId < STRING_COUNT)
    {
        return gpStringName[stringId].GetBuffer();
    }
    return "";
}

void StringManager::InitMap()
{
    for(int i=0; i<STRING_COUNT; i++)
    {
        //cout << i << ":" << StringNames[i] << endl;
        msi[StringNames[i]]=i;
    }
}

bool StringManager::SetLanguage(string name)
{
    std::vector<std::string> availableLangs = SystemSettingInfo::GetInstance()->GetAvailableLanguages();

    if (name.find('.') == std::string::npos)
    {
        name = name + ".txt";
    }
    string l_name(name);
    l_name= LANGUAGE_FILE_PATH +l_name;    //just the good way

    ifstream fin(l_name.c_str());   //打开并处理文件，将ID和内容分别读进内存，并更换gpStringName数组，达到更换目的
    if(!fin)
    {
        cerr << "Can't open language resources." << endl; 
        return false;
    }

    //cout << (msi.find("ABOUT_DUOKAN") == msi.end()) << endl;
    //cout << msi.count("ABOUT_DUOKAN") << endl;
    string str_tmp;
    while(getline(fin,str_tmp))
    {
        int res=str_tmp.find('=');
        int len=str_tmp.size();
        if(str_tmp[len-1]=='\r')
        {
            len--;
        }

        if(msi.count(str_tmp.substr(0,res))!=0)
        {
            string value = StringUtil::ReplaceString(str_tmp.substr(res+1,len-res-1), "\\n", "\n");
            gpStringName[msi[str_tmp.substr(0,res)]] = value.c_str();
        }
    }

    fin.close();
    return true;
}

void StringManager::ReloadLang()
{
    for(int i=0; i<STRING_COUNT; i++)
    {
        gpStringName[i]=gpStringNameDefault[i];
    }
}

StringManager* StringManager::GetInstance()
{
    if(NULL == s_pStringManager)
    {
        s_pStringManager = new StringManager();
    }

    return s_pStringManager;
}

void InitLanguage()
{
    StringManager::InitMap();
    vector<string> vs_name;
    string dir(LANGUAGE_FILE_PATH);
    GetFilesInDir(dir,vs_name);

    std::string lang = SystemSettingInfo::GetInstance()->GetLanguage();
    std::vector<std::string> availableLangs = SystemSettingInfo::GetInstance()->GetAvailableLanguages();


    if (availableLangs[0] != lang)
    {
        StringManager::SetLanguage(lang);
    }
    else
    {
        StringManager::ReloadLang();
    }
}


const char* StringManager::GetLoginErrorMessage(int errorCode, const char* msg)
{
    if (SystemSettingInfo::GetInstance()->IsCurrentLanguageChinese())
    {
        return msg;
    }

    char buf[100];
    snprintf(buf, DK_DIM(buf), "LOGIN_ERROR_%d", errorCode);
    DK_AUTO(pos, msi.find(buf));
    if(pos != msi.end())
    {
        return GetPCSTRById((SOURCESTRINGID)pos->second);
    }
    return (NULL != msg) ? msg : GetPCSTRById(UNEXPECTED_ERROR);
}

const char* StringManager::GetCommentReplyErrorMessage(int errorCode, const char* msg)
{
    char buf[100];
    snprintf(buf, DK_DIM(buf), "ERROR_GET_COMMENT_REPLY_%d", errorCode);
    DK_AUTO(pos, msi.find(buf));
    if(pos != msi.end())
    {
        return GetPCSTRById((SOURCESTRINGID)pos->second);
    }
    return (NULL != msg) ? msg : GetPCSTRById(BOOKSTORE_GETCOMMENTINFO_FAIL);
}

