/* --------------------------------------------------------------
    Timer and priority functions.

    Copyright 1998-2009 by Dr. Raul N.Shakirov, IMach of RAS(UB).
    All Rights Reserved.

    Permission has been granted to copy, distribute and modify
    software in any context without fee, including a commercial
    application, provided that the aforesaid copyright statement
    is present here as well as exhaustive description of changes.

    THE SOFTWARE IS DISTRIBUTED "AS IS". NO WARRANTY OF ANY KIND
    IS EXPRESSED OR IMPLIED. YOU USE AT YOUR OWN RISK. THE AUTHOR
    WILL NOT BE LIABLE FOR DATA LOSS, DAMAGES, LOSS OF PROFITS OR
    ANY OTHER KIND OF LOSS WHILE USING OR MISUSING THIS SOFTWARE.
-------------------------------------------------------------- */
#include <time.h>       /* Header file for standard time functions */
#include "Gettimer.h"   /* Header file for GetTimer function */

#ifdef  __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* --------------------------------------------------------------
    Function GetTimer returns difference in ms between current
    process elapsed time and given value.

    Parameter:
    timer           Time to subtract from elapsed time.

    Returns:        Difference between current elapsed time
                    and given value.
-------------------------------------------------------------- */

long    GetTimer (long timer)
{
#if CLOCKS_PER_SEC == 1000 || CLOCKS_PER_SEC == 100 ||  CLOCKS_PER_SEC == 10
  long time = (long) ((long)clock() * (1000 / CLOCKS_PER_SEC) - timer);
#else
  long time = (long) ((long)clock() * (1000. / CLOCKS_PER_SEC) - timer);
#endif

  return (time >= 0? time: 0L);
}

/* --------------------------------------------------------------
    Function SetHighPriority sets high priority class for
    the application. At first start, is also stores existing
    priority class, which is set for application by system.

    Function SetIdlePriority sets idle priority class for
    the application. At first start, is also stores existing
    priority class, which is set for application by system.

    Function ResetPriority either resets the stored priority
    class or does noting if no priority class is stored.

    Returns:        ~0  on success, 0 on fail.

    NOTE:   Implemented for Win32 if macro __STDC__ is not set.
-------------------------------------------------------------- */

#ifndef __STDC__
#ifdef  _WIN32
#define COMPILING_FOR_WIN32
#endif/*_WIN32*/
#ifdef  __WIN32__
#ifndef __DPMI32__
#define COMPILING_FOR_WIN32
#endif/*__DPMI32__*/
#endif/*__WIN32__*/
#endif/*__STDC__*/

#ifdef  COMPILING_FOR_WIN32
void* __stdcall GetCurrentProcess (void);
unsigned __stdcall GetPriorityClass (void * hProcess);
int __stdcall SetPriorityClass (void * hProcess, unsigned dwPriorityClass);
#define NORMAL_PRIORITY_CLASS       0x00000020
#define IDLE_PRIORITY_CLASS         0x00000040
#define HIGH_PRIORITY_CLASS         0x00000080
#define REALTIME_PRIORITY_CLASS     0x00000100
#endif/*COMPILING_FOR_WIN32*/

#ifdef  COMPILING_FOR_WIN32
unsigned dwPriorityClass = 0;
#endif/*COMPILING_FOR_WIN32*/

int     SetHighPriority (void)
{
  int retcode = 0;
#ifdef  COMPILING_FOR_WIN32
  if (dwPriorityClass == 0)
  {
    dwPriorityClass = GetPriorityClass (GetCurrentProcess());
  }
  if (dwPriorityClass != 0)
  {
    retcode = SetPriorityClass (GetCurrentProcess(), HIGH_PRIORITY_CLASS);
  }
#endif/*COMPILING_FOR_WIN32*/
  return retcode;
}

int     SetIdlePriority (void)
{
  int retcode = 0;
#ifdef  COMPILING_FOR_WIN32
  if (dwPriorityClass == 0)
  {
    dwPriorityClass = GetPriorityClass (GetCurrentProcess());
  }
  if (dwPriorityClass != 0)
  {
    retcode = SetPriorityClass (GetCurrentProcess(), IDLE_PRIORITY_CLASS);
  }
#endif/*COMPILING_FOR_WIN32*/
  return retcode;
}

int     ResetPriority (void)
{
  int retcode = 0;
#ifdef  COMPILING_FOR_WIN32
  if (dwPriorityClass != 0)
  {
    retcode = SetPriorityClass (GetCurrentProcess(), dwPriorityClass);
  }
#endif/*COMPILING_FOR_WIN32*/
  return retcode;
}

#ifdef  __cplusplus
}
#endif/*__cplusplus*/
