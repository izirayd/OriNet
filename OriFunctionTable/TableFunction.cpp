#include "TableFunction.h"
#include <iostream>
#include "../OriUntil/OriThread.h"

#pragma warning(disable : 4996)

#define STANDART_COUNT_TB 512

TableFunction::TableFunction()
{
	ClearCount();
	CreateTB(STANDART_COUNT_TB);
	SetRunInNewThread(false);
}

TableFunction::TableFunction(int CountFunction)
{
	ClearCount();
	CreateTB(CountFunction);
	SetRunInNewThread(false);
}

void TableFunction::ClearCount()
{
	this->CountTBBuffer     = 0;
	this->CountTBLng        = 0;
	this->CountTBLngClient  = 0;
	this->CountTBNull       = 0;
	this->CountTBSockBuffer = 0;
}

TableFunction::~TableFunction()
{
	delete[] this->TBSockBuffer;
	delete[] this->TBBuffer;
	delete[] this->TBLng;
	delete[] this->TBLngClient;
	delete[] this->TBNull;
}

void TableFunction::CreateTB(int Count)
{
	CountMaxFunction = Count;
	
	this->TBSockBuffer = new TBStructSockBuffer[Count];
	this->TBBuffer     = new TBStructBuffer    [Count];
	this->TBLng        = new TBStructLng       [Count];
	this->TBLngClient  = new TBStructLngClient [Count];
	this->TBNull       = new TBStructNull      [Count];

}


void TableFunction::ReCreateTB(int SizeTB)
{
	delete[] this->TBSockBuffer;
	delete[] this->TBBuffer;
	delete[] this->TBLng;
	delete[] this->TBLngClient;
	delete[] this->TBNull;

	CreateTB(SizeTB);
}

#include <string.h>

bool TableFunction::AddFunction(char *Name, FunctionSockBuffer Function)
{
	if (CountTBSockBuffer < this->CountMaxFunction)  {
		TBSockBuffer[CountTBSockBuffer].Function = Function;
		strcpy(TBSockBuffer[CountTBSockBuffer].Name, Name);
		CountTBSockBuffer++;
		return true;
	}

	return false;
	
}

bool TableFunction::AddFunction(char *Name, FunctionBuffer     Function)
{
	if (CountTBBuffer < this->CountMaxFunction)  {
		TBBuffer[CountTBBuffer].Function = Function;
		strcpy(TBBuffer[CountTBBuffer].Name, Name);
		CountTBBuffer++;
		return true;
	}

	return false;
}

bool TableFunction::AddFunction(char *Name, FunctionNull       Function)
{
	if (CountTBNull < this->CountMaxFunction)  {
		TBNull[CountTBNull].Function = Function;
		strcpy(TBNull[CountTBNull].Name, Name);
		CountTBNull++;
		return true;
	}

	return false;
}

bool TableFunction::AddFunction(char *Name, FunctionLngClient  Function)
{

	if (CountTBLngClient < this->CountMaxFunction)  {
		TBLngClient[CountTBLngClient].Function = Function;
		strcpy(TBLngClient[CountTBLngClient].Name, Name);
		CountTBLngClient++;
		return true;
	}

	return false;
}

bool TableFunction::AddFunction(char *Name, FunctionLng        Function)
{
	if (CountTBLng  < this->CountMaxFunction)  {
		TBLng[CountTBLng].Function = Function;
		strcpy(TBLng[CountTBLng].Name, Name);
		CountTBLng++;
		return true;
	}

	return false;
}

void TableFunction::PrintListTables()
{

	std::cout << "\nPrint Function protocol";

	if (CountTBSockBuffer > 0) 	{
		std::cout << "\nFunction Sock/Buffer";

		for (int i = 0; i < CountTBSockBuffer; i++)
			std::cout << "\n" << TBSockBuffer[i].Name << " : " << TBSockBuffer[i].Function;
	}

	if (CountTBBuffer > 0)  	{
		std::cout << "\nFunction Buffer";

		for (int i = 0; i < CountTBBuffer; i++)
			std::cout << "\n" << TBBuffer[i].Name << " : " << TBBuffer[i].Function;
	}

	if (CountTBNull > 0) 	{
		std::cout << "\nFunction Null";

		for (int i = 0; i < CountTBNull; i++)
			std::cout << "\n" << TBNull[i].Name << " : " << TBNull[i].Function;
	}

	if (CountTBLngClient > 0) 	{
		std::cout << "\nFunction Lng/Client";

		for (int i = 0; i < CountTBLngClient; i++)
			std::cout << "\n" << TBLngClient[i].Name << " : " << TBLngClient[i].Function;
	}

	if (CountTBLng > 0)  	{
		std::cout << "\nFunction Lng";

		for (int i = 0; i < CountTBLng; i++)
			std::cout << "\n" << TBLng[i].Name << " : " << TBLng[i].Function;
	}

	std::cout << std::endl;

	return;
}

bool TableFunction::RunFunction(char *Name, int Sock, char *Buffer)
{
	if (CountTBSockBuffer > 0) 	
		for (int i = 0; i < CountTBSockBuffer; i++)	
		if (strcmp(TBSockBuffer[i].Name, Name) == 0)	{
			((*TBSockBuffer[i].Function)(Sock, Buffer));
			return true;
		}
			
				
	if (CountTBBuffer > 0)
		for (int i = 0; i < CountTBBuffer; i++)
		if (strcmp(TBBuffer[i].Name, Name) == 0)	{
			((*TBBuffer[i].Function)(Buffer));
			return true;
		}
				

	if (CountTBNull > 0)
		for (int i = 0; i < CountTBNull; i++)
		if (strcmp(TBNull[i].Name, Name) == 0)	{
			((*TBNull[i].Function)());
			return true;
		}
			
	return false;
}

struct stRunPacket
{
	FunctionLngClient *Function;
	OriServerClient   Client;
	OriLng            Lng;
};


OriFunctionThread(_Run, _Arg) {	
	stRunPacket *m_stRunPacket = (stRunPacket*) _Arg;
	((*m_stRunPacket->Function)((OriServerClient &)m_stRunPacket->Client, (OriLng &)m_stRunPacket->Lng));
	delete m_stRunPacket;
	KernelCloseThread;
}

#include "Gettimer.h"

//#define SPEED_TEST

#ifdef SPEED_TEST

#define StartSpeed timer = GetTimer(0);
#define EndSpeed(text)  timer = GetTimer(timer);  std::cout << "\nSpeed : " << timer << "ms" << " - " << text << std::endl;

#else
#define StartSpeed ;
#define EndSpeed ;
#endif


bool TableFunction::RunFunctionLng(char *Name, OriServerClient &Client, OriLng &Lng)
{

#ifdef SPEED_TEST
	SetIdlePriority();
	long timer = GetTimer(0);
	timer = GetTimer(timer);

	#ifndef ORI_CLIENT
	std::cout << "\nStart Function: " << Name;
    #endif
#endif

	StartSpeed;

	if (CountTBLngClient > 0)
		for (int i = 0; i < CountTBLngClient; i++) {
			if (strcmp(TBLngClient[i].Name, Name) == 0) {



				if (GetRunInNewThread() || TBLngClient[i].GetRunInNewThread()) {
					stRunPacket *m_stRunPacket = new stRunPacket;
					m_stRunPacket->Client = Client;
					m_stRunPacket->Lng = Lng;
					m_stRunPacket->Function = &TBLngClient[i].Function;
					OriCreateThread(_Run, (void *)m_stRunPacket);
				}
				else ((*TBLngClient[i].Function)(Client, Lng));

				EndSpeed("End function");

				return true;
			}

			}

	if (CountTBLng > 0)
	for (int i = 0; i < CountTBLng; i++)
	if (strcmp(TBLng[i].Name, Name) == 0) 	 {
		((*TBLng[i].Function)(Lng));

		EndSpeed("End function");

		return true;
	}

	EndSpeed("End function");

	return false;
}


TableFunctionStandart::TableFunctionStandart() {
	this->ClearCount();
	this->CreateTB(STANDART_COUNT_TB);
}

TableFunctionStandart::TableFunctionStandart(int CountFunction) {
	this->ClearCount();
	this->CreateTB(CountFunction);
}

void TableFunctionStandart::ClearCount()
{
	this->CountTBNull = 0;
	
}

void TableFunctionStandart::CreateTB(int Count)
{
	CountMaxFunction = Count;
	this->TBNull = new TBStructNull[Count];
}

void TableFunctionStandart::ReCreateTB(int SizeTB) {
	
	delete[] this->TBNull;
	CreateTB(SizeTB);
}

bool TableFunctionStandart::AddFunction(FunctionNull Function) {

	if (CountTBNull < this->CountMaxFunction)  {
		TBNull[CountTBNull].Function = Function;
		CountTBNull++;
		return true;
	}

	return false;
}