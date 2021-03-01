#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mycmd.h"
#include <stdio.h>
int main()
{
	int fd;
	char buf[10];
	fd = open("/dev/hello",O_RDWR);
	perror("open:");
	getchar();
	read(fd, buf, 10);
	printf("read finished\n");
	getchar();
	write(fd,"123", 3);
	printf("write finished\n");
	getchar();
	ioctl(fd, LED_ON, 123);
	printf("ioctl led_on finished\n");
	getchar();
	ioctl(fd, LED_OFF, 1123);
	printf("ioctl led_off finished\n");
	getchar();
	ioctl(fd, LED_BLINK, 12345);
	printf("ioctl led_blink finished\n");
	getchar();

	close(fd);
	return 0;
	
}
