/********************************************************/
/*  Протокол OriTcp 2015 (c)                            */
/*  Создатель Сергей Щербаков                           */
/********************************************************/
#ifndef OriIPH
#define OriIPH

#include "OriBinFile.h"


//Класс IP
class OriIP
{
  public:
	OriIP();
	  // Присваивнаия строки к ip
	OriIP& operator = (char *ip_src);
	  // Сравнение IP адресов
	bool operator == (OriIP &src);
	  // Проверка какой ip  больше другого. Операция используется в GeoIP для поиска принадлежности ip к зоне
	bool operator >  (OriIP &src);
	  // Антианологично операции выше
	bool operator <  (OriIP &src);
	  // Присваивание ip к ip
	OriIP& operator = (OriIP *ip);
	  // Находим по домену IP адрес
	bool HostName(char *DomainName);
	// Чистим буффер
	void Clear();
	 // Операция для printf или std::iostream
	operator const char*() const	{
		return this->buf_ip;
	}

	// Компилируемый буффер. Используется когда производительность запроса критическая
	int A, B, C, D;
	void Compile();

	bool operator >>  (OriIP &src);
	bool operator <<  (OriIP &src);

	char buf_ip[16];
  private:
	  // Парсер строки
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