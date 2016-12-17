#ifndef KernelSocketH
#define KernelSocketH

#include "../OriKernel/KernelPlatform.h"
#include "../OriGeoIP/OriIP.h"


#if defined ORI_WINDOWS
#define ORI_WSAINIT                                  \
  int iResult;                                       \
  WSADATA wsaData;                                   \
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);    \
  if (iResult != 0) return false;
#endif

#if defined ORI_WINDOWS

 #include "KernelWinAPI.h"

 #define OriSocket    SOCKET
 #define OriSocketLen int32_t
#define KernelCloseSocket(NumberSocket) closesocket(NumberSocket);

#endif

#if defined ORI_LINUX

 #define ORI_WSAINIT

 #define OriSocket int
 #define OriSocketLen socklen_t
 #define KernelCloseSocket(NumberSocket) close(NumberSocket);

	 #include <sys/socket.h>
	 #include <netinet/in.h>
	 #include <arpa/inet.h>
	 #include <netdb.h>
	 #include <unistd.h>
     #include <sys/types.h>
     #include <netinet/tcp.h>

#endif

typedef unsigned short ORI_ADDRESS_FAMILY;


#endif
