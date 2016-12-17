#ifndef oriLangH
#define oriLangH

#define ORI_LNG_VERSION  0.2
#define MAX_BUFFER_LNG   64000
#define MAX_TEG_LEN      1024
#define OPEN_TEG_SYMBOL  '<'
#define CLOSE_TEG_SYMBOL '>'
#define MAX_SYMBOL_NAME  33
#define LEN_BUFFER_FILE  2048

#define ERROR_LEN_TEG     "Error 1! Name teg in symbol: [%d] have > 32 symbol"
#define ERROR_SEARCH_TEG  "Error 2! Tag %s Can not find a tag or missing couple"
#define ERROR_MAX_BUFFER  "Error 3! Arg > max buffer"
#define ERROR_ZERO_BUFFER "Error 4! Buffer len == 0"

#define ORI_STRING str
#define ORI_INT    int
#define ORI_CHAR   char
#define ORI_BOOL   bool

#pragma warning(disable : 4996)

#include <stdio.h>
#include <stdarg.h>
#include "../OriKernel/KernelPlatform.h"
#include "../OriUntil/OriFile.h"

int SizeFile(const char *Name);
// Хранит в себе теги и их адреса в буфере
struct Cache_Teg {
  char Teg[MAX_SYMBOL_NAME - 1];          // Имя тега
  int  StartSymbol;      // Стартовый символ
  int  EndSymbol;        // Конечный символ
};

/* Класс для работы с скриптовым языком */
class OriLng
{
  public:

	OriLng();
	OriLng(int _SizeBuffer);
	OriLng(const char *str);
	~OriLng();
	
	// Возврашает в буффер данные, которые были указаны в качестве второго параетра (Тега)
	int  ReturnData(char  *buffer, char  *Tag, int Size);
	int  Result(char  *buffer, int Size, char  *Tag, ...);
	int  Result(int32_t &buffer, int Size, char  *Tag, ...);

	char  *Result(char  *Tag, int Size, ...);

	// Возвращает число в теге
	int  ReturnDataInt(char *Tag, ...);

	//int  ReturnDataInt(char  *Tag);
	void ReturnDataInt(int Buffer, char  *Tag);

	// Возвращает число в теге
	//__int64 RetuntDataInt64(char  *Tag);
	//void RetuntDataInt64(__int64 Buffer,char  *Tag);

	// Записывает данные в указанный буффер, по тегу.
	int  WriteData     (char  *buffer, char *Tag);
	// Записывает данные в указанный буффер, по тегу.
	void  WriteDataFormat(char *Tag, const char* Buffer, ...);

	int WriteData2(char  *buffer, const  char *Tag);

	// Сдвинуть данные влево, с позиции WithPostion, на позицию ToPostion. ToPostion - так же будет перезаписана
	void MoveDataLeft(uint32_t ToPostion, uint32_t WithPostion);

	// Сдвинуть данные в право, освободив NewPostion пустых ячеек. Сдвиг начинается с ToPostion
	void MoveDataRight(uint32_t ToPostion, uint32_t CountNewPostion);

	// Выводит данные в теге
	int  PrintData 	   (char  *Tag);
	// Выводит данные в теге сл. строчкой
	int  PrintDataLine (char  *Tag);
	// Читает файл с кодом
	int  OpenFile(const char *NameFile);
	int  OpenFile(OriFile *File);

	int OpenCryptFile(char *NameFile, char *Key);
	int OpenCryptFile(OriFile *File,  char *Key);
	int SaveCryptFile(char *NameFile, char *Key);
	int SaveCryptFile(OriFile *File,  char *Key);
	// Сохраняет файл с кодом

	int  SaveFile	   (const char *NameFile);
	int  SaveFile      (OriFile *File);
	// Читает код
	OriLng& operator <<(const char *LngCode);

	OriLng& operator =  (const char *LngCode);
	OriLng& operator +  (const char *LngCode);
	OriLng& operator +  (OriLng &Lng);
	OriLng& operator =  (OriLng &Lng);
    bool    operator == (OriLng &Lng);

	void AddCode(const char* CodeLng, ...);
	
	int32_t GetSize()           { return lenBuffer; }
	void SetSize(int32_t _Size) { lenBuffer = _Size; }
	int32_t RunSize()           { return strlen(Buffer); }
	
    // Чистит буффер класса от скриптового кода
	void Clear();
    // Хранит Ошибки
	char ErrorTeg[512];

	// Криптует весь буфер
	int CryptBuffer(char *Key) { 
		if (!isCrypt) isCrypt = true; else isCrypt = false;
		XORCrypt(this->Buffer, Key);
		return this->MaxSizeBuffer;
	}

	int CryptBuffer(char *Key, uint32_t SizeBuffer) {
		if (!isCrypt) isCrypt = true; else isCrypt = false;
		XORCrypt(this->Buffer, Key, SizeBuffer);
		return this->MaxSizeBuffer;
	}

	bool isCrypt;
	void XORCrypt(char*buffer, char *key);
	void XORCrypt(char *buffer, char *src, uint32_t SizeBuffer);

	// Раскриптовывает буфер
	int ReCryptBuffer(char *Key);
    // Создаёт рандомный ключ, который знает только сервер, для работы с OriServer
	int CryptRandomBufferForServer();
	// Затерает буфер нулями, но зачем?

	//
	void DeleteBuffer();
	
	void SpeedOpenFile(char *NameFile);

	void SpeedOpenFileNoCreateBuffer(char *NameFile);
		
	void MemClear();


	// В протокольной реализации мы получаем в качестве класса друга, класс OriClient. Это делается для доступа к буфферу.
	friend class OriClient;
	friend class OriServerClient;

   
	int  MaxSizeBuffer;

	operator const char*() const	{
		return this->Buffer;
	}

	char *GetBuffer() { return Buffer; }
	const char *GetConstBuffer() { return Buffer; }

	// Основной буффер, хранит в себе код
	char *Buffer;
	void CreateBuffer(int _SizeBuffer);

	typedef uint32_t Encoding;

  private:

	

	  char    *ResultBuffer;
	  unsigned SizeResultBuffer;
	  void     CreateMemoryResultBuffer(unsigned _SizeMem) { ResultBuffer = new char[_SizeMem];  SizeResultBuffer = _SizeMem; }
	  // Возвращает true, если аргумент _SizeMem больше памяти буфера
	  bool     CheckLimitResultBuffer(unsigned _SizeMem)   { if (SizeResultBuffer < _SizeMem) return true; else return false; }
	  void UseResultBuffer(unsigned _SizeMem) { if (CheckLimitResultBuffer(_SizeMem)) CreateMemoryResultBuffer(_SizeMem); }
	  void DeleteResultBuffer() {
		  if (SizeResultBuffer != 0) { ResultBuffer = 0; delete[] ResultBuffer; SizeResultBuffer = 0; }
	  }
	  void InitResultBuffer() { SizeResultBuffer = 0; ResultBuffer = 0; }



    // Не основная копирующая переменная
	 char *g_buf; //[MAX_BUFFER_LNG];

	// Длинна буффера
	int  lenBuffer;

	// Количество тегов
	int  CountTeg;
	// Данные тегов
	Cache_Teg  *Teg; //[MAX_TEG_LEN * 6];

	// Функция парсера. Клиенту её использовать не следует
	int  StartScan(int StartPosition, char* NameTeg, int &StartPositionTeg, int &EndPositionTeg);

	int endRead;
	int a;
	int b;
	char g_buffer[MAX_SYMBOL_NAME];


	void SpeedRunBuffer();
	bool isUseMem;
};

typedef OriLng lng;
//typedef str OriString;
#endif
