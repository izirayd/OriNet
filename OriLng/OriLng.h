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
// ������ � ���� ���� � �� ������ � ������
struct Cache_Teg {
  char Teg[MAX_SYMBOL_NAME - 1];          // ��� ����
  int  StartSymbol;      // ��������� ������
  int  EndSymbol;        // �������� ������
};

/* ����� ��� ������ � ���������� ������ */
class OriLng
{
  public:

	OriLng();
	OriLng(int _SizeBuffer);
	OriLng(const char *str);
	~OriLng();
	
	// ���������� � ������ ������, ������� ���� ������� � �������� ������� �������� (����)
	int  ReturnData(char  *buffer, char  *Tag, int Size);
	int  Result(char  *buffer, int Size, char  *Tag, ...);
	int  Result(int32_t &buffer, int Size, char  *Tag, ...);

	char  *Result(char  *Tag, int Size, ...);

	// ���������� ����� � ����
	int  ReturnDataInt(char *Tag, ...);

	//int  ReturnDataInt(char  *Tag);
	void ReturnDataInt(int Buffer, char  *Tag);

	// ���������� ����� � ����
	//__int64 RetuntDataInt64(char  *Tag);
	//void RetuntDataInt64(__int64 Buffer,char  *Tag);

	// ���������� ������ � ��������� ������, �� ����.
	int  WriteData     (char  *buffer, char *Tag);
	// ���������� ������ � ��������� ������, �� ����.
	void  WriteDataFormat(char *Tag, const char* Buffer, ...);

	int WriteData2(char  *buffer, const  char *Tag);

	// �������� ������ �����, � ������� WithPostion, �� ������� ToPostion. ToPostion - ��� �� ����� ������������
	void MoveDataLeft(uint32_t ToPostion, uint32_t WithPostion);

	// �������� ������ � �����, ��������� NewPostion ������ �����. ����� ���������� � ToPostion
	void MoveDataRight(uint32_t ToPostion, uint32_t CountNewPostion);

	// ������� ������ � ����
	int  PrintData 	   (char  *Tag);
	// ������� ������ � ���� ��. ��������
	int  PrintDataLine (char  *Tag);
	// ������ ���� � �����
	int  OpenFile(const char *NameFile);
	int  OpenFile(OriFile *File);

	int OpenCryptFile(char *NameFile, char *Key);
	int OpenCryptFile(OriFile *File,  char *Key);
	int SaveCryptFile(char *NameFile, char *Key);
	int SaveCryptFile(OriFile *File,  char *Key);
	// ��������� ���� � �����

	int  SaveFile	   (const char *NameFile);
	int  SaveFile      (OriFile *File);
	// ������ ���
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
	
    // ������ ������ ������ �� ����������� ����
	void Clear();
    // ������ ������
	char ErrorTeg[512];

	// �������� ���� �����
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

	// ��������������� �����
	int ReCryptBuffer(char *Key);
    // ������ ��������� ����, ������� ����� ������ ������, ��� ������ � OriServer
	int CryptRandomBufferForServer();
	// �������� ����� ������, �� �����?

	//
	void DeleteBuffer();
	
	void SpeedOpenFile(char *NameFile);

	void SpeedOpenFileNoCreateBuffer(char *NameFile);
		
	void MemClear();


	// � ������������ ���������� �� �������� � �������� ������ �����, ����� OriClient. ��� �������� ��� ������� � �������.
	friend class OriClient;
	friend class OriServerClient;

   
	int  MaxSizeBuffer;

	operator const char*() const	{
		return this->Buffer;
	}

	char *GetBuffer() { return Buffer; }
	const char *GetConstBuffer() { return Buffer; }

	// �������� ������, ������ � ���� ���
	char *Buffer;
	void CreateBuffer(int _SizeBuffer);

	typedef uint32_t Encoding;

  private:

	

	  char    *ResultBuffer;
	  unsigned SizeResultBuffer;
	  void     CreateMemoryResultBuffer(unsigned _SizeMem) { ResultBuffer = new char[_SizeMem];  SizeResultBuffer = _SizeMem; }
	  // ���������� true, ���� �������� _SizeMem ������ ������ ������
	  bool     CheckLimitResultBuffer(unsigned _SizeMem)   { if (SizeResultBuffer < _SizeMem) return true; else return false; }
	  void UseResultBuffer(unsigned _SizeMem) { if (CheckLimitResultBuffer(_SizeMem)) CreateMemoryResultBuffer(_SizeMem); }
	  void DeleteResultBuffer() {
		  if (SizeResultBuffer != 0) { ResultBuffer = 0; delete[] ResultBuffer; SizeResultBuffer = 0; }
	  }
	  void InitResultBuffer() { SizeResultBuffer = 0; ResultBuffer = 0; }



    // �� �������� ���������� ����������
	 char *g_buf; //[MAX_BUFFER_LNG];

	// ������ �������
	int  lenBuffer;

	// ���������� �����
	int  CountTeg;
	// ������ �����
	Cache_Teg  *Teg; //[MAX_TEG_LEN * 6];

	// ������� �������. ������� � ������������ �� �������
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
