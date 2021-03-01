#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mycmd.h"
#include <sys/ioctl.h>

#define BUF_SIZE  16

int main()
{
	int count;
	char buf[10] = {0};

	int fd = open("/dev/hehe0", O_RDWR);
	if (fd == -1 ) {
		perror("open");
		printf("error: can't open file /dev/haha\n");
		return -1;
	}
	
	getchar();

	count = 10;
	read(fd, buf, count);

	printf("read from module, buf = %s\n",buf);


	getchar();


	memset(buf,0,10);
	strcpy(buf, "1234567");
	write(fd, buf, strlen(buf));

	getchar();

	ioctl(fd,CMD_LED_ON, 0);

	getchar();
	ioctl(fd, CMD_LED_OFF, 0);

	getchar();

	close(fd);	

	return 0;
}
