#include "OriGeoIP.h"
#include "../OriUntil/OriFile.h"

void  OriGeoIP::ReadRegionBinFile()
{

	OriPath Path;
	char    PathFile[MAX_PATH] = { 0 };

	StrCpy(PathFile, Path.GetPathDir(L"\\BaseRegion.ori"));

	std::fstream FileInit;
	FileInit.open(PathFile, std::ios::binary | std::ios::in);
	FileInit.seekp(0, std::ios::beg);
	CountRegion = 0;
	OpenLC(FileInit, CountRegion);
	BaseRegion = new GeoL[CountRegion];

	for (int i = 0; i < CountRegion; i++)
		ReadBlockRegion(FileInit, BaseRegion[i]);

	FileInit.close();
}

void  OriGeoIP::ReadBlockRegion(std::fstream &A, GeoL &m_GeoL)
{
	OpenLC(A, m_GeoL.ID);
	OpenLC(A, m_GeoL.City);
	OpenLC(A, m_GeoL.SubRegion);
	OpenLC(A, m_GeoL.Region);
}

void OriGeoIP::ReadGeoBinFile() {

	OriPath Path;
	char    PathFile[MAX_PATH] = { 0 };

	StrCpy(PathFile, Path.GetPathDir(L"\\BaseIP.ori"));
	
	std::fstream FileInit;
	FileInit.open(PathFile, std::ios::binary | std::ios::in);
	FileInit.seekp(0, std::ios::beg);
	CountIP = 0;
	OpenLC(FileInit, CountIP);
	BaseIP = new GeoIP[CountIP];

	for (int i = 0; i < CountIP; i++) 
		ReadBlock(FileInit, BaseIP[i]);

	FileInit.close();
}

void OriGeoIP::ReadBlock(std::fstream &A, GeoIP &m_GeoIP)  {

	OpenLC(A, m_GeoIP.ID);
	
	int count_len = 0;
	A.read((char*)& count_len, 4);
	A.read((char*)& m_GeoIP.StartIP.buf_ip, count_len);
	m_GeoIP.StartIP.buf_ip[count_len] = '\0';
	m_GeoIP.StartIP.Compile();

	A.read((char*)& count_len, 4);
	A.read((char*)& m_GeoIP.EndIP.buf_ip, count_len);
	m_GeoIP.EndIP.buf_ip[count_len] = '\0';
	m_GeoIP.EndIP.Compile();
		
	OpenLC(A, m_GeoIP.Country);
}

void OriGeoIP::Search(GeoIP &m_GeoIP)  {
	for (int i = 0; i < CountRegion; i++)	
		if (m_GeoIP.ID == this->BaseRegion[i].ID)		
			this->InfoRegion = BaseRegion[i];	
}

void OriGeoIP::Search(OriIP & IP) {

	IP.Compile();

	if ((IP >> CashIP.StartIP) && (IP << CashIP.EndIP))	{
		Info = CashIP;
		return;
	}

	for (int i = 0; i < CountIP; i++)
	if ((IP >> BaseIP[i].StartIP) && (IP << BaseIP[i].EndIP))	{
		Info = BaseIP[i];
		CashIP = BaseIP[i];
		this->Search(Info);
		return;
	}
}

bool OriGeoIP::InitGeo() {
	ReadGeoBinFile();
	ReadRegionBinFile();
	return true;
}

OriGeoIP::OriGeoIP() {

}

OriGeoIP::~OriGeoIP() {
	delete[] this->BaseIP;
	delete[] this->BaseRegion;
}
