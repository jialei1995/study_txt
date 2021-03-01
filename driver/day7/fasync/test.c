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
	char buf[10] = {0};
	fd = open("/dev/hello0",O_RDWR);
	perror("open:");
	memset(buf,0,10);
	strcpy(buf,"123");
	write(fd,buf, strlen(buf));
	printf("write finished\n");
	close(fd);
	return 0;
	
}
