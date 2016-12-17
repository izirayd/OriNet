#include "OriCrypt.h"
#include "Sha256.h"

#include "string.h"
#include "stdio.h"
#include <math.h>
#include <ctime>
#include <iostream>
#include "../OriLng/OriLng.h"


#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#pragma warning(disable : 4267)
#pragma warning(disable : 4244)

void XORCrypt(char *buffer, char *src)
{
 char * p = buffer;
 unsigned a = 0;
 int sz =  strlen(buffer);
 int SZ =  strlen(src);

 for(; p < buffer + sz ; p++) {
  *p= (*p) ^ src[a] + (p-buffer) * 2;
  a++;
  if(a>SZ) a=0;
 }
}

OriCrypt::OriCrypt() {
	//BufferKey = new char[2];
}

void OriCrypt::DeleteDynamicBuffer()
{
	delete [] BufferKey;
}


void OriCrypt::XORCrypt(char*buffer)
{
  char * p = buffer;
  unsigned a = 0;
  int sz =  strlen(buffer);

 for(; p < buffer + sz ; p++)
 {
  *p= (*p) ^ this->BufferKey[a] + (p-buffer) * 2;
  a++;
  if(a > this->lenKey) a=0;
 }
}

OriCrypt& OriCrypt::operator << (char *key)
{

  // Получим длинну ключа
  lenKey = strlen(key) + 1;
  BufferKey = new char[lenKey]; // Выделем под него память
  strcpy(BufferKey, key); // Копируем ключ в память
  
  return *this;
}


void SHA256Crypt(const char *buffer, char *Sha256)
{
	sha256_context ctx;
	unsigned char sha256sum[32];

	sha256_starts(&ctx);
	sha256_update(&ctx, (uint8 *) buffer, strlen(buffer));
	sha256_finish(&ctx, sha256sum);

	Sha256[0] = '\x0';

	for (int j = 0; j < 32; j++)
		sprintf(Sha256 + strlen(Sha256), "%02x", sha256sum[j]);

}

//#define SPEED_TEST

#ifdef SPEED_TEST

#define StartSpeed timer = GetTimer(0);
#define EndSpeed(text)  timer = GetTimer(timer);  std::cout << "\nSpeed : " << timer << "ms" << " - " << text;

#else
#define StartSpeed ;
#define EndSpeed ;
#endif


int  FileSHA256(char *NameFile, char *Sha256)
{
#ifdef SPEED_TEST
	SetIdlePriority();
	long timer = GetTimer(0);
	timer = GetTimer(timer);
#endif

	StartSpeed;
	sha256_context ctx;
	unsigned char sha256sum[32];

	EndSpeed("Создание перемен");

	StartSpeed;
	FILE *f;
	int SizeFileForHash = SizeFile(NameFile);
	if (SizeFileForHash < 0) return 0;
	EndSpeed("Чтение размера файла");

	StartSpeed;
	if ((f = fopen(NameFile, "rb")) == NULL) 
		 return 0;

	sha256_starts( &ctx );
	EndSpeed("fopen");

	StartSpeed;
	unsigned char* buf = new unsigned char[SizeFileForHash];
	EndSpeed("Выделенние ОЗУ под файл");

	StartSpeed;
	fread(buf, 1, SizeFileForHash, f);
	EndSpeed("Считывание файла в озу");
		
	StartSpeed;
	sha256_update(&ctx, buf, SizeFileForHash);
	EndSpeed("Обновление хеша");

	StartSpeed;
	delete[] buf;
	fclose(f);
	EndSpeed("Удаление файла");

	StartSpeed;
        sha256_finish(&ctx, sha256sum );
	EndSpeed("Компрессия хеша");

    	Sha256[0] = 0x00;

	StartSpeed;
	for (int j = 0; j < 32; j++)
		sprintf(Sha256 + strlen(Sha256), "%02x", sha256sum[j]);
	EndSpeed("Перевод хеша с формата UCHAR в CHAR");
	

 return 1;
}


int  FileSHA256(OriFile *File, char *Sha256) {

	if (File == NULL || File->GetDesc() == NULL || File->GetSizeFile() == 0) return ORI_ERROR;
	char* buf = new char[File->GetSizeFile() + 1];
	File->FullReadFile(buf, sizeof(char));
	buf[File->GetSizeFile()] = 0x00;

	SHA256Crypt(buf, Sha256);

	delete[] buf;
	return ORI_SUCCESS;

	return 1;
}

unsigned int HashH37(const char * str)
{
	unsigned int hash = 2139062143;
	for (; *str; str++)
		hash = 37 * hash + *str;
	return hash;
}


unsigned int BinHashH37(const char * Data, int Size)
{
	unsigned int hash = 2139062143;
	for (int i = 0; i < Size; i++)
		hash = 37 * hash + Data[i];
	return hash;
}

int FileHashH37(const char *NameFile, unsigned int &Hash) {

#ifdef SPEED_TEST
	SetIdlePriority();
	long timer = GetTimer(0);
	timer = GetTimer(timer);
#endif

	StartSpeed;
	FILE *f;
	int SizeFileForHash = SizeFile(NameFile);
	if (SizeFileForHash < 0) return 0;
	EndSpeed("Чтение размера файла");

	StartSpeed;
	if ((f = fopen(NameFile, "rb")) == NULL)
		return 0;
	EndSpeed("fopen");

	StartSpeed;
	char* buf = new char[SizeFileForHash];
	EndSpeed("Выделенние ОЗУ под файл");

	StartSpeed;
	fread(buf, 1, SizeFileForHash, f);
	EndSpeed("Считывание файла в озу");

	StartSpeed;
	Hash = BinHashH37(buf, SizeFileForHash);
	EndSpeed("Обновление хеша");

	StartSpeed;
	delete[] buf;
	fclose(f);
	EndSpeed("Удаление файла");

	
	return 1;
}


unsigned int HashFAQ6(const char * str) {

	unsigned int hash = 0;

	for (; *str; str++)
	{
		hash += (unsigned char)(*str);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

unsigned int BinHashFAQ6(const char * Date, int Size) {

	unsigned int hash = 0;

	for (int i = 0; i < Size; i++)
	{
		hash += (unsigned char)(Date[i]);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

unsigned int BinHashRot13(const char * Date, int Size)
{
	unsigned int hash = 0;
	for (int i = 0; i < Size; i++) 	{
		hash += (unsigned char)(Date[i]);
		hash -= (hash << 13) | (hash >> 19);
	}

	return hash;
}

unsigned int HashRot13(const char * str)
{
	unsigned int hash = 0;
	for (; *str; str++) 	{
		hash += (unsigned char)(*str);
		hash -= (hash << 13) | (hash >> 19);
	}

	return hash;
}

unsigned int BinHashLy(const char * Date, int Size)
{
	unsigned int hash = 0;
	for (int i = 0; i < Size; i++)
		hash = (hash * 1664525) + (unsigned char)(Date[i])+1013904223;
	return hash;
}

unsigned int HashLy(const char * str)
{
	unsigned int hash = 0;
	for (; *str; str++)
		hash = (hash * 1664525) + (unsigned char)(*str) + 1013904223;
	return hash;
}


unsigned int BinHashRs(const char * Date, int Size)
{
	static const unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;

	for (int i = 0; i < Size; i++)	{
		hash = hash * a + (unsigned char)(Date[i]);
		a *= b;
	}

	return hash;
}

unsigned int HashRs(const char * str)
{
	static const unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;

	for (; *str; str++)	{
		hash = hash * a + (unsigned char)(*str);
		a *= b;
	}

	return hash;
}


int FileHashFAQ6(const char *NameFile, unsigned int &Hash) {

#ifdef SPEED_TEST
	SetIdlePriority();
	long timer = GetTimer(0);
	timer = GetTimer(timer);
#endif

	StartSpeed;
	FILE *f;
	int SizeFileForHash = SizeFile(NameFile);
	if (SizeFileForHash < 0) return 0;
	EndSpeed("Чтение размера файла");

	StartSpeed;
	if ((f = fopen(NameFile, "rb")) == NULL)
		return 0;
	EndSpeed("fopen");

	StartSpeed;
	char* buf = new char[SizeFileForHash + 1];
	EndSpeed("Выделенние ОЗУ под файл");
	buf[0] = 0x00;
	StartSpeed;
	fread(buf, 1, SizeFileForHash, f);
	fclose(f);
	
	EndSpeed("Считывание файла в озу");
	buf[SizeFileForHash] = 0x00
	StartSpeed;
	Hash = BinHashFAQ6(buf, SizeFileForHash);
	EndSpeed("Обновление хеша");

	StartSpeed;
	delete[] buf;




	return 1;
}


int FileHashRot13(const char *NameFile, unsigned int &Hash) {

#ifdef SPEED_TEST
	SetIdlePriority();
	long timer = GetTimer(0);
	timer = GetTimer(timer);
#endif

	StartSpeed;
	FILE *f;
	int SizeFileForHash = SizeFile(NameFile);
	if (SizeFileForHash < 0) return 0;
	EndSpeed("Чтение размера файла");

	StartSpeed;
	if ((f = fopen(NameFile, "rb")) == NULL)
		return 0;
	EndSpeed("fopen");

	StartSpeed;
	char* buf = new char[SizeFileForHash];
	memset(buf, 0x00, SizeFileForHash);

	EndSpeed("Выделенние ОЗУ под файл");

	StartSpeed;
	fread(buf, 1, SizeFileForHash, f);
	EndSpeed("Считывание файла в озу");

	StartSpeed;
	Hash = BinHashRot13(buf, SizeFileForHash);
	EndSpeed("Обновление хеша");

	StartSpeed;
	delete[] buf;
	fclose(f);
	EndSpeed("Удаление файла");


	return 1;
}

int FileHashLy(OriFile *File, unsigned int &Hash) {

	if (File == NULL || File->GetDesc() == NULL || File->GetSizeFile() == 0) return ORI_ERROR;
	char* buf = new char[File->GetSizeFile() + 1];
	File->FullReadFile(buf, sizeof(char));
	buf[File->GetSizeFile()] = 0x00;

	Hash = BinHashLy(buf, File->GetSizeFile());

	delete[] buf;
	return ORI_SUCCESS;
}

int FileHashFAQ6(OriFile *File, unsigned int &Hash) {

	if (File == NULL || File->GetDesc() == NULL || File->GetSizeFile() == 0) return ORI_ERROR;
	char* buf = new char[File->GetSizeFile() + 1];
	File->FullReadFile(buf, sizeof(char));
	buf[File->GetSizeFile()] = 0x00;

	Hash = BinHashFAQ6(buf, File->GetSizeFile());

	delete[] buf;
	return ORI_SUCCESS;
}


int FileHashRot13(OriFile *File, unsigned int &Hash) {

	if (File == NULL || File->GetDesc() == NULL || File->GetSizeFile() == 0) return ORI_ERROR;
	char* buf = new char[File->GetSizeFile() + 1];
	File->FullReadFile(buf, sizeof(char));
	buf[File->GetSizeFile()] = 0x00;

	Hash = BinHashRot13(buf, File->GetSizeFile());

	delete[] buf;
	return ORI_SUCCESS;
}

int FileHashRs(OriFile *File, unsigned int &Hash) {

	if (File == NULL || File->GetDesc() == NULL || File->GetSizeFile() == 0) return ORI_ERROR;
	char* buf = new char[File->GetSizeFile() + 1];
	File->FullReadFile(buf, sizeof(char));
	buf[File->GetSizeFile()] = 0x00;

	Hash = BinHashRs(buf, File->GetSizeFile());

	delete[] buf;
	return ORI_SUCCESS;
}


int FileHashH37(OriFile *File, unsigned int &Hash) {

	if (File == NULL || File->GetDesc() == NULL || File->GetSizeFile() == 0) return ORI_ERROR;
	char* buf = new char[File->GetSizeFile() + 1];
	File->FullReadFile(buf, sizeof(char));
	buf[File->GetSizeFile()] = 0x00;

	Hash = BinHashH37(buf, File->GetSizeFile());

	delete[] buf;
	return ORI_SUCCESS;
}



int FileHashLy(const char *NameFile, unsigned int &Hash) {


	FILE *f;
	int SizeFileForHash = SizeFile(NameFile);
	if (SizeFileForHash < 0) return 0;

	if ((f = fopen(NameFile, "rb")) == NULL)
		return 0;

	char* buf = new char[SizeFileForHash];

	fread(buf, 1, SizeFileForHash, f);

	Hash = BinHashLy(buf, SizeFileForHash);


	delete[] buf;
	fclose(f);

	return 1;
}


int FileHashRs(const char *NameFile, unsigned int &Hash) {

#ifdef SPEED_TEST
	SetIdlePriority();
	long timer = GetTimer(0);
	timer = GetTimer(timer);
#endif

	StartSpeed;
	FILE *f;
	int SizeFileForHash = SizeFile(NameFile);
	if (SizeFileForHash < 0) return 0;
	EndSpeed("Чтение размера файла");

	StartSpeed;
	if ((f = fopen(NameFile, "rb")) == NULL)
		return 0;
	EndSpeed("fopen");

	StartSpeed;
	char* buf = new char[SizeFileForHash];
	EndSpeed("Выделенние ОЗУ под файл");

	StartSpeed;
	fread(buf, 1, SizeFileForHash, f);
	EndSpeed("Считывание файла в озу");

	StartSpeed;
	Hash = BinHashRs(buf, SizeFileForHash);
	EndSpeed("Обновление хеша");

	StartSpeed;
	delete[] buf;
	fclose(f);
	EndSpeed("Удаление файла");


	return 1;
}