#pragma once


#include "../OriKernel/KernelPacket.h"

struct InfoFile
{
	char NameFile[SIZE_FILE_NANE];  // ��� �����
	int  SizeFile;       // ������ �����
	int  MaxBuffer;      // ������ ������� ���������, ��� �������� �����
	int  NumberSegment;  // ����� ��������
	int  MaxSegment;     // ����� ���������

	/* 
	 ������ ��� ����������� �����.	
	*/
	int SizeEndByte; 
};


struct OriFileSend
{
	char NameFile[SIZE_FILE_NANE];  // ��� �����
	int  SizeFile;       // ������ �����
	int  MaxBuffer;      // ������ ������� ���������, ��� �������� �����
	int  NumberSegment;  // ����� ��������
	int  MaxSegment;     // ����� ���������
	int  SizeEndByte;    // ������ ��� ����������� �����.

	// ������
	char BufferFile[SIZE_BUFFER_FILE];
};

// ������ �����. ������� ����� � ����.
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

