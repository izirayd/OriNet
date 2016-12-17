#ifndef TableFuntionH
#define TableFuntionH

#include "../OriLng/OriLng.h"
#include "../OriServer/OriServerClient.h"

typedef void(*FunctionSockBuffer)(int Sock, char *Buffer);
typedef void(*FunctionBuffer)(char *Buffer);
typedef void(*FunctionNull)();

typedef void(*FunctionLngClient)(
//#ifndef CLIENT_REALESE
	OriServerClient &Client,
//#endif
	OriLng &Lng);

typedef void(*FunctionLng)(OriLng &Lng);

typedef void(*FunctionEndConnection)(OriServerClient &Client);
typedef void(*FunctionStartConnection)(OriServerClient &Client);

struct TBStructSockBuffer
{
	char Name[32];
	FunctionSockBuffer 
	        Function;
};

struct TBStructBuffer
{
	char Name[32];
	FunctionBuffer 
	        Function;
};

struct TBStructNull
{
	char Name[32];
	FunctionNull 
	        Function;
};

struct TBStructLngClient
{
	char Name[32];
	FunctionLngClient 
	        Function;
	bool GetRunInNewThread() { return isRunInNewThread; }
	bool isRunInNewThread;
	TBStructLngClient()         { isRunInNewThread = false; }
	void RunInNewThread()       { isRunInNewThread = true;  }
	void DisableRunInNewThrea() { isRunInNewThread = false; }
};

struct TBStructLng
{
	char Name[32];
	FunctionLng
	        Function;
};

struct TBStructEndConnection
{
	FunctionEndConnection 
	        Function;
};

struct TBStructStartConnection
{
	FunctionStartConnection 
	        Function;
};


class TableFunction
{
public:
	TableFunction();
	TableFunction(int CountFunction);
	~TableFunction();

	bool AddFunction(char *Name, FunctionSockBuffer Function);
	bool AddFunction(char *Name, FunctionBuffer     Function);
	bool AddFunction(char *Name, FunctionNull       Function);
	bool AddFunction(char *Name, FunctionLngClient  Function);
	bool AddFunction(char *Name, FunctionLng        Function);

	bool RunFunction(char    *Name, int Sock, char *Buffer);
	bool RunFunctionLng(char *Name, OriServerClient &Client, OriLng &Lng);

	void PrintListTables();

	void ReCreateTB(int SizeTB);

	int GetCountMaxFunction() { return CountMaxFunction; }
	void SetCountMaxFunction(int _count) { ReCreateTB(_count); }

	int GetCountUseFunction() { return CountTBSockBuffer + CountTBBuffer + CountTBNull + CountTBLngClient + CountTBLng; }

	void SetRunInNewThread(bool _Y_N) { isUseAlgoritmRunFunctionInNewThread = _Y_N; }
	bool GetRunInNewThread()          { return isUseAlgoritmRunFunctionInNewThread; }

	void UseRunFunctionInThread(char *NameFunction)     { for (int i = 0; i < CountTBLngClient; i++) if (strcmp(NameFunction, TBLngClient[i].Name) == 0) TBLngClient[i].RunInNewThread(); }
	void DisableRunFunctionInThread(char *NameFunction) { for (int i = 0; i < CountTBLngClient; i++) if (strcmp(NameFunction, TBLngClient[i].Name) == 0) TBLngClient[i].DisableRunInNewThrea(); }

private:
	void ClearCount();
	void CreateTB(int Count);

	int CountMaxFunction;
	TBStructSockBuffer *TBSockBuffer; int CountTBSockBuffer;
	TBStructBuffer     *TBBuffer;     int CountTBBuffer;
	TBStructNull       *TBNull;       int CountTBNull;
	TBStructLngClient  *TBLngClient;  int CountTBLngClient;
	TBStructLng        *TBLng;        int CountTBLng;

	bool isUseAlgoritmRunFunctionInNewThread;
};

class TableFunctionStandart
{
  public:
	TableFunctionStandart();
	TableFunctionStandart(int CountFunction);
	bool AddFunction(FunctionNull Function);
	void ReCreateTB(int SizeTB);

	TBStructNull *TBNull;  int CountTBNull;
	
  private:
	void ClearCount();
	void CreateTB(int Count);
	int  CountMaxFunction;

	
};

#endif