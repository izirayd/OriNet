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
	// �������� ����� � ������� �������� ������� �������.
	OriServer(int SizeTB); 
	// �������� ������ �� 127.0.0.1 ip � ������� ������ ��������� port.
	bool CreateServer(int port);                  
	// �������� ������ �� 127.0.0.1, ��� TypeServer ��� TCP (ORI_TCP) ��� UDP(ORI_UDP) ��������� 3�� ������ �� ��� �����.
	bool CreateServer(int port, int TypeServer); 
	//  �������� ������, ��� �� ������ ������ Ip � ����,  ��� TypeServer ��� TCP (ORI_TCP) ��� UDP(ORI_UDP) ��������� 3�� ������ �� ��� �����.
	bool CreateServer(const char *ip, int port, int TypeServer); 
	//  �������� ������, ��� �� ������ ������ Ip � ����.
	bool CreateServer(const OriIP ip, int port)                 { return CreateServer(ip.buf_ip, port, ORI_TCP);             }  
	//  �������� ������, ��� �� ������ ������ Ip � ����,  ��� TypeServer ��� TCP (ORI_TCP) ��� UDP(ORI_UDP) ��������� 3�� ������ �� ��� �����.
	bool CreateServer(const OriIP ip, int port, int TypeServer) { return CreateServer(ip.buf_ip, port, TypeServer); }  

	// ������ �������.
	void RunServer();  
	// ��������� ������ �������.
	void StopServer(); 
	
	// m_Key - ���� �� ���������� �������, ������ ��� ���������� ����������� ����� �������� � ��������.
	void      SetKeySession(char *m_Key)            { strcpy(m_KeySession, m_Key); }
	// ������� ���� ������.
	char      *GetKeySession()                      { return m_KeySession;         }
	// ����� , ������� ����� �������, �� ��, ��� �� �������� ���� ������, ���� �� ��� ����� ���� �� ��� �������, ������� ��������.
	void      SetTimeSession(int m_Time)            { TimeSession = m_Time;        }
	// ���������� ����� ������.
	int       GetTimeSession()                      { return TimeSession;          }
	// ������������� �������� ����������� ������ �� ��������� ����������� ����� Sha256. �� ��������� ���������.
	void      SetUseSha256(bool _UseY_N)            { isSha256 = _UseY_N;          }
	bool      GetUseSha256()                        { return isSha256;             }
	// ��������� � ���������� �������� �� ����� ������.
	void      SetSystemSession(bool _UseY_N)        { isSystemSession = _UseY_N;   }
	bool      GetSystemSession()                    { return isSystemSession;      }  
	// ������������ ������������ ��� ������ �������. ������ ����� ����������� �� ����� SHA256, ���������� �������. ���� ����������� �� ��������� �����-�������
	void      SetUseCrypt(bool _UseY_N)             { isUseCrypt = _UseY_N;        }
	bool      GetUseCrypt()                         { return isUseCrypt;           }
	// ���������� �������� �����, ������� ����� �������� ������ ��� ������������ �������� ����� ����������. �� ��������� ������������ 32� ������ ����. ������ ��� ������ ������ �����, ��� ������ ������� ������ �� �������� ���������� � �������, �� ������ ����� n ��������.
	void      SetBitSizeKey(unsigned SizeBit)       { SizeBitCrypt = SizeBit;      }
	unsigned  GetBitSizeKey()                       { return SizeBitCrypt;         }
	// ���������� ������� ���. �� ������������ ��������. ����� ������, ��� ������ OriServer �� ���������� �������, �� ������������ �������� �������� � �������� ��������� ���� ��������.
	int       GetCountConnectionClient()            { return CountClient;          }
	// ��������� ����� �������.
	OriSocket GetSocketServer()                     { return sock;                 }
	// �������� ����� �������.
	void      SetSocketServer(OriSocket _NewSocket) { sock = _NewSocket;           }
	// ����������� ��� ��������� �������, UDP ��� TCP.
	int       GetTypeServer()                       { return TypeThisServer;       }
	// ������ ��� ���������, ������� ���������� ������, ��� ������ ��������, ���� ORI_TCP ���� ORI_UDP.
	void      SetTypeServer(int _NewType)           { TypeThisServer = _NewType;   }
	// ���������� ������������ ������ ������, ������� ������������ �������� ��� ������ �������� ����������� TCP ��� UDP. �� ��������� ��� IPv4 ��� 2 ^ 16.
	int       GetMaxSizePacket()                    { return SIZE_KERNEL_PACKET;   }
	// �������� ������������ ������ ������, ������� ������������ �������� ��� ������ �������� ����������� TCP ��� UDP. �� ��������� ��� IPv4 ��� 2 ^ 16.
	void      SetMaxSizePacket(int _Size)           { SIZE_KERNEL_PACKET = _Size;  }
	// ���������� ������������ ���������� �������, ������� ����� ���������� � ������� �������.
	int       GetMaxCountFunction()                 { return TBFuction.GetCountMaxFunction(); }
	// ������ ������������ ���������� �������, ������� ����� ���������� � ������� �������.
	void      SetMaxCountFunction(int _Count)       { TBFuction.SetCountMaxFunction(_Count);  }
	// ���������� ���������� �������, ������� ���� ��� ��������� � ������� �������.
	int       GetCountUseFunction()                 { return TBFuction.GetCountUseFunction(); }
	// ���������� Listen, �� ��������� 5.
	int       GetCountListen()                      { return m_Listen;                        }
	// �������� Listen, �� ��������� 5.
	void      SetCountListen(int _Count)            { m_Listen = _Count;                      }
	// ���������� ���� �������.
	int       GetPortServer()                       { return (int)ntohs(server.sin_port);	  }
	// ���������� IP �������.
	char     *GetIpServer()                         { return inet_ntoa(server.sin_addr);	  }
	// ������� �������, ��� ���� �� ���������� ������� � ���, ��� ������ ��� ��������.
	void      KillClient(OriSocket Socket)          { EndConnection(Socket, "Kill Server");   }
	// ������� �������, ��� ���� �� ���������� ������� � ���, ��� ������ ��� ��������.
	void      KillClient(OriServerClient Client)    { EndConnection(Client.GetSocket(), "Kill Server"); }
	// ��������� ����������� � �������, �� ��� ���� ���������� ������� � ���, ��� ������ ��� ��������.
	void      CloseClient(OriServerClient Client)   { NoConnection(Client.GetSocket(), Client);         }
	// ������� � ������� ����� ������� ���� ��������� � ������� �������
	void      PrintListTableFunction()              { TBFuction.PrintListTables();                      }
	// ���������� ���������� � ���, ��� �� ������� ��� �������� �������� �������� ������ ������, ��� ��������� ������ �� �������.
	bool      GetUseAlgRunInNewThread()             { return TBFuction.GetRunInNewThread();             }
	// ��������� ������ ������� ��������� ����������� �������� ������ ������, ��� ���� ��� �� ������� ������� ����� � �� �������� �������� ������������� �������. �� ��������� ���������.
	void      SetUseAlgRunInNewThread(bool _UseY_N) { TBFuction.SetRunInNewThread(_UseY_N);             }
	//  ������ ������� NameFunction ����� �������������� � ����� ������.
	void      UseRunFunctionInThread(char *NameFunction)     { TBFuction.UseRunFunctionInThread(NameFunction); }
	//  ���������� ��������� NameFunction � ����� ������.
	void      DisableRunFunctionInThread(char *NameFunction) { TBFuction.DisableRunFunctionInThread(NameFunction); }
	// �������� ����� ��������
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

	OriServer& operator << (const char *Name_Packet); // ��� ������
	OriServer& operator << (int SizeLngBuffer);       // ������ �������������� ������� ��� ������� ��������� Lng
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
  	
	//������ ��������� ��������� ������ ���� ������ �������
	void File(char *Name) { this->TBFiles.AddFile(Name); }
	
private:
	void         InitArchServer(); // �������������� �������� � �����.
	bool         CreateTcpServer(const char *ip, int port);
	bool         CreateUdpServer(const char *ip, int port);
	bool         InitSocketServer(const char *ip, int port);
	bool         InitListenServer();
	void         UdpPacketServer();
	void         EndConnection(OriSocket Sock, char *CommentError);      // ���������� ��� ������� ����� � ��������
	void         NoConnection (OriSocket Sock, OriServerClient &Client); // ���������� ����� ��������� ����� � ��������
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
	char          m_KeySession[32]; // ���� ������. ������������ ��� ����, ��� �� ��������� ������� ������� ������� ��������.
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
