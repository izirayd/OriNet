// ---------------------------------------------------------------------------

#pragma hdrstop

#include "../OriKernel/KernelPlatform.h"

#ifndef ORI_BORLAND

#include "OriConsole.h"

#include <iostream>

#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)


#if defined ORI_WINDOWS

#include <Windows.h>
#include <tchar.h>
#include <conio.h>
#include <strsafe.h>

#endif

#if defined ORI_WINDOWS

#else

/*
enum ConsoleColor {
	Black = 30, Blue = 34, Green = 32, Cyan = 21, Red = 31, Magenta = 35, Brown = 33,
	LightGray = 32, DarkGray = 32, LightBlue = 36, LightGreen = 32, LightCyan = 18,
	LightRed = 31, LightMagenta = 35, Yellow = 33, White = 1
};*/

int ReturnLinuxColor(int IndexWindowColor)
{
	switch (IndexWindowColor)
	{
	case 0: return 30;
	case 1: return 34;
	case 2: return 32;
	case 3: return 36;
	case 4: return 31;
	case 5: return 35;
	case 6: return 21;
	case 7: return 1;
	case 8: return 1;
	case 9: return 36;
	case 10: return 32;
	case 11: return 36;
	case 12: return 31;
	case 13: return 35;
	case 14: return 33;
	case 15: return 7;
	}

	return 7;
}
#endif

void SetColor(int text, int background, char *str) {

#if defined ORI_WINDOWS
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
	std::cout << str;
#else
	printf("\033[%dm%s\033[0m", ReturnLinuxColor(text), str);
#endif

}

void OriConsole::OriMsg(const char *msg1) {
    std::cout << msg1;
}

void OriConsole::OriLMsg(const char *msg1) {

#if defined ORI_WINDOWS
    printf("\n%s", msg1);
#else
    time(&t);
    tm = localtime(&t);
    sprintf(LocalTime, "%d/%d/%d : %d:%d:%d", tm->tm_mon + 1, tm->tm_mday,
	tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
    printf("\n%s : %s", LocalTime, msg1);
#endif

}

void OriConsole::printf(char *_Str, ...) {

	char g_buf[512] = { 0 };

	va_list vl;
	va_start(vl, _Str);
	VSNPRINTF(g_buf, 512 - 1, _Str, vl);
	::printf("%s", g_buf);
	va_end(vl);
}

void OriConsole::Msg(const char TypeMsg, const int color, char *msg, ...) {

	LogMutex.lock();

	char g_buf[512] = { 0 };

	va_list vl;
	va_start(vl, msg);
	VSNPRINTF(g_buf, 512 - 1, msg, vl);

	char _result[512] = { 0 };

    if ((TypeMsg == 'Q') || (TypeMsg == 'q')) {

		SetColor(color, 0, g_buf);
		LogMutex.unlock();
	    return;
    }

    if ((TypeMsg == 'N') || (TypeMsg == 'n')) {
	
		SetColor(color, 0, "\n");
		SetColor(color, 0, g_buf);
	  
		LogMutex.unlock();
        return;
    }

	strcpy(_result, LTime->LocalTime);

	if ((TypeMsg == 'S') || (TypeMsg == 's'))
	 strcat(_result, " SYSTEM : ");

    if ((TypeMsg == 'C') || (TypeMsg == 'c'))
	 strcat(_result, " CLIENT : ");

    if ((TypeMsg == 'A') || (TypeMsg == 'a'))
	 strcat(_result, " ADMIN : ");

    if ((TypeMsg == 'E') || (TypeMsg == 'e'))
	 strcat(_result, " SYS_ERROR : ");

	if ((TypeMsg == 'G') || (TypeMsg == 'g'))
		strcat(_result, " ГЛАЗ : ");

	if ((TypeMsg == 'U') || (TypeMsg == 'u'))
		strcat(_result, " SERVES : ");
	
	setlocale(0, "RUSSIAN");

	SetColor(color, 0, "\n");
	SetColor(color, 0, _result);
	SetColor(color, 0, g_buf);

	if (isSaveLog && LogFile.is_open())
	{
		LogFile << _result << g_buf << "\n";
		LogFile.flush();
	}

	va_end(vl);

	LogMutex.unlock();
}

OriConsole& OriConsole::operator << (const char *Str){
	std::cout << Str;
	return *this;
}

#if defined ORI_WINDOWS
void KernelApiConsole::ConsoleTitle(char *NameTitle)
{
	char szOldTitle[MAX_PATH];
	char szNewTitle[MAX_PATH];

	if (GetConsoleTitleA(szOldTitle, MAX_PATH))	{
		StringCchPrintfA(szNewTitle, MAX_PATH, NameTitle);
		SetConsoleTitleA(szNewTitle);
	}
}

void KernelApiConsole::HideCursor()
{
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor = { 1, false };
	SetConsoleCursorInfo(hCons, &cursor);
}


void KernelApiConsole::ShowCursor()
{
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor = { 1, true };
	SetConsoleCursorInfo(hCons, &cursor);
}

void KernelApiConsole::ShowCursor(int Size)
{
	if ((Size < 1) || (Size > 100)) return;
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor = { Size, true };
	SetConsoleCursorInfo(hCons, &cursor);
}

void KernelApiConsole::OriWriteConsole(int CoordX, int CoordY, char *Text)
{
	DWORD cWritten;
	COORD coord;

	coord.X = CoordX;
	coord.Y = CoordY;

	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);

	WriteConsoleOutputCharacterA(
		hCons,
		Text,
		strlen(Text),
		coord,
		&cWritten);

}

void KernelApiConsole::PositionCursor(int &_X, int &_Y)
{
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(hCons, &csbiInfo);

	_X = csbiInfo.dwCursorPosition.X;
	_Y = csbiInfo.dwCursorPosition.Y;
}


struct Column
{
	int Size;
	int X, Y;
	char Header[32];

};

class TableConsole
{
public:
	// Функции пользователя
	void CreateTable(int CountRow, int CountCol);
	void TitleTable(const char *Text);

	void AddColumn(int Size);


	void Draw();
	void DrawTable(int CordX, int CordY, int Size);

private:
	// Графика
	void DrawLineCol(int  CordX, int CordY, int Len);
	void DrawLineRow(int  CordX, int CordY, int Len);
	void DrawRectangle(int CordX, int CordY, int Size);


	Column m_Column[50];
	int CountColumn;

	char m_TitleTable[32];

	int _Row;
	int _Col;

	int TableCordX;
	int TableCordY;
	int TableSize;

	int MaxSizeRow;
	int MaxSizeCol;
	int SizeHeader;
	int Srednee;

	KernelApiConsole ApiConsole;
};

void TableConsole::AddColumn(int Size)
{
	m_Column[CountColumn].X = this->TableCordX + Size;
	m_Column[CountColumn].Y = this->TableCordY + 1;

	m_Column[CountColumn].Size = Size;
	CountColumn++;

}

void TableConsole::TitleTable(const char *Text)
{
	strcpy(m_TitleTable, Text);
}

void TableConsole::CreateTable(int CountRow, int CountCol)
{
	_Row = CountRow;
	_Col = CountCol;
}

#define COL_POINT "|"
#define ROW_POINT "-"
#define MAX_LEN_ROW 79

void TableConsole::DrawLineCol(int SizeX, int SizeY, int Len)
{
	for (int i = 0; i < Len; i++)
		ApiConsole.OriWriteConsole(SizeX, i + SizeY, COL_POINT);
}

void TableConsole::DrawLineRow(int SizeX, int SizeY, int Len)
{
	for (int i = 0; i < Len; i++)
		ApiConsole.OriWriteConsole(i + SizeX, SizeY, ROW_POINT);
}

void TableConsole::DrawRectangle(int CordX, int CordY, int Size)
{
	DrawLineCol(CordX, CordY, Size);
	DrawLineCol(CordX + Size, CordY, Size);
	DrawLineRow(CordX, CordY, Size + 1);
	DrawLineRow(CordX, CordY + Size, Size + 1);
}

void TableConsole::Draw()
{
	DrawTable(10, 10, 20);
}

void TableConsole::DrawTable(int CordX, int CordY, int Size)
{
	if (Size < 2) return;
	CountColumn = 0;

	DrawRectangle(CordX, CordY, Size);
	DrawLineRow(CordX, CordY + 2, Size);

	int LimitLen = Size - 2;

	int Len = strlen(m_TitleTable);
	if (Len < LimitLen)
		ApiConsole.OriWriteConsole(CordX + 2, CordY + 1, m_TitleTable);
	else
	{
		m_TitleTable[LimitLen] = '\0';
		ApiConsole.OriWriteConsole(CordX + 2, CordY + 1, m_TitleTable);
	}

	TableCordX = CordX;
	TableCordY = CordY;
	TableSize = Size;

}

ORI_THREAD_FOR_CLASS(OriProgressConsoleBar);
void OriProgressConsoleBar::Run()  {


	int LocalPositionX = PositionX;
	int LocalPositionY = PositionY;
	int LocalSizeBar   = SizeBar;
	int Loval_MaxValue = _MaxValue;

	if (Style == 0)
	{
		if (_MaxValue == 0) return;

		float Procent = 1;

		char Buffer[512] = { 0 };

		int OldValue = Value;

		sprintf(Buffer, "[%d%%] -> ", (int) Procent);
		ApiConsole.OriWriteConsole(LocalPositionX, LocalPositionY, Buffer);

		int StartBarPosition = LocalPositionX + strlen(Buffer);

		sprintf(Buffer, "[");

		if (LocalSizeBar > 256) LocalSizeBar = 256;

		for (size_t i = 0; i < LocalSizeBar; i++)
			sprintf(Buffer + (strlen(Buffer)), " ");

		sprintf(Buffer + (strlen(Buffer)), "]");

		ApiConsole.OriWriteConsole(StartBarPosition, LocalPositionY, Buffer);

		if (Value == 0) Value++;
		while ((isStart) && (Value < _MaxValue))
		{

			if (Value != OldValue)
			{
				OldValue = Value;
				// Расчитаем процент
				Procent = (float)((float)Value / (float)_MaxValue);
				float CountBar = (float)((float)Procent * (float)LocalSizeBar);
				Procent *= 100;
				// Срендерим
				if (_MaxValue < 101)
					sprintf(Buffer, "[%2.0f%%] -> [", Procent);	else
					if (_MaxValue < 1001)
						sprintf(Buffer, "[%2.1f%%] -> [", Procent);  else
						if (_MaxValue < 5001)
							sprintf(Buffer, "[%2.2f%%] -> [", Procent);  else
							if (_MaxValue < 10001)
								sprintf(Buffer, "[%2.3f%%] -> [", Procent);  else
								sprintf(Buffer, "[%2.4f%%] -> [", Procent);

				StartBarPosition = LocalPositionX + strlen(Buffer) - 1;

				ApiConsole.OriWriteConsole(LocalPositionX, LocalPositionY, Buffer);


				sprintf(Buffer, "[");

				for (size_t i = 0; i < CountBar; i++)
					sprintf(Buffer + (strlen(Buffer)), "#");

				for (size_t i = CountBar; i < LocalSizeBar; i++)
					sprintf(Buffer + (strlen(Buffer)), " ");

				sprintf(Buffer + (strlen(Buffer)), "] ");

				// Выводим звёздочку
				ApiConsole.OriWriteConsole(StartBarPosition, LocalPositionY, Buffer);
			}

			Sleep(1);
		}

		sprintf(Buffer, "[100%%] -> ");
		ApiConsole.OriWriteConsole(LocalPositionX, LocalPositionY, Buffer);
		StartBarPosition = LocalPositionX + strlen(Buffer);
		sprintf(Buffer, "[");

		for (size_t i = 0; i < LocalSizeBar; i++)
			sprintf(Buffer + (strlen(Buffer)), "#");

		sprintf(Buffer + (strlen(Buffer)), "]");

		ApiConsole.OriWriteConsole(StartBarPosition, LocalPositionY, Buffer);
	}

	if (Style == 1)
	{

		if (_MaxValue == 0) return;

		float Procent = 1;

		char Buffer[512] = { 0 };

		int OldValue = Value;

		sprintf(Buffer, "[0 / %d] -> ", _MaxValue);
		ApiConsole.OriWriteConsole(LocalPositionX, LocalPositionY, Buffer);

		int StartBarPosition = LocalPositionX + strlen(Buffer);

		sprintf(Buffer, "[");

		if (LocalSizeBar > 256) LocalSizeBar = 256;

		for (size_t i = 0; i < LocalSizeBar; i++)
			sprintf(Buffer + (strlen(Buffer)), " ");

		sprintf(Buffer + (strlen(Buffer)), "]");

		ApiConsole.OriWriteConsole(StartBarPosition, LocalPositionY, Buffer);

		if (Value == 0) Value++;
		while ((isStart) && (Value < _MaxValue))
		{

			if (Value != OldValue)
			{
				OldValue = Value;
				// Расчитаем процент
				Procent = (float)((float)Value / (float)_MaxValue);
				float CountBar = (float)((float)Procent * (float)LocalSizeBar);
				Procent *= 100;

				sprintf(Buffer, "[%d / %d] -> [", Value, _MaxValue);
					
				StartBarPosition = LocalPositionX + strlen(Buffer) - 1;

				ApiConsole.OriWriteConsole(LocalPositionX, LocalPositionY, Buffer);

				sprintf(Buffer, "[");

				for (size_t i = 0; i < CountBar; i++)
					sprintf(Buffer + (strlen(Buffer)), "#");

				for (size_t i = CountBar; i < LocalSizeBar; i++)
					sprintf(Buffer + (strlen(Buffer)), " ");

				sprintf(Buffer + (strlen(Buffer)), "] ");

				// Выводим звёздочку
				ApiConsole.OriWriteConsole(StartBarPosition, LocalPositionY, Buffer);
			}

			Sleep(1);
		}

		sprintf(Buffer, "[%d / %d] -> ", _MaxValue, _MaxValue);
		ApiConsole.OriWriteConsole(LocalPositionX, LocalPositionY, Buffer);
		StartBarPosition = LocalPositionX + strlen(Buffer);
		sprintf(Buffer, "[");

		for (size_t i = 0; i < LocalSizeBar; i++)
			sprintf(Buffer + (strlen(Buffer)), "#");

		sprintf(Buffer + (strlen(Buffer)), "]");

		ApiConsole.OriWriteConsole(StartBarPosition, LocalPositionY, Buffer);

	}



	if (Style == 2)
	{
		if (_MaxValue == 0) return;
		float Procent = 1;
		char Buffer[512] = { 0 };

		int OldValue = Value;

		sprintf(Buffer, "[0 / %d:0%%] -> ", _MaxValue);
		ApiConsole.OriWriteConsole(LocalPositionX, LocalPositionY, Buffer);

		int StartBarPosition = LocalPositionX + strlen(Buffer);

		sprintf(Buffer, "[");

		if (LocalSizeBar > 256) LocalSizeBar = 256;

		for (size_t i = 0; i < LocalSizeBar; i++)
			sprintf(Buffer + (strlen(Buffer)), " ");

		sprintf(Buffer + (strlen(Buffer)), "]");

		ApiConsole.OriWriteConsole(StartBarPosition, LocalPositionY, Buffer);

		if (Value == 0) Value++;
		while ((isStart) && (Value < _MaxValue))
		{

			if (Value != OldValue)
			{
				OldValue = Value;
				// Расчитаем процент
				Procent = (float)((float)Value / (float)_MaxValue);
				float CountBar = (float)((float)Procent * (float)LocalSizeBar);
				Procent *= 100;

				sprintf(Buffer, "[%d / %d:%4.2f%%] -> [", Value, _MaxValue, Procent);

				StartBarPosition = LocalPositionX + strlen(Buffer) - 1;

				ApiConsole.OriWriteConsole(LocalPositionX, LocalPositionY, Buffer);

				sprintf(Buffer, "[");

				for (size_t i = 0; i < CountBar; i++)
					sprintf(Buffer + (strlen(Buffer)), "#");

				for (size_t i = CountBar; i < LocalSizeBar; i++)
					sprintf(Buffer + (strlen(Buffer)), " ");

				sprintf(Buffer + (strlen(Buffer)), "] ");

				// Выводим звёздочку
				ApiConsole.OriWriteConsole(StartBarPosition, LocalPositionY, Buffer);
			}

			Sleep(1);
		}



		sprintf(Buffer, "[%d / %d:100%%] -> ", Loval_MaxValue, Loval_MaxValue);
		ApiConsole.OriWriteConsole(LocalPositionX, LocalPositionY, Buffer);
		StartBarPosition = LocalPositionX + strlen(Buffer);
		sprintf(Buffer, "[");

		for (size_t i = 0; i < LocalSizeBar; i++)
			sprintf(Buffer + (strlen(Buffer)), "#");

		sprintf(Buffer + (strlen(Buffer)), "]");

		ApiConsole.OriWriteConsole(StartBarPosition, LocalPositionY, Buffer);

	}


}

OriProgressConsoleBar::~OriProgressConsoleBar()
{
	this->Stop();
}

void OriProgressConsoleBar::Text(const char *_Text) {

	printf("%s", _Text);

}

OriProgressConsoleBar::OriProgressConsoleBar() {
	_MaxValue = 0;
	PositionY = 0;
	PositionX = 0;
	isStart   = true;;
	Value     = 0;
	SizeBar   = 10;
	Style     = 0;
}

void OriProgressConsoleBar::Stop()
{
	isStart = false;
	Sleep(50);
}

void OriProgressConsoleBar::Create(const char *_Text) {
	Text(_Text);
	Create();
}

int OriProgressConsoleBar::MaxValue()   {
	return _MaxValue;
}

int OriProgressConsoleBar::MaxValue(int _NewValue)   {
	_MaxValue = _NewValue;
	return _MaxValue;
}

void OriProgressConsoleBar::Create()
{
	ApiConsole.HideCursor();
	ApiConsole.PositionCursor(PositionX, PositionY);
	Value = 0;

	this->CreateThread();
	Sleep(10);
}

#endif

#endif