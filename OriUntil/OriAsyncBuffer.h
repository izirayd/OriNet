#ifndef OriAsyncBufferH
#define OriAsyncBufferH

#include <stdint.h>
#include "../OriServer/OriServerClient.h"
#include "../OriFunctionTable/TableFunction.h"

typedef char* PacketByte;

struct ObjPacket 	{
	ObjPacket() {
		Size   = 0;
		Packet = 0;
	}

	uint32_t Size;
	PacketByte Packet;
};

class OriAsyncBuffer
{
public:
	OriAsyncBuffer();
	~OriAsyncBuffer();

	OriSocket       Socket;
	OriServerClient ServerClient;


	bool EndData();
	bool CreateListPacket(uint32_t Size);
	bool ReCreatePacketList(uint32_t Size);
	bool AddPacket(PacketByte Byte, uint32_t SizePacket);
	bool GetPacket(PacketByte Byte, uint32_t &Size);

	void RunAllPacket();

	uint32_t GetSizeLastBuffer();

	TableFunction *TBFunction;

private:
	uint32_t      SizeList, LastIndex;
	ObjPacket    *ListPacket;
	bool          isCreateListPacket;
	uint32_t      MemoryCopy(char *Buffer, char* Obj, uint32_t PositionBuffer, uint32_t StartReadPosition, uint32_t EndReadPosition);
	void          RunPacket(char *buf, int Sock, OriServerClient &Client);
};


#endif