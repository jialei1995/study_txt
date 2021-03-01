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
	fd = open("/dev/hello",O_RDWR);//这个文件是已有的设备，O_CREAT才是创建，现在打开设备操作他
	perror("open:");
	getchar();
	read(fd,buf,10);
	printf("read over\n");
	getchar();
	write(fd,"hello",5);
	printf("write over\n");
	getchar();

	ioctl(fd,LED_ON,123);
	getchar();
	ioctl(fd,LED_OFF,123);
	getchar();
	ioctl(fd,LED_BLINK,123);
	close(fd);
	return 0;
}
