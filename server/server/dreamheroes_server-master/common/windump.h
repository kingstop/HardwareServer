#ifdef WIN32

#pragma once


#include <Windows.h>
#include <WinBase.h>
#include <string>
#include "dbghelp.h"

typedef void (*CRASHCALLBACKFUNC)();
typedef void (*CRASHCALLBACKFUNC_WITHPATH)( const char* pszDumpFileName,const char* pszDumpResult );


class MiniDumper
{
private:
    static LPCSTR m_szAppName;
    static LPCSTR m_szAppVersion;
    static LPCSTR m_szDumpDesc;

#ifdef _DEBUG
public:
#endif
    static LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExceptionInfo);

    static char m_szCmdLinePrefix[MAX_PATH];   // �쳣������ִ�е�������
    static char m_szExeNameToReboot[MAX_PATH]; // �쳣������������������ִ�еĳ����ļ���
    
public:
	static CRASHCALLBACKFUNC pCrashFunc;
	static CRASHCALLBACKFUNC_WITHPATH pCrashFunc_WithPath;
    static bool m_bDebugMode;

public:
    // ȱʡ��ʽ��ֻ���� Dump �ļ�������
    // �������Ի���֪ͨ
    MiniDumper( LPCSTR DumpFileNamePrefix, LPCSTR AppVersion, LPCSTR DumpDesc );

    // BugReport��ʽ�������ļ���ִ�� CmdLine
    // �����������ʹ��󱨸��

    // Param List:    
    // DumpFileNamePrefix ������Dump�ļ����ļ���ǰ׺
    // CmdLine            ����Dump֮��ִ�е�WinExec��������ʽΪ��"Bugreport.exe + �ո� + ������������, + �˿ڲ���, +·������"
    // ExeNameToReboot    ���Dump����������Crash���ĳ�����ļ�����Ŀǰֻ���BugReporter.exe��
    MiniDumper(LPCSTR DumpFileNamePrefix, LPCSTR AppVersion, LPCSTR DumpDesc, LPCSTR CmdLine, LPCSTR ExeNameToReboot = NULL);

	~MiniDumper();
private:
	bool SetUnhandleExpt();
};


#endif