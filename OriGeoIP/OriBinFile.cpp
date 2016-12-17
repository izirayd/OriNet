#pragma hdrstop

#include "OriBinFile.h"
#include "../OriKernel/KernelSocket.h"

using namespace  std;

void OpenLC(fstream &FileInit, LenName &Data) {
	Data.count_len = 0;
	FileInit.read((char*) & Data.count_len, 4);
	FileInit.read((char*) & Data.name, Data.count_len);
	Data.name[Data.count_len] = '\0';
}

void OpenLC(fstream &FileInit, int &Data) {
	FileInit.read((char*) & Data, 4);
}

void OpenLCINT(fstream &FileInit, int &Data) {
	FileInit.read((char*) & Data, 4);
}

void OpenLC(fstream &FileInit, unsigned char &Data) {
	FileInit.read((char *)& Data, 1);
}

void SaveLC(fstream &FileInit, LenName &Data) {
	Data.count_len = strlen(Data.name);
	FileInit.write((char*) & Data.count_len, 4);
	FileInit.write((char*) & Data.name, Data.count_len);
}

void SaveLC(fstream &FileInit, const char *buf) {
	int len = strlen(buf);
	FileInit.write((char*)&len, 4);
	FileInit.write((char*)&buf, len);
}

void SaveLC(fstream &FileInit, int &Data) {
	FileInit.write((char*)&Data, 4);
}

void SaveLC(fstream &FileInit, unsigned char &Data) {
	FileInit.write((char *)& Data, 1);
}

