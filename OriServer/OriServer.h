#ifndef OriServerH
#define OriServerH

#include "../OriKernel/OriPacket.h"
#include "../OriKernel/KernelPacket.h"
#include "../OriKernel/KernelSocket.h"
#include "../OriUntil/Untils.h"
#include "../OriUntil/OriThread.h"
#include "../OriUntil/OriError.h"

class OriServer {

public:
	OriServer() { InitArchServer(); }
	// Создаётся класс с заданым размером таблицы функций.
	OriServer(int SizeTB); 
	// Создаётся сервер на 127.0.0.1 ip с заданым портом аргумента port.
	bool CreateServer(int port);                  
	// Создаётся сервер на 127.0.0.1, где TypeServer это TCP (ORI_TCP) или UDP(ORI_UDP) протоколы 3го уровня на ваш выбор.
	bool CreateServer(int port, int TypeServer); 
	//  Создаётся сервер, где вы можете задать Ip и Порт,  где TypeServer это TCP (ORI_TCP) или UDP(ORI_UDP) протоколы 3го уровня на ваш выбор.
	bool CreateServer(const char *ip, int port, int TypeServer); 
	//  Создаётся сервер, где вы можете задать Ip и Порт.
	bool CreateServer(const OriIP ip, int port)                 { return CreateServer(ip.buf_ip, port, ORI_TCP);             }  
	//  Создаётся сервер, где вы можете задать Ip и Порт,  где TypeServer это TCP (ORI_TCP) или UDP(ORI_UDP) протоколы 3го уровня на ваш выбор.
	bool CreateServer(const OriIP ip, int port, int TypeServer) { return CreateServer(ip.buf_ip, port, TypeServer); }  

	// Запуск сервера.
	void RunServer();  
	// Остановка работы сервера.
	void StopServer(); 
	
	// m_Key - Ключ на валидность клиента, служит для уникальной авторизации между клиентом и сервером.
	void      SetKeySession(char *m_Key)            { strcpy(m_KeySession, m_Key); }
	// Вернуть ключ сессии.
	char      *GetKeySession()                      { return m_KeySession;         }
	// Время , которое даётся клиенту, на то, что бы прислать ключ сессии, если за это время ключ не был прислан, клиента кикается.
	void      SetTimeSession(int m_Time)            { TimeSession = m_Time;        }
	// Возвращает время сессии.
	int       GetTimeSession()                      { return TimeSession;          }
	// Использование проверки целостности пакета по средствам хеширования хешем Sha256. По умолчанию отключено.
	void      SetUseSha256(bool _UseY_N)            { isSha256 = _UseY_N;          }
	bool      GetUseSha256()                        { return isSha256;             }
	// Включение и отключения проверки по ключу сессии.
	void      SetSystemSession(bool _UseY_N)        { isSystemSession = _UseY_N;   }
	bool      GetSystemSession()                    { return isSystemSession;      }  
	// Использовать криптографию для защиты пакетов. Пакеты будут шифроваться по ключу SHA256, алгоритмом Вернама. Ключ фармируется по протоколу Деффи-Хелмана
	void      SetUseCrypt(bool _UseY_N)             { isUseCrypt = _UseY_N;        }
	bool      GetUseCrypt()                         { return isUseCrypt;           }
	// Определяет битность ключа, который будет рандомно создан при формирование главного ключа шифрования. По умолчанию используется 32х битный ключ. Учтите Чем больше длинна ключа, тем больше времени уходит на операции возведения в степень, по модулю числа n битности.
	void      SetBitSizeKey(unsigned SizeBit)       { SizeBitCrypt = SizeBit;      }
	unsigned  GetBitSizeKey()                       { return SizeBitCrypt;         }
	// Возвращает текущее кол. во подключенных клиентов. Стоит учесть, что объект OriServer не занимается работой, по фармированию массивов клиентов и массовой обработке этих массивов.
	int       GetCountConnectionClient()            { return CountClient;          }
	// Возврщает сокет сервера.
	OriSocket GetSocketServer()                     { return sock;                 }
	// Изменяет сокет сервера.
	void      SetSocketServer(OriSocket _NewSocket) { sock = _NewSocket;           }
	// Вовзвращает тип протокола сервера, UDP или TCP.
	int       GetTypeServer()                       { return TypeThisServer;       }
	// Меняет тип протокола, который использует сервер, для обмена пакетами, либо ORI_TCP либо ORI_UDP.
	void      SetTypeServer(int _NewType)           { TypeThisServer = _NewType;   }
	// Возвращает максимальный размер пакета, который используется сервером для обмена пакетами протоколами TCP или UDP. По умолчанию для IPv4 это 2 ^ 16.
	int       GetMaxSizePacket()                    { return SIZE_KERNEL_PACKET;   }
	// Изменяет максимальный размер пакета, который используется сервером для обмена пакетами протоколами TCP или UDP. По умолчанию для IPv4 это 2 ^ 16.
	void      SetMaxSizePacket(int _Size)           { SIZE_KERNEL_PACKET = _Size;  }
	// Возвращает максимальное количество функций, которое может находиться в таблице функций.
	int       GetMaxCountFunction()                 { return TBFuction.GetCountMaxFunction(); }
	// Меняет максимальное количество функций, которое может находиться в таблице функций.
	void      SetMaxCountFunction(int _Count)       { TBFuction.SetCountMaxFunction(_Count);  }
	// Возвращает количество функций, которое было уже добавлено в таблицы функций.
	int       GetCountUseFunction()                 { return TBFuction.GetCountUseFunction(); }
	// Возвращает Listen, по умолчанию 5.
	int       GetCountListen()                      { return m_Listen;                        }
	// Изменяет Listen, по умолчанию 5.
	void      SetCountListen(int _Count)            { m_Listen = _Count;                      }
	// Возвращает порт сервера.
	int       GetPortServer()                       { return (int)ntohs(server.sin_port);	  }
	// Возвращает IP сервера.
	char     *GetIpServer()                         { return inet_ntoa(server.sin_addr);	  }
	// Убивает клиента, при этом не вызывается функция о том, что клиент был отключен.
	void      KillClient(OriSocket Socket)          { EndConnection(Socket, "Kill Server");   }
	// Убивает клиента, при этом не вызывается функция о том, что клиент был отключен.
	void      KillClient(OriServerClient Client)    { EndConnection(Client.GetSocket(), "Kill Server"); }
	// Закрывает подключение к клиенту, но при этом вызывается функция о том, что клиент был отключен.
	void      CloseClient(OriServerClient Client)   { NoConnection(Client.GetSocket(), Client);         }
	// Выводит в консоли какие функции были добавлены в таблицу функций
	void      PrintListTableFunction()              { TBFuction.PrintListTables();                      }
	// Возвращает информацию о том, был ли включен или отключен алгоритм создания нового потока, для обработки пакета от клиента.
	bool      GetUseAlgRunInNewThread()             { return TBFuction.GetRunInNewThread();             }
	// Включение данной функции запускает возможность создания нового потока, для того что бы таблица функций могла в нём вызывать заданную пользователем функцию. По умолчанию отключено.
	void      SetUseAlgRunInNewThread(bool _UseY_N) { TBFuction.SetRunInNewThread(_UseY_N);             }
	//  Именно функция NameFunction будет обрабатываться в новом потоке.
	void      UseRunFunctionInThread(char *NameFunction)     { TBFuction.UseRunFunctionInThread(NameFunction); }
	//  Отключение обработки NameFunction в новом потоке.
	void      DisableRunFunctionInThread(char *NameFunction) { TBFuction.DisableRunFunctionInThread(NameFunction); }
	// Эмуляция пинга сервером
	void      EmulatePing(uint32_t _Ping) { EmulatePingEnable(); m_EmulatePing = _Ping; }

	void EmulatePingDisable() { isEmulatePing = false; }
	void EmulatePingEnable()  { isEmulatePing = true;  }
	bool GetEmulatePingStatus()     { return isEmulatePing; }
	uint32_t GetEmulatePing()       { return m_EmulatePing; }

	TableFunction &GetTableFunction()               { return TBFuction;                       }
	TableFiles    &GetTableFiles()                  { return TBFiles;                         }
	
	int32_t  GetSockOpt(int32_t FlagLevel, int32_t FlagOption);
	int32_t  GetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t &Values);
	int32_t  SetSockOpt(int32_t FlagLevel, int32_t FlagOption, int32_t Values);

	OriServer& operator << (const char *Name_Packet); // Имя пакета
	OriServer& operator << (int SizeLngBuffer);       // Размер освобождаемого буффера под плоскую структуру Lng
	OriServer& operator << (FunctionSockBuffer Func); // Socket + Char
	OriServer& operator << (FunctionBuffer     Func); // Char
	OriServer& operator << (FunctionNull       Func); // Void
	OriServer& operator << (FunctionLng        Func); // OriLng
	OriServer& operator << (FunctionLngClient  Func); // OriServerClient + OriLng
	OriServer& operator << (DefinedEndType     Func); // OriServerClient
	OriServer& operator << (FuncNewConnection  Func); // OriServerClient + NewConnection
	OriServer& operator << (FuncEndConnection  Func); // OriServerClient + EndConnection

	OriServer &operator[](char *NameFunc) {		strcpy(NameFunction, NameFunc);		return *this;	}

	OriServer& operator = (FunctionSockBuffer Func) { *this << Func; return *this; }
	OriServer& operator = (FunctionBuffer     Func) { *this << Func; return *this; }
	OriServer& operator = (FunctionNull       Func) { *this << Func; return *this; }
	OriServer& operator = (FunctionLng        Func) { *this << Func; return *this; }
	OriServer& operator = (FunctionLngClient  Func) { *this << Func; return *this; }
	OriServer& operator = (DefinedEndType     Func) { *this << Func; return *this; }
	OriServer& operator = (FuncNewConnection  Func) { *this << Func; return *this; }
	OriServer& operator = (FuncEndConnection  Func) { *this << Func; return *this; }

	bool SendPacket(OriServerClient Client, char *NameFunction, void *_Packet, int _Size);
  	
	//Сервер разрешает скачивать данный файл любому клиенту
	void File(char *Name) { this->TBFiles.AddFile(Name); }
	
private:
	void         InitArchServer(); // Инициализирует протокол и сокет.
	bool         CreateTcpServer(const char *ip, int port);
	bool         CreateUdpServer(const char *ip, int port);
	bool         InitSocketServer(const char *ip, int port);
	bool         InitListenServer();
	void         UdpPacketServer();
	void         EndConnection(OriSocket Sock, char *CommentError);      // Вызывается для разрыва связи с клиентом
	void         NoConnection (OriSocket Sock, OriServerClient &Client); // Вызывается когда пропадает связь с клиентом
	void         RunFunctionNewClient(OriServerClient &Client);
	void         Packet(char *buf, OriSocket Sock, OriServerClient &Client);
	bool         MethodSession(OriSocket Sock, sockaddr_in AddrClient, int Len);
	bool         MethodXOR(OriSocket Sock, OriCrypt &Crypt, sockaddr_in AddrClient, int Len);
	void         StandartPacketServer(OriSocket Sock, sockaddr_in AddrClient, int Len);
	void         pingTimeSession(OriSocket Sock, int Ping);
	OriSocketLen GetFromLen()                 { return fromlen;        }
	void         SetFromLen(OriSocketLen _fl) { fromlen = _fl;         }
	sockaddr_in  GetFrom()                    { return from;           }
	void         SetFrom(sockaddr_in _From)   { from = _From;          }
	void         SetAddr(sockaddr_in NewAddr) { server = NewAddr;      }
	void         CreateAddr(ORI_ADDRESS_FAMILY _family, const char *ip, int port)  { server.sin_family = _family; server.sin_addr.s_addr = inet_addr(ip); server.sin_port = htons(port); }
	sockaddr_in  GetAddr()                    { return server;         }
	int          GetSizeAddr()                { return sizeof(server); }
		 
	int          SIZE_KERNEL_PACKET;
	char         KeySessionSha256[65];
	int          m_Listen;
	OriSocket    sock, client;
	OriSocketLen fromlen;
	sockaddr_in  from;	 
		 
#if defined ORI_WINDOWS	
	sockaddr_in  server;
#endif
#if defined ORI_LINUX
	struct sockaddr_in server;
#endif
	sockaddr_in   fromClientThread;
	int           fromlenClientThread;
	int           Local_SizeLngBuffer;
	bool          ServerStart;
	char          NameFunction[32];
	char          m_KeySession[32]; // Ключ сессии. Используется для того, что бы действитя считать данного клиента валидным.
	int           TimeSession;
	bool          isSystemSession;
	bool          isSha256;
	bool          isUseNewConn;
	bool          isUseEndConn;
	bool          isUseCrypt;
	int           TypeThisServer;
	int           CountClient;
	bool          isEmulatePing;
	uint32_t      m_EmulatePing;

	FuncNewConnection FuncNewConn;
	FuncEndConnection FuncEndConn;
	TableFunction     TBFuction;
	TableFiles        TBFiles;
	unsigned          SizeBitCrypt;

	ThreadObj;
	void UseThread(int Id, sockaddr_in AddrClient, int Len);

	int        UseSocketClient;
	bool       UseRun;

	OriMutex   m_Mutex;
	MutexArray m_MutexArray;
	OriMutex   m_MutexPacket;


	void MemoryCopy(char * p, char *b, uint32_t sz) { for (uint32_t i = 0; i < sz; i++) p[i] = b[i]; }

	struct FromStruct     { int fromlen; sockaddr_in from;        };
	struct ArgumentThread { int Index;   FromStruct m_FromStruct; };

	public:
	int32_t GetIPProto() {
		if (this->GetTypeServer() == ORI_UDP)
			return IPPROTO_UDP;
		if (this->GetTypeServer() == ORI_TCP)
			return IPPROTO_TCP;
		return 0;
	}

};

#endif
