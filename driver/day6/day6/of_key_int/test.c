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
#if 0
	getchar();
	read(fd, buf, 7);
	printf("read finished, buf = %s\n",buf);
	getchar();
	memset(buf,0,10);
	strcpy(buf,"123");
	write(fd,buf, strlen(buf));
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
#endif
	while(1)
	{
	ioctl(fd, LED_ON, 123);
	sleep(1);
	ioctl(fd, LED_OFF, 1123);
	sleep(1);

#if 0		
		getchar();
		adcval = ioctl(fd,READ_ADC);
		printf("adc result is %d\n",adcval);
		printf("vr voltage is %f\n",adcval*1.8/1023);
#endif		
	}
	close(fd);
	return 0;
	
}
