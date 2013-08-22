#ifndef __UTILITY_CONFIGFILE_H__
#define __UTILITY_CONFIGFILE_H__

#include <string>
#include <map>
#include <vector>

namespace dk
{
namespace utility
{

class ConfigFile
{
public:
    bool LoadFromFile(const char* configFile);
    bool SaveToFile(const char* configFile);
    bool GetInt(const char* key, int* result) const;
    void SetInt(const char* key, int value);
    bool GetDouble(const char* key, double* result) const;
    bool GetString(const char* key, std::string* result) const;
    void SetString(const char* key, const char* value);

    bool GetIntWithDefault(const char* key, int* result, int defaultValue) const;
    bool GetDoubleWithDefault(const char* key, double* result, double defaultValue) const;
    bool GetStringWithDefault(const char* key, std::string* result, const char* defaultValue) const;

    std::vector<std::string> GetAllKeys();
private:
    std::map<std::string, std::string> m_configData;

};

} // namespace util
} // namespace dk
#endif
