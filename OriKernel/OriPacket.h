#ifndef OriPacketH
#define OriPacketH

#include "KernelPacket.h"

#define SALT "ASaslfkqP12r@o#J4T3QsD"

#define SIZE_ORI_BUFFER_PACKET 2 * 1024
#define SIZE_BUFFER_PACKET_LNG 4000

struct oriPacket {
	char salt[32];
	int  sizepacket;
	char typecomand[10];
	char buffer[SIZE_ORI_BUFFER_PACKET];
};

struct OriLngPacket {
	void SetNamePacket(char *NewName) { strcpy(NamePacket, NewName); }
	void SetBuffer(char *NewBuffer)   { strcpy(buffer, NewBuffer);   }
	char NamePacket[32];
	char buffer[SIZE_BUFFER_LNG];
	int  numberPacket;
	int  countPacket;
};

struct OriPointVoidPacket {
	char NamePacket[32];
	char buffer[SIZE_BUFFER_POINT];
	int  numberPacket;
	int  countPacket;
};

struct OriSuperPacket {
	unsigned char type_packet_protocol;
	char NamePacket[32];
	char buffer[SIZE_BUFFER_TSSP];
	unsigned long long  numberPacket;
	unsigned long long  countPacket;
	unsigned long long  FullSize;
	unsigned long long  EndPacketSize;
	int  IndexTSSP;
};


#endif