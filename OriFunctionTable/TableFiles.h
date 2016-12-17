#pragma once


#include "../OriKernel/KernelPacket.h"

struct InfoFile
{
	char NameFile[SIZE_FILE_NANE];  // Имя файла
	int  SizeFile;       // Размер файла
	int  MaxBuffer;      // Размер буффера протокола, при передаче файла
	int  NumberSegment;  // Номер сегмента
	int  MaxSegment;     // Всего сегментов

	/* 
	 Размер вне сегментного блока.	
	*/
	int SizeEndByte; 
};


struct OriFileSend
{
	char NameFile[SIZE_FILE_NANE];  // Имя файла
	int  SizeFile;       // Размер файла
	int  MaxBuffer;      // Размер буффера протокола, при передаче файла
	int  NumberSegment;  // Номер сегмента
	int  MaxSegment;     // Всего сегментов
	int  SizeEndByte;    // Размер вне сегментного блока.

	// Буффер
	char BufferFile[SIZE_BUFFER_FILE];
};

// Размер файла. Указать можно и путь.
int SizeFile(char *Name);

struct ListFiles
{
	char Name[256];
};

class TableFiles
{
public:
	TableFiles();
	TableFiles(int SizeTB);
	void ReCreateTB(int SizeTB);
	~TableFiles();
	void AddFile(char *Name);
	bool CheckFile(char *Name);
	int CountFiles;
	int MaxCountFiles;

	ListFiles *TBFiles;
};

