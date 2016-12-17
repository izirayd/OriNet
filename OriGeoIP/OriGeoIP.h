/********************************************************/
/*  Протокол OriTcp 2015 (c)                            */
/*  Создатель Сергей Щербаков                           */
/********************************************************/

#pragma once
#include "OriIP.h"

struct CacheIP
{
	int CountUse;
	GeoIP m_GeoIP;
};

#define COUNT_CACHEIP 100

class OriGeoIP
{
public:
	OriGeoIP();
	~OriGeoIP();
	
	bool InitGeo();

	void  Search(OriIP& IP);

	GeoIP *BaseIP;
	GeoL  *BaseRegion;

	int   CountRegion;
	int	  CountIP;

	GeoL  InfoRegion;
	GeoIP Info;
	GeoIP CashIP;

private:
	void  ReadGeoBinFile();
	void  ReadBlock(std::fstream &A, GeoIP &m_GeoIP);

	void  ReadRegionBinFile();
	void  ReadBlockRegion(std::fstream &A, GeoL &m_GeoL);

	void Search(GeoIP &m_GeoIP);
};