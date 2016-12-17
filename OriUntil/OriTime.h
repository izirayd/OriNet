// ---------------------------------------------------------------------------

#ifndef OriTimeH
#define OriTimeH

#include "../OriUntil/OriThread.h"
#include "../OriKernel/KernelSocket.h"
#include "../OriUntil/OriMutex.h"

#include <stdint.h>

struct sOriTime {

#undef min
	sOriTime() :
		mon(0), day(0), year(0), hour(0), sec(0), min(0) { 
		dateStr[0] = 0x00;
		timeStr[0] = 0x00;
	}


	char dateStr[10];
	char timeStr[10];

	/*
	    Y - год
		M - месяц
		D - день
		h - час
		m - минута
		s - секунда

	*/
	void GetFormat(char *BufferDate)
	{
		if (GetMon() < 10)
			sprintf(BufferDate
			, "20%d-0%d", GetYear(), GetMon());
		else
			sprintf(BufferDate
			, "20%d-%d", GetYear(), GetMon());

		if (GetDay() < 10)
			sprintf(BufferDate
			+ strlen(BufferDate), "-0%d", GetDay());
		else
			sprintf(BufferDate
			+ strlen(BufferDate), "-%d", GetDay());


		if (GetHour() < 10)
			sprintf(BufferDate
			+ strlen(BufferDate), " 0%d", GetHour());
		else
			sprintf(BufferDate
			+ strlen(BufferDate), " %d", GetHour());

		if (GetMin() < 10)
			sprintf(BufferDate
			+ strlen(BufferDate), "::0%d", GetMin());
		else
			sprintf(BufferDate
			+ strlen(BufferDate), "::%d", GetMin());


		if (GetSec() < 10)
			sprintf(BufferDate
			+ strlen(BufferDate), "::0%d", GetSec());
		else
			sprintf(BufferDate
			+ strlen(BufferDate), "::%d", GetSec());

	}



	void Now() { this->PresentTime(); }
	
	int32_t year;
	int32_t mon;
	int32_t day;
	int32_t hour;
	int32_t min;
	int32_t sec;

	void PresentTime() {
#if defined ORI_WINDOWS
	_strdate(this->dateStr);
	_strtime(this->timeStr);

	// Это ужасно, да.. -_-
	char result[3] = { 0 };

	memset(result, '\0', 2);

	result[0] = this->dateStr[0];
	result[1] = this->dateStr[1];
	mon = atoi(result);

	result[0] = this->dateStr[3];
	result[1] = this->dateStr[4];
	day = atoi(result);

	result[0] = this->dateStr[6];
	result[1] = this->dateStr[7];
	year = atoi(result);

	result[0] = this->timeStr[0];
	result[1] = this->timeStr[1];
	hour = atoi(result);

	result[0] = this->timeStr[3];
	result[1] = this->timeStr[4];
	min = atoi(result);

	result[0] = this->timeStr[6];
	result[1] = this->timeStr[7];
	sec = atoi(result);

#else
	struct tm *gm;
	struct tm *tm;
	time_t t;

	time(&t);
	tm = localtime(&t);

	mon  = tm->tm_mon + 1;
	day  = tm->tm_mday;
	year = tm->tm_year + 1900 - 2000;
	hour = tm->tm_hour;
	min  = tm->tm_min;
	sec  = tm->tm_sec;
#endif
	}

	int32_t GetYear() { return year; }
	int32_t GetMon()  { return mon;  }
	int32_t GetDay()  { return day;  }
	int32_t GetHour() { return hour; }
	int32_t GetMin()  { return min;  }
	int32_t GetSec()  { return sec;  }

	void SetYear(int32_t Value) { year = Value; }
	void SetMon (int32_t Value) { mon  = Value; }
	void SetDay (int32_t Value) { day  = Value; }
	void SetHour(int32_t Value) { hour = Value; }
	void SetMin (int32_t Value) { min  = Value; }
	void SetSec (int32_t Value) { sec  = Value; }


	void IsenOnSec(int32_t Sec)
	{
		Clear();
		sec = Sec;
	}

	void IsenOnMin(int32_t Min)
	{
		Clear();
		min = Min;
	}

	void IsenOnDay(int32_t Day)
	{
		Clear();
		day = Day;
	}

	void IsenOnHour(int32_t Hour)
	{
		Clear();
		hour = Hour;
	}

	void IsenOnMon(int32_t Mon)
	{
		Clear();
		mon = Mon;
	}

	void IsenOnYear(int32_t Year)
	{
		Clear();
		year = Year;
	}


	int CountDayInMon(int32_t _Mon)
	{
		return ((_Mon + _Mon / 8) | 30) - (4 / _Mon & 2);
	}

	int CountDayInMon(int32_t Mon, int32_t Year)
	{
		return 31
			-
			(3) * (Mon == 2)
			+
			(1) * (Mon == 2) * ((Year % 4 == 0) - (Year % 100 == 0) + (Year % 400 == 0))
			-
			(1) * (Mon == 4 || Mon == 6 || Mon == 9 || Mon == 11);
	}

	bool operator == (sOriTime &_Time)	{
		if ((this->sec == _Time.sec) && (this->min == _Time.min) && (this->hour == _Time.hour) && (this->day == _Time.day) && (this->mon == _Time.mon) && (this->year == _Time.year)) return true;
		return false;
	}

	bool operator == (sOriTime *_Time)	{
		if ((this->sec == _Time->sec) && (this->min == _Time->min) && (this->hour == _Time->hour) && (this->day == _Time->day) && (this->mon == _Time->mon) && (this->year == _Time->year)) return true;
		return false;
	}


	bool operator > (sOriTime &_Time)	{

		if (this == &_Time) return false;

		if (this->year != _Time.year)
			if (CheckYear(this->year, _Time.year)) return true; else return false;

		if (this->mon != _Time.mon)
			if (CheckMon(this->mon, _Time.mon, this->year, _Time.year)) return true; else return false;

		if (this->day != _Time.day)
			if (CheckDay(this->day, _Time.day))  return true; else return false;

		if (this->hour != _Time.hour)
			if (CheckHour(this->hour, _Time.hour)) return true; else return false;

		if (this->min != _Time.min)
			if (CheckMin(this->min, _Time.min))  return true; else return false;

		if (this->sec != _Time.sec)
			if (CheckSec(this->sec, _Time.sec))  return true; else return false;

		return false;
	}

	bool operator > (sOriTime *_Time)	{

		if (this == _Time) return false;

		if (this->year != _Time->year)
			if (CheckYear(this->year, _Time->year)) return true; else return false;

		if (this->mon != _Time->mon)
			if (CheckMon(this->mon, _Time->mon, this->year, _Time->year)) return true; else return false;

		if (this->day != _Time->day)
			if (CheckDay(this->day, _Time->day))  return true; else return false;

		if (this->hour != _Time->hour)
			if (CheckHour(this->hour, _Time->hour)) return true; else return false;

		if (this->min != _Time->min)
			if (CheckMin(this->min, _Time->min))  return true; else return false;

		if (this->sec != _Time->sec)
			if (CheckSec(this->sec, _Time->sec))  return true; else return false;

		return false;

	}

	bool operator < (sOriTime *_Time)   {
		if (this > _Time) return false; return true;
	}

	bool operator >= (sOriTime *_Time)	{

		if ((this->operator> (_Time)) || (this->operator== (_Time))) return true;

		return false;
	}

	bool operator <= (sOriTime *_Time)	{
		if ((this->operator< (_Time)) || (this->operator== (_Time))) return true;

		return false;
	}

	int GetOffset(int32_t _value, int32_t max, int32_t &residual)
	{
		// 60, 121
		// 121 / 60 = 2
		// 2 * 60 = 120; 121 - 120 = 1  residual = 1, result = 2

		int32_t Result = 0;
		if (_value >= max)
		{
			int32_t tmp;
			Result   = _value / max;
			tmp      = Result * max;
			residual = _value - tmp;
			if (_value == max)
				return 0; else return Result;
		}
		else
		{
			residual = _value;
			return 0;
		}
		return 0;
	}

	sOriTime& operator - (sOriTime &_Time) {

		this->sec  = this->sec  + (-1) * _Time.sec;
		this->min  = this->min  + (-1) * _Time.min;
		this->hour = this->hour + (-1) * _Time.hour;
		this->day  = this->day  + (-1) * _Time.day;
		this->mon  = this->mon  + (-1) * _Time.mon;
		this->year = this->year + (-1) * _Time.year;

		return *this;
	}

	sOriTime& operator = (sOriTime *_Time)
	{
		this->sec  = _Time->sec;
		this->min  = _Time->min;
		this->hour = _Time->hour;
		this->day  = _Time->day;
		this->mon  = _Time->mon;
		this->year = _Time->year;

		return *this;
	}

	sOriTime operator + (sOriTime &_Time) {

		sOriTime Result;
		Result = this;

		int32_t tmp = 0, tmp1 = 0, tmp2 = 0;

		// Секунды
		tmp = Result.sec + _Time.sec;
		tmp1 = GetOffset(tmp, 60, tmp2);
		Result.sec = tmp2;
		Result.min += tmp1;

		// Минуты
		tmp = Result.min + _Time.min;
		tmp1 = GetOffset(tmp, 60, tmp2);
		Result.min = tmp2;
		Result.hour += tmp1;

		// Часы
		tmp = Result.hour + _Time.hour;
		tmp1 = GetOffset(tmp, 24, tmp2);
		Result.hour = tmp2;
		Result.day += tmp1;

		// Дни
		tmp = Result.day + _Time.day;
		tmp1 = GetOffset(tmp, CountDayInMon(mon, year) + 1, tmp2);
		Result.day = tmp2;
		Result.mon += tmp1;

		// Месяц
		tmp = Result.mon + _Time.mon;
		tmp1 = GetOffset(tmp, 13, tmp2);
		Result.mon = tmp2;
		Result.year += tmp1;

		// Год
		Result.year += _Time.year;

		return Result;		
	}

	sOriTime& operator - (sOriTime *_Time) {

		this->sec  -= _Time->sec;
		this->min  -= _Time->min;
		this->hour -= _Time->hour;
		this->day  -= _Time->day;
		this->mon  -= _Time->mon;
		this->year -= _Time->year;

		return *this;
	}

	sOriTime& operator + (sOriTime *_Time) {

		this->sec  += _Time->sec;
		this->min  += _Time->min;
		this->hour += _Time->hour;
		this->day  += _Time->day;
		this->mon  += _Time->mon;
		this->year += _Time->year;

		return *this;
	}

	void Print()
	{
		char FormatBuffer[32];
		this->GetFormat(FormatBuffer);
		printf("\n%s", FormatBuffer);
	}

	void Clear()
	{
		year = 0;
		mon  = 0;
		hour = 0;
		day  = 0;
		min  = 0;
		sec  = 0;
	}

	// Проверка, какая секунда больше, если первая больше или ровна вернет true, если вторая, то false.
	bool CheckSec(int32_t _sec1, int32_t _sec2)    { if (_sec1 > _sec2) return true; return false; }
	bool CheckMin(int32_t _min1, int32_t _min2)    { if (CheckSec(60 * _min1, 60 * _min2)) return true; return false; }
	bool CheckHour(int32_t _hour1, int32_t _hour2) { if (CheckMin(60 * _hour1, 60 * _hour2)) return true; return false; }
	bool CheckDay(int32_t _day1, int32_t _day2)    { if (CheckHour(24 * _day1, 24 * _day2)) return true; return false; }
	bool CheckMon(int32_t _mon1, int32_t _mon2, int32_t _year1, int32_t _year2)   
	                                               { if (CheckDay(CountDayInMon(_mon1, _year1) * _mon1, CountDayInMon(_mon2, _year2) * _mon2)) return true; return false; }
	bool CheckYear(int32_t _year1, int32_t _year2) { if (CheckMon(12 * _year1, 12 * _year2, _year1, _year2)) return true; return false; }
};

// 2016:05:04:12:50:30 и 12:50:15

typedef sOriTime stOriTime;

class OriTime {

public:
	ThreadObj;
	OriBaseMutex TimeMutex;

    // Вернем полное текущее время.
    // Расчитываем время
    char *FullTime();
    void BuildTime();

    sOriTime OriTm;
	char LocalTime[100];

#if defined ORI_WINDOWS
    char dateStr[9];
    char timeStr[9];
#else
    struct tm *gm;
    struct tm *tm;
    time_t t;
#endif

};

// ---------------------------------------------------------------------------
#endif
