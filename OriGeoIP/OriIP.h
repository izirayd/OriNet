/********************************************************/
/*  �������� OriTcp 2015 (c)                            */
/*  ��������� ������ ��������                           */
/********************************************************/
#ifndef OriIPH
#define OriIPH

#include "OriBinFile.h"


//����� IP
class OriIP
{
  public:
	OriIP();
	  // ������������ ������ � ip
	OriIP& operator = (char *ip_src);
	  // ��������� IP �������
	bool operator == (OriIP &src);
	  // �������� ����� ip  ������ �������. �������� ������������ � GeoIP ��� ������ �������������� ip � ����
	bool operator >  (OriIP &src);
	  // �������������� �������� ����
	bool operator <  (OriIP &src);
	  // ������������ ip � ip
	OriIP& operator = (OriIP *ip);
	  // ������� �� ������ IP �����
	bool HostName(char *DomainName);
	// ������ ������
	void Clear();
	 // �������� ��� printf ��� std::iostream
	operator const char*() const	{
		return this->buf_ip;
	}

	// ������������� ������. ������������ ����� ������������������ ������� �����������
	int A, B, C, D;
	void Compile();

	bool operator >>  (OriIP &src);
	bool operator <<  (OriIP &src);

	char buf_ip[16];
  private:
	  // ������ ������
	int check_parcer_int(char *src, int type);

};


struct GeoIP {
	int ID;
	LenName Start;
	LenName End;
	OriIP StartIP;
	OriIP EndIP;
	LenName Country;
};


// Geo Location
struct GeoL
{
	int ID;
	LenName City;
	LenName SubRegion;
	LenName Region;
};

#endif