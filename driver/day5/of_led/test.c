#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mycmd.h"
#include <stdio.h>
#include <string.h>
int main()
{
	int fd;
	fd = open("/dev/hello0",O_RDWR);
	perror("open:");

	while(1)
	{
		ioctl(fd,LED_ON,1);
		sleep(1);
		ioctl(fd,LED_OFF,2);
		ioctl(fd,LED1_ON,3);
		sleep(1);
		ioctl(fd,LED1_OFF,4);
	}
		
	close(fd);
	return 0;
	
}
