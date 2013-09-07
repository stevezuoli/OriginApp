#include "Utility/JsonObject.h"
#include <stdio.h>

JsonObject::JsonObject(json_object* rawJsonObj)
    : m_jsonObj(rawJsonObj)
{
}

JsonObject::~JsonObject()
{
    if (m_jsonObj != NULL)
    {
		json_object_put(m_jsonObj);
    }
}

JsonObjectSPtr JsonObject::CreateFromRawObject(json_object* v)
{
    return NULL == v ?  JsonObjectSPtr() : JsonObjectSPtr(new JsonObject(v));
}

JsonObjectSPtr JsonObject::CreateFromString(const char* content)
{
	if(!content || 0 == content[0])
	{
		return JsonObjectSPtr();
	}
	json_object* pJsonObj = json_tokener_parse(content);
	if(is_error(pJsonObj))				//is_error判断不准确，如果该字符串不符合json格式，此处仍有可能通过
	{
		return JsonObjectSPtr();
	}
	const char* strTmp = json_object_to_json_string(pJsonObj);
	if( !strTmp || strTmp[0] != '{' )
	{
		json_object_put(pJsonObj);
		return JsonObjectSPtr();
	}
	return CreateFromRawObject(pJsonObj);
}

JsonObjectSPtr JsonObject::CreateJsonObject()
{
	json_object* v = json_object_new_object();
	return NULL == v ?  JsonObjectSPtr() : JsonObjectSPtr(new JsonObject(v));
}

JsonObjectSPtr JsonObject::CreateJsonArrayObject()
{
	json_object* v = json_object_new_array();
	return NULL == v ?  JsonObjectSPtr() : JsonObjectSPtr(new JsonObject(v));
}

JsonObjectSPtr JsonObject::CreateJsonStringObject(const char* key)
{
	json_object* v = json_object_new_string(key);
	return NULL == v ?  JsonObjectSPtr() : JsonObjectSPtr(new JsonObject(v));
}

JsonObjectSPtr JsonObject::CreateJsonIntObject(int key)
{
	json_object* v = json_object_new_int(key);
	return NULL == v ?  JsonObjectSPtr() : JsonObjectSPtr(new JsonObject(v));
}

bool JsonObject::GetDoubleValue(const char* key, double* value) const
{
    json_object* v = GetRawSubObject(key);
    if (!v || !json_object_is_type(v, json_type_double))
    {
        return false;
    }
    *value = json_object_get_double(v);
    json_object_put(v);
    return true;
}

bool JsonObject::GetIntValue(const char* key, int * value) const
{
    json_object* v = GetRawSubObject(key);
    if (!v || !json_object_is_type(v, json_type_int))
    {
        return false;
    }
    *value = json_object_get_int(v);
    json_object_put(v);
    return true;
}

bool JsonObject::GetInt64Value(const char* key, int64_t * value) const
{
    json_object* v = GetRawSubObject(key);
    if (!v || !json_object_is_type(v, json_type_int))
    {
        return false;
    }
    *value = json_object_get_int64(v);
    json_object_put(v);
    return true;
}

bool JsonObject::GetIntValue(int * value) const
{
	if(!m_jsonObj)
	{
		return false;
	}
    json_object* v = m_jsonObj;
    if (!v || !json_object_is_type(v, json_type_int))
    {
        return false;
    }
    *value = json_object_get_int(v);
    return true;
}

bool JsonObject::GetDoubleValue(double* value) const
{
	if(!m_jsonObj)
	{
		return false;
	}
    json_object* v = m_jsonObj;
    if (!v || !json_object_is_type(v, json_type_double))
    {
        return false;
    }
    *value = json_object_get_double(v);
    return true;
}

bool JsonObject::GetIntValueFromIntOrString(const char *key, int *value) const
{
	if(!m_jsonObj || !key || !value)
	{
		return false;
	}
    json_object* v = GetRawSubObject(key);
    if (!v)
    {
        return false;
    }
    if (json_object_is_type(v, json_type_int))
    {
        *value = json_object_get_int(v);
        json_object_put(v);
        return true;
    }
    else if (json_object_is_type(v, json_type_string))
    {
        *value = atoi(json_object_get_string(v));
        json_object_put(v);
        return true;
    }
    else
    {
        json_object_put(v);
        return false;
    }
}

bool JsonObject::GetBooleanValue(const char* key, bool* value) const
{
	if(!m_jsonObj || !key || !value)
	{
		return false;
	}
    json_object* v = GetRawSubObject(key);
    if (!v || !json_object_is_type(v, json_type_boolean))
    {
        return false;
    }
    *value = json_object_get_boolean(v);
    json_object_put(v);
    return true;
}

bool JsonObject::GetStringValue(const char* key, std::string* value) const
{
	if(!m_jsonObj || !key || !value)
	{
		return false;
	}
    json_object* v = GetRawSubObject(key);
    if (!v || !json_object_is_type(v, json_type_string))
    {
        return false;
    }
    *value = json_object_get_string(v);
    json_object_put(v);
    return true;
}

bool JsonObject::GetStringValue(std::string* value) const
{
	if(!m_jsonObj || !value)
	{
		return false;
	}
    json_object* v = m_jsonObj;
    if (!v || !json_object_is_type(v, json_type_string))
    {
        return false;
    }
    *value = json_object_get_string(v);
    return true;
}

int JsonObject::GetArrayLength() const
{
	if(!m_jsonObj)
	{
		return false;
	}
    if (!IsArray())
    {
        return -1;
    }
	return json_object_array_length(m_jsonObj);
}

JsonObjectSPtr JsonObject::GetElementByIndex(unsigned int index) const
{
	if(NULL == m_jsonObj || index < 0 || (int)index >= GetArrayLength())
	{
		return JsonObjectSPtr();
	}
	json_object* json_tmp = json_object_array_get_idx(m_jsonObj, index);
	if(is_error(json_tmp))
	{
		json_object_put(json_tmp);
		return JsonObjectSPtr();
	}
	else
	{
		json_object_get(json_tmp);
		return JsonObjectSPtr(new JsonObject(json_tmp));
	}
}

JsonObjectSPtr JsonObject::GetSubObject(const char* key) const
{
	if(!m_jsonObj || !key)
	{
		return JsonObjectSPtr();
	}
    return CreateFromRawObject(GetRawSubObject(key));
}

bool JsonObject::AddJsonObject(const char* key,const JsonObjectSPtr pJsonObj)
{
	if(!key || !pJsonObj || !m_jsonObj)
	{
		return false;
	}
	json_object_get(pJsonObj->GetJsonObject());
	json_object_object_add(m_jsonObj, key, pJsonObj->GetJsonObject());
	return true;
}

bool JsonObject::AddJsonObjectArray(const JsonObjectSPtr pJsonObj)
{
	if(!pJsonObj || !m_jsonObj)
	{
		return false;
	}
	json_object_get(pJsonObj->GetJsonObject());
	json_object_array_add(m_jsonObj, pJsonObj->GetJsonObject());
	return true;
}

bool JsonObject::AddVector(const char* key,const std::vector<int> & vValue)
{
	if(!key || !m_jsonObj)
	{
		return false;
	}
	json_object* JsonvValues = json_object_new_array();
	for(int i = 0; i < (int)vValue.size(); i++)
	{
		json_object* jsonValue = json_object_new_int(vValue[i]);
		json_object_array_add(JsonvValues, jsonValue);
	}
	json_object_object_add(m_jsonObj, key, JsonvValues);
	return true;
}

bool JsonObject::AddStringVector(const char* key,const std::vector<std::string> & vValue)
{
	if(!key || !m_jsonObj)
	{
		return false;
	}
	json_object* JsonvValues = json_object_new_array();
	for(int i = 0; i < (int)vValue.size(); i++)
	{
		json_object* jsonValue = json_object_new_string(vValue[i].c_str());
		json_object_array_add(JsonvValues, jsonValue);
	}
	json_object_object_add(m_jsonObj, key, JsonvValues);
	return true;
}

bool JsonObject::AddIntValue(const char *key, int value)
{
	if(!key || !m_jsonObj)
	{
		return false;
	}
	json_object* jsonValue = json_object_new_int(value);
	json_object_object_add(m_jsonObj, key, jsonValue);
	return true;
}

bool JsonObject::AddDoubleValue(const char *key, double value)
{
	if(!key || !m_jsonObj)
	{
		return false;
	}
	json_object* jsonValue = json_object_new_double(value);
	json_object_object_add(m_jsonObj, key, jsonValue);
	return true;
}

bool JsonObject::AddStringValue(const char *key, const std::string& value)
{
	if(!key || !m_jsonObj)
	{
		return false;
	}
	json_object* jsonValue = json_object_new_string(value.c_str());
	json_object_object_add(m_jsonObj, key, jsonValue);
	return true;
}

const char* JsonObject::GetJsonString()
{
	if(!m_jsonObj)
	{
		return NULL;
	}
	return json_object_to_json_string(m_jsonObj);
}

json_object* JsonObject::GetRawSubObject(const char* key) const
{
	if (!key || !m_jsonObj)
    {
        return NULL;
    }
	json_object* jsonObj = json_object_object_get(m_jsonObj, key);
	if(jsonObj)
	{
		json_object_get(jsonObj);
	}
    return jsonObj;
}

bool JsonObject::DeleteJsonObject(const char *key)
{
	if (!key || !m_jsonObj)
    {
        return false;
    }
	json_object_object_del(m_jsonObj, key);
	return true;
}

bool JsonObject::IsValid() const
{
	if(m_jsonObj || is_error(m_jsonObj))
	{
		return false;
	}

	//is_error判断不准确，如果该字符串不符合json格式，此处仍有可能通过
	const char* strJson = json_object_to_json_string(m_jsonObj);
	if( !strJson || strJson[0] != '{' )
	{
		return false;
	}
	return true;
}

bool JsonObject::IsArray() const
{
	if(!m_jsonObj)
	{
		return false;
	}
    return json_object_is_type(m_jsonObj, json_type_array);
}

json_object* JsonObject::GetJsonObject() const
{
	return m_jsonObj;
}

std::vector<std::string> JsonObject::GetAllKeys() const
{
    std::vector<std::string> results;
    if (!IsObject())
    {
        return results;
    }

    json_object_object_foreach(m_jsonObj, key, val)
    {
        results.push_back(key);
    }

    (void)val;

    return results;
}

bool JsonObject::IsObject() const 
{
    if (is_error(m_jsonObj))
    {
        return false;
    }

    return json_object_is_type(m_jsonObj, json_type_object);
}
