#include "OriSleep.h"
#include "../OriKernel/KernelSocket.h"

void OriSleep(int TimeSleep) 
{
#if defined ORI_WINDOWS
	Sleep(1000 * TimeSleep);
#endif

#if defined ORI_LINUX
    sleep(TimeSleep);
#endif
}

void OriMicroSleep(int MicroTimeSleep)
{
#if defined ORI_WINDOWS
	Sleep(MicroTimeSleep);
#endif

#if defined ORI_LINUX
	usleep(MicroTimeSleep);
#endif
}