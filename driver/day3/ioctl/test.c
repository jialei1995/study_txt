#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mycmd.h"
#include <stdio.h>
#include <string.h>
int main()
{
	int adcval;
	int fd;
	char buf[10] = {0};
	fd = open("/dev/hello0",O_RDWR);//这个文件是已有的设备，O_CREAT才是创建，现在打开设备操作他
	perror("open:");
#if 0
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
#endif
	while(1)
	{
		getchar();
		adcval = ioctl(fd,READ_ADC);
		printf("v=%d\n",adcval);
		
	}
	close(fd);
	return 0;
}
