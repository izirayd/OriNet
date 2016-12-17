#ifndef KernelPacketH
#define KernelPacketH

  #include "KernelSocket.h"
  #include "TypePacketProtocol.h"

#define KERNEL_SALT "wdAS123"

#define SIZE_MAX_TCP_STACK   65536
#define SIZE_TYPE_PROTOCOL   1
#define KERNEL_SALT_SIZE_MAX 10
#define KERNEL_INDEX_PC 65


#define SIZE_BUFFER (SIZE_MAX_TCP_STACK - (SIZE_TYPE_PROTOCOL + KERNEL_SALT_SIZE_MAX + KERNEL_INDEX_PC))

#define SIZE_INT   4
#define SIZE_INT64 8
#define SIZE_NAME_FUNCTION 32

#define POINT_KERNEL_BUFFER 10

#define SIZE_STRUCT_KERNEL SIZE_TYPE_PROTOCOL + KERNEL_SALT_SIZE_MAX + KERNEL_INDEX_PC + POINT_KERNEL_BUFFER;

// FILE
#define SIZE_FILE_NANE 260
#define COUNT_WRITE_IN_STRUCT_FILE 5
#define SIZE_BUFFER_FILE  (SIZE_BUFFER - (SIZE_FILE_NANE +  SIZE_INT * COUNT_WRITE_IN_STRUCT_FILE) - POINT_KERNEL_BUFFER)

// LNG
#define COUNT_WRITE_IN_STRUCT_LNG 2
#define SIZE_STRUCT_LNG   (SIZE_NAME_FUNCTION + SIZE_INT * COUNT_WRITE_IN_STRUCT_LNG)
#define SIZE_BUFFER_LNG   (SIZE_BUFFER - SIZE_STRUCT_LNG - POINT_KERNEL_BUFFER)

// POINT 
#define COUNT_WRITE_IN_STRUCT_POINT 2
#define SIZE_BUFFER_POINT (SIZE_BUFFER - (SIZE_NAME_FUNCTION + SIZE_INT * COUNT_WRITE_IN_STRUCT_POINT) - POINT_KERNEL_BUFFER)

// TSSP
#define COUNT_WRITE_IN_STRUCT_POINT_8BIT 4
#define COUNT_WRITE_IN_STRUCT_POINT_4BIT 1
#define SIZE_BUFFER_TSSP  (SIZE_BUFFER - (SIZE_TYPE_PROTOCOL +  SIZE_NAME_FUNCTION +  COUNT_WRITE_IN_STRUCT_POINT_8BIT * SIZE_INT64 + COUNT_WRITE_IN_STRUCT_POINT_4BIT * SIZE_INT) - POINT_KERNEL_BUFFER)

#pragma warning(disable : 4996)

struct KernelPacket
{
  void SetTypeProtocol(unsigned char NewType) { type_packet_protocol = NewType; }
  void SetSalt(char *NewSalt) { strcpy(salt_packet, NewSalt); }

  unsigned char type_packet_protocol;
  char salt_packet[KERNEL_SALT_SIZE_MAX];
  char indexPC[KERNEL_INDEX_PC];
  char buffer[SIZE_BUFFER];
};

struct KernelDynamicPacket
{
	void SetTypeProtocol(unsigned char NewType) { type_packet_protocol = NewType; }
	void SetSalt(char *NewSalt) { strcpy(salt_packet, NewSalt); }
	unsigned char type_packet_protocol;
	char salt_packet[KERNEL_SALT_SIZE_MAX];
	char NamePacket[32];
	unsigned long long FullSize;
	char *buffer;
};

#endif
