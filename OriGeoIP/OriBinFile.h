//---------------------------------------------------------------------------

#ifndef LCFileH
#define LCFileH
#include <fstream>


struct LenName {
 int  count_len;   // Кол. во символов в строке
 char name[48];  // Строка
};

void OpenLC(std::fstream &FileInit, LenName &Data);
void OpenLC(std::fstream &FileInit, int &Data);
void OpenLC(std::fstream &FileInit, unsigned char &Data);
//void OpenLC(std::fstream &FileInit, __int64 &Data);


void SaveLC(std::fstream &FileInit, LenName &Data);
void SaveLC(std::fstream &FileInit, const char *buf);
void SaveLC(std::fstream &FileInit, int &Data);
void SaveLC(std::fstream &FileInit, unsigned char &Data);
//void SaveLC(std::fstream &FileInit, __int64 &Data);


#endif
