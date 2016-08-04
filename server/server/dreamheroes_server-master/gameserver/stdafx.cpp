#include "stdafx.h"

std::string Utf8ToGBK(const char* strUtf8)
{
	//int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, NULL,0); 
	//unsigned short * wszGBK = new unsigned short[len+1];       
	//memset(wszGBK, 0, len * 2 + 2); 
	//MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, (LPWSTR)wszGBK, len);
	//len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	//
	//char *szGBK=new char[len + 1]; 
	//memset(szGBK, 0, len + 1); 
	//WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL,NULL);
	std::string temp(strUtf8);
	//delete szGBK;
	return temp; 
}

std::string GBKToUtf8(const char* strGBK)
{ 
	//int len=MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL,0); 
	//unsigned short * wszUtf8 = new unsigned short[len+1]; 
	//memset(wszUtf8, 0, len * 2 + 2); 
	//MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, (LPWSTR)wszUtf8, len);
	//len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
	//char *szUtf8=new char[len + 1]; 
	//memset(szUtf8, 0, len + 1); 
	//WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, (LPSTR)szUtf8, len, NULL,NULL);
	std::string temp(strGBK);
	//delete szUtf8;
	return temp; 
}