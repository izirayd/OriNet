#ifndef OriIndexPCH
#define OriIndexPCH

#include "../OriKernel/KernelPlatform.h"
#include "../OriCrypt/OriCrypt.h"

#ifdef ORI_LINUX
 #include <sys/ioctl.h>
 #include <linux/hdreg.h>
 #include <unistd.h>
#endif

class OriIndexPC
{
public:
	OriIndexPC() { }
	bool Init()
	{
		HashPC[0] = 0x00;
		isStatus = false;
        char  SerialNumberStr[64] = { 0 };
		
#ifdef ORI_WINDOWS

		DWORD SerialNumber = 0;
		

		if (GetVolumeInformation(NULL, NULL, NULL, &SerialNumber, NULL, NULL, NULL, NULL))
		{
			isStatus = true;
			sprintf(SerialNumberStr, "Windows%u", SerialNumber);
			SHA256Crypt(SerialNumberStr, HashPC);
		}
		else isStatus = false;
		
#else
		int fd;
		struct hd_driveid *id;

		if ((fd = open("/dev/sda", O_RDONLY | O_NONBLOCK)) < 0)
			isStatus = false;
		else
		{
			if (ioctl(fd, HDIO_GET_IDENTITY, id) == 0)
			{			
			  if ((id->config & (1 << 7)) || (id->command_set_1 & 4)) {
                isStatus = true;
				sprintf(SerialNumberStr, "Linux%s", id->serial_no);
				SHA256Crypt(SerialNumberStr, HashPC);
             } else isStatus = false;			
			}
			else 	isStatus = false;

			close(fd);
		}

#endif

		return isStatus;
	}

	char *GetHashPC()  { return HashPC; }
	bool  GetStatus()  { return isStatus; }

private:
	char HashPC[65];
	bool isStatus;

};

#endif