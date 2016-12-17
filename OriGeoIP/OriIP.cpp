/********************************************************/
/*  Протокол OriTcp 2015 (c)                            */
/*  Создатель Сергей Щербаков                           */
/********************************************************/

#include "OriIP.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define SEC 1000

#else

#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SEC 1
#endif

#pragma warning(disable : 4996)

#define SYMBOL_END_IP_ZONE '.'

OriIP::OriIP() {
	Clear();
}

OriIP& OriIP:: operator = (char *ip_src) {
	Clear();
	strcpy(this->buf_ip, ip_src);
	return *this;
}



OriIP& OriIP::operator=(OriIP *ip)	{
	Clear();
	strcpy(this->buf_ip, ip->buf_ip);
	return *this;
}

bool OriIP:: operator == (OriIP &src) {
	if (strcmp(this->buf_ip, src.buf_ip) == 0)
		return true;
	return false;
}

int OriIP::check_parcer_int(char *src, int type) {
	int  lenIP = strlen(src);
	int  count_zone = 0;
	char parcerBuf[4];

	for (int i = 0; i < lenIP; i++) {
		if (count_zone == type) {
			int w = 0;
			while (src[i + w] != SYMBOL_END_IP_ZONE) {
				parcerBuf[w] = src[i + w];
				w++;

				if ((w == 3) || (src[i + w] == '\x00')) {
					parcerBuf[w] = '\x00';
					return atoi(parcerBuf);
				}
			}

			parcerBuf[w] = '\x00';
			return atoi(parcerBuf);
		}

		if (src[i] == SYMBOL_END_IP_ZONE)
			count_zone++;
	}

	return 0;
}

bool OriIP:: operator > (OriIP &src) {
	int my  = check_parcer_int(this->buf_ip, 0);
	int you = check_parcer_int(src.buf_ip, 0);

	if (my > you)
		return true;

	else if (my < you)
		return false;
	else {
		my = check_parcer_int(this->buf_ip, 1);
		you = check_parcer_int(src.buf_ip, 1);

		if (my > you)
			return true;
		else if (my < you)
			return false;
		else {
			my = check_parcer_int(this->buf_ip, 2);
			you = check_parcer_int(src.buf_ip, 2);

			if (my > you)
				return true;

			else if (my < you)
				return false;
			else {
				my = check_parcer_int(this->buf_ip, 3);
				you = check_parcer_int(src.buf_ip, 3);

				if (my > you)
					return true;
				else
					return false;
			}

		}
	}
	return false;
}

bool OriIP:: operator < (OriIP &src) {
	if (this->operator == (src))
		return false;
	return !this->operator > (src);
}

void OriIP::Clear() {
	this->buf_ip[0] = '\x0';
}

bool OriIP::HostName(char *DomainName)
{
	struct hostent *remoteHost;
	struct in_addr addr;
	
#ifdef _WIN32 
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)	{
		return false;
	}
#endif

	remoteHost = gethostbyname(DomainName);

	if (remoteHost == NULL)
		return false; else

    addr.s_addr = *(u_long *)remoteHost->h_addr_list[0];

	Clear();

	strcpy(this->buf_ip, inet_ntoa(addr));
	return true;	
}


void OriIP::Compile()
{
	this->A = check_parcer_int(this->buf_ip, 0);
	this->B = check_parcer_int(this->buf_ip, 1);
	this->C = check_parcer_int(this->buf_ip, 2);
	this->D = check_parcer_int(this->buf_ip, 3);
}


bool OriIP:: operator >> (OriIP &src) {

	if (this->A > src.A)
		return true; else if (this->A != src.A) return false;

	if (this->B > src.B)
		return true; else if (this->B != src.B) return false;

	if (this->C > src.C)
		return true; else if (this->C != src.C) return false;

	if (this->D > src.D)
		return true; else if (this->D!= src.D)  return false;

	return false;
}

bool OriIP:: operator << (OriIP &src) {
	if ((this->A == src.A) && (this->B == src.B) && (this->C == src.C) && (this->D == src.D)) return false;
	return !this->operator >> (src);
}