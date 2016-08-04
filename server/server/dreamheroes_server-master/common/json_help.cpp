#include "json_help.h"

JsonHelper::JsonHelper(const char* pathRelative)
{
	m_pathRelative = pathRelative;
}

JsonHelper::~JsonHelper(void)
{
}

bool JsonHelper::loadJson()
{
	Json::Reader reader;
    unsigned long szFile = 0;

    FILE* file = fopen( m_pathRelative.c_str(),"rb");
    if (!file)
    { return false;}

    fseek(file,0,SEEK_END);
    szFile = ftell(file);
    fseek(file,0,SEEK_SET);
    unsigned char* pObjData = new unsigned char[szFile + 1];
    szFile = fread( pObjData, sizeof( unsigned char), szFile, file);
    pObjData[szFile] = 0;
    fclose(file);

    if(pObjData != NULL)
    {
        if ( !reader.parse((char*)pObjData, m_root) )
        {
            delete[] pObjData;
            return false;
        }
        
        delete[] pObjData;
        return true;
    }
    return false;
}

bool JsonHelper::saveJson()
{		
	Json::StyledWriter writer;
	std::string buffer = writer.write( m_root );
	FILE* file = fopen( m_pathRelative.c_str(), "w+b" );
	if ( file )
	{
		fwrite( buffer.c_str(), sizeof(char), buffer.size(), file);
		fclose( file );
		return true;
	}
	return false;
}

bool JsonHelper::saveJsonToFile( const char* path ){
	Json::StyledWriter writer;
	std::string buffer = writer.write( m_root );
	FILE* file = fopen( path, "w+b" );
	if ( file )
	{
		fwrite( buffer.c_str(), sizeof(char), buffer.size(), file);
		fclose( file );
		return true;
	}
	return false;
}

const Json::Value& JsonHelper::root()
{
	return m_root;
}

Json::Int JsonHelper::getInt_Value(const char* key, const Json::Value& root)
{
	Json::Int outValue = 0;
	const Json::Value& KeyValue = root[key];
	if( KeyValue.isIntegral() )
		outValue = KeyValue.asInt();

	return outValue;
}

Json::UInt JsonHelper::getUInt_Value(const char* key, const Json::Value& root)
{
	Json::UInt outValue = 0;
	const Json::Value& KeyValue = root[key];
	if( KeyValue.isIntegral() )
		outValue = KeyValue.asUInt();

	return outValue;
}

double JsonHelper::getDouble_Value(const char* key, const Json::Value& root)
{
	double outValue = 0;
	const Json::Value& KeyValue = root[key];
	if( KeyValue.isDouble() )
		outValue = KeyValue.asDouble();

	return outValue;
}

std::string JsonHelper::getString_Value(const char* key,const Json::Value& root)
{
	std::string outString;
	const Json::Value& KeyValue = root[key];
	if( KeyValue.isString() )
		outString = KeyValue.asString();

	return outString;
}

void JsonHelper::getCString_Value(const char* key,char* out_cstr,int out_length,const Json::Value& root)
{
	memset(out_cstr, 0, out_length);
	const Json::Value& KeyValue = root[key];
	if( KeyValue.isString() )
		strncpy(out_cstr, KeyValue.asCString(), out_length-1);
}

void JsonHelper::setInt_Value(const char* key,Json::Int value,Json::Value& root)
{
	root[key] = value;
}

void JsonHelper::setUInt_Value(const char* key,Json::UInt value,Json::Value& root)
{
	root[key] = value;
}

void JsonHelper::setDouble_Value(const char* key,double value,Json::Value& root)
{
	root[key] = value;
}

void JsonHelper::setString_Value(const char* key,std::string value,Json::Value& root)
{
	root[key] = value;
}

void JsonHelper::setCString_Value(const char* key,const char* cstr_Value,Json::Value& root)
{
	root[key] = cstr_Value;
}

void JsonHelper::pushRootValue(const char* key,Json::Value& value)
{
	m_root[key] = value;
}