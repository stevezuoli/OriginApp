#include "Utility/ConfigFile.h"
#include <fstream>
#include <ctype.h>
#include "KernelDef.h"
#include <string>
#include <iostream>

using namespace std;

namespace dk
{
namespace utility
{

bool ConfigFile::LoadFromFile(const char* configFile)
{
    if (NULL == configFile)
    {
        return false;
    }
    std::ifstream istream(configFile);
    //const unsigned int MAX_LINE = 2048;
    //char line[MAX_LINE];
    std::string line;
    while (getline(istream, line))
    {
        //istream.getline(line, MAX_LINE);
        const char* p = line.data();
        while (isspace(*p))
        {
            ++p;
        }

        if (*p == '#') // skip comment
        {
            continue;
        }
        const char* keyStart = p;

        // find key end
        while (*p && *p != '=' && !isspace(*p))
        {
            ++p;
        }
        if (!*p)
        {
            continue;
        }
        const char* keyEnd = p;

        // skip space between keyend and '='
        while (isspace(*p))
        {
            ++p;
        }
        if (!*p)
        {
            continue;
        }
        if (*p != '=')
        {
            continue;
        }

        // skip space before value start
        ++p;
        while (isspace(*p))
        {
            continue;
        }
#if 0
        if (!*p)
        {
            continue;
        }
#endif

        const char* valueStart = p;
        const char* valueEnd = line.data() + line.length();
        while (valueEnd > valueStart && isspace(*(valueEnd - 1)))
        {
            --valueEnd;
        }
        if (0 == *valueEnd)
        {
            --valueEnd;
        }
        if (valueEnd + 1 < valueStart)
        {
            continue;
        }


        std::string key(keyStart, keyEnd - keyStart), value(valueStart, valueEnd - valueStart + 1);
        m_configData[key] = value;
    }
    return true;
}

bool ConfigFile::GetInt(const char* key, int* result) const
{
    if (NULL == key || NULL == result)
    {
        return false;
    }
    DK_AUTO(iter, m_configData.find(key));
    if (iter == m_configData.end())
    {
        return false;
    }
    *result = atoi(iter->second.c_str());
    return true;
}

bool ConfigFile::GetDouble(const char* key, double* result) const
{
    if (NULL == key || NULL == result)
    {
        return false;
    }
    DK_AUTO(iter, m_configData.find(key));
    if (iter == m_configData.end())
    {
        return false;
    }
    *result = atof(iter->second.c_str());
    return true;
}

bool ConfigFile::GetString(const char* key, std::string* result) const
{
    if (NULL == key || NULL == result)
    {
        return false;
    }
    DK_AUTO(iter, m_configData.find(key));
    if (iter == m_configData.end())
    {
        return false;
    }
    *result = iter->second;
    return true;
}

bool ConfigFile::GetIntWithDefault(const char* key, int* result, int defaultValue) const
{
    if (NULL == key || NULL == result)
    {
        return false;
    }
    if (!GetInt(key, result))
    {
        *result = defaultValue;
    }
    return true;
}

bool ConfigFile::GetDoubleWithDefault(const char* key, double* result, double defaultValue) const
{
    if (NULL == key || NULL == result)
    {
        return false;
    }
    if (!GetDouble(key, result))
    {
        *result = defaultValue;
    }
    return true;
}

bool ConfigFile::GetStringWithDefault(const char* key, std::string* result, const char* defaultValue) const
{
    if (NULL == key || NULL == result || NULL == defaultValue)
    {
        return false;
    }
    if (!GetString(key, result))
    {
        *result = defaultValue;
    }
    return true;
    {
        *result = defaultValue;
    }
    return true;
}

void ConfigFile::SetInt(const char* key, int value)
{
    char buf[100];
    snprintf(buf, DK_DIM(buf), "%d", value);
    m_configData[key] = buf;
}

void ConfigFile::SetString(const char* key, const char* value)
{
    char buf[] = "";
    m_configData[key] = value != NULL? value : buf;
}

bool ConfigFile::SaveToFile(const char* configFile)
{
    if (NULL == configFile)
    {
        return false;
    }
    std::ofstream ostream(configFile);
    for (DK_AUTO(iter, m_configData.begin()); iter != m_configData.end(); ++iter)
    {
        ostream << iter->first << "=" << iter->second << std::endl;
    }
    ostream.close();
    return true;
}

std::vector<std::string> ConfigFile::GetAllKeys()
{
    std::vector<std::string> results;
    for (DK_AUTO(cur, m_configData.begin()); cur != m_configData.end(); ++cur)
    {
        results.push_back(cur->first);
    }
    return results;
}

} // namespace utility
} // namespace dk
