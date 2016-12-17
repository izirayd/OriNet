#ifndef OriCryptH
#define OriCryptH

#include "OriKeyBlock.h"
#include "../OriUntil/OriFile.h"

		//   ������� ���. ^ 32
		unsigned int HashFAQ6(const char * str);
		//   ������� ���. ^ 32
		unsigned int HashRot13(const char * str);
		//   ������� ���. ^ 32
		unsigned int HashLy(const char * str);
		//   ������� ���. ^ 32
		unsigned int HashRs(const char * str);
		//   ������� ���. ^ 32
		unsigned int HashH37(const char * str);

		int FileHashLy(const char    *NameFile, unsigned int &Hash);
		int FileHashFAQ6(const char  *NameFile, unsigned int &Hash);
		int FileHashRot13(const char *NameFile, unsigned int &Hash);
		int FileHashRs(const char    *NameFile, unsigned int &Hash);
		int FileHashH37(const char   *NameFile, unsigned int &Hash);

		int FileHashLy(   OriFile *File,  unsigned int &Hash);
		int FileHashFAQ6( OriFile *File,  unsigned int &Hash);
		int FileHashRot13(OriFile *File,  unsigned int &Hash);
		int FileHashRs(   OriFile *File,  unsigned int &Hash);
		int FileHashH37(  OriFile *File,  unsigned int &Hash);

		unsigned int BinHashH37(const char * Data, int Size);
		unsigned int BinHashRs(const char * Data, int Size);
		unsigned int BinHashLy(const char * Data, int Size);
		unsigned int BinHashRot13(const char * Data, int Size);
		unsigned int BinHashFAQ6(const char * Data, int Size);

	// �������� ������� ���������� ������, �� �����
	void XORCrypt(char*buffer, char *key);
	// ��������� ���� SHA256, ������������ ������
	void SHA256Crypt(const char *buffer, char *Sha256);
	// ��������� ���� SHA256, ������������ ����� �����
	int  FileSHA256(char *NameFile, char *Sha256);
	int  FileSHA256(OriFile *File, char *Sha256);


struct XORData {
	char PublicINT_G[256];
	char PublicINT_P[256];
	char PublicKey[256];
};


/* 
 ����� ���������� ������ �� ����� �������.
 � ��������� ����������� ������ �����, ��� ��� 
 ����������� ��� ������� ����������, �� ���� ����,
 ��� ������ ����� ������� ���� ����������, ����� �� ��� �����. ������ ��������!
 ������� �� ������ ����! :D
*/
class OriCrypt
{
  public:
	  OriCrypt();
	  
	  void DeleteDynamicBuffer();
	  // �������� ����
     OriCrypt& operator << (char *key);
	 // ������� ���������� ��������������
	 void XORCrypt(char*buffer);
	 char *BufferKey;
	 private:
	  // ����� ������� ����
	 
	  // � ����� ��� �����
      int  lenKey;
};

#endif
