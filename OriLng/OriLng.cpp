/*
   Orion Game 22.10.2014
   Скриптовый язык Ori Lng
*/

#include  <stdio.h>
#include  <string.h>
#include  <fstream>
#include  <iostream>
#include  <cstdlib>
#include  <stdlib.h>

#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#pragma warning(disable : 4267)

#include  "OriLng.h"


//#define LOG

#if defined LOG
#define log(t) std::cout << "\n" << t;
#else
#define log(t)
#endif



#include <stdarg.h>
#include <ctype.h>

#if defined (_WIN32) || defined (_WIN64)
#define VSNPRINTF		 _vsnprintf
#else
#define VSNPRINTF		 vsnprintf
#endif

OriLng::OriLng()
{
	isUseMem = false;
	CreateBuffer(MAX_BUFFER_LNG);
	InitResultBuffer();
}

OriLng::~OriLng()
{
  DeleteBuffer();
  DeleteResultBuffer();
}

void OriLng::DeleteBuffer()
{
	if (isUseMem) {
		isUseMem = false;
		delete[]  this->Buffer;
		delete[]  this->g_buf;
		delete[]  this->Teg;
	}
}

OriLng::OriLng(const char *str)
{
    isUseMem = false;
	CreateBuffer(strlen(str));
	this->operator=(str);
	InitResultBuffer();
}


OriLng::OriLng(int _SizeBuffer)
{
	isUseMem = false;
	CreateBuffer(_SizeBuffer);	
	InitResultBuffer();
}

//#include "LngTest\LngTest\Gettimer.h"


//#define SPEED_TEST

#ifdef SPEED_TEST

#define StartSpeed timer = GetTimer(0);
#define EndSpeed(text)  timer = GetTimer(timer); if (timer > 0) std::cout << "\nSpeed : " << timer << "ms" << " - " << text;

#else
#define StartSpeed ;
#define EndSpeed ;
#endif


void OriLng::CreateBuffer(int _SizeBuffer)
{

	if (_SizeBuffer < 1) MaxSizeBuffer = MAX_BUFFER_LNG; else
		MaxSizeBuffer = _SizeBuffer;

	if (isUseMem == false) {
		isUseMem = true;
		this->Buffer = new char[MaxSizeBuffer];
		this->g_buf  = new char[MaxSizeBuffer];
		int _CountTag = MaxSizeBuffer / 12;
		this->Teg = new Cache_Teg[_CountTag];
	}
	this->Clear();

	this->a = 0;
	this->b = 0;
}

int SizeFile(const char *Name) {
	FILE *f;
	if ((f = fopen(Name, "rb")) == NULL) return -1;
	fseek(f, 0L, SEEK_END);
	int Result = ftell(f);
	fclose(f);
	return Result;
}


// Открытие файла
int OriLng::OpenFile(const char *NameFile) {

	int SizeF = SizeFile(NameFile);
	if (SizeF < 0) return -1;
	FILE *fp = NULL;
	if ((fp = fopen(NameFile, "rb")) == NULL) return -1;

	this->DeleteBuffer();
	this->CreateBuffer(SizeF + MAX_BUFFER_LNG);
	this->MemClear();

	fread(Buffer, 1, SizeF, fp);
	fclose(fp);

	Buffer[SizeF] = 0x00;

	this->SpeedRunBuffer();

	return SizeF;

}

int OriLng::OpenCryptFile(char *NameFile, char *Key) {

	int SizeF = SizeFile(NameFile);
	if (SizeF < 0) return -1;
	FILE *fp = NULL;
	if ((fp = fopen(NameFile, "rb")) == NULL) return -1;

	this->DeleteBuffer();
	this->CreateBuffer(SizeF + MAX_BUFFER_LNG);
	this->MemClear();

	fread(Buffer, 1, SizeF, fp);
	fclose(fp);

	Buffer[SizeF] = 0x00;

	CryptBuffer(Key, SizeF);
	this->SpeedRunBuffer();

	return SizeF;
}


int OriLng::OpenFile(OriFile *File) {
	
	if (File == NULL || File->GetDesc() == NULL)
		return ORI_ERROR;

	DeleteBuffer();
	CreateBuffer(File->GetSizeFile() + MAX_BUFFER_LNG);
	MemClear();

	File->FullReadFile(Buffer, sizeof(char));

	Buffer[File->GetSizeFile()] = 0x00;

	SpeedRunBuffer();

	return ORI_SUCCESS;
}

int OriLng::OpenCryptFile(OriFile *File, char *Key) {

	if (File == NULL || File->GetDesc() == NULL)
		return ORI_ERROR;

	DeleteBuffer();
	CreateBuffer(File->GetSizeFile() + MAX_BUFFER_LNG);
	MemClear();

	File->FullReadFile(Buffer, sizeof(char));

	Buffer[File->GetSizeFile()] = 0x00;

	CryptBuffer(Key, File->GetSizeFile());
	SpeedRunBuffer();

	return ORI_SUCCESS;
}

/* 
 Открытие файла на максимально возможной скорости с создание динамического буффера.
 Разница между обычным открыванием в том, что данный способ
 не создаёт дополнительный буффер, в который считываются данные.
 Запись данных происходит в главный буффер, который так же
 компилируется специальной функией, которое не производит рассчетов длины
 нового буффера.
*/
void OriLng::SpeedOpenFile(char *NameFile)  {
	this->DeleteBuffer();
	int SizeF = SizeFile(NameFile);
	this->CreateBuffer(SizeF + 65000);
	int Hanle = open(NameFile, O_RDONLY);
	read(Hanle, this->Buffer, SizeF);
	close(Hanle);
	this->SpeedRunBuffer();
}

/*
  Еще одна функция, её суть заключается в том, что
  считывается только то кол. во байтов из файла, сколько
  было создано памяти в OriLng.

  Функция выигрывает по скорости среди остальных функций.
*/
void  OriLng::SpeedOpenFileNoCreateBuffer(char *NameFile)
{
	int SizeF = SizeFile(NameFile);
	int Hanle = open(NameFile, O_RDONLY);
	read(Hanle, this->Buffer, this->MaxSizeBuffer);
	Buffer[SizeF - 1] = 0x00;
	close(Hanle);
	this->SpeedRunBuffer();
}

int OriLng::SaveFile(const char *NameFile)
{
  // Создаём переменную для записи
  std::ofstream myfile (NameFile);

  int size_buffer = 0;
  int size_teg    = 0;

  // Счетчик внутренего цикла
  int w = 0;
  if (myfile.is_open())
  {
	for (int i = 0; i < this->CountTeg; i++)
	{
	  for (int k = i + 1; k < this->CountTeg; k++)
	  {
		if (strcmp(this->Teg[i].Teg, this->Teg[k].Teg) == 0)
		{
             // Создаём буфферную переменную
			 size_buffer =  Teg[k].StartSymbol - Teg[i].EndSymbol;
			 size_teg    =  strlen(Teg[i].Teg);

			 // Оп, проверочки
			  if (size_teg < 1)         return -1;
			  if (size_buffer + 1 < 0)  return -1;
			  if (size_buffer + 1 == 0)
			  {
				log("==");
				break;
			  }

			  //Да, он создаст огромную строку, если это потребуется 
			 char *fileBuffer = new char[size_teg * 2 + size_buffer + 1];
			 // Чистим, вроде байт, а хорошо
			 fileBuffer[0] = '\x00';

			 strcpy(fileBuffer, Teg[i].Teg);
			 w = size_teg;

			 // Еше цикл?
			 for (int j = Teg[i].EndSymbol + 1; j < Teg[k].StartSymbol; j++)
			 {
			    fileBuffer[w] = Buffer[j];
				w++;
			 }
             
			 // Закроем конец, а то третий цикл не умеет этого, да и strcat нам не простит этого
			 fileBuffer[w] = '\x00';
			 // Закроем тегом
			 strcat(fileBuffer, Teg[i].Teg);
			 // Перекинем строку
			 fileBuffer[w + size_teg] = '\n';
			 // И закроем эту строку еще раз
			 fileBuffer[w + size_teg + 1] = '\x00';
			 // Вот, всё ради этой строчки кода
			 myfile << fileBuffer;
			 // Конечно же удалить буффер
			 delete [] fileBuffer;
			 
			 // Вообще странное действие, но вполне логичное, получается что если был найден какой-то ловый внутрений тег, внутри парного тега
			 // То он является внутреним, ведь структура плоская, поэтому этот внутрений просто данные двух верхних тегов, а значит последний тег от
			 // верхнего, должен завершать собой чтение тегов, следовательно вот так вот
			 i = k;
			 break;
		}
	  }
	}

	myfile.close();
	return 1;
  }

 log("Func - OriLng::SaveFile Error open file: " << NameFile);
 return 0;
}

int  OriLng::SaveFile(OriFile *File)
{
	if (File == NULL || File->GetDesc() == NULL)
		return ORI_ERROR;

	File->FullWriteFile(Buffer, sizeof(char), this->GetSize());

	return ORI_SUCCESS;
}


void OriLng::MoveDataRight(uint32_t ToPostion, uint32_t CountNewPostion)
{
	uint32_t tmpToPostion  = ToPostion;
	uint32_t NewEndPositon = lenBuffer + CountNewPostion;

	for (uint32_t i = lenBuffer; i > tmpToPostion; i--)
	{
		Buffer[NewEndPositon] = Buffer[i];
		Buffer[i] = ' ';
		NewEndPositon--;
	}
	
	lenBuffer += CountNewPostion;
	Buffer[lenBuffer] = 0x00;
}

void OriLng::MoveDataLeft(uint32_t ToPostion, uint32_t WithPostion)
{
	if (ToPostion > WithPostion) return;
	 
	uint32_t tmpToPostion = ToPostion;
	for (uint32_t i = WithPostion; i < lenBuffer; i++) 
	{
		Buffer[ToPostion] = Buffer[i];
		ToPostion++;
	}

	lenBuffer = lenBuffer - (WithPostion - tmpToPostion);
	Buffer[lenBuffer] = 0x00;
}

int OriLng::WriteData(char  *buffer, char *NameTag)
{
	char Tag[32] = { 0 };
	if (NameTag[0] != OPEN_TEG_SYMBOL)
		sprintf(Tag, "<%s>", NameTag);
	else strcpy(Tag, NameTag);

	uint32_t SizeNewData = strlen(buffer);
	if (SizeNewData < 1) return -1;

	// Найдем индекс тега, который хотим перезаписать.
	int32_t IndexTagStart = 0; // Это индекс первого тега
	int32_t IndexTagEnd = 0; // Это индекс второго тега

	bool    isFindTag = false;
	for (; IndexTagStart < CountTeg; IndexTagStart++)
	{
		if (strcmp(this->Teg[IndexTagStart].Teg, Tag) == 0)
		{
			isFindTag = true;
			break;
		}
	}

	if (!isFindTag) return -1;

	// Найдем его пару.
	isFindTag = false;

	IndexTagEnd = IndexTagStart + 1;

	for (; IndexTagEnd < CountTeg; IndexTagEnd++)
	{
		if (strcmp(this->Teg[IndexTagEnd].Teg, Tag) == 0)
		{
			isFindTag = true;
			break;
		}

	}

	if (!isFindTag) return -1;

	// Теперь у нас есть два тега, стартовый и конечный. Нужно найти их данные.
	int32_t StartPositionData = Teg[IndexTagStart].EndSymbol + 1;
	int32_t EndPositionData = Teg[IndexTagEnd].StartSymbol ;

	// Вычислим длинну старых данных
	int32_t SizeOldData = EndPositionData - StartPositionData;

	if (SizeOldData < 0) return -1;

	// Нам нужно перезаписать данные, которые уже хранятся в тегах. В случаее, если данных новых больше или меньше, нужно сдвинуть буффер либо в лево, либо в право.

	// Если размер буфферов одинаковый.
	if (SizeNewData == SizeOldData)
	{
		uint32_t Pos = 0;
		for (uint32_t i = StartPositionData; i < EndPositionData; i++)
		{
			Buffer[i] = buffer[Pos];
			Pos++;
		}

		return 1;
	}


	// Если новый буффер больше старого, то нужно сдвинуть положение вправо и освободить пустые ячейки
	if (SizeNewData > SizeOldData)
	{
		int32_t CountNewPostion = SizeNewData - SizeOldData;

		// Сдвигаем на последнюю позицию данных
		MoveDataRight(EndPositionData - 1, CountNewPostion);


		uint32_t Pos = 0;
		for (uint32_t i = StartPositionData; i < (EndPositionData + CountNewPostion); i++)
		{
			Buffer[i] = buffer[Pos];
			Pos++;
		}

		// Буффер нужно перекомпилировать, так как позиции сбросились
		SpeedRunBuffer();
		return 1;
	}

	// Если новые данные меньше старых
	if (SizeNewData < SizeOldData)
	{
		MoveDataLeft(StartPositionData + SizeNewData, EndPositionData);

		uint32_t Pos = 0;
		for (uint32_t i = StartPositionData; i < (StartPositionData + SizeNewData); i++)
		{
			Buffer[i] = buffer[Pos];
			Pos++;
		}

		SpeedRunBuffer();
		return 1;
	}

	return -1;
}

int OriLng::WriteData2(char  *buffer, const  char *Tag)
{
  /*
   1-Алгоритм
	 1. Находим адрес последнего байта данных
	 2. Становимся туда запись и пишем новые данные
	 3. Меняем адрес в структуре тегов. Тег будет возвращать новые данные..

	 Минус:
	  1. Будет мусор.
	 Плюсы:
	  2. Высокая производительность

   2-Алгоритм
	 1. Находим конечный адрес тега и весь буффер копируем в новый буффер после конечного адреса
	 2. Ставим запись в первый байт тега, записываем новые данные
	 3. Закрываем тегом
	 4. Перезаписываем всё поверху после закрытия тега, копирующим буффером
	 5. Производим пересчет всех тегов и их адресов

	 Минусы:
	  1. Долгая операция
	  2. Задратная операция

	 Плюсы:
	  1. Чистенько

	  Другие алгоритмы вообще полное извращенство..

	  
	  // Победил 2й алгоритм
  */
  
  // Код вообще сложно читается, именно у этой функции

  // ЗАПИСЬ В КОНЕЦ
    int lenTagBuffer = strlen(Tag) + lenBuffer;

    Cache_Teg StartTeg;
	strcpy(StartTeg.Teg, Tag);
	StartTeg.StartSymbol = lenBuffer;
	StartTeg.EndSymbol   = lenTagBuffer - 1;

  int w = 0;
  // Напишем имя тега в буффер
  for (int i = lenBuffer; i < lenTagBuffer; i++)
  {
	 Buffer[i] = Tag[w];
	 w++;
  }

  // Буффер имеет новую длину, изменим её.
  lenBuffer = lenTagBuffer;

  // Запишем данные которые нужны для перезаписи
  int lenData = strlen(buffer) + lenBuffer;

  w = 0;
	for (int i = lenBuffer; i < lenData; i++)
	{
	   Buffer[i] = buffer[w];
	   w++;
	}

	lenBuffer = lenData;

	lenTagBuffer = strlen(Tag) + lenBuffer;

	Cache_Teg EndTag;
	strcpy(EndTag.Teg, Tag);
	EndTag.StartSymbol = lenBuffer;
	EndTag.EndSymbol   = lenTagBuffer;

	w = 0;
	int adrBuf = 0;
	for (int i = lenBuffer; i < lenTagBuffer; i++)
	{
	 Buffer[i] = Tag[w];
	 w++;
	 adrBuf = i;
	}

	Buffer[adrBuf + 1] = '\0';

	lenBuffer = lenTagBuffer;

   int adrTag = 0;

  for (int i = 0; i < this->CountTeg; i++)
	 if (strcmp(this->Teg[i].Teg, Tag) == 0)
	 {
		this->Teg[i] = StartTeg;
		adrTag = i + 1;
		break;
	 }

  for (int i = adrTag; i < this->CountTeg; i++)
	 if (strcmp(this->Teg[i].Teg, Tag) == 0)
	 {
		this->Teg[i] = EndTag;
		break;
	 }


	 return 1;
}

// Банально, а при отладке вообще божественная вещь
int OriLng::PrintData(char *Tag)
{
   int Result = this->ReturnData(g_buf, Tag, this->MaxSizeBuffer);
   std::cout << g_buf;
   return Result;
}

// Смотерть коммент выше
int OriLng::PrintDataLine (char *Tag)
{
	int Result = this->ReturnData(g_buf, Tag, this->MaxSizeBuffer);
   std::cout << "\n" << g_buf;
   return Result;
}
void OriLng::SpeedRunBuffer()
{	
	ErrorTeg[0] = '\0';
	g_buf[0] = '\0';
	CountTeg = 0;
	SetSize(strlen(Buffer));


	endRead = 0;
	a = 0;
	b = 0;

	if (CountTeg != 0)
		b = Teg[CountTeg - 1].EndSymbol - 1;

	while (endRead >= 0)
	{
		endRead = StartScan(b, g_buffer, a, b);

		if (endRead == 1)
		{
			Teg[CountTeg].StartSymbol = a;
			Teg[CountTeg].EndSymbol = b;
			strcpy(Teg[CountTeg].Teg, g_buffer);
			CountTeg++;
		}  //EndIf
	}  //EndWhile

}

/**************************************************************
 * Функция принимает код и начинает сортировку в массив тегов.*
 * LngCode - скрипт код                                       *
 ************************************************************ */
OriLng& OriLng:: operator << (const char *LngCode) {

#ifdef SPEED_TEST
	SetIdlePriority();
	long timer = GetTimer(0);
	timer = GetTimer(timer);
	std::cout << "\nSpeed Read file LNG";
#endif

	
	StartSpeed
	int lenLngCode = strlen(LngCode);
	EndSpeed("strlen lngcode");

	// Если новый буффер пустой
	if (lenLngCode == 0) {
		sprintf(ErrorTeg, ERROR_MAX_BUFFER);
		return *this;
	}

	// Если длина добавленой новой строки выше длины от размера буффера - уже используемой памяти буфера, тогда запись в буфер невозможна
	if (lenLngCode > (MaxSizeBuffer - strlen(Buffer)))	{

		unsigned NewSizeBuffer = lenLngCode + (MaxSizeBuffer * 2);
		char *tmp = new char[MaxSizeBuffer + lenLngCode];
		strcpy(tmp, this->GetBuffer());
		strcat(tmp, LngCode);
		this->DeleteBuffer();
		this->CreateBuffer(NewSizeBuffer);
		strcpy(this->Buffer, tmp);
		this->SpeedRunBuffer();
		delete[] tmp;
		return *this;
	}

	// Пишем в конец буффера
	strcat(Buffer, LngCode);

    // Пересчитаем текущую длину буфера
	lenBuffer = strlen(Buffer);

	// Теперь просто банально нужно записать адреса у нового буффера
	 endRead = 0;
	 a = 0;
	 b = 0;

	if (CountTeg != 0)
	 b = Teg[CountTeg - 1].EndSymbol - 1;

	while (endRead >= 0)
	{
		endRead = StartScan(b, g_buffer, a, b);

	 if (endRead == 1)
	 {
			Teg[CountTeg].StartSymbol = a;
			Teg[CountTeg].EndSymbol   = b;
			strcpy(Teg[CountTeg].Teg, g_buffer);
			CountTeg++;
	 }  //EndIf
	}  //EndWhile

	return *this;
}

OriLng& OriLng:: operator = (const char *LngCode) {
	this->Clear();
	this->operator<< (LngCode);
	return *this;
}

OriLng& OriLng:: operator + (const char *LngCode) {
	this->operator<< (LngCode);
	return *this;
}


OriLng& OriLng:: operator + (OriLng &Lng) {
	this->operator << (Lng);
/*	lng buf;
	buf << this->Buffer;
	buf << Lng.Buffer;*/
	return *this;
}


OriLng& OriLng:: operator = (OriLng &Lng) {
	this->operator<< (Lng.Buffer);
	return *this;
}
bool OriLng::operator == (OriLng &Lng)
{
	if (strcmp(this->Buffer, Lng.Buffer) == 0) return true;
	return false;
}


void OriLng::AddCode(const char* CodeLng, ...)
{
	va_list vl;
	va_start(vl, CodeLng);	
	VSNPRINTF(g_buf, this->MaxSizeBuffer - 1, CodeLng, vl);
	this->operator<< (g_buf);
	va_end(vl);
}


void OriLng::WriteDataFormat(char *Tag, const char* CodeLng, ...)
{
	va_list vl;
	va_start(vl, CodeLng);
	VSNPRINTF(g_buf, this->MaxSizeBuffer - 1, CodeLng, vl);
	this->WriteData(g_buf, Tag);
	va_end(vl);
}

/***********************************************************
 Функция ищет имя тега вместе с указаными скобками.        *
 StartPosition    - Адрес с которого начинается поиск      *
 NameTeg          - Буффер для найденого тега              *
 StartPositionTeg - Адрес, где был открыт тег              *
 EndPositionTeg   - Адрес, где был закрыт тег              *
 ***********************************************************/
int OriLng::StartScan(int StartPosition, char* NameTeg, int &StartPositionTeg,
	int &EndPositionTeg) {

	NameTeg[0] = '\0';

	if (StartPosition > MaxSizeBuffer)	{
		sprintf(ErrorTeg, ERROR_MAX_BUFFER);
		return false;
	}

	/*if (lenBuffer == 0) 	{
       sprintf(ErrorTeg, ERROR_MAX_BUFFER);
	   return false;
	}*/

	int _count_symbol = 0;
	int CountTegSymbol = 0;

	for (int i = StartPosition; i <= lenBuffer; i++) 
	{
		if (Buffer[i] == OPEN_TEG_SYMBOL) 
		{
			StartPositionTeg = i;
	
			for (int w = i; w <= (i + MAX_SYMBOL_NAME); w++)
			{
				if (CountTegSymbol > MAX_SYMBOL_NAME - 1) 	{
					sprintf(ErrorTeg, ERROR_LEN_TEG, StartPositionTeg);
					return 0;
				}

				if (Buffer[w] == 0x00) 
					return -1;

				CountTegSymbol++;

				if (Buffer[w] == CLOSE_TEG_SYMBOL)	{
					
					EndPositionTeg = w;

					for (int k = StartPositionTeg; k <= EndPositionTeg; k++) 
					{
						NameTeg[_count_symbol] = Buffer[k];
						_count_symbol++;
					}

					NameTeg[_count_symbol] = '\0';
					return 1;
				} //end if close symbol
			} // endfori
		} //end if symbol
	} //endfori

	return -1;
}

/***********************************************************
 Функция кладёт в буффер данные, указаные в скрипт языке.  *
 _buffer - буффер                                          *
 NameTeg - имя тега                                        *
 ***********************************************************/
int OriLng::ReturnData(char *_buffer, char *Name, int Size) {

	//Фича, если не было указаны скобки у тега, то они подставляются сами
	char NameTeg[MAX_SYMBOL_NAME + 1] = { 0 };

	if (Name[0] != OPEN_TEG_SYMBOL)	
		sprintf(NameTeg, "<%s>", Name);
	else strcpy(NameTeg, Name);

	_buffer[0] = '\0';
	int StartDataRead = 0;
	int EndDataRead = 0;
	int NTeg = 0;
	
	// Найдем первый тег
	for (int i = 0; i < CountTeg; i++)
		if (strcmp(NameTeg, Teg[i].Teg) == 0) {
			StartDataRead = Teg[i].EndSymbol;
			NTeg = i + 1;
			break;
		}

	// Найдем его пару
	for (int i = NTeg; i < CountTeg; i++)
		if (strcmp(NameTeg, Teg[i].Teg) == 0) {
			EndDataRead = Teg[i].StartSymbol;
			break;
		}

	if (EndDataRead == 0)
	{
		sprintf(ErrorTeg, ERROR_SEARCH_TEG, NameTeg);
		return 0;
	}

	int count = 0;
	for (int i = StartDataRead + 1; i < EndDataRead; i++) {
		_buffer[count] = this->Buffer[i];
		count++;
		if (count >= Size) break;
	}

	_buffer[count] = '\0';
	return 1;
}


int OriLng::Result(int32_t &buffer, int Size, char  *Tag, ...) {


	char tag_buf[32] = { 0 };
	va_list vl;
	va_start(vl, Tag);
	VSNPRINTF(tag_buf, 32 - 1, Tag, vl);
	va_end(vl);

	char IntResult[32] = { 0 };
	ReturnData(IntResult, tag_buf, Size);

	buffer = atoi(IntResult);
	
	return 1;
}


int OriLng::Result(char *buffer, int Size, char  *Tag, ...) {


	char tag_buf[32] = { 0 };
	va_list vl;
	va_start(vl, Tag);
	VSNPRINTF(tag_buf, 32 - 1, Tag, vl);
	va_end(vl);

	ReturnData(buffer, tag_buf, Size);

	 return 1;
}

char  *OriLng::Result(char  *Tag, int Size, ...) {

	UseResultBuffer(Size);

	char tag_buf[32] = { 0 };
	va_list vl;
	va_start(vl, Size);
	VSNPRINTF(tag_buf, 32 - 1, Tag, vl);
	va_end(vl);

	ReturnData(ResultBuffer, tag_buf, Size);

	return ResultBuffer;
}

/*
 int OriLng::ReturnDataInt(char * Tag)
 {
	 char IntResult[32];
	 IntResult[0] = '\0';
	 this->ReturnData(IntResult, Tag, 32);
	 return atoi(IntResult);
 }
 */
 void OriLng::ReturnDataInt(int Buffer, char * Tag)
 {
	 char IntResult[32];
	 IntResult[0] = '\0';
	 this->ReturnData(IntResult, Tag, 32);
	 Buffer = atoi(IntResult);
 }

 int OriLng::ReturnDataInt(char *Tag, ...)
 {
	 char IntResult[32];
	 IntResult[0] = '\0';

	 char tag_buf[32] = { 0 };
	 va_list vl;
	 va_start(vl, Tag);
	 VSNPRINTF(tag_buf, 32 - 1, Tag, vl);
	 va_end(vl);
	 this->ReturnData(IntResult, tag_buf, 32);
	 return atoi(IntResult);
 }

 /*
 __int64 OriLng::RetuntDataInt64(char * Tag)
 {
	 char IntResult[32];
	 IntResult[0] = '\0';
	 this->ReturnData(IntResult, Tag, 32);
	 return _atoi64(IntResult);
 }

 void OriLng::RetuntDataInt64(__int64 Buffer, char  *Tag)
 {
	 char IntResult[32];
	 IntResult[0] = '\0';
	 this->ReturnData(IntResult, Tag, 32);
	 Buffer = _atoi64(IntResult);
 }*/

/***********************************************************
  Функция чистит буфер и теги
 ********************************************************** */
void OriLng::Clear() {
	isCrypt     = false;
	Buffer[0]   = '\0';
	ErrorTeg[0] = '\0';
	g_buf[0]    = '\0';
	CountTeg    =   0;
	SetSize(0);
	return;
}

struct PointPosition {
	int Start;
	int End;
};


int OriLng::SaveCryptFile(char *NameFile, char *Key) {

	int SizeF = strlen(this->Buffer);
	if (SizeF < 0) return -1;
	FILE *fp = NULL;
	if ((fp = fopen(NameFile, "wb+")) == NULL) return -1;

	if (!isCrypt)
	  CryptBuffer(Key);

	fwrite(Buffer, 1, SizeF, fp);
	fclose(fp);

	if (isCrypt)
		CryptBuffer(Key, SizeF);

	return SizeF;
}

int OriLng::SaveCryptFile(OriFile *File, char *Key) {

	if (File == NULL || File->GetDesc() == NULL)
		return ORI_ERROR;

	int SizeF = strlen(this->Buffer);
	if (SizeF < 0)
		 return ORI_ERROR;

	if (!isCrypt)
		CryptBuffer(Key);

	fwrite(Buffer, 1, SizeF, File->GetDesc());

	if (isCrypt)
		CryptBuffer(Key, SizeF);

	return SizeF;
}

void OriLng::XORCrypt(char *buffer, char *src, uint32_t SizeBuffer)
{
	char * p = buffer;
	unsigned a = 0;
	int SZ = strlen(src);

	for (; p < buffer + SizeBuffer; p++) {
		*p = (*p) ^ src[a] + (p - buffer) * 2;
		a++;
		if (a > SZ) a = 0;
	}
}

void OriLng::XORCrypt(char *buffer, char *src)
{
	char * p = buffer;
	unsigned a = 0;
	int sz = strlen(buffer);
	int SZ = strlen(src);

	for (; p < buffer + sz; p++) {
		*p = (*p) ^ src[a] + (p - buffer) * 2;
		a++;
		if (a>SZ) a = 0;
	}
}

void OriLng::MemClear() {

	memset(this->Buffer, 0x00, this->MaxSizeBuffer);
	memset(this->g_buf,  0x00, this->MaxSizeBuffer);
}