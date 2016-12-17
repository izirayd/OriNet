#include "OriClient.h"
#include "../OriUntil/OriThread.h"

#define EndConnectionSocketAndThread(NumberSocket) 	KernelCloseSocket(NumberSocket); 	KernelCloseThread;

OriClient::OriClient()
{

	SetArch(Thread);

	isUseEndConn = false;
	isUseNewConn = false;

	b_Type_Packet   = false;
	m_KeySession[0] = '\0';
	strcpy(this->m_KeySession, KEY_SESSION);
	isSha256        = false;
	isSystemSession = true;
	isUseCrypt      = false;
	TypePacket      = ORI_TCP;
	TSSPBuffer.CreateList(16);
	SetConnect(false);

};

OriClient::OriClient(int SizeTB) {

	SetArch(Thread);

	this->TBFunction.ReCreateTB(SizeTB);
	this->TBFiles.ReCreateTB(SizeTB);

	isUseEndConn = false;
	isUseNewConn = false;


	b_Type_Packet   = false;
	m_KeySession[0] = '\0';
	strcpy(this->m_KeySession, KEY_SESSION);
	isSha256        = false;
	isSystemSession = true;
	isUseCrypt      = false;
	TSSPBuffer.CreateList(16);
	SetConnect(false);
}

#pragma warning(disable : 4018)

void OriClient::strnull(char * p, char *b, unsigned sz)
{
	for (int i = 0; i < sz; i++)
		p[i] = b[i];
}

#include <stdio.h>
#include <iostream>

void strnull(char * p, char *b, unsigned sz)
{
	for (int i = 0; i < sz; i++)
		p[i] = b[i];
}

/*
void SendFile(char *FileName, int sock)
{
	OriFileSend *FileSend = new OriFileSend;

	// Получим размер стека TCP
	FileSend->MaxBuffer = SIZE_BUFFER_FILE;
	// Получаем размер файла
	FileSend->SizeFile = SizeFile(FileName);

	// Найдем сколько сегметов относительно максимального размера стека TCP 64k
	FileSend->MaxSegment = FileSend->SizeFile / FileSend->MaxBuffer;

	// Получим новый размер, так как операция деления сокращает размер
	int TestSize = FileSend->MaxBuffer * FileSend->MaxSegment;

	int ResultSize = 0;

	// Если сокращенный размер больше стандартного, то получим на сколько
	if (TestSize > FileSend->SizeFile)
		ResultSize = TestSize - FileSend->SizeFile;
	else
		ResultSize = FileSend->SizeFile - TestSize;

	strcpy(FileSend->NameFile, FileName);

	FileSend->NumberSegment = 0;
	FileSend->SizeEndByte   = ResultSize;

	KernelPacket *Packet = new KernelPacket;
	Packet->type_packet_protocol = TYPE_FILE_PROTOCOL_SEND;
	strcpy(Packet->salt_packet, KERNEL_SALT);

	FILE *f;

	f = fopen(FileName, "rb");

	memset(FileSend->BufferFile, '\0', FileSend->MaxBuffer);
	memset(Packet->buffer, '\0', SIZE_BUFFER);

	while (fread(&FileSend->BufferFile, 1, FileSend->MaxBuffer, f))  	{
		FileSend->NumberSegment++;
		strnull(Packet->buffer, (char*)FileSend, sizeof(OriFileSend));
		send(sock, (char*)Packet, sizeof(KernelPacket), 0);
	}


	delete[] Packet;
	delete[] FileSend;

	fclose(f);

}*/

void OriClient::RunPacket(char *buf, int Sock, OriServerClient &Client)
{

	KernelPacket *PacketKernel = (KernelPacket *)buf;

	if (PacketKernel->type_packet_protocol == TYPE_STRUCT_PROTOCOL)
	{
		oriPacket *Packet = (oriPacket *)PacketKernel->buffer;
		if (strcmp(Packet->salt, SALT) != 0) return;
		TBFunction.RunFunction(Packet->typecomand, Sock, Packet->buffer);
	}

	if (PacketKernel->type_packet_protocol == TYPE_LANGUAGE_PROTOCOL)	{

		OriLngPacket *Packet = (OriLngPacket *)PacketKernel->buffer;
		OriLng LngBuffer(SIZE_BUFFER_LNG);
		LngBuffer << Packet->buffer;
		TBFunction.RunFunctionLng(Packet->NamePacket, Client, LngBuffer);

	}

	if (PacketKernel->type_packet_protocol == TYPE_FILE_PROTOCOL_SEND)
	{
		OriFileSend *FileSend = (OriFileSend *)PacketKernel->buffer;

		// Это защита, что бы сервер не передавал нам файлы,
		// Которые мы не разрешили ему нам передать.
		if (TBFiles.CheckFile(FileSend->NameFile))
		{
			FILE *f;
			f = fopen(FileSend->NameFile, "ab+");
			if (FileSend->NumberSegment == FileSend->MaxSegment) {
				fwrite(&FileSend->BufferFile, 1, FileSend->SizeEndByte, f);
			}
			else fwrite(&FileSend->BufferFile, 1, FileSend->MaxBuffer, f);
			fclose(f);
		}
	}

	if (PacketKernel->type_packet_protocol == TYPE_FILE_PROTOCOL_READ)	{
		Client.SendFile(PacketKernel->buffer);
	}

}

void OriClient::Packet(char *buf, int Sock, OriServerClient &Client, int32_t SizePacket)
{
	KernelPacket *PacketKernel = (KernelPacket *)buf;

	if (PacketKernel->type_packet_protocol == TYPE_SEND_SUPER_PACKETS)
	{
		OriSuperPacket *SuperPacker = (OriSuperPacket *)PacketKernel->buffer;

		TSSPBuffer.CreateBuffer(SuperPacker->IndexTSSP, SuperPacker->FullSize);

		if (SuperPacker->numberPacket == SuperPacker->countPacket)
			TSSPBuffer.AddBlockData(SuperPacker->IndexTSSP, SuperPacker->buffer, 0, SuperPacker->EndPacketSize);
		else
		    TSSPBuffer.AddBlockData(SuperPacker->IndexTSSP, SuperPacker->buffer, 0, SIZE_BUFFER_TSSP);
	
		if (TSSPBuffer.EndBuffer(SuperPacker->IndexTSSP))
		{
			if (GetTypeArch() == Thread)
			{
				if (SuperPacker->type_packet_protocol == TYPE_LANGUAGE_PROTOCOL) 	{
					OriLng LngBuffer(SuperPacker->FullSize);
					LngBuffer << TSSPBuffer.GetBuffer(SuperPacker->IndexTSSP);
					TSSPBuffer.DeleteBuffer(SuperPacker->IndexTSSP);
					TBFunction.RunFunctionLng(SuperPacker->NamePacket, Client, LngBuffer);
				}
			}

			if (GetTypeArch() == RecvThreadRunAsync)
			{
				KernelDynamicPacket DynamicPacket;
				DynamicPacket.buffer = TSSPBuffer.GetBuffer(SuperPacker->IndexTSSP);
				DynamicPacket.SetTypeProtocol(TYPE_RUN_ASYNC_SUPER_PACKET);
				DynamicPacket.SetSalt(PacketKernel->salt_packet);

				DynamicPacket.FullSize = SuperPacker->FullSize;

				strcpy(DynamicPacket.NamePacket, SuperPacker->NamePacket);

				AsyncBuffer.ServerClient = Client;
				AsyncBuffer.AddPacket((char *)&DynamicPacket, TSSPBuffer.GetSizeBuffer(SuperPacker->IndexTSSP) + KERNEL_SALT_SIZE_MAX + 1 + 32 + 8);
				TSSPBuffer.DeleteBuffer(SuperPacker->IndexTSSP);
			}


		}

		return;
	}

	// Если потоковая архитектура обработки пакетов
	if (GetTypeArch() == Thread)
		RunPacket(buf, Sock, Client);

	if (GetTypeArch() == RecvThreadRunAsync)
	{
		if ((PacketKernel->type_packet_protocol == TYPE_STRUCT_PROTOCOL) ||
			(PacketKernel->type_packet_protocol == TYPE_LANGUAGE_PROTOCOL))
		{
			AsyncBuffer.ServerClient = Client;
			AsyncBuffer.AddPacket(buf, SizePacket);
		}
	}
}

void OriClient::EndConnectionClient(int Client)
{
   KernelCloseSocket(Client);
}


void OriClient::NoConnectionClient(int Sock, OriServerClient &Client)
{

   if (this->isUseEndConn)
		((*this->FuncEndConn)(Client, _Null));

  EndConnectionSocketAndThread(Sock);
}


void OriClient::TimeSessionClient(int Sock, int Ping)
{
	timeval optval;
	optval.tv_sec = 0;
	optval.tv_usec = Ping * 1000;
	if (::setsockopt(Sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval)) < 0)
		EndConnectionClient(Sock);
}


void OriClient::StandartPacketClient(int Sock, sockaddr_in AddrClient, int Len)
{
	int RusultPacket = 0;
	OriServerClient Client;

	Client.SetAddr(AddrClient);
	Client.SetSocket(Sock);


	if (this->isUseNewConn)
		((*this->FuncNewConn)(Client, _Null));

	const int size_Kernel_Packet = sizeof(KernelPacket);
	char *buf = new char[size_Kernel_Packet];

	buf[0] = '\0';

	

	while (true)
	{
		RusultPacket = recv(Sock, buf, size_Kernel_Packet, 0);

		if (RusultPacket <= 0)
		{
			delete[] buf;
			NoConnectionClient(Sock, Client);
		}


		Packet(buf, Sock, Client, RusultPacket);
	}

}

bool OriClient::MethodXOR(int Sock, OriCrypt &Crypt, sockaddr_in AddrClient, int Len)
{
	int RusultPacket = 0;
	OriServerClient Client;

	Client.SetAddr(AddrClient);
	Client.SetSocket(Sock);

	const int size_Kernel_Packet = sizeof(KernelPacket);
	char *buf = new char[size_Kernel_Packet];
	buf[0] = '\0';
	
	while (true)
	{
		RusultPacket = recv(Sock, buf, size_Kernel_Packet, 0);

		if (RusultPacket <= 0)
		{
			delete[] buf;
			Crypt.DeleteDynamicBuffer();
			NoConnectionClient(Sock, Client);
		}

		Crypt.XORCrypt(buf);

		Packet(buf, Sock, Client, RusultPacket);
	}

}


OriClient & OriClient::operator >> (FuncEndConnection Func) {
	isUseEndConn = true;
	this->FuncEndConn = Func;
	return *this;
}

OriClient & OriClient::operator >> (FuncNewConnection Func) {
	isUseNewConn = true;
	this->FuncNewConn = Func;
	return *this;
}


OriClient&  OriClient::operator = (const char *TypePacket)
{
	 b_Type_Packet = true;
	 strcpy(Local_Type_Packet, TypePacket);

 return *this;
}

OriClient&  OriClient::operator << (const char *msg)
{
	 oriPacket *p_OriPacket = new oriPacket;

	 strcpy(p_OriPacket->salt,  SALT);
	 strcpy(p_OriPacket->buffer, msg);

	 if (b_Type_Packet)
	 strcpy(p_OriPacket->typecomand, Local_Type_Packet);

	KernelPacket *Packet = new KernelPacket;
	strcpy(Packet->salt_packet, KERNEL_SALT);

	Packet->type_packet_protocol = TYPE_STRUCT_PROTOCOL;
	strnull(Packet->buffer, (char*) p_OriPacket, sizeof(oriPacket));

	OriSendKernel(Packet);
  b_Type_Packet = false;

  delete p_OriPacket;

 return *this;
}

OriClient&  OriClient::operator << (const oriPacket *p_OriPacket)
{

 return *this;
}

OriClient&  OriClient::operator << (oriPacket *p_OriPacket)
{
	KernelPacket *Packet = new KernelPacket;
	strcpy(Packet->salt_packet, KERNEL_SALT);
	strcpy(p_OriPacket->salt,   SALT);

	  if (b_Type_Packet)
		 strcpy(p_OriPacket->typecomand, Local_Type_Packet);

	Packet->type_packet_protocol = TYPE_STRUCT_PROTOCOL;
	strnull(Packet->buffer, (char*) p_OriPacket, sizeof(oriPacket));

	OriSendKernel(Packet);
  b_Type_Packet = false;

 return *this;
}

void OriClient::strSIZE(char *buffer, char *out, int start, int count)
{
   int j = 0;
   for (int i = start; i < count; i++)   {
	  buffer[j] = out[i];
	  j++;
   }
}

#pragma warning(disable : 4267)

OriClient&  OriClient::operator << (OriLng &Lng)
{

	KernelPacket *Packet = new KernelPacket;
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

		return *this;
}

void OriClient::KeySession(char *Key)
{
	strcpy(m_KeySession, Key);
}

#include "../SuperInt/Cbignum.h"
#include "../SuperInt/Cbignums.h"
#include "../SuperInt/Random3.h"

bool OriClient::ConnectionTcp(const char *IP, int Port) {

	TypePacket = ORI_TCP;

	ORI_WSAINIT;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0)
		return false;

	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(Port);
	addr.sin_addr.s_addr = inet_addr(IP);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		return false;

	port = Port;
	strcpy(ip, IP);
	// Посчитаем уникальный номер пк
	IndexPC.Init();

	if (isSystemSession)	{
		char ShaBuf[SIZE_SHA256] = { 0 }, ShaBuf2[SIZE_SHA256] = { 0 };
		SHA256Crypt(this->m_KeySession, ShaBuf);
		SHA256Crypt(ShaBuf, ShaBuf2);
		send(sock, (char *)ShaBuf2, SIZE_SHA256, 0);
	}

	if (isUseCrypt)
	{
		XORData *Data = new XORData;
		cBigNumber PublicINT_G, PublicINT_P, PublicKeyA, PublicKeyB, PrivateKEY_A, SecretKey_A;

		// Размер чисел, в битах
#define BIT_CRYPT 32

		// Создаём публичные ключи
		PublicINT_G = cBigRandom(lRand3, BIT_CRYPT);
		PublicINT_P = cBigRandom(lRand3, BIT_CRYPT);

		// Создадим приватный ключ
		PrivateKEY_A = cBigRandom(lRand3, BIT_CRYPT);

		// Создадим наш публичный ключ на основе приватного
		PublicKeyA.setpowmod(PublicINT_G, PrivateKEY_A, PublicINT_P);

		// отправим наши данные серверу.
		cBigString StrBuffer;

		PublicINT_G.toa(StrBuffer);
		strcpy(Data->PublicINT_G, StrBuffer.base());
		PublicINT_P.toa(StrBuffer);
		strcpy(Data->PublicINT_P, StrBuffer.base());
		PublicKeyA.toa(StrBuffer);
		strcpy(Data->PublicKey, StrBuffer.base());

		send(sock, (char *)Data, sizeof(XORData), 0);

		int  RusultPacket = 0;
		timeval optval;
		optval.tv_sec = 0;
		optval.tv_usec = 95 * 1000;

		// Не знаю почему не работает =/ в итоге виснит весь поток
		if (::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval)) < 0)
			return false;

		RusultPacket = recv(sock, (char *)Data, sizeof(XORData), 0);

		if (RusultPacket <= 0)
		{
			//EndConnectionClient(sock);
			delete Data;
			return false;
		}
		else
		{
			PublicKeyB = cBigNumber(Data->PublicKey, 0);
			SecretKey_A.setpowmod(PublicKeyB, PrivateKEY_A, PublicINT_P);
			delete Data;


			SecretKey_A.toa(StrBuffer);
			char Sha256KeyInteger[65] = { 0 };
			SHA256Crypt(StrBuffer.base(), Sha256KeyInteger); // Это будет хешем секретного ключа

			// Сделаем двойной ключ хеша сессии
			char Sha256KeySession[65] = { 0 };


			char Buffer[65 * 2] = { 0 };
			SHA256Crypt(this->m_KeySession, Sha256KeySession);
			strcpy(Sha256KeySession, Buffer);
			SHA256Crypt(Buffer, Sha256KeySession);  // Двойной хешь сессии

			char Sha256KeyXOR[65] = { 0 }; // Ключ шифрования пакетов

			sprintf(Buffer, "%s%s", Sha256KeyInteger, Sha256KeySession);
			SHA256Crypt(Buffer, Sha256KeyXOR);

			Crypt << Sha256KeyXOR; // Шифруем сообщения

			isUseCrypt = true;
		}
	}

	KernelInitArch(sock, addr, (sizeof(addr)));
	InitMSS();
	return true;
}

bool OriClient::ConnectionServer(const char *IP, int Port)
{
	if (ConnectionTcp(IP, Port)) {
		SetConnect(true);
		return true;
	} 
	

	SetConnect(false);
	return false;
}


bool OriClient::ConnectionServer(int Port) {

	if (ConnectionServer("127.0.0.1", Port)) return true;
	return false;
}

bool OriClient::ConnectionServer(const char *IPServer, int PortServer, const char *IPClient, int PortClient) {
	
	// Выполним подключение по протоколу UDP
	if (ConnectionUdp(IPServer, PortServer, IPClient, PortClient)) return true;

	return false;
}



bool OriClient::ConnectionUdp(const char *IPServer, int PortServer, const char *IPClient, int PortClient) {

	TypePacket = ORI_UDP;

	ORI_WSAINIT;

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0)
		return false;

	// 
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(PortClient);
	addr.sin_addr.s_addr = inet_addr(IPClient);

	//
	AddrServer.sin_family      = AF_INET;
	AddrServer.sin_port        = htons(PortServer);
	AddrServer.sin_addr.s_addr = inet_addr(IPServer);

	AddrLen = sizeof(AddrServer);

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		return false;
	}

	KernelInitArch(sock, addr, (sizeof(addr)));
	return true;
}

void OriClient::KernelInitArch(int Id, sockaddr_in AddrClient, int Len) {

	if (GetTypeArch() == Thread)
	{
		UseThread(sock, addr, (sizeof(addr)));
	}

	if (GetTypeArch() == RecvThreadRunAsync)	{

		AsyncBuffer.Socket = GetSocketClient();
		AsyncBuffer.CreateListPacket(512);
		AsyncBuffer.TBFunction = &TBFunction;
		UseThread(sock, addr, (sizeof(addr)));
	}


}

void OriClient::UdpPacketServer() {

	int SizePack = sizeof(KernelPacket);
	char *buf = new char[SizePack];
	memset((void *)buf, 0x00, SizePack);

	OriServerClient Server; // Сервера
	OriServerClient Client; // Мой

	Server.SetAddr(AddrServer);
	Client.SetAddr(addr);

	int RusultPacket = 0;
	while (true)  	{

		/* Принимаем не криптованные пакеты */
#if  defined (__BORLANDC__) || defined(__CODEGEARC__) || defined(__TURBOC__) || defined(__CODEGEARC_VERSION__)
		RusultPacket = recvfrom(this->sock, (char *)buf, SizePack, 0, Client.GetAddr(), (int *)Client.GetSizeAddr());
#endif

#if defined(__GNUC__) || defined(__GNUG__)
		RusultPacket = recvfrom(this->sock, (char *)buf, SizePack, 0, Client.GetAddr(), (socklen_t *)Client.GetSizeAddr());
#endif

#if defined(_MSC_VER)
		RusultPacket = recvfrom(this->sock, (char *)buf, SizePack, 0, (struct sockaddr *)&Client.AddrClient, &Client.SizeClientAddr);
#endif


		if (RusultPacket > 0) 	{

			Server.SetAddr(AddrServer);
			Server.SetSocket(sock);
			Server.SetTypeProtocol(ORI_UDP);

			Packet(buf, sock, Server, RusultPacket);
		}
	}

	delete[] buf;
}

#include "../OriUntil/OriSleep.h"


int32_t  OriClient::GetSockOpt(int32_t FlagLevel, int32_t FlagOption)
{
	int32_t Values;
	return GetSockOpt(FlagLevel, FlagOption, Values);
}


int32_t  OriClient::GetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t &Values) {

#ifdef ORI_WINDOWS
	if (getsockopt(GetSocketClient(), FlagLevel, FlagOption, (char *)&Values, (int *) sizeof(int32_t)) == ORI_ERROR)
		return ORI_ERROR;
#endif
#ifdef ORI_LINUX
	if (getsockopt(GetSocketClient(), FlagLevel, FlagOption, &Values, (socklen_t *) sizeof(int32_t)) == ORI_ERROR)
		return ORI_ERROR;
#endif

	return Values;
}

int32_t  OriClient::SetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t Values)    {

#ifdef ORI_WINDOWS
	if (setsockopt(GetSocketClient(), FlagLevel, FlagOption, (char*)&Values, sizeof(int32_t)) == ORI_ERROR)
		return ORI_ERROR;
#endif

#ifdef ORI_LINUX
	if (setsockopt(GetSocketClient(), FlagLevel, FlagOption, &Values, sizeof(int32_t)) == ORI_ERROR)
		return ORI_ERROR;
#endif

	return Values;
}

void OriClient::UseThread(int Id, sockaddr_in AddrClient, int Len) {

	CreateThread();


	/*m_Mutex.lock();

	if (!UseRun)
	{
		UseRun = true; // Поток занят
		m_Mutex.unlock();

		if (CreateThread()) return; else 
		{
			OriSleep(1);
			UseRun = false; 
			UseThread(Id, AddrClient, Len);
		}
	}
	else
	{
		m_Mutex.unlock();
		UseThread(Id, AddrClient, Len);
	}*/

}

bool OriClient::CreateThread() {

#ifdef _WIN32
	if (_beginthread(OriClient::RunThread, 4100, this) == ORI_ERROR)
		return false;
#else
	pthread_t th;
	if (pthread_create(&th, NULL, OriClient::RunThread, this) != 0)
		return false;
#endif
	return true;
}

#ifdef _WIN32

void OriClient::RunThread(void* p) {
	OriClient* _this = (OriClient*)p;
	_this->Run();
	_endthread();
}
#else

void* OriClient::RunThread(void* p) {

	OriClient* _this = (OriClient*)p;
	_this->Run();
	pthread_exit(NULL);
	return NULL;
}
#endif


void OriClient::Run()  {
	//
	m_Mutex.lock();

	 UseRun = false; // Поток свободен

	m_Mutex.unlock();

	if (this->TypePacket == ORI_TCP)
	{
		if (isUseCrypt) {
			MethodXOR(this->sock, this->Crypt, this->addr, sizeof(addr));
		}

		StandartPacketClient(this->sock, this->addr, sizeof(addr));
	}
	 
	if (this->TypePacket == ORI_UDP)
		this->UdpPacketServer();

	EndConnectionClient(sock);
}


bool OriClient::NewThreadServer(void* Param) {

    return true;
}

void OriClient::CloseConnection()
{
  KernelCloseSocket(this->sock);
}

bool OriClient::oriSend(oriPacket *p)
{
	 strcpy(p->salt, SALT);

	  if (this->TypePacket == ORI_UDP)
		  sendto(this->sock, (char*)p, sizeof(oriPacket), 0, (struct sockaddr *)&AddrServer, AddrLen);
	  else send(this->sock, (char*)p, sizeof(oriPacket), 0);

	  delete [] p;
	return true;
}

bool  OriClient::OriSendKernel(KernelPacket * p, uint32_t Size) {

	if (IndexPC.GetStatus()) strcpy(p->indexPC, IndexPC.GetHashPC());

	if (isUseCrypt)
		Crypt.XORCrypt((char*)p);

	if (this->TypePacket == ORI_UDP) sendto(this->sock, (char*)p, Size, 0, (struct sockaddr *)&AddrServer, AddrLen);
	else send(this->sock, (char*)p, Size, 0);
	delete p;
	return true;
}

bool OriClient::OriSendKernel(KernelPacket *p)
{
	if (IndexPC.GetStatus()) strcpy(p->indexPC, IndexPC.GetHashPC());

	if (isUseCrypt)
		Crypt.XORCrypt((char*)p);

	  if (this->TypePacket == ORI_UDP) 
		   sendto(this->sock, (char*)p, sizeof(KernelPacket), 0, (struct sockaddr *)&AddrServer, AddrLen);
	  else send(this->sock, (char*)p, sizeof(KernelPacket), 0);
	  delete p;
	return true;
}

OriClient& OriClient::operator >> (FunctionLngClient Func)
{
	TBFunction.AddFunction(Local_Cmd_Char_Buffer, Func);
	return *this;
}

OriClient& OriClient::operator >> (const char *Name_Packet)
{
	strcpy(this->Local_Cmd_Char_Buffer, Name_Packet);
	return *this;
}

OriClient& OriClient::operator >> (FunctionSockBuffer Func)
{
	TBFunction.AddFunction(Local_Cmd_Char_Buffer, Func);
	return *this;
}

OriClient& OriClient::operator >> (FunctionBuffer Func)
{
	TBFunction.AddFunction(Local_Cmd_Char_Buffer, Func);
	return *this;
}

OriClient& OriClient::operator >> (FunctionLng Func)
{
	TBFunction.AddFunction(Local_Cmd_Char_Buffer, Func);
	return *this;
}

OriClient& OriClient::operator >> (FunctionNull Func)
{
	TBFunction.AddFunction(Local_Cmd_Char_Buffer, Func);
	return *this;
}

void OriClient::ReadFile(char *NameFile)
{
	// Добавим файл в список разрешаемых для загрузки
	TBFiles.AddFile(NameFile);
	KernelPacket *Packet = new KernelPacket;
	Packet->type_packet_protocol = TYPE_FILE_PROTOCOL_READ;
	strcpy(Packet->buffer, NameFile);
	OriSendKernel(Packet);
}

void OriClient::UseCrypt()
{
	this->isUseCrypt = true;
}

void OriClient::OffSystemSession()
{
	isSystemSession = false;
}
