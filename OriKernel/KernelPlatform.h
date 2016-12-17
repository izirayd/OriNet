#if defined(_WIN32) || defined(_WIN64)
 #define ORI_WINDOWS
#else
 #define ORI_LINUX
#endif

#define ORI_CDECL   __cdecl
#define ORI_STDCALL __stdcall
#define ORI_CONVETION ORI_CDECL

#define STD98 1998
#define STD03 2003
#define STD11 2011
#define STD14 2014
#define STD17 2017

#if (VERSION_STD >= STD11)
 #define  USE_MUTEX_STD11
#endif

#pragma warning(disable : 4482)
#pragma warning(disable : 4995)

/* Использовать -> https://sourceforge.net/p/predef/wiki/Compilers/ */

#if defined(__clang__)
/* Clang/LLVM. ---------------------------------------------- */
 #define ORI_CLANG

#elif defined(__ICC) || defined(__INTEL_COMPILER)
/* Intel ICC/ICPC. ------------------------------------------ */
 #define ORI_ICC

#elif defined(__GNUC__) || defined(__GNUG__)
/* GNU GCC/G++. --------------------------------------------- */
  #define ORI_GCC

#elif defined(__HP_cc) || defined(__HP_aCC)
/* Hewlett-Packard C/aC++. ---------------------------------- */
  #define ORI_HPCC

#elif defined(__IBMC__) || defined(__IBMCPP__)
/* IBM XL C/C++. -------------------------------------------- */
  #define ORI_IBM

#elif defined(_MSC_VER)
/* Microsoft Visual Studio. --------------------------------- */
  #define ORI_VSTUDIO

#elif defined(__PGI)
/* Portland Group PGCC/PGCPP. ------------------------------- */
  #define ORI_PGI

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
/* Oracle Solaris Studio. ----------------------------------- */
  #define ORI_SUNPRO

#elif defined(__BORLANDC__) || (__CODEGEARC__) || (__TURBOC__)
/* Borland C++ or C++ Builder */
  #define ORI_BORLAND

#endif

#ifndef ORI_USE_DEBUG

#ifdef ORI_BORLAND
   #pragma comment(lib, "OriNet.lib")
#endif

#ifdef ORI_VSTUDIO
#pragma comment(lib, "OriNet.lib")
#pragma comment(lib, "OriCrypt.lib")
#pragma comment(lib, "OriFunctionTable.lib")
#pragma comment(lib, "OriGeoIP.lib")
#pragma comment(lib, "OriKernel.lib")
#pragma comment(lib, "OriLng.lib")
#pragma comment(lib, "OriUntil.lib")
#pragma comment(lib, "SuperInt.lib")
#endif

#endif

#ifdef ORI_GCC
  #pragma comment(lib, "libOriNet.so")
#endif

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <fcntl.h>
#include <cstring>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <limits.h>
#include <stdarg.h>

#if defined(ORI_WINDOWS) 

  #include <xstring>
  #include <io.h>

#endif

#if defined(ORI_LINUX) 

#include <unistd.h>

#endif

#if defined(_WIN32) || defined(_WIN64)
// #ifdef  __TURBOC__
 //  #define VSNPRINTF	StringCchVPrintf
 //  #define VSNWPRINTF	StringCchVWPrintf
 //  #define OriSpintfUnicode wsprintfW
// #else 
   #define VSNPRINTF		 _vsnprintf
   #define VSNWPRINTF		 _vsnwprintf
   #define OriSpintfUnicode  wsvnprintf
// #endif
#else
    #define VSNPRINTF		 vsnprintf
    #define VSNWPRINTF		 vswprintf
    #define OriSpintfUnicode swprintf
#endif

#define ORI_ERROR -1
typedef int32_t  OriError;
