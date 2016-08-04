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

    static char m_szCmdLinePrefix[MAX_PATH];   // 异常发生后执行的命令行
    static char m_szExeNameToReboot[MAX_PATH]; // 异常发生后用于重启程序执行的程序文件名
    
public:
	static CRASHCALLBACKFUNC pCrashFunc;
	static CRASHCALLBACKFUNC_WITHPATH pCrashFunc_WithPath;
    static bool m_bDebugMode;

public:
    // 缺省方式，只保存 Dump 文件到本地
    // 并弹出对话框通知
    MiniDumper( LPCSTR DumpFileNamePrefix, LPCSTR AppVersion, LPCSTR DumpDesc );

    // BugReport方式，保存文件并执行 CmdLine
    // 可以用来发送错误报告等

    // Param List:    
    // DumpFileNamePrefix 产生的Dump文件的文件名前缀
    // CmdLine            生成Dump之后，执行的WinExec参数，形式为："Bugreport.exe + 空格 + 服务器名参数, + 端口参数, +路径参数"
    // ExeNameToReboot    完成Dump后，用于重启Crash掉的程序的文件名（目前只针对BugReporter.exe）
    MiniDumper(LPCSTR DumpFileNamePrefix, LPCSTR AppVersion, LPCSTR DumpDesc, LPCSTR CmdLine, LPCSTR ExeNameToReboot = NULL);

	~MiniDumper();
private:
	bool SetUnhandleExpt();
};


#endif