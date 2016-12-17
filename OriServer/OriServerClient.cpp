#include "OriServerClient.h"
#include "../OriUntil/Untils.h"
#include <string.h>
#include <stdio.h>
#include <iostream>


int32_t  OriServerClient::GetSockOpt(int32_t FlagLevel, int32_t FlagOption)
{
	int32_t Values;
	return GetSockOpt(FlagLevel, FlagOption, Values);
}

int32_t  OriServerClient::GetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t &Values) {

#ifdef ORI_WINDOWS
	if (getsockopt(GetSocket(), FlagLevel, FlagOption, (char *)&Values, (int *)sizeof(int32_t)) == ORI_ERROR)
		return -1;
#endif
#ifdef ORI_LINUX
	if (getsockopt(GetSocket(), FlagLevel, FlagOption, &Values, (socklen_t *)sizeof(int32_t)) == ORI_ERROR)
		return ORI_ERROR;
#endif

	return Values;
}

int32_t  OriServerClient::SetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t Values)    {

#ifdef ORI_WINDOWS
	if (setsockopt(GetSocket(), FlagLevel, FlagOption, (char*)&Values, sizeof(int32_t)) == ORI_ERROR)
		return -1;
#endif

#ifdef ORI_LINUX
	if (setsockopt(GetSocket(), FlagLevel, FlagOption, &Values, sizeof(int32_t)) == ORI_ERROR)
		return ORI_ERROR;
#endif

	return Values;
}

OriServerClient::OriServerClient()
{
	this->isUseCrypt = false;
	TypePacket = ORI_TCP;
	SetMaxSizePacket(sizeof(KernelPacket));
	IndexPC[0] = 0x00;
}

OriServerClient&  OriServerClient::operator << (int Sock)
{
	this->sock = Sock;
	return *this;
}

OriServerClient&  OriServerClient::operator << (char *buffer)
{
	oriPacket *Packet = new oriPacket;

	strcpy(Packet->salt, SALT);
	strcpy(Packet->typecomand, this->Local_Type_Packet);
	strcpy(Packet->buffer, buffer);

	if (this->TypePacket == ORI_UDP) sendto(this->sock, (char*)Packet, sizeof(oriPacket), 0, (struct sockaddr *)&AddrClient, SizeClientAddr);
	else send(this->sock, (char*)Packet, sizeof(oriPacket), 0);

	delete Packet;
	return *this;
}

void  OriServerClient::strSIZE(char *buffer, char *out, int start, int count)
{
	int j = 0;
	for (int i = start; i < count; i++) {
		buffer[j] = out[i];
		j++;
	}
}

bool OriServerClient::SendPacket(char *NameFunction, void *_Packet, int _Size) {

	KernelPacket *Packet = new KernelPacket;
	Packet->type_packet_protocol = TYPE_POINT_PROTOCOL;
	strcpy(Packet->salt_packet, KERNEL_SALT);
	OriPointVoidPacket *Point = new OriPointVoidPacket;
	strcpy(Point->NamePacket, NameFunction);
	strnull(Point->buffer,  (char *)_Packet, _Size);
	strnull(Packet->buffer, (char*)Point, sizeof(OriPointVoidPacket));
	if (OriSendKernel(Packet))
	{
		delete Point;
		return true;
	} 

	delete Point;
	return false;
	
}

#include "../OriUntil/OriSleep.h"

bool OriServerClient::SendSuperPacket(char *NameFunction, unsigned char TypeProtocol, void *_Packet, unsigned long long SizePacket, int IndexTSSP)
{
	if (TypeProtocol == TYPE_SEND_SUPER_PACKETS) return false;

	KernelPacket   *Packet            = new KernelPacket;
	OriSuperPacket *SuperPacker       = new OriSuperPacket;

	Packet->type_packet_protocol      = TYPE_SEND_SUPER_PACKETS;
	SuperPacker->type_packet_protocol = TypeProtocol;

	strcpy(Packet->salt_packet, KERNEL_SALT);
	strcpy(SuperPacker->NamePacket, NameFunction);
	unsigned SizeSuperPacket = sizeof(OriSuperPacket);
	SuperPacker->FullSize    = SizePacket;
	SuperPacker->IndexTSSP   = IndexTSSP;


	SuperPacker->countPacket = SuperPacker->FullSize / (unsigned long long) SIZE_BUFFER_TSSP;
	unsigned long long SegmentFullSize = (unsigned long long) SIZE_BUFFER_TSSP * SuperPacker->countPacket;
	SuperPacker->EndPacketSize = SuperPacker->FullSize - SegmentFullSize;

	for (SuperPacker->numberPacket = 0; SuperPacker->numberPacket < SuperPacker->countPacket; SuperPacker->numberPacket++)
	{
		MemoryCopy(SuperPacker->buffer, (char *)_Packet, SIZE_BUFFER_TSSP * SuperPacker->numberPacket, (SIZE_BUFFER_TSSP * SuperPacker->numberPacket) + SIZE_BUFFER_TSSP);
		strnull(Packet->buffer, (char*)SuperPacker, SizeSuperPacket);
		send(this->sock, (char*)Packet, GetMaxSizePacket(), 0);
	}

	MemoryCopy(SuperPacker->buffer, (char *)_Packet, SegmentFullSize, SegmentFullSize + SuperPacker->EndPacketSize);
	strnull(Packet->buffer, (char*)SuperPacker, SizeSuperPacket);
	send(this->sock, (char*)Packet, GetMaxSizePacket(), 0);

	delete Packet;
	delete SuperPacker;

	return true;
}

OriServerClient &  OriServerClient::operator<< (OriLng& Lng)
{
	
	if (Lng.GetSize() > SIZE_BUFFER_LNG)
		SendSuperPacket(Local_Type_Packet, TYPE_LANGUAGE_PROTOCOL, Lng.GetBuffer(), Lng.MaxSizeBuffer, rand());
	else 
	{
		KernelPacket *Packet    = new KernelPacket;
		OriLngPacket *LngPacket = new OriLngPacket;

		Packet->SetTypeProtocol(TYPE_LANGUAGE_PROTOCOL);
		Packet->SetSalt(KERNEL_SALT);

		LngPacket->SetNamePacket(Local_Type_Packet);
		LngPacket->SetBuffer(Lng.GetBuffer());

		int32_t SizePacket = SIZE_STRUCT_LNG + Lng.GetSize();
		strnull(Packet->buffer, (char*)LngPacket, SizePacket);
		SizePacket += SIZE_STRUCT_KERNEL;

		OriSendKernel(Packet, SizePacket);

		delete LngPacket;

	/*	KernelPacket *Packet = new KernelPacket;
		Packet->type_packet_protocol = TYPE_LANGUAGE_PROTOCOL;
		strcpy(Packet->salt_packet, KERNEL_SALT);
		OriLngPacket *LngPacket = new OriLngPacket;
		strcpy(LngPacket->NamePacket, Local_Type_Packet);
		strcpy(LngPacket->buffer, Lng.Buffer);
		strnull(Packet->buffer, (char*)LngPacket, sizeof(OriLngPacket));
		OriSendKernel(Packet);
		delete LngPacket;*/
	}
	
	return *this;
}

OriServerClient&  OriServerClient::operator = (const char *TypePacket)
{
	strcpy(this->Local_Type_Packet, TypePacket);
	return *this;
}

bool OriServerClient::operator=(OriServerClient * ServerClient)
{
	this->sock = ServerClient->sock;
	this->AddrClient = ServerClient->AddrClient;
	return true;
}

bool OriServerClient::operator==(OriServerClient& ServerClient)
{
	if (this->TypePacket == ORI_TCP) if (ServerClient.sock == this->sock) return true;
	return false;
}

bool  OriServerClient::OriSendKernel(KernelPacket * p, uint32_t Size) {

	if (isUseCrypt)
		Crypt.XORCrypt((char*)p);

	if (this->TypePacket == ORI_UDP) sendto(this->sock, (char*)p, Size, 0, (struct sockaddr *)&AddrClient, SizeClientAddr);
	else 
	{
		send(this->sock, (char*)p, Size, 0);
	}
	delete p;
	return true;
}

bool  OriServerClient::OriSendKernel(KernelPacket *p)
{
	if (isUseCrypt)
		Crypt.XORCrypt((char*)p);

	if (this->TypePacket == ORI_UDP) sendto(this->sock, (char*)p, GetMaxSizePacket(), 0, (struct sockaddr *)&AddrClient, SizeClientAddr);
	else {
		send(this->sock, (char*)p, GetMaxSizePacket(), 0);
	}
	delete p;
	return true;
}

#pragma warning(disable : 4018)

void  OriServerClient::strnull(char * p, char *b, unsigned sz)
{
	for (int i = 0; i < sz; i++)
		p[i] = b[i];
}

void OriServerClient::MemoryCopy(char *Buffer, char* Obj, unsigned long long StartReadPosition, unsigned long long EndReadPosition)
{
	unsigned long long w = 0;
	for (unsigned long long i = StartReadPosition; i < EndReadPosition; i++)
	{
		Buffer[w] = Obj[i];
		w++;
	}
}


// я тут был 27.02.2015, привет из прошлого :D ѕривееет, перечитывал это спуст€ год, 23.04.2016!
// 01.03.2015, переписываю алгоритм, так как прошлый не устраивает
void OriServerClient::SendFile(char *FileName)
{
	OriFileSend *FileSend = new OriFileSend;

	// ѕолучим размер стека TCP
	FileSend->MaxBuffer = SIZE_BUFFER_FILE;
	// ѕолучаем размер файла
	FileSend->SizeFile = SizeFile(FileName);

	// Ќайдем сколько сегметов относительно максимального размера стека TCP 64k
	FileSend->MaxSegment = FileSend->SizeFile / FileSend->MaxBuffer;

	// ѕолучим новый размер, так как операци€ делени€ сокращает размер
	int TestSize = FileSend->MaxBuffer * FileSend->MaxSegment;

	int ResultSize = 0;

	// ≈сли сокращенный размер больше стандартного, то получим на сколько
	if (TestSize > FileSend->SizeFile)	
		ResultSize = TestSize - FileSend->SizeFile;	
    else 
		ResultSize = FileSend->SizeFile - TestSize;
    
	strcpy(FileSend->NameFile, FileName);

	FileSend->NumberSegment = 0;
	FileSend->SizeEndByte = ResultSize;

	KernelPacket *Packet = new KernelPacket;
	Packet->type_packet_protocol = TYPE_FILE_PROTOCOL_SEND;
	strcpy(Packet->salt_packet, KERNEL_SALT);
		
	FILE *f;

	 f = fopen(FileName, "rb");
	 
	 memset(FileSend->BufferFile, '\0', FileSend->MaxBuffer);
	 memset(Packet->buffer, '\0', SIZE_BUFFER);

	 if (this->TypePacket == ORI_UDP)
	 while (fread(&FileSend->BufferFile, 1, FileSend->MaxBuffer, f))  {
		 FileSend->NumberSegment++;
		 strnull(Packet->buffer, (char*)FileSend, sizeof(OriFileSend));
    	 sendto(this->sock, (char*)Packet, sizeof(KernelPacket), 0, (struct sockaddr *)&AddrClient, SizeClientAddr);
	 } 

	 if (this->TypePacket == ORI_TCP)
		 while (fread(&FileSend->BufferFile, 1, FileSend->MaxBuffer, f))  {
			 FileSend->NumberSegment++;
			 strnull(Packet->buffer, (char*)FileSend, sizeof(OriFileSend));
			 send(this->sock, (char*)Packet, sizeof(KernelPacket), 0);
		 }
	
	delete Packet;
	delete FileSend;

	fclose(f);

}

void OriServerClient::DownloadFile(char *NameFile)
{
	KernelPacket *Packet = new KernelPacket;
	Packet->type_packet_protocol = TYPE_FILE_PROTOCOL_READ;
	strcpy(Packet->buffer, NameFile);
	OriSendKernel(Packet);
}

OriServerClient &  OriServerClient::operator<<(OriCrypt &_Crypt)
{
	Crypt = _Crypt;
	isUseCrypt = true;
	return *this;
}
