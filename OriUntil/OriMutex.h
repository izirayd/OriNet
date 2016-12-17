#ifndef OriMutexH
#define OriMutexH

 #include "../OriKernel/KernelPlatform.h"

#ifdef ORI_WINDOWS
  #include "../OriKernel/KernelWinAPI.h"
#endif

#ifdef ORI_LINUX
  #include <pthread.h>
#endif // ORI_LINUX



#ifdef ORI_WINDOWS

class OriBaseMutex
{
public:
	OriBaseMutex() 	{	ghMutex = CreateMutex(NULL, FALSE, NULL);	}

	~OriBaseMutex() {
		if (ghMutex != NULL)
			CloseHandle(ghMutex);
	}

	void lock()	{
		WaitForSingleObject(ghMutex, INFINITE);
	}

	void unlock()	{
		ReleaseMutex(ghMutex);
	}

	HANDLE ghMutex;
};

#endif // ORI_WINDOWS

#ifdef ORI_LINUX
class OriBaseMutex
{
public:
	OriBaseMutex() 	{
		pthread_mutex_init(&ghMutex, NULL);
	}

	~OriBaseMutex() {

		pthread_mutex_destroy(&ghMutex);
	}

	void lock()	{
		pthread_mutex_lock(&ghMutex);
	}

	void unlock()	{
		pthread_mutex_unlock(&ghMutex);
	}

private:
	pthread_mutex_t ghMutex;

};
#endif // ORI_LINUX

#ifndef USE_MUTEX_STD11
typedef OriBaseMutex OriMutex;
#else
#include <mutex>
typedef OriBaseMutex  OriMutex;
#endif

#endif
