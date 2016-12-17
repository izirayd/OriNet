#include "TableFiles.h"
#pragma warning(disable : 4996)

TableFiles::TableFiles()
{
	this->MaxCountFiles = 512;
	TBFiles = new ListFiles[MaxCountFiles];
}

TableFiles::TableFiles(int SizeTB) {

	if (MaxCountFiles == 512)
		delete[] TBFiles;

	this->MaxCountFiles = SizeTB;
	TBFiles = new ListFiles[MaxCountFiles];
	
}
void TableFiles::ReCreateTB(int SizeTB) {

	
	delete[] TBFiles;

	this->MaxCountFiles = SizeTB;
	TBFiles = new ListFiles[MaxCountFiles];

}


TableFiles::~TableFiles()
{
	delete[] TBFiles;
}

#include <string.h>

void TableFiles::AddFile(char *Name)
{
	strcpy(TBFiles[this->CountFiles].Name, Name);
	CountFiles++;
}

bool TableFiles::CheckFile(char *Name)
{
	if (strlen(Name) > 255) return false;
	for (int i = 0; i < CountFiles; i++)
		if (strcmp(TBFiles[i].Name, Name) == 0)	
			return true;
	return false;
}

#include <stdio.h>

int SizeFile(char *Name)
{
    FILE *f;
	f = fopen(Name, "rb");
	fseek(f, 0L, SEEK_END);
	int Result = ftell(f);
	fclose(f);

	return Result;
}