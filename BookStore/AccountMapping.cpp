#include "Utility/PathManager.h"
#include "BookStore/AccountMapping.h"
#include "Utility/JsonObject.h"
#include "Utility/SystemUtil.h"
#include <fstream>
//#include <iostream>

using namespace std;
using namespace dk::utility;

#define XIAOMIACCOUNT "xiaomiAccount"
#define DUOKANACCOUNT "duokanAccount"


AccountMapping::AccountMapping()
{
	LoadFromFile();
}

bool AccountMapping::GetDuokanByXiaomiAccount(const char* xiaomiAccount, std::vector<std::string>* duokanAccounts)
{
	if(xiaomiAccount && duokanAccounts && !m_xiaomiAccount.empty())
	{
		if(m_xiaomiAccount.compare(xiaomiAccount) == 0)
		{
			duokanAccounts->clear();
			std::copy(m_duokanAccounts.begin(), m_duokanAccounts.end(), std::back_inserter(*duokanAccounts));
			return true;
		}
	}
	return false;
}

bool AccountMapping::AddMapping(const char* xiaomiAccount, JsonObjectSPtr jsonDuokanAccounts)
{
	clear();
	if(xiaomiAccount && jsonDuokanAccounts.get())
	{
		m_xiaomiAccount = xiaomiAccount;
        bool changed = false;
		for(int i = 0; i < jsonDuokanAccounts->GetArrayLength(); i++)
		{
			JsonObjectSPtr jsonAccount = jsonDuokanAccounts->GetElementByIndex(i);
			if(jsonAccount)
            {
				string tmpAccount = "";
                if(jsonAccount->GetStringValue(&tmpAccount))
                {
					m_duokanAccounts.push_back(tmpAccount);
                    changed = true;
				}
			}
		}
        if (changed)
        {
            SaveToFile();
        }
        return changed;
	}
	return false;
}

void AccountMapping::LoadFromFile()
{
	clear();
	string strJson = SystemUtil::ReadStringFromFile(PathManager::GetAccountMappingPath().c_str(), false);
	JsonObjectSPtr jsonMapping = JsonObject::CreateFromString(strJson.c_str());
	if(jsonMapping.get())
	{
		jsonMapping->GetStringValue(XIAOMIACCOUNT, &m_xiaomiAccount);
		JsonObjectSPtr jsonDuokanAccount = jsonMapping->GetSubObject(DUOKANACCOUNT);
		if(jsonDuokanAccount.get())
		{
			for(int i = 0; i < jsonDuokanAccount->GetArrayLength(); i++)
			{
				JsonObjectSPtr jsonAccount = jsonDuokanAccount->GetElementByIndex(i);
				if(jsonAccount)
				{
					string tmpAccount = "";
					if(jsonAccount->GetStringValue(&tmpAccount))
					{
						m_duokanAccounts.push_back(tmpAccount);
					}
				}
			}
		}
	}
}

void AccountMapping::SaveToFile()
{
	JsonObjectSPtr jsonMapping = JsonObject::CreateJsonObject();
	if(jsonMapping)
	{
		jsonMapping->AddStringValue(XIAOMIACCOUNT, m_xiaomiAccount);
		jsonMapping->AddStringVector(DUOKANACCOUNT, m_duokanAccounts);

		string strJson = jsonMapping->GetJsonString();
		FILE* fp = fopen(PathManager::GetAccountMappingPath().c_str(), "wb+");
		fwrite(strJson.c_str(), strJson.length(), 1, fp);
		fclose(fp);
	}
}

void AccountMapping::clear()
{
	m_xiaomiAccount = "";
	m_duokanAccounts.clear();
}


