#ifndef OriThreadH
#define OriThreadH

#include "../OriKernel/KernelPlatform.h"
#include "../OriFunctionTable/TableFunction.h"

#include "OriMutex.h"

// Function Thread in OS
#if defined ORI_WINDOWS

#include "process.h"

#define KernelCloseThread _endthread();// _endthreadex(0);
#endif

#if defined ORI_LINUX

#include <pthread.h>

 #define KernelCloseThread pthread_exit(0);
#endif

#if defined ORI_WINDOWS

#define ORI_THREAD_FOR_CLASS(NameClass)                  \
bool  NameClass::CreateThread() {                        \
		                                                 \
if (_beginthread(NameClass::RunThread, NULL, this) == -1)\
   return false;                                         \
	 return true;                                        \
	 	                                                 \
}                                                        \
	                                                     \
void NameClass::RunThread(void* p) {                     \
	                                                     \
	NameClass* _this = (NameClass*)p;                    \
	_this->Run();                                        \
	_endthread();                                        \
	                                                     \
}                                                        

#define ThreadObj                                        \
	public:	                                             \
    bool CreateThread();                                 \
	private:                                             \
    void Run();                                          \
    static void  RunThread(void* p);                     \
	public:

#endif


#if defined ORI_LINUX

#define ORI_THREAD_FOR_CLASS(NameClass)                  \
bool  NameClass::CreateThread() {                        \
	                                                     \
   pthread_t th;                                         \
	                                                     \
   if (pthread_create(&th, NULL, NameClass::RunThread, this) != 0) \
     return false;                                       \
                                                         \
   pthread_detach(th);                                   \
                                                         \
   return true;                                          \
	                                                     \
}                                                        \
	                                                     \
void* NameClass::RunThread(void* p) {                    \
                                                         \
	NameClass* _this = (NameClass*)p;                    \
	_this->Run();                                        \
	pthread_exit(0);                                     \
	                                                     \
}                                                        


#define ThreadObj                                        \
                                                         \
   bool CreateThread();                                  \
   void Run();                                           \
   static void*  RunThread(void* p);                     \

#endif


// Function thread               
#define thread(n) ORI_THREAD_FOR_CLASS(n) void n::Run()

class OriThread
{
public:
	void Run();
public:
	OriThread(int MaxTableThread);
public:
	OriThread& operator << (FunctionNull FuncStart);
};

#ifdef ORI_WINDOWS  
#define OriFunctionThread(NameFunction, NameArg)    \
static void NameFunction(void* NameArg)   
#endif                                           

#ifdef ORI_LINUX                     
#define OriFunctionThread(NameFunction, NameArg)    \
static void* NameFunction(void* NameArg)                
#endif    

#if defined ORI_WINDOWS
#define FUNCTION_RUN void(ORI_CONVETION * _StartAddress) (void *)
#endif

#if defined ORI_LINUX
#define FUNCTION_RUN void *_StartAddress  (void *)
#endif

bool OriCreateThread(FUNCTION_RUN, void* Param);


struct MutexArray {
	OriMutex NewConnection;
	OriMutex EndConnection;
};


#endif

