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
	int adcval;
	char buf[10] = {0};
	fd = open("/dev/hello0",O_RDWR);
	perror("open:");

	while(1)
	{
	ioctl(fd, LED_ON, 123);
	sleep(1);
	ioctl(fd, LED_OFF, 1123);
	sleep(1);
		
	}
	close(fd);
	return 0;
	
}
