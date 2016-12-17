#include "OriServer.h"

#include "../OriKernel/KernelPlatform.h"

#include "../OriCrypt/OriCrypt.h"
#include "../OriUntil/OriThread.h"
#include "../OriUntil/OriSleep.h"

#include "../SuperInt/Cbignum.h"
#include "../SuperInt/Cbignums.h"
#include "../SuperInt/Random3.h"

#include "../OriUntil/Untils.h"

#ifdef ORI_USE_DEBUG
 #define ORI_DEBUG(obj) // std::cout << obj;
#else 
 #define ORI_DEBUG(obj)
#endif

#define EndConnectionSocketAndThread(NumberSocket) \
	                                               \
    KernelCloseSocket(NumberSocket);               \
	CountClient--;                                 \
    KernelCloseThread;                                  


DefinedEndType EndConnectionDefined;
NewConnection _Null;

OriServer::OriServer(int SizeTB)
{
	SetMaxCountFunction(SizeTB);
	InitArchServer();
}

void OriServer::InitArchServer()
{
	isUseNewConn = false;
	isUseEndConn = false;

	SetKeySession(KEY_SESSION);
	SetSystemSession(true);
	SetTimeSession(95);
	SetUseCrypt(false);
	SetUseSha256(false);
	SetMaxSizePacket(sizeof(KernelPacket));
	SetCountListen(5);
	SetBitSizeKey(32);
	EmulatePingDisable();
}

void OriServer::EndConnection(OriSocket Client, char *CommentError)
{
	ORI_DEBUG("\n");
	ORI_DEBUG("Socket: ");
	ORI_DEBUG(Client);
	ORI_DEBUG("\nError:\n ");
	ORI_DEBUG(CommentError);
	EndConnectionSocketAndThread(Client);
}

void OriServer::NoConnection(OriSocket Sock, OriServerClient &Client)
{

	m_MutexArray.EndConnection.lock();

	 if (this->isUseEndConn)
		((*this->FuncEndConn)(Client, _Null));


	m_MutexArray.EndConnection.unlock();


	EndConnectionSocketAndThread(Sock);
}

void OriServer::RunFunctionNewClient(OriServerClient &Client)
{

	m_MutexArray.NewConnection.lock();

	if (this->isUseNewConn)
		((*this->FuncNewConn)(Client, _Null));

	m_MutexArray.NewConnection.unlock();

} 

bool OriServer::InitSocketServer(const char *ip, int port) {

	ORI_WSAINIT;

	CreateAddr(AF_INET, ip, port);
	SetSocketServer(socket(AF_INET, GetTypeServer(), GetIPProto()));

#if defined ORI_WINDOWS
	if (GetSocketServer() == INVALID_SOCKET)
		return false;
#endif

	if (bind(GetSocketServer(), (sockaddr*)&this->server, sizeof(this->server)) != 0)  return false;
												  

	SetFromLen(sizeof(from));

	this->ServerStart = true;
	CountClient = 0;

	char ShaBuf[SIZE_SHA256] = { 0 };

	SHA256Crypt(this->m_KeySession, ShaBuf);
	SHA256Crypt(ShaBuf, KeySessionSha256);
	return true;
}

bool OriServer::InitListenServer() {
	if (listen(GetSocketServer(), GetCountListen()) != 0) return false;
	return true;
}

bool OriServer::CreateTcpServer(const char *ip, int port) {

	SetTypeServer(ORI_TCP);
	if (!InitSocketServer(ip, port)) return false;
	if (!InitListenServer()) return false;

	return true;
}

bool  OriServer::CreateUdpServer(const char *ip, int port) {

	SetTypeServer(ORI_UDP);
	if (!InitSocketServer(ip, port)) return false;

	return true;
}

bool OriServer::CreateServer(int port) {

	if (CreateTcpServer("127.0.0.1", port))
		  return true;
	return false;

}

bool OriServer::CreateServer(const char *ip, int port, int TypeServer = ORI_TCP) {

	if (TypeServer == ORI_TCP)
		 if (CreateTcpServer(ip, port)) 
			 return true;

	if (TypeServer == ORI_UDP)
		 if (CreateUdpServer(ip, port))
			 return true;

	return false;
}


bool OriServer::CreateServer(int port, int TypeServer) {

	if (TypeServer == ORI_TCP)
		 if (CreateTcpServer("127.0.0.1", port))
			 return true;

	if (TypeServer == ORI_UDP) 
		 if (CreateUdpServer("127.0.0.1", port))
			 return true;

	return false;

}

void OriServer::RunServer() 
{
   UseRun = false;

   if (GetTypeServer() == ORI_TCP)
   while (true)
   {		

    ORI_DEBUG("\nStart accept");

	client = accept(sock, (struct sockaddr*)&from, &fromlen);

	UseThread(client, from, fromlen);

	if (!ServerStart)  break;
   }

   if (GetTypeServer() == ORI_UDP)
	   UdpPacketServer();
}


void OriServer::UseThread(int Id, sockaddr_in AddrClient, int Len) 
{

	ORI_DEBUG("\nUseThread");

	m_Mutex.lock();

		 UseSocketClient       = Id;
		 fromClientThread      = AddrClient;
		 fromlenClientThread   = Len;

	if (CreateThread())
			 m_Mutex.unlock();
		
}

void OriServer::StopServer() {
  this->ServerStart = false;
  KernelCloseSocket(sock);
}

ORI_THREAD_FOR_CLASS(OriServer)
void OriServer::Run()  
{
	
	ORI_DEBUG("\nRun");

	 int         SocketClient = this->UseSocketClient;
	 sockaddr_in AddrClient   = this->fromClientThread;
	 int         AddrLen      = this->fromlenClientThread;

	m_Mutex.unlock();


		if (isSystemSession)
		{
			ORI_DEBUG("\nisSystemSession");
			MethodSession(SocketClient, AddrClient, AddrLen);
		}

		if (isUseCrypt) 
		{
			ORI_DEBUG("\nisUseCrypt");
			OriCrypt Crypt;
			MethodXOR(SocketClient, Crypt, AddrClient, AddrLen);
		}
		else 
		{ 
			ORI_DEBUG("\nStandartPacketServer");
			StandartPacketServer(SocketClient, AddrClient, AddrLen); 
		}

}

bool OriServer::MethodSession(OriSocket Sock, sockaddr_in AddrClient, int Len)
{
	int  RusultPacket = 0;
	char buf[SIZE_SHA256] = { 0 };

	ORI_DEBUG("\nTimeSession");

	//pingTimeSession(Sock, TimeSession);

	RusultPacket = recv(Sock, (char *)buf, SIZE_SHA256, 0);

	if (RusultPacket <= 0)
		EndConnection(Sock, "MethodSession > Don`t packet with key session");

	else if (strcmp(buf, KeySessionSha256) == 0)	{
		pingTimeSession(Sock, 36000);
		return true;
	}
	else EndConnection(Sock, "MethodSession > Packet in KeySessionClient does not match with key server");

	EndConnection(Sock, "MethodSession > Point end function thread");
	return false; // Требует компилятор, хотя End_Connection просто остановит поток
}

void  OriServer::pingTimeSession(OriSocket Sock, int Ping)
{
	
	/*
	timeval optval;
	optval.tv_sec = 0;
	optval.tv_usec = Ping * 1000;
	if (::setsockopt(Sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval)) < 0)	
		EndConnection(Sock, "pingTimeSession > Session end, because client don`t send session packet and time session have limit");*/
	
}

bool OriServer::MethodXOR(OriSocket Sock, OriCrypt &Crypt, sockaddr_in AddrClient, int Len) {
	XORData *Data = new XORData;

	int RusultPacket = 0;
	pingTimeSession(Sock, TimeSession);
	RusultPacket = recv(Sock, (char *)Data, sizeof (XORData), 0);

	if (RusultPacket <= 0)  {
		EndConnection(Sock, "MethodXOR > Don`t packet");
	}
	else
	{

		cBigNumber PublicINT_G, PublicINT_P, PublicKeyA, PublicKeyB, PrivateKEY_B, SecretKey_B;

		PublicINT_G  = cBigNumber(Data->PublicINT_G, 0);
		PublicINT_P  = cBigNumber(Data->PublicINT_P, 0);
		PublicKeyA   = cBigNumber(Data->PublicKey, 0);

		PrivateKEY_B = cBigRandom(lRand3, GetBitSizeKey());

		PublicKeyB.setpowmod(PublicINT_G, PrivateKEY_B, PublicINT_P);

		cBigString StrBuffer;
		PublicKeyB.toa(StrBuffer);
		strcpy(Data->PublicKey, StrBuffer.base());

		send(Sock, (char *)Data, sizeof (XORData), 0);

		SecretKey_B.setpowmod(PublicKeyA, PrivateKEY_B, PublicINT_P);

		SecretKey_B.toa(StrBuffer);
		char Sha256KeyInteger[SIZE_SHA256] = { 0 };
		SHA256Crypt(StrBuffer.base(), Sha256KeyInteger); // Это будет хешем секретного ключа

		// Сделаем двойной ключ хеша сессии
		char Sha256KeySession[SIZE_SHA256] = { 0 };

		char Buffer[SIZE_SHA256 * 2] = { 0 };
		
		SHA256Crypt(m_KeySession, Sha256KeySession);
		strcpy(Sha256KeySession, Buffer);
		SHA256Crypt(Buffer, Sha256KeySession);  // Двойной хешь сессии

		char Sha256KeyXOR[SIZE_SHA256] = { 0 }; // Ключ шифрования пакетов

		sprintf(Buffer, "%s%s", Sha256KeyInteger, Sha256KeySession);
		SHA256Crypt(Buffer, Sha256KeyXOR);

		Crypt << Sha256KeyXOR; // Шифруем сообщения

	}


	delete Data;
	
	char *buf = new char[GetMaxSizePacket()];
	memset((void *)buf, '\x0', GetMaxSizePacket());

	OriServerClient Client;
	Client.Create(Sock, AddrClient, ORI_TCP);

	Client << Crypt;

	RunFunctionNewClient(Client);

	RusultPacket = 0;

	while (true)
	{
		/* Принимаем криптованные пакеты */
		RusultPacket = recv(Sock, (char *)buf, GetMaxSizePacket(), 0);

		if (RusultPacket <= 0)
		{
			delete[] buf;
			// Удаляем буфер ключа, иначе будет ошибка памяти
			Crypt.DeleteDynamicBuffer();
			NoConnection(Sock, Client);
		}

		Crypt.XORCrypt(buf);

		Packet(buf, Sock, Client);
	}

	delete[] buf;
}

void OriServer::UdpPacketServer() {

	char *buf = new char[GetMaxSizePacket()];
	memset((void *)buf, '\x0', GetMaxSizePacket());

	OriServerClient Client;
	Client.SetSizeAddr(sizeof(Client.GetAddrIn()));

	int RusultPacket = 0;
	while (true)  	{

		if (!ServerStart)  break;

	// Принимаем не криптованные пакеты
	#if defined(__BORLANDC__) || (__CODEGEARC__) || (__TURBOC__)
	    RusultPacket = recvfrom(this->sock, (char *)buf, GetMaxSizePacket(), 0, Client.GetAddr(), (int *) Client.GetSizeAddr());
	#endif

    #if defined(__GNUC__) || defined(__GNUG__)
		RusultPacket = recvfrom(this->sock, (char *)buf, GetMaxSizePacket(), 0, Client.GetAddr(), (socklen_t *) Client.GetSizeAddr());
    #endif

    #if defined(_MSC_VER)
		RusultPacket = recvfrom(this->sock, (char *)buf, GetMaxSizePacket(), 0, (struct sockaddr *)&Client.AddrClient, &Client.SizeClientAddr);
	#endif

		
		if (RusultPacket > 0) 	{

			Client.InitAddr();
			Client.SetSocket(sock);
			Client.SetTypeProtocol(ORI_UDP);

			Packet(buf, sock, Client);
		}

	}

	delete[] buf;
}



void OriServer::StandartPacketServer(OriSocket Sock, sockaddr_in AddrClient, int Len)
{
	char *buf = new char[GetMaxSizePacket()];
	memset((void *)buf, '\x0', GetMaxSizePacket());

	OriServerClient Client;
	Client.Create(Sock, AddrClient, ORI_TCP);
	
	RunFunctionNewClient(Client);

	int RusultPacket = 0;
	while (true)  	{

		ORI_DEBUG("\nStart recv packet");

		/* Принимаем не криптованные пакеты */
		RusultPacket = recv(Sock, (char *)buf, GetMaxSizePacket(), 0);

		if (RusultPacket <= 0) 	{
			delete[] buf;
			NoConnection(Sock, Client);
		}

		Packet(buf, Sock, Client);
	}

	delete[] buf;
}



void OriServer::Packet(char *buf, OriSocket Sock, OriServerClient &Client)
{
	if (GetEmulatePingStatus())	 
		OriMicroSleep(GetEmulatePing()); 

	KernelPacket *PacketKernel = (KernelPacket *)buf;

	char *IndexPC = Client.GetIndexPC();

	if (IndexPC[0] == 0x00)
	 Client.SetIndexPC(PacketKernel->indexPC);
	

	if (PacketKernel->type_packet_protocol == TYPE_STRUCT_PROTOCOL)	 {

		oriPacket *Packet = (oriPacket *)PacketKernel->buffer;
		if (strcmp(Packet->salt, SALT) != 0)
			TBFuction.RunFunction(Packet->typecomand, Sock, Packet->buffer);
	}

	if (PacketKernel->type_packet_protocol == TYPE_LANGUAGE_PROTOCOL)	 {
		OriLngPacket *Packet = (OriLngPacket *)PacketKernel->buffer;
		
		OriLng LngBuffer(SIZE_BUFFER_LNG);
		LngBuffer << Packet->buffer;
		TBFuction.RunFunctionLng(Packet->NamePacket, Client, LngBuffer);
	}

	if (PacketKernel->type_packet_protocol == TYPE_FILE_PROTOCOL_READ)
	if (TBFiles.CheckFile(PacketKernel->buffer))
		Client.SendFile(PacketKernel->buffer);

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
}


bool OriServer::SendPacket(OriServerClient Client, char* NameFunction, void *_Packet, int _Size) {

	if ((_Packet != NULL) && (_Size > 0))
		if (Client.SendPacket(NameFunction, _Packet, _Size)) return true;

	return false;
}


OriServer& OriServer::operator << (const char *Name_Packet){
	strcpy(this->NameFunction, Name_Packet);
	return *this;
}

OriServer& OriServer::operator<<  (int SizeLngBuffer){
	this->Local_SizeLngBuffer = SizeLngBuffer;
	return *this;
}


OriServer& OriServer::operator << (FunctionSockBuffer Func) {
	TBFuction.AddFunction(NameFunction, Func);
	return *this;
}

OriServer& OriServer::operator << (FunctionBuffer Func) {
	TBFuction.AddFunction(NameFunction, Func);
	return *this;
}

OriServer& OriServer::operator << (FunctionLng Func) {
	TBFuction.AddFunction(NameFunction, Func);
	return *this;
}

OriServer & OriServer::operator<<(FunctionLngClient Func) {
	TBFuction.AddFunction(NameFunction, Func);
	return *this;
}

OriServer& OriServer::operator << (FunctionNull Func) {
	TBFuction.AddFunction(NameFunction, Func);
	return *this;
}

OriServer & OriServer::operator<<(DefinedEndType Func) {
	EndConnectionDefined = Func;
	return *this;
}


OriServer & OriServer::operator<<(FuncNewConnection Func) {
	isUseNewConn = true;
	this->FuncNewConn = Func;
	return *this;
}


OriServer & OriServer::operator<<(FuncEndConnection Func) {
	isUseEndConn = true;
	this->FuncEndConn = Func;
	return *this;
}


int32_t  OriServer::GetSockOpt(int32_t FlagLevel, int32_t FlagOption)
{
	int32_t Values;
	return GetSockOpt(FlagLevel, FlagOption, Values);
}

int32_t  OriServer::GetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t &Values) {

	
#ifdef ORI_WINDOWS

	if (getsockopt(GetSocketServer(), FlagLevel, FlagOption, (char *)&Values, (int *) sizeof(int32_t)) == -1)
		return -1;
#endif
#ifdef ORI_LINUX
	
	if (getsockopt(GetSocketServer(), FlagLevel, FlagOption, &Values,  (socklen_t *) sizeof(int32_t)) == -1) 
		return -1;	
#endif
	
	return Values;
}

int32_t  OriServer::SetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t Values)    {

#ifdef ORI_WINDOWS
	if (setsockopt(GetSocketServer(), FlagLevel, FlagOption, (char*)&Values, sizeof(int32_t)) == -1)
		return -1;
#endif

#ifdef ORI_LINUX
	if (setsockopt(GetSocketServer(), FlagLevel, FlagOption, &Values, sizeof(int32_t)) == -1)
		return -1;
#endif

	return Values;
}

