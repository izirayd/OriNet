#include "OriThread.h"
#include "../OriKernel/KernelSocket.h"

TableFunctionStandart TBFunction;

#if defined ORI_WINDOWS
unsigned __stdcall _RunStart(void* param)
#endif
#if defined ORI_LINUX
void *_RunStart(void* param)
#endif
{
	int ID = *((int*)(&param));
	((*TBFunction.TBNull[ID].Function)());
	return 0;
}

bool NewThread(void* Param) {

#if defined ORI_WINDOWS

	unsigned threadID;
	if (_beginthreadex(NULL, 800, _RunStart, Param, 0, &threadID) == 0)
		return false;

#endif

#if defined ORI_LINUX

	pthread_t th;
	if (pthread_create(&th, NULL, _RunStart, Param) != 0)
		return false;
	pthread_detach(th);

#endif
	return true;
}

void OriThread::Run()  {
	for (int i = 0; i < TBFunction.CountTBNull; i++)
	NewThread((void *)i);
}

OriThread& OriThread::operator << (FunctionNull FuncStart) {
	TBFunction.AddFunction(FuncStart);
	return *this;
}

OriThread::OriThread(int CountThread) {
	TBFunction.ReCreateTB(CountThread);
}

bool OriCreateThread(FUNCTION_RUN, void* Param)
{
#if defined ORI_WINDOWS

	if (_beginthread(_StartAddress, 8, Param) == 0)
		return false;

#endif

#if defined ORI_LINUX

	pthread_t th;
	if (pthread_create(&th, NULL, _StartAddress, Param) != 0)
		return false;
	pthread_detach(th);

#endif
	return true;
}