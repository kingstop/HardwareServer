#ifndef __JSONHELPER_H__
#define __JSONHELPER_H__
#include <stdio.h>
#include <string.h>
#include "json/json.h"

#define LAZY_JSON_GET_UINT(OUT,KEY,ROOT)\
	do{\
		OUT = JsonHelper::getUInt_Value(KEY,ROOT);\
	}while(0)

#define LAZY_JSON_GET_INT(OUT,KEY,ROOT)\
	do{\
	OUT = JsonHelper::getInt_Value(KEY,ROOT);\
	}while(0)

#define LAZY_JSON_GET_FLOAT(OUT,KEY,ROOT)\
	do{\
		OUT = JsonHelper::getDouble_Value(KEY,ROOT);\
	}while(0)

#define LAZY_JSON_GET_CSTRING(OUT,OUT_LENGTH,KEY,ROOT)\
	do{\
		JsonHelper::getCString_Value(KEY,OUT,OUT_LENGTH,ROOT);\
	}while(0)

#define LAZY_JSON_GET_STRING(OUT,KEY,ROOT)\
	do{\
		OUT = JsonHelper::getString_Value(KEY,ROOT);\
	}while(0)

#define LAZY_JSON_SET_UINT(VALUE,KEY,ROOT)\
	do{\
		JsonHelper::setUInt_Value(KEY,VALUE,ROOT);\
	}while(0)

#define LAZY_JSON_SET_INT(VALUE,KEY,ROOT)\
	do{\
	JsonHelper::setInt_Value(KEY,VALUE,ROOT);\
	}while(0)

#define LAZY_JSON_SET_FLOAT(VALUE,KEY,ROOT)\
	do{\
		JsonHelper::setDouble_Value(KEY,VALUE,ROOT);\
	}while(0)

#define LAZY_JSON_SET_CSTRING(VALUE,KEY,ROOT)\
	do{\
		JsonHelper::setCString_Value(KEY,VALUE,ROOT);\
	}while(0)

class JsonHelper
{
public:
	JsonHelper(const char* pathRelative);
	~JsonHelper(void);

	bool loadJson();
	bool saveJson();
	bool saveJsonToFile( const char* path );

	const Json::Value& root();
	
	static Json::Int getInt_Value(
		const char* key, 
		const Json::Value& root);

	static Json::UInt getUInt_Value(
		const char* key, 
		const Json::Value& root);

	static double getDouble_Value(
		const char* key, 
		const Json::Value& root);

	static std::string getString_Value(
		const char* key,
		const Json::Value& root);

	static void getCString_Value(
		const char* key, 
		char* out_cstr, 
		int out_length,
		const Json::Value& root);

	static void setInt_Value(
		const char* key,
		Json::Int value,
		Json::Value& root
		);

	static void setUInt_Value(
		const char* key,
		Json::UInt value,
		Json::Value& root
		);
	
	static void setDouble_Value(
		const char* key,
		double value,
		Json::Value& root
		);

	static void setString_Value(
		const char* key,
		std::string value,
		Json::Value& root
		);

	static void setCString_Value(
		const char* key,
		const char* cstr_Value,
		Json::Value& root
		);

	void pushRootValue(
		const char* key,
		Json::Value& value
		);

private:
	Json::Value m_root;
	std::string m_pathRelative;
};

#endif
