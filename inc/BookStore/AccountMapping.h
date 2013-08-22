#ifndef __ACCOUNTMAPPING_H__
#define __ACCOUNTMAPPING_H__

#include <vector>
#include <string>
#include "Utility/JsonObject.h"

class AccountMapping
{
public:
	AccountMapping();
	virtual ~AccountMapping(){};
	bool AddMapping(const char* xiaomiAccount, JsonObjectSPtr jsonDuokanAccounts);
	bool GetDuokanByXiaomiAccount(const char* xiaomiAccount, std::vector<std::string>* duokanAccounts);
	void clear();
private:
	void LoadFromFile();
	void SaveToFile();
	std::string m_xiaomiAccount;
	std::vector<std::string> m_duokanAccounts;
};
#endif

