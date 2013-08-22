#ifndef __KERNEL_TESTS_TESTBASE_TESTCONFIG_H__
#define __KERNEL_TESTS_TESTBASE_TESTCONFIG_H__

#include "KernelType.h"
#include "KernelRetCode.h"
#include <map>
#include <string>
#include <vector>

class TestConfig
{
public:
    DK_ReturnCode LoadFromFile(const DK_CHAR* configFile);
    DK_ReturnCode SaveToFile(const DK_CHAR* configFile);
    DK_ReturnCode GetInt(const DK_CHAR* key, DK_INT* result) const;
    DK_VOID SetInt(const DK_CHAR* key, DK_INT value);
    DK_ReturnCode GetDouble(const DK_CHAR* key, DK_DOUBLE* result) const;
    DK_ReturnCode GetBox(const DK_CHAR* key, DK_BOX* result) const;
    DK_ReturnCode GetArgbColor(const DK_CHAR* key, DK_ARGBCOLOR* result) const;
    DK_ReturnCode GetString(const DK_CHAR* key, DK_STRING* result) const;
    DK_VOID SetString(const DK_CHAR* key, const DK_CHAR* value);
    DK_ReturnCode GetFlowPosition(const DK_CHAR* key, DK_FLOWPOSITION* result) const;
    DK_VOID SetFlowPosition(const DK_CHAR* key, const DK_FLOWPOSITION& value);

    DK_ReturnCode GetIntWithDefault(const DK_CHAR* key, DK_INT* result, DK_INT defaultValue) const;
    DK_ReturnCode GetDoubleWithDefault(const DK_CHAR* key, DK_DOUBLE* result, DK_DOUBLE defaultValue) const;
    DK_ReturnCode GetDkBoxWithDefault(const DK_CHAR* key, DK_BOX* result, const DK_BOX& defaultValue) const;
    DK_ReturnCode GetStringWithDefault(const DK_CHAR* key, DK_STRING* result, const DK_CHAR* defaultValue) const;
private:
    DK_ReturnCode GetDoubleList(const DK_CHAR* key, std::vector<DK_DOUBLE>* result) const;
    DK_ReturnCode GetIntList(const DK_CHAR* key, std::vector<DK_INT>* result) const;
    std::map<std::string, std::string> m_configData;

};
#endif
