#ifndef OriFileH
#define OriFileH

#include "../OriKernel/KernelPlatform.h"
#include "../OriKernel/KernelWinAPI.h"

#include "StringFunction.h"
#pragma warning(disable : 4996)

#ifdef ORI_LINUX
 #define MAX_PATH 256
#endif

#ifdef ORI_WINDOWS

enum TableEncoding
{
	ORI_UTF8 = 0 ,
	ORI_UTF16LE,
	ORI_UNICODE,
	ORI_UNKNOW
} ;

enum OriFileStatus
{
	READ        = 0,    // Открывает для чтения. Если файл не существует или его невозможно найти, вызов fopen завершается ошибкой.
	WRITE       = 1,    // Открывает пустой файл для записи. Если указанный файл существует, его содержимое удаляется.
	NWRITE      = 2,    // Открывается для записи в конце файла (добавление) без удаления маркера в конце файла (EOF) перед записью новых данных в файл. Создает файл, если он не существует.
	NREWR       = 3,    // Открывает для чтения и записи. Файл должен существовать.
	CLREWR      = 4,    // Открывает пустой файл для чтения и записи.Если файл существует, его содержимое удаляется.
	NCRREWR     = 5     // Открывается для чтения и добавления. Операция добавления включает удаления маркера EOF перед записью новых данных в файл. Маркер EOF не восстанавливается по окончании записи. Создает файл, если он не существует.
};

#else

typedef int32_t TableEncoding;
typedef int32_t OriFileStatus;

#define ORI_UTF8      0
#define ORI_UTF16LE   1
#define ORI_UNICODE   2
#define ORI_UNKNOW    3

#define     READ        0
#define 	WRITE       1
#define 	NWRITE      2
#define 	NREWR       3
#define 	CLREWR      4
#define 	NCRREWR     5

#endif

#define OR  ||
#define AND &&


enum
{
	ORI_SUCCESS = 1             // Удачно
};

class OriFile
{

public:

	OriFile() {
		FileDescriptor = NULL;
	}

	uint32_t GetSizeFile()	{
	
		fseek(GetDesc(), 0, SEEK_END);
		return ftell(GetDesc());
	}

	OriError FullWriteFile(char *Buffer, uint32_t WriteSize, uint32_t SizeBuffer)
	{
		fseek(GetDesc(), 0, SEEK_SET);
		fwrite(Buffer, WriteSize, SizeBuffer, GetDesc());
		return ORI_SUCCESS;
	}

	OriError FullReadFile(char *Buffer, uint32_t ReadSize)
	{
		uint32_t SizeFile = GetSizeFile();
		fseek(GetDesc(), 0, SEEK_SET);
	
		fread(Buffer, ReadSize, SizeFile, GetDesc());

		return ORI_SUCCESS;
	}

	OriError OpenFile(const char *NameFile, OriFileStatus StatusOpen = READ) 	{

	 if (NameFile == NULL)     return ORI_ERROR;
	 if (NameFile[0] == 0x00)  return ORI_ERROR;

	 if (StatusOpen == READ)
		if ((FileDescriptor = fopen(NameFile,  "rb")) == NULL) return ORI_ERROR;

	 if (StatusOpen == WRITE)
		 if ((FileDescriptor = fopen(NameFile, "wb")) == NULL) return ORI_ERROR;

	 if (StatusOpen == NWRITE)
		 if ((FileDescriptor = fopen(NameFile, "ab")) == NULL) return ORI_ERROR;

	 if (StatusOpen == NREWR)
		 if ((FileDescriptor = fopen(NameFile, "rb+")) == NULL) return ORI_ERROR;

	 if (StatusOpen == CLREWR)
		 if ((FileDescriptor = fopen(NameFile, "wb+")) == NULL) return ORI_ERROR;

	 if (StatusOpen == NCRREWR)
		 if ((FileDescriptor = fopen(NameFile, "ab+")) == NULL) return ORI_ERROR;
	
		return ORI_SUCCESS;
	}

	OriError OpenFile(const wchar_t *NameFile, TableEncoding TypeEncoding = ORI_UNKNOW, OriFileStatus StatusOpen = READ)	{

	 if (NameFile == NULL)     return ORI_ERROR;
	 if (NameFile[0] == 0x00)  return ORI_ERROR;


#ifdef ORI_LINUX

	 char AsciiNameFile[256] = { 0 };
	 ConvertStr(AsciiNameFile, NameFile);
	 return OpenFile(AsciiNameFile, StatusOpen);

#endif

#ifdef ORI_WINDOWS

	 wchar_t FlagFile[32] = { 0 };
	 
	 if (StatusOpen == OriFileStatus::READ)
		 StrCpy(FlagFile, L"rb");

	 if (StatusOpen == OriFileStatus::WRITE)
		 StrCpy(FlagFile, L"wb");

	 if (StatusOpen == OriFileStatus::NWRITE)
		 StrCpy(FlagFile, L"ab");

	 if (StatusOpen == OriFileStatus::NREWR)
		 StrCpy(FlagFile, L"rb+");

	 if (StatusOpen == OriFileStatus::CLREWR)
		 StrCpy(FlagFile, L"wb+");

	 if (StatusOpen == OriFileStatus::NCRREWR)
		 StrCpy(FlagFile, L"ab+");


	 if (TypeEncoding == ORI_UNICODE)
		 StrCat(FlagFile, L",ccs=UNICODE");

	 if (TypeEncoding == ORI_UTF8)
		 StrCat(FlagFile, L",ccs=UTF-8");


	 if (TypeEncoding == ORI_UTF16LE)
		 StrCat(FlagFile, L",UTF-16LE");
		 	
	 if ((FileDescriptor = _wfopen(NameFile, FlagFile)) == NULL)
		 return ORI_ERROR;
	
#endif // ORI_WINDOWS

		return ORI_SUCCESS;
	}


	void CloseFile()	{
		if (FileDescriptor != NULL)
			fclose(FileDescriptor);
	}

	FILE *GetDesc() { return FileDescriptor; }

	void StartPostion()	{
		fseek(GetDesc(), 0L, SEEK_END);
	}

private:
	FILE *FileDescriptor;

};



#ifdef ORI_WINDOWS
#define RunArg	wchar_t ResultArg[MAX_PATH];   va_list vl;	va_start(vl, Arg);	VSNWPRINTF(ResultArg, MAX_PATH - 1, Arg, vl);	va_end(vl);
#else
#define RunArg wchar_t ResultArg[MAX_PATH];    va_list vl;	va_start(vl, Arg);	vswprintf(ResultArg, MAX_PATH - 1, Arg, vl);	va_end(vl);
#endif



class OriPath
{

public:

	OriPath()
	{
		PathExe[0] = 0x00;
		PathDir[0] = 0x00;
	}

#ifdef ORI_LINUX
	void GetPathExeLinux(wchar_t *Path) {
	  char buff[MAX_PATH] = { 0 };
	  Path[0] = 0x00;

	  ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);

	  if (len != -1) {
	       buff[len] = 0x00;
	       StrCpy(Path, buff);
	  }
	}
#endif

	wchar_t PathExe[MAX_PATH];
	wchar_t PathDir[MAX_PATH];

	// Получаем путь нашей программы
	wchar_t *GetPathExe() {
#ifdef ORI_WINDOWS
		GetModuleFileNameW(NULL, PathExe, sizeof(PathExe));
#else
		GetPathExeLinux(PathExe);
#endif
		return PathExe;
	}

	wchar_t *GetPathExe(wchar_t const *Arg, ...) {
		RunArg;

#ifdef ORI_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathExe(), ResultArg);
#endif
#ifdef ORI_GCC
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathExe(), ResultArg);
#endif

		return PathDir;
	}

	wchar_t *GetPathExe(wchar_t *Str, wchar_t const *Arg, ...) {
		RunArg;

#ifdef ORI_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathExe(), ResultArg);
#endif
#ifdef ORI_GCC
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathExe(), ResultArg);
#endif


		wcscpy(Str, PathDir);
	}

	void GetPathExe(wchar_t *Str) {
		wcscpy(Str, GetPathExe());
	}

	wchar_t *GetPathDir() {

#ifdef ORI_WINDOWS
		wcscpy(PathDir, GetPathExe());
		PathRemoveFileSpecW(PathDir);
#endif

#ifdef ORI_LINUX

		char   PathBuffer[MAX_PATH]  = { };
		getcwd(PathBuffer, MAX_PATH);
		StrCpy(PathDir, PathBuffer);

#endif

		return PathDir;
	}

	wchar_t *GetPathDir(wchar_t const *Arg, ...)  {
		RunArg;

#ifdef ORI_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathDir(), ResultArg);
#endif
#ifdef ORI_GCC
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathDir(), ResultArg);
#endif

		return PathDir;
	}

	void GetPathDir(wchar_t *Str) { wcscpy(Str, GetPathDir()); }

	void GetPathDir(wchar_t *Str, wchar_t const *Arg, ...) {

		RunArg;
		
#ifdef ORI_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathDir(), ResultArg);
#endif
#ifdef ORI_GCC
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathDir(), ResultArg);
#endif

		wcscpy(Str, PathDir);
	}

#ifdef ORI_WINDOWS

	wchar_t* GetPathProcess(DWORD processID)  {

		HMODULE hMods[1024];
		HANDLE  hProcess;
		DWORD   cbNeeded;

		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

		if (NULL == hProcess)
			return L"";

		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			for (uint32_t i = 0; i < 1; i++)
				GetModuleFileNameExW(hProcess, hMods[i], PathExe, sizeof(PathExe));

            return PathExe;
		}

		CloseHandle(hProcess);
		return L"";
	}

#endif


};

#ifdef ORI_WINDOWS
class OriExe
{
public:

	enum ShowStatus
	{
		SHIDE = 0,
		SHOWNORMAL = 1,
		NORMAL = 1,
		SHOWMINIMIZED = 2,
		SHOWMAXIMIZED = 3,
		MAXIMIZE = 3,
		SHOWNOACTIVATE = 4,
		SHOW = 5,
		MINIMIZE = 6,
		SHOWMINNOACTIVE = 7,
		SHOWNA = 8,
		RESTORE = 9,
		SHOWDEFAULT = 10,
		FORCEMINIMIZE = 11,
		MAX = 11
	};

	OriError Run(const wchar_t *NameFile, uint32_t ShowWindowsParameter, const wchar_t *Arg, ...)
	{

		RunArg;

		_SHELLEXECUTEINFOW ShExecInfo;

		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = NULL;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = NameFile;
		ShExecInfo.lpParameters = ResultArg;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = ShowWindowsParameter;
		ShExecInfo.hInstApp = NULL;

		BOOL Status = ShellExecuteExW(&ShExecInfo);
		if (Status == 1) return ORI_SUCCESS;

		return ORI_ERROR;
	}


};

#endif

#endif
