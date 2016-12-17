#ifndef OriClientH
#define OriClientH

#include "../OriKernel/KernelPacket.h"
#include "../OriKernel/KernelSocket.h"
#include "../OriLng/OriLng.h"
#include "../OriUntil/Untils.h"
#include "../OriKernel/OriPacket.h"
#include "../OriCrypt/OriCrypt.h"
#include "../OriUntil/OriMutex.h"
#include "../OriUntil/OriAsyncBuffer.h"
#include "../OriUntil/OriIndexPC.h"
#include "../OriUntil/OriTSSPBuffer.h"

enum OriTypeArch  {
	 Thread  = 1,
	 RecvThreadRunAsync = 2
};

class OriClient
{
public:
	OriClient();
	// �������� ����� � ������� �������� �������
	OriClient(int SizeTB);

	void SetArch(OriTypeArch nTypeArch = Thread)
	{

		if (nTypeArch == RecvThreadRunAsync)
		{
			AsyncBuffer.Socket = GetSocketClient();
			AsyncBuffer.CreateListPacket(512);
			AsyncBuffer.TBFunction = &TBFunction;
		}

		TypeArch = nTypeArch; 
	}

	uint32_t GetTypeArch() { return TypeArch; }

	// ������������ � �������
	bool ConnectionServer(const char *IP, int Port);
	/*
	   IPServer - ip ����� �������, � ������� ����� ����� �������
	   PortServer - ������������� ���� ������� �������

	   IPClient - ip �� ������� ������ ������ ����� ��������� �����
	   PortClient - ���� �� ������� ����� �������� ��������� �� �������

	   �� ���� ��� OriClient ������� � ������ UDP ���� ������������ ����������� � ������� �������� ������, ��
	   �� � ������� UseCrypt � SessionKey.
	   */
	bool ConnectionServer(const char *IPServer, int PortServer, const char *IPClient, int PortClient);
	bool ConnectionServer(int Port);

	// ����������� �� �������
	void CloseConnection();
	// ���� ��� ������
	void KeySession(char *Key);
	// ��������� ���� ������ ������
	void OffSystemSession();
	// ������������� ����������, ��� ������ ��������
	void UseCrypt();
	// ������� ����
	void ReadFile(char *NameFile);
	// �������� � ���� � ������� ����������
	void File(char *Name);
	// ������� ��������� ��� ��������� � � ����������� �� ���� ����������, ��������� ������������� ��������
	// true  - ����� ����������� �� ������� ����� �����, ��������� ������� ������������
	// false - ����� �� ���������. ����� ����������� ������ �� �����, ������� ���� ��������� � ��������
	void DownloadAllFile(bool typeY_N);

	/* �������� ������� */

	// �������� ���������� ���������, �������� � ������ ���� OriVersion 0.2
	OriClient& operator << (const char *msg);
	// �������� ����������� ��������
	OriClient& operator << (const oriPacket *p_OriPacket);
	// �������� ����������� ��������
	OriClient& operator << (oriPacket *p_OriPacket);
	// �������� LNG ������
	OriClient& operator << (OriLng &Lng);
	// �������� ����� ������, �� �������� ����� �������� ��������������� ������� �� ������� �������, ���� ������� ��� ����
	OriClient& operator =  (const char *TypePacket);

	/* ������� */

	// ������� ����� �������, ������� ����� ���������� ��������, ���� ������ ������ �����, � ������ ������
	OriClient& operator >> (const char *Name_Packet);
	OriClient& operator >> (FunctionSockBuffer Func);
	OriClient& operator >> (FunctionBuffer Func);
	OriClient& operator >> (FunctionNull Func);
	OriClient& operator >> (FunctionLng Func);

	OriClient& operator >> (FunctionLngClient Func);


	OriClient& operator >> (FuncEndConnection Func);
	OriClient& operator >> (FuncNewConnection Func);

	/* ������� */
	TableFunction TBFunction;
	TableFiles    TBFiles;

	int TypePacket;

	int32_t  GetSockOpt(int32_t FlagLevel, int32_t FlagOption);
	int32_t  GetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t &Values);
	int32_t  SetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t Values);

	OriSocket  GetSocketClient() { return sock; }
	int32_t    GetPort() { return port; }
	char*      GetIP()     { return ip; }

	uint32_t GetTypeServer() { return TypePacket; }



	bool GetConnect() { if (GetTypeServer() == ORI_UDP) return true; else return isConnect; }
	void SetConnect(bool _T_F) { isConnect = _T_F; }

	int32_t GetTypeProtocol()	{ return TypePacket;  }

  /* MSS */
  int32_t KernelGetMSS() {
	  if (GetTypeProtocol() == ORI_UDP) return ORI_ERROR;
	  return GetSockOpt(IPPROTO_TCP, 4);
	  // TCP_MAXSEG - 4
  }

  void    InitMSS()      { MSS = KernelGetMSS(); }
  int32_t GetMSS()       { return MSS; }
  int32_t MSS;

  void AsyncRun()
    { AsyncBuffer.RunAllPacket(); }


private:

	// ������� ����, ����� ������� ������ ����������� ������ ��������� �����������
	bool ConnectionUdp(const char *IPServer, int PortServer, const char *IPClient, int PortClient);
	bool ConnectionTcp(const char *IP, int Port);

  /* ������� �������� ������ ������ ���� ���������� */
  bool oriSend(oriPacket *p); // ������ ������
  bool OriSendKernel(KernelPacket *p);  // ����� ������, � ������ ������ ��������
  bool OriSendKernel(KernelPacket * p, uint32_t Size);

  /* ������� ��������� ����������� */
  void strnull(char * p, char * b, unsigned sz); 
  void strSIZE(char * buffer, char * out, int start, int count);
  
  // ������� �������� ������ ��� ��������� �������
  bool NewThreadServer(void* Param);
  // ����� �������
  OriSocket sock;
  // ��������� ������
  struct sockaddr_in addr;
  // 
  bool b_Type_Packet;
  // ��� ������
  char Local_Type_Packet[32];
  // ����������� �������, ������������ ����������� � �������
  int  IDServer;
  // ��� �������
  char Local_Cmd_Char_Buffer[100];
  // ��� ������
  char m_KeySession[SIZE_SHA256];
  // ������������� �������� ����������� ������
  bool isSha256;
  // ������������� ���������� ������
  bool isUseCrypt;
  // ������������� ��������, ��� ����.
  bool isSystemSession;

  uint32_t TypeArch;

  OriIndexPC IndexPC;
  int32_t port;
  char   ip[32];

  // ����� ������
  OriCrypt Crypt;

  sockaddr_in AddrServer;
  int         AddrLen;

  void End_Connection(int Client);
  void NoConnection(int Sock, OriServerClient &Client);
  void RunFunctionNewClient(  OriServerClient &Client);

  void Packet(char *buf, int Sock, OriServerClient &Client, int32_t SizePacket);
  void RunPacket(char *buf, int Sock, OriServerClient &Client);

  /*******************************************************************************/
  bool MethodSession(int Sock, sockaddr_in AddrClient, int Len);
  bool MethodXOR(int Sock, OriCrypt &Crypt, sockaddr_in AddrClient, int Len);
  void StandartPacketClient(int Sock, sockaddr_in AddrClient, int Len);

  void TimeSessionClient(int Sock, int Ping);

  
  char KeySessionSha256[65];
  /*******************************************************************************/

  void EndConnectionClient(int Client);
  void NoConnectionClient(int Sock, OriServerClient &Client);

  // ������ �������
  /*******************************************************************************/
  bool CreateThread();
  void Run();

#ifdef ORI_WINDOWS
  static void RunThread(void* p);
#else
  static void* RunThread(void* p);
#endif

  int UseSocketClient;
  bool UseRun;
  OriMutex m_Mutex;

  void UseThread(int Id, sockaddr_in AddrClient, int Len);

  void KernelInitArch(int Id, sockaddr_in AddrClient, int Len);

  /*******************************************************************************/

  void UdpPacketServer();

  CTSSPBuffer TSSPBuffer;
  bool isConnect;

  FuncNewConnection FuncNewConn;
  FuncEndConnection FuncEndConn;

  bool          isUseNewConn;
  bool          isUseEndConn;

  NewConnection  _Null;
  OriAsyncBuffer AsyncBuffer;
 
};

#endif
