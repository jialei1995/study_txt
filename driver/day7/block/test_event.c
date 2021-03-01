#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "mycmd.h"
#include <unistd.h>
#define BUF_SIZE  16

int main()
{
	char *buf;
	pid_t pid=0;
	//char buf[BUF_SIZE] = {0};
	int count=9;
	int len;
	buf = (char*)malloc(10);
	memset(buf,0,10);

	int fd = open("/dev/hello0", O_RDWR);
	if (fd == -1 ) {
		perror("open");
		printf("error: can't open file /dev/hahaha\n");
		return -1;
	}

	pid =fork();
	if(pid < 0)
	{
		printf("fork error, pid = %d\n",pid);
		return -1;
	}
	if(pid == 0)
	{
		
		printf("read from module, starting\n");
		count = 8;
		len = read(fd, buf, count);

		printf("read from module, buf = %s\n",buf);
		exit(0);
	}
	else
	{
		sleep(3);
		printf("wirte starting\n");
		buf = "abcdefg";

		len = write(fd, buf, count);
		
		printf("wirte finished\n");
		wait(0);
	}
	
	close(fd);	

	return 0;
}
