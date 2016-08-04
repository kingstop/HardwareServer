#ifdef WIN32



#include "windump.h"

#pragma warning( push )
#pragma warning( disable : 4311 )

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                         CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                         CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                         CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                         );

LPCSTR	MiniDumper::m_szAppName;
LPCSTR	MiniDumper::m_szAppVersion;
LPCSTR	MiniDumper::m_szDumpDesc;
char	MiniDumper::m_szCmdLinePrefix[ MAX_PATH ];		// 异常发生后执行的命令行
char	MiniDumper::m_szExeNameToReboot[ MAX_PATH ];	// 异常发生后用于重启程序执行的程序文件名
bool    MiniDumper::m_bDebugMode = false;

CRASHCALLBACKFUNC MiniDumper::pCrashFunc = NULL;        // 异常发生后的回调函数，设置函数地址时请声明类以后继续指定

CRASHCALLBACKFUNC_WITHPATH MiniDumper::pCrashFunc_WithPath = NULL; // 异常后的回调函数，参数为dumper文件的路径

#if defined _M_X64 || defined _M_IX86
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI 
MyDummySetUnhandledExceptionFilter(
								   LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return NULL;
}
#else
#error "This code works only for x86 and x64!"
#endif

BOOL PreventSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibrary(TEXT("kernel32.dll"));
	if (hKernel32 == NULL) return FALSE;
	void *pOrgEntry = GetProcAddress(hKernel32, 
		"SetUnhandledExceptionFilter");
	if(pOrgEntry == NULL) return FALSE;

	DWORD dwOldProtect = 0;
	SIZE_T jmpSize = 5;
#ifdef _M_X64
	jmpSize = 13;
#endif
	BOOL bProt = VirtualProtect(pOrgEntry, jmpSize, 
		PAGE_EXECUTE_READWRITE, &dwOldProtect);
	BYTE newJump[20];
	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
#ifdef _M_IX86
	DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
	dwOrgEntryAddr += jmpSize; // add 5 for 5 op-codes for jmp rel32
	DWORD dwNewEntryAddr = (DWORD) pNewFunc;
	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;
	// JMP rel32: Jump near, relative, displacement relative to next instruction.
	newJump[0] = 0xE9;  // JMP rel32
	memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
#elif _M_X64
	// We must use R10 or R11, because these are "scratch" registers 
	// which need not to be preserved accross function calls
	// For more info see: Register Usage for x64 64-Bit
	// http://msdn.microsoft.com/en-us/library/ms794547.aspx
	// Thanks to Matthew Smith!!!
	newJump[0] = 0x49;  // MOV R11, ...
	newJump[1] = 0xBB;  // ...
	memcpy(&newJump[2], &pNewFunc, sizeof (pNewFunc));
	//pCur += sizeof (ULONG_PTR);
	newJump[10] = 0x41;  // JMP R11, ...
	newJump[11] = 0xFF;  // ...
	newJump[12] = 0xE3;  // ...
#endif
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, newJump, jmpSize, &bytesWritten);

	if (bProt != FALSE)
	{
		DWORD dwBuf;
		VirtualProtect(pOrgEntry, jmpSize, dwOldProtect, &dwBuf);
	}
	return bRet;
}

MiniDumper::~MiniDumper()
{
	if( m_szAppName )
	{
		free( (void*)m_szAppName );
		m_szAppName = 0;
	}
	if( m_szAppVersion )
	{
		free( (void*)m_szAppVersion );
		m_szAppVersion = 0;
	}
	if( m_szDumpDesc )
	{
		free( (void*)m_szDumpDesc );
		m_szDumpDesc = 0;
	}
}
MiniDumper::MiniDumper( LPCSTR DumpFileNamePrefix, LPCSTR AppVersion, LPCSTR DumpDesc )
{
    // if this // ToLog fires then you have two instances of MiniDumper
    // which is not allowed
    // ToLog(m_szAppName == NULL);

    m_szCmdLinePrefix[0] = '\0';
    m_szAppName    = DumpFileNamePrefix ? _strdup(DumpFileNamePrefix) : _strdup("Application");
    m_szAppVersion = AppVersion ? _strdup(AppVersion) : _strdup("0.0.0.1");
    m_szDumpDesc   = DumpDesc ? _strdup(DumpDesc) : _strdup("Saved dump file to '%s'");

    //::SetUnhandledExceptionFilter(TopLevelFilter);
	SetUnhandleExpt();
}

MiniDumper::MiniDumper(LPCSTR DumpFileNamePrefix, LPCSTR AppVersion, LPCSTR DumpDesc, LPCSTR CmdLine, LPCSTR ExeNameToReboot /* = NULL */ )
{
    // if this // ToLog fires then you have two instances of MiniDumper
    // which is not allowed
    // ToLog(m_szAppName==NULL);

    m_szCmdLinePrefix[0] = '\0';
    m_szExeNameToReboot[0] = '\0';
    m_szAppName = DumpFileNamePrefix ? _strdup(DumpFileNamePrefix) : _strdup("Application");
    m_szAppVersion = AppVersion ? _strdup(AppVersion) : _strdup("0.0.0.1");
    m_szDumpDesc   = DumpDesc ? _strdup(DumpDesc) : _strdup("Saved dump file to '%s'");

    //::SetUnhandledExceptionFilter(TopLevelFilter);
	SetUnhandleExpt();
	
    strncpy_s( m_szCmdLinePrefix, sizeof( m_szCmdLinePrefix ), CmdLine, sizeof( m_szCmdLinePrefix ) - 1 );

    if( ExeNameToReboot != NULL )
    { strncpy_s( m_szExeNameToReboot, sizeof( m_szExeNameToReboot ), ExeNameToReboot, sizeof( m_szExeNameToReboot ) - 1 ); }
}

LONG MiniDumper::TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
    //MessageBox(0,"catch err",0,0);
    LONG retval = EXCEPTION_CONTINUE_SEARCH;
    HWND hParent = NULL;						// find a better value for your app
    SYSTEMTIME time;
    GetLocalTime(&time);

    // firstly see if dbghelp.dll is around and has the function we need
    // look next to the EXE first, as the one in System32 might be old 
    // (e.g. Windows 2000)
    HMODULE hDll = NULL;
    char szDbgHelpPath[_MAX_PATH];
    std::string strFilePath;
    strFilePath.clear();

    /*
    if(GetModuleFileNameA( NULL, szDbgHelpPath, _MAX_PATH ))
    {
        char *pSlash = strchr(szDbgHelpPath, '\\');
        if (pSlash)
        {
            strcpy(pSlash+1, "DBGHELP.DLL");
            hDll = ::LoadLibraryA( szDbgHelpPath);
        }
    }
    */

    if(GetCurrentDirectoryA(sizeof(szDbgHelpPath)/sizeof(char) - 1,szDbgHelpPath) != 0)
    {
        strFilePath = szDbgHelpPath;
        strFilePath += "\\dbghelp.dll";
        hDll = LoadLibraryA(strFilePath.c_str());
    }


    if (hDll==NULL)
    {
        // load any version we can
        hDll = ::LoadLibraryA("DBGHELP.DLL");
    }

    LPCSTR  szResult = NULL;
    char	szDumpPath[_MAX_PATH];
    char	szScratch [_MAX_PATH];

    if (hDll)
    {
        MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
        if (pDump)
        {
            // work out a good place for the dump file
            //			if (!GetTempPath( _MAX_PATH, szDumpPath ))
            //				_tcscpy( szDumpPath, "c:\\temp\\" );
#ifdef GAME_CLIENT            
			::CreateDirectory( "Log", NULL );
			//sprintf(szDumpPath, "Log\\%s_%d-%d_%d-%d-%d.dmp", m_szAppName, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
			char acTemp[255] = {0};
			MEMORYSTATUS M;
			GlobalMemoryStatus(&M);
			sprintf_s( acTemp, "\n总物理内存: %d, 可用物理内存: %d.\n总虚拟内存: %d, 可用虚拟内存: %d", 
				M.dwTotalPhys / 1024 / 1024, M.dwAvailPhys / 1024 / 1024,
				M.dwTotalVirtual / 1024 / 1024, M.dwAvailVirtual / 1024 / 1024);
			if( ( ( M.dwAvailPhys / 1024 / 1024 ) < 100 ) || ( ( M.dwAvailVirtual / 1024 / 1024 ) < 100 ) )//low memory
				sprintf(szDumpPath, "Log\\%s-%d_%d_%d_%d_%d-LowMemory.dmp", m_szAppName, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );
			else
				sprintf(szDumpPath, "Log\\%s-%d_%d_%d_%d_%d.dmp", m_szAppName, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );
#else
            sprintf(szDumpPath, "%s[%s]_%d-%d_%d-%d-%d.dmp", m_szAppName, m_szAppVersion, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
#endif // CLIENT

            // don't ask user
            // ask the user if they want to save a dump file
            //if (strlen(m_szCmdLinePrefix) > 0 )||
                //::MessageBoxA(NULL, "WARNING: An exception occured,save the dump file to debug it？",m_szAppName, MB_YESNO )==IDYES)
            {
                // create the file
                HANDLE hFile = ::CreateFileA( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, NULL );

                if (hFile!=INVALID_HANDLE_VALUE)
                {
                    _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

                    ExInfo.ThreadId = ::GetCurrentThreadId();
                    ExInfo.ExceptionPointers = pExceptionInfo;
                    ExInfo.ClientPointers = NULL;

                    // write the dump
                    // MiniDumpNormal

                    BOOL bOK = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
                    if (bOK)
                    {
#ifdef GAME_CLIENT
                        // sprintf(szScratch, theXmlString.GetString( eDumper_MessageBox_ClientError ) );
                        sprintf(szScratch, m_szDumpDesc );
#else                   
						sprintf(szScratch, m_szDumpDesc, szDumpPath);
#endif //GAME_CLIENT
                        szResult = szScratch;
                        retval = EXCEPTION_EXECUTE_HANDLER;
                    }
                    else
                    {
// #ifdef GAME_CLIENT
//                         sprintf(szScratch, theXmlString.GetString( eDumper_MessageBox_ClientError ) );
// #else
// 						sprintf(szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError());
// #endif // GAME_CLIENT
                        sprintf_s(szScratch,sizeof(szScratch)/sizeof(char) - 1,"1 error %u",GetLastError() );
                        szResult = szScratch;

                    }
                    ::CloseHandle(hFile);
                }
                else
                {
// #ifdef GAME_CLIENT
// 					sprintf(szScratch, theXmlString.GetString( eDumper_MessageBox_ClientError ) );
// #else
//                     sprintf(szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError());
// #endif // GAME_CLIENT
                    sprintf_s(szScratch,sizeof(szScratch)/sizeof(char) - 1,"2 error %u",GetLastError());
                    szResult = szScratch;
                }
            }
        }
        else
        {
            szResult = "DBGHELP.DLL too old";
        }
    }
    else
    {
        szResult = "DBGHELP.DLL not found";
    }

    // 如果不使用BugReport的话

    if(strlen(m_szCmdLinePrefix) == 0 || m_bDebugMode)
    {
        if (szResult)
        { ::MessageBoxA(NULL, szResult, m_szAppName, MB_OK); }
    }

    if(strlen(m_szCmdLinePrefix) > 0)
    {
        char CmdLine[MAX_PATH] = {0};

        if(strlen(m_szExeNameToReboot) == 0)
            // 对应执行 CmdLine 后不需要重启程序的参数
            sprintf(CmdLine,"%s,%s", m_szCmdLinePrefix, szDumpPath);
        else
            // 对应执行 CmdLine 后需要重启程序的参数
            sprintf(CmdLine,"%s,%s,%s", m_szCmdLinePrefix, szDumpPath, m_szExeNameToReboot);

        WinExec(CmdLine, SW_SHOWNORMAL);
    }

	if( pCrashFunc )
	{
		pCrashFunc();
	}

	if( pCrashFunc_WithPath )
	{
		pCrashFunc_WithPath( szDumpPath,szResult );
	}

    return retval;
}

bool MiniDumper::SetUnhandleExpt()
{
	::SetUnhandledExceptionFilter(TopLevelFilter);
	return PreventSetUnhandledExceptionFilter() != FALSE;
}

#pragma warning( pop )
#endif