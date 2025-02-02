#ifndef __CONFIGURATION_TOOL_H__
#define __CONFIGURATION_TOOL_H__

#include "JSON/parson.h"

#include "Globals.h"

/* JSON custom wrapper */
class JSON_Wrapper
{
public:
	JSON_Wrapper(const char* file, const char* node);
	~JSON_Wrapper();

	JSON_Wrapper& GetNode(const char* section_name);

	bool AddArray(const char* array_name);

public:
	int GetInt(const char* name);
	uint GetUint(const char* name);
	float GetFloat(const char* name);
	bool GetBool(const char* name);
	const char* GetString(const char* name);

	void SetInt(const char* name, const int& value);
	void SetUint(const char* name, const uint& value);
	void SetFloat(const char* name, const float& value);
	void SetBool(const char* name, const bool& value);
	void SetString(const char* name, const char* value);

public:
	JSON_Value* j_value = nullptr;
	JSON_Object* j_object = nullptr;
	JSON_Object* j_node_object = nullptr;
	JSON_Array* j_array = nullptr;
};

#endif /* __CONFIGURATION_TOOL_H__ */