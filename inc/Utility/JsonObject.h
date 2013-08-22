//=======================================================================
// Summary:
//		对json-c库函数的封装json方法分析器
// Usage:
//		封装得到最基本的元数据的过程，所有需要json解
//		析的类都可以继承自该类
// Remarks:
//		NULL
// Date:
//		2012-05-15
// Author:
//		Xu Kai(xukai@duokan.com)
//=======================================================================

#ifndef __UTILITY_JSONOBJECT_H__
#define __UTILITY_JSONOBJECT_H__

#include <json.h>
#include <string>
#include <tr1/memory>
#include "KernelDef.h"
#include <vector>

class JsonObject;
typedef std::tr1::shared_ptr<JsonObject> JsonObjectSPtr;

class JsonObject
{

	private:
		JsonObject(json_object* rawJsonObj);
	public:
		~JsonObject();
		static JsonObjectSPtr CreateFromString(const char* content);
		static JsonObjectSPtr CreateJsonObject();
		static JsonObjectSPtr CreateJsonArrayObject();
		static JsonObjectSPtr CreateJsonStringObject(const char* key);
		static JsonObjectSPtr CreateJsonIntObject(int key);
		
		bool GetIntValue(const char *key, int *value) const;
		bool GetIntValue(int * value) const;
		bool GetIntValueFromIntOrString(const char *key, int *value) const;
		bool GetDoubleValue(const char *key, double *value) const;
        bool GetDoubleValue(double* value) const;
		bool GetBooleanValue(const char *key, bool *value) const;
		bool GetStringValue(const char* key, std::string* value) const;
		bool GetStringValue(std::string* value) const;
		JsonObjectSPtr GetSubObject(const char* key) const;
		const char* GetJsonString();
		int GetArrayLength() const;
		// 此处未做边界检查，调用者需要保证不越界
		JsonObjectSPtr GetElementByIndex(unsigned int index) const;
        std::vector<std::string> GetAllKeys() const;
	
		bool AddJsonObject(const char* key,const JsonObjectSPtr pJsonObj);
		bool AddJsonObjectArray(const JsonObjectSPtr pJsonObj);
		bool AddVector(const char* key,const std::vector<int> & vValue);
		bool AddStringVector(const char* key,const std::vector<std::string> & vValue);
		bool AddIntValue(const char *key, int value);
		bool AddDoubleValue(const char *key, double value);
		bool AddStringValue(const char *key,const std::string& value);

		bool DeleteJsonObject(const char *key);
		
	private:
		json_object* GetJsonObject() const;
		static JsonObjectSPtr CreateFromRawObject(json_object* v);
		bool IsValid() const;
		bool IsArray() const;
        bool IsObject() const ;
		json_object* GetRawSubObject(const char* key) const;
		bool IsIntType() const;
		DISALLOW_COPY_AND_ASSIGN(JsonObject);
		json_object* m_jsonObj;
};

#endif//_UTILITY_DKJSONBASALPARSER_H_

