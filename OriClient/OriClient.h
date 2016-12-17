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
	// Создаётся класс с заданым размером таблицы
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

	// Подключиться к серверу
	bool ConnectionServer(const char *IP, int Port);
	/*
	   IPServer - ip адрес сервера, с которым будет обмен пакетов
	   PortServer - соотвественно порт данного сервера

	   IPClient - ip на который сервер должен будет присылать ответ
	   PortClient - порт на который будут приходит сообщения от сервера

	   По сути два OriClient объекта в режиме UDP могу обмениваться сообщениями и отлично работать вместе, но
	   не в режимах UseCrypt и SessionKey.
	   */
	bool ConnectionServer(const char *IPServer, int PortServer, const char *IPClient, int PortClient);
	bool ConnectionServer(int Port);

	// Отключиться от сервера
	void CloseConnection();
	// Ключ для сессии
	void KeySession(char *Key);
	// Отключить приём пакета сессии
	void OffSystemSession();
	// Использование шифрования, при обмене пакетами
	void UseCrypt();
	// Скачать файл
	void ReadFile(char *NameFile);
	// Добавить в файл в таблицу разрешений
	void File(char *Name);
	// Функция принимает два параметра и в зависимости от этих параметров, выполняет определенённые действия
	// true  - Будут приниматься от сервера любые файлы, игнорируя таблицу безопасности
	// false - Стоит по умолчанию. Будут приниматься только те файлы, которые были разрешены к принятию
	void DownloadAllFile(bool typeY_N);

	/* Передача пакетов */

	// Передача одиночного сообщения, работало в вресии ниже OriVersion 0.2
	OriClient& operator << (const char *msg);
	// Передача структурным способом
	OriClient& operator << (const oriPacket *p_OriPacket);
	// Передача структурным способом
	OriClient& operator << (oriPacket *p_OriPacket);
	// Передача LNG пакета
	OriClient& operator << (OriLng &Lng);
	// Создание имени пакета, по которому будет вызванна соответствующая функция на стороне сервера, если конечно она есть
	OriClient& operator =  (const char *TypePacket);

	/* Функции */

	// Задание имени функции, которая будет вызываться клиентом, если сервер пришлёт пакет, с данным именем
	OriClient& operator >> (const char *Name_Packet);
	OriClient& operator >> (FunctionSockBuffer Func);
	OriClient& operator >> (FunctionBuffer Func);
	OriClient& operator >> (FunctionNull Func);
	OriClient& operator >> (FunctionLng Func);

	OriClient& operator >> (FunctionLngClient Func);


	OriClient& operator >> (FuncEndConnection Func);
	OriClient& operator >> (FuncNewConnection Func);

	/* Таблицы */
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

	// Насамом деле, данна функция создаёт аналогичный сервер обработки подключений
	bool ConnectionUdp(const char *IPServer, int PortServer, const char *IPClient, int PortClient);
	bool ConnectionTcp(const char *IP, int Port);

  /* Функции передачи пакета уровня ядра библиотеки */
  bool oriSend(oriPacket *p); // Старая версия
  bool OriSendKernel(KernelPacket *p);  // Новая версия, в данный момент основная
  bool OriSendKernel(KernelPacket * p, uint32_t Size);

  /* Функции байтового копирования */
  void strnull(char * p, char * b, unsigned sz); 
  void strSIZE(char * buffer, char * out, int start, int count);
  
  // Функция создания потока для обработки пакетов
  bool NewThreadServer(void* Param);
  // Сокет сервера
  OriSocket sock;
  // Структура адреса
  struct sockaddr_in addr;
  // 
  bool b_Type_Packet;
  // Имя пакета
  char Local_Type_Packet[32];
  // Индификатор клиента, относительно подключения к серверу
  int  IDServer;
  // Имя функции
  char Local_Cmd_Char_Buffer[100];
  // Имя сессии
  char m_KeySession[SIZE_SHA256];
  // Использование проверка целостности пакета
  bool isSha256;
  // Использование шифрование пакета
  bool isUseCrypt;
  // Использование проверки, при соед.
  bool isSystemSession;

  uint32_t TypeArch;

  OriIndexPC IndexPC;
  int32_t port;
  char   ip[32];

  // Класс крипта
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

  // Сектор потоков
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
