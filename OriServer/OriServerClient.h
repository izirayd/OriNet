#ifndef OriServerClientH
#define OriServerClientH

#include "../OriKernel/KernelPacket.h"
#include "../OriKernel/KernelSocket.h"
#include "../OriLng/OriLng.h"
#include "../OriKernel/OriPacket.h"
#include "../OriCrypt/OriCrypt.h"

#pragma warning(disable : 4996)


class OriServerClient {

public:
	void Create(OriSocket Socket, sockaddr_in _Addr, int TypeProtocol) { SetSocket(Socket); SetAddr(_Addr); SetTypeProtocol(TypeProtocol); }
	void Kick() { KernelCloseSocket(GetSocket()); }
	void DownloadFile(char *FileName);
	void SendFile(char *FileName);
	void File(char *FileName);
	
	OriServerClient& operator << (int Sock);
	OriServerClient& operator << (char *buffer);
	OriServerClient& operator << (OriLng &Lng);
	OriServerClient& operator << (OriCrypt &_Crypt);
	OriServerClient& operator =  (const char *TypePacket);
	
	bool operator =   (OriServerClient *ServerClient);
	bool operator ==  (OriServerClient &ServerClient);

	int32_t  GetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t &Values);
	int32_t  GetSockOpt(int32_t FlagLevel, int32_t FlagOption);
	int32_t  SetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t Values);

	// MSS
	int32_t  KernelGetMSS() {	if (GetTypeProtocol() == ORI_UDP) return ORI_ERROR;	return GetSockOpt(IPPROTO_TCP, 4);	}

	void    InitMSS()      { MSS = KernelGetMSS(); }
	int32_t GetMSS()       { return MSS; }
	

	OriSocket GetSocket() { return sock; }
	int       GetPort()   { return port; }
	char*     GetIP()     { return ip;   }
	char*     GetIndexPC(){ return IndexPC; }

	sockaddr_in GetAddrIn()    { return AddrClient; }
	sockaddr*    GetAddr()     { return (struct sockaddr *)& AddrClient; }
	
	int GetSizeAddr()     { return SizeClientAddr; }
	int GetTypeProtocol() { return TypePacket; }

#undef SetPort
	void SetTypeProtocol(int NewType) { TypePacket = NewType; }
	void SetSocket(OriSocket Socket)  { sock = Socket;        }
	void SetPort(int _port)           { port = _port;         }
	void SetIP(char* NewIP)           { strcpy(ip, NewIP);    }
	void SetAddr(sockaddr_in NewAddr) { AddrClient = NewAddr; InitAddr(); }
	void SetSizeAddr(int _Size)       { SizeClientAddr = _Size; }
	void SetIndexPC(char *NewIndex)   { strcpy(IndexPC, NewIndex); }

	void InitAddr() { SetIP(inet_ntoa(AddrClient.sin_addr)); SetPort((int)ntohs(AddrClient.sin_port)); SetSizeAddr(sizeof(AddrClient)); }

	OriServerClient();

	bool SendPacket(char *NameFunction, void *_Packet, int _Size);
	bool SendSuperPacket(char *NameFunction, unsigned char TypeProtocol, void *_Packet, unsigned long long SizePacket, int IndexTSSP);

	sockaddr_in  AddrClient;
	OriSocketLen SizeClientAddr;

	int       GetMaxSizePacket()                    { return SIZE_KERNEL_PACKET; }
	void      SetMaxSizePacket(int _Size)           { SIZE_KERNEL_PACKET = _Size; }

private:
	int  SIZE_KERNEL_PACKET;
	bool OriSendKernel(KernelPacket * p);
	bool OriSendKernel(KernelPacket * p, uint32_t Size);

	void strnull(char * p, char * b, unsigned sz);
	void strSIZE(char * buffer, char * out, int start, int count);
	char Local_Type_Packet[32];
	void MemoryCopy(char *Buffer, char* Obj, unsigned long long StartReadPosition, unsigned long long EndReadPosition);

	OriSocket  sock;
	int  port;
	char ip[32];
	int  TypePacket;
	char IndexPC[65];
	int32_t MSS;
	/* Крипт*/
	OriCrypt Crypt;
	bool isUseCrypt; // Взято от клиент класс

};
#endif