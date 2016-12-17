// ---------------------------------------------------------------------------

#pragma hdrstop
#include "OriTime.h"
#include "../OriUntil/OriSleep.h"

#pragma warning(disable : 4996)

ORI_THREAD_FOR_CLASS(OriTime);
void OriTime::Run() 
{
    while (true) 
	{
     BuildTime();
     OriSleep(1);
    }
}

void OriTime::BuildTime() {

	TimeMutex.lock();

#if defined _WIN32
    _strdate(this->dateStr);
    _strtime(this->timeStr);

    // Это ужасно, да.. -_-
	char result[3] = { 0 };

    memset(result, '\0', 2);

    result[0] = this->dateStr[0];
    result[1] = this->dateStr[1];
    this->OriTm.mon = atoi(result);

    result[0] = this->dateStr[3];
    result[1] = this->dateStr[4];
    this->OriTm.day = atoi(result);

    result[0] = this->dateStr[6];
    result[1] = this->dateStr[7];
    this->OriTm.year = atoi(result);

    result[0] = this->timeStr[0];
    result[1] = this->timeStr[1];
    this->OriTm.hour = atoi(result);

    result[0] = this->timeStr[3];
    result[1] = this->timeStr[4];
    this->OriTm.min = atoi(result);

    result[0] = this->timeStr[6];
    result[1] = this->timeStr[7];
    this->OriTm.sec = atoi(result);

#else
    time(&t);
    tm = localtime(&t);

    this->OriTm.mon  = tm->tm_mon + 1;
    this->OriTm.day  = tm->tm_mday;
	this->OriTm.year = tm->tm_year + 1900 - 2000;
    this->OriTm.hour = tm->tm_hour;
    this->OriTm.min  = tm->tm_min;
    this->OriTm.sec  = tm->tm_sec;
#endif

	memset(LocalTime, 0x00, 100);

	if (OriTm.mon < 10)
		sprintf(LocalTime
		  ,"20%d-0%d", OriTm.year, OriTm.mon);
	else 
		sprintf(LocalTime
		  ,"20%d-%d", OriTm.year, OriTm.mon);

	if (OriTm.day < 10)
		sprintf(LocalTime
		 + strlen(LocalTime), "-0%d", OriTm.day);
	else 
		sprintf(LocalTime
		 + strlen(LocalTime), "-%d", OriTm.day);


	if (OriTm.hour < 10)
		sprintf(LocalTime
		+ strlen(LocalTime), " 0%d", OriTm.hour);
	else
		sprintf(LocalTime
		+ strlen(LocalTime), " %d", OriTm.hour);

	if (OriTm.min < 10)
		sprintf(LocalTime
		+ strlen(LocalTime), "::0%d", OriTm.min);
	else
		sprintf(LocalTime
		+ strlen(LocalTime), "::%d", OriTm.min);


	if (OriTm.sec < 10)
		sprintf(LocalTime
		+ strlen(LocalTime), "::0%d", OriTm.sec);
	else
		sprintf(LocalTime
		+ strlen(LocalTime), "::%d", OriTm.sec);


	TimeMutex.unlock();
}


