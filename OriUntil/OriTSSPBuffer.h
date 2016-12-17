#ifndef OriTSSPBufferH
#define OriTSSPBufferH

#include <stdint.h>

struct stTSSPBuffer
{

	char  *Buffer;
	int32_t   IndexBuffer;

	char *GetBuffer() { return Buffer; }
	uint32_t PositionUsingMemory;
	uint32_t SizeBuffer;

	stTSSPBuffer() : PositionUsingMemory(0), IndexBuffer(-1), SizeBuffer(0) {  }
	~stTSSPBuffer() { DeleteMemory(); }
	void CreateMemory(uint32_t _Memory)
	{
		Buffer = new char[_Memory];
		SizeBuffer = _Memory;
	}
	void DeleteMemory() {
		if (SizeBuffer > 0)
		{
			delete[] Buffer;
			SizeBuffer = 0;
		}
	}

	void Clear() { PositionUsingMemory = 0; IndexBuffer = -1; SizeBuffer = 0; }

	template<typename TypeData>
	void AddBlockData(TypeData _Data, uint32_t StartReadPosition, uint32_t EndReadPosition)
	{
		uint32_t NewPostion = MemoryCopy(Buffer, (char *)_Data, PositionUsingMemory, StartReadPosition, EndReadPosition);
		PositionUsingMemory = NewPostion;
	}

	// Проверка на конец буффера
	bool EndBuffer() {

		if (PositionUsingMemory >= SizeBuffer)
			return true;
		else return false;
	}

	uint32_t MemoryCopy(char *Buffer, char* Obj, uint32_t PositionBuffer, uint32_t StartReadPosition, uint32_t EndReadPosition)
	{

		uint32_t w = PositionBuffer;
		for (uint32_t i = StartReadPosition; i < EndReadPosition; i++)
		{
			Buffer[w] = Obj[i];
			w++;
		}

		return w;
	}
};

class CTSSPBuffer
{
public:
	stTSSPBuffer *ListBuffer;
	uint32_t CountList;
	void CreateList(uint32_t _CountList)
	{
		ListBuffer = new stTSSPBuffer[_CountList];
		CountList = _CountList;
	}
	void DeleteList() { if (ListBuffer) delete[] ListBuffer; CountList = 0; }

	void CreateBuffer(int32_t IndexBuffer, uint32_t SizeBuffer)
	{
		int32_t Check = GetIndexList(IndexBuffer);
		if (Check > -1) return;

		int32_t MyIndexBuffer = -1;
		for (uint32_t i = 0; i < CountList; i++)
			if (ListBuffer[i].IndexBuffer == -1) { MyIndexBuffer = i; break; }

		if (MyIndexBuffer == -1)
			return;

		if (ListBuffer[MyIndexBuffer].SizeBuffer > 0) return;

		ListBuffer[MyIndexBuffer].Clear();
		ListBuffer[MyIndexBuffer].CreateMemory(SizeBuffer);
		ListBuffer[MyIndexBuffer].IndexBuffer = IndexBuffer;
	}

	void ClearAll() 	{
		for (uint32_t i = 0; i < CountList; i++)
			ListBuffer[i].Clear();
	}

	void DeleteBuffer(int32_t IndexBuffer)
	{
		int32_t MyIndexBuffer = -1;
		for (uint32_t i = 0; i < CountList; i++)
			if (ListBuffer[i].IndexBuffer == IndexBuffer) MyIndexBuffer = i;

		ListBuffer[MyIndexBuffer].DeleteMemory();
		ListBuffer[MyIndexBuffer].IndexBuffer = -1;
	}

	bool EndBuffer(int32_t IndexBuffer) {
		int32_t MyIndexBuffer = GetIndexList(IndexBuffer);
		if (MyIndexBuffer < 0) return false;

		return ListBuffer[MyIndexBuffer].EndBuffer();
	}

	int32_t GetIndexList(int32_t IndexBuffer)
	{
		int32_t MyIndexBuffer = -1;
		for (uint32_t i = 0; i < CountList; i++)
			if (ListBuffer[i].IndexBuffer == IndexBuffer) MyIndexBuffer = i;
		return MyIndexBuffer;
	}

	template<typename TypeData>
	bool AddBlockData(int32_t IndexBuffer, TypeData _Data, uint32_t StartWritePosition, uint32_t EndWritePosition)
	{
		int32_t MyIndexBuffer = GetIndexList(IndexBuffer);
		if (MyIndexBuffer < 0) return false;
		ListBuffer[MyIndexBuffer].AddBlockData<TypeData>(_Data, StartWritePosition, EndWritePosition);
		return true;
	}

	char *GetBuffer(int IndexBuffer)
	{
		int32_t MyIndexBuffer = GetIndexList(IndexBuffer);
		if (MyIndexBuffer < 0) return "";
		return ListBuffer[MyIndexBuffer].GetBuffer();
	}


	uint32_t GetSizeBuffer(int IndexBuffer)
	{
		int32_t MyIndexBuffer = GetIndexList(IndexBuffer);
		if (MyIndexBuffer < 0) return 0;
		return ListBuffer[MyIndexBuffer].SizeBuffer;
	}


	CTSSPBuffer() : CountList(0)
	{
	}

	~CTSSPBuffer()
	{
		DeleteList();
	}

};




#endif