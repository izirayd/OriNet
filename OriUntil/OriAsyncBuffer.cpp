#include "OriAsyncBuffer.h"
#include "OriMutex.h"


#define OR ||

OriAsyncBuffer::OriAsyncBuffer() {
	SizeList = 0;
	LastIndex = 0;
	isCreateListPacket = false;
}

OriAsyncBuffer::~OriAsyncBuffer()
{
	if (isCreateListPacket) 	{
		delete[] ListPacket;
		ListPacket = NULL;
	}
}


bool OriAsyncBuffer::EndData()
{
	if (LastIndex != 0) 
		return true;

	return false;
}

bool OriAsyncBuffer::CreateListPacket(uint32_t Size)	   {

	if (isCreateListPacket == true)
		return false;

	isCreateListPacket = true;

	ListPacket = new ObjPacket[Size];
	SizeList = Size;

	return true;
}

bool OriAsyncBuffer::ReCreatePacketList(uint32_t Size) {

	if (isCreateListPacket)
	{
		delete[] ListPacket;
		ListPacket = NULL;
	}

	SizeList = 0;
	LastIndex = 0;
	isCreateListPacket = false;

	CreateListPacket(Size);
	return true;
}

bool OriAsyncBuffer::AddPacket(PacketByte Byte, uint32_t SizePacket)	   {

	if (Byte == NULL OR SizePacket == 0 OR(LastIndex + 1) > SizeList)
		return false;

	OriMutex Mutex;

	Mutex.lock();
	LastIndex++;
	ListPacket[LastIndex].Packet = new char[SizePacket];
	ListPacket[LastIndex].Size = SizePacket;
	MemoryCopy(ListPacket[LastIndex].Packet, Byte, 0, 0, SizePacket);
	Mutex.unlock();

	return true;
}

uint32_t OriAsyncBuffer::GetSizeLastBuffer() {

	if (LastIndex == 0)
		return 0;

	return ListPacket[LastIndex].Size;
}


bool OriAsyncBuffer::GetPacket(PacketByte Byte, uint32_t &Size) {

	if (Byte == NULL OR LastIndex == 0 OR ListPacket[LastIndex].Packet == NULL OR GetSizeLastBuffer() == 0)
		return false;

	OriMutex Mutex;
	Mutex.lock();

	MemoryCopy(Byte, ListPacket[LastIndex].Packet, 0, 0, ListPacket[LastIndex].Size);
	Size = ListPacket[LastIndex].Size;


	delete[] ListPacket[LastIndex].Packet;
	ListPacket[LastIndex].Packet = NULL;
	ListPacket[LastIndex].Size = 0;
	LastIndex--;

	Mutex.unlock();

	return true;
}

uint32_t OriAsyncBuffer::MemoryCopy(char *Buffer, char* Obj, uint32_t PositionBuffer, uint32_t StartReadPosition, uint32_t EndReadPosition)
{

	uint32_t w = PositionBuffer;
	for (uint32_t i = StartReadPosition; i < EndReadPosition; i++)
	{
		Buffer[w] = Obj[i];
		w++;
	}

	return w;
}

void  OriAsyncBuffer::RunPacket(char *buf, int Sock, OriServerClient &Client) {

	KernelPacket *PacketKernel = (KernelPacket *)buf;

	if (PacketKernel->type_packet_protocol == TYPE_STRUCT_PROTOCOL)
	{
		oriPacket *Packet = (oriPacket *)PacketKernel->buffer;
		if (strcmp(Packet->salt, SALT) != 0) return;
		TBFunction->RunFunction(Packet->typecomand, Sock, Packet->buffer);
	}

	if (PacketKernel->type_packet_protocol == TYPE_LANGUAGE_PROTOCOL)	{

		OriLngPacket *Packet = (OriLngPacket *)PacketKernel->buffer;
		OriLng LngBuffer(SIZE_BUFFER_LNG);
		LngBuffer << Packet->buffer;
		TBFunction->RunFunctionLng(Packet->NamePacket, Client, LngBuffer);

	}

	if (PacketKernel->type_packet_protocol == TYPE_RUN_ASYNC_SUPER_PACKET)	{
		KernelDynamicPacket *DynamicPacket = (KernelDynamicPacket *)buf;
		
		OriLng LngBuffer(DynamicPacket->FullSize);
		LngBuffer << DynamicPacket->buffer;

		TBFunction->RunFunctionLng(DynamicPacket->NamePacket, Client, LngBuffer);

	}
}

void OriAsyncBuffer::RunAllPacket() {

	while (this->EndData())
	{
		char *BufferPacket = new char[GetSizeLastBuffer()];

		uint32_t Size;
		GetPacket(BufferPacket, Size);
		RunPacket(BufferPacket, Socket, ServerClient);
		delete[] BufferPacket;
	}
}