#ifndef KernelWinAPIH
#define KernelWinAPIH

#include "KernelPlatform.h"

#if defined ORI_WINDOWS

#define USE_WIN_API


#ifdef USE_WIN_API

//#include <WS2tcpip.h>
//#include <winsock2.h>

#include <windows.h>

#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <Shlwapi.h>

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "Shlwapi.lib")

#endif // USE_WIN_API

#endif

#endif