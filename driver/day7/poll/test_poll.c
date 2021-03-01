#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "mycmd.h"
#define BUF_SIZE  16

int main()
{
	char buf[10] = {0};
	int count,nfds,len;
	struct timeval timeout = {
		.tv_sec = 0,
		.tv_usec = 0,
	};
	fd_set rfds;  // 读文件描述符集
	int fd = open("/dev/hello0", O_RDWR);
	if (fd == -1 ) {
		perror("open");		printf("error: can't open file /dev/hahaha\n");		return -1;
	}
	FD_ZERO(&rfds); //读文件描述符集清零 
	FD_SET(fd,&rfds);            //置位我们要监视的文件 
	nfds = fd+1;//最多可以监视nfds 个文件
	while(1)
	{	
		select(nfds,&rfds,NULL, NULL,NULL);
		if(FD_ISSET(fd,&rfds))//读事件是fd即hello0的读事件，程序进入if
		{						//（读事件是*需要fd进行读*的事件，有人往fd里面写东西就会触发fd的读事件）
			count = 8;
			len = read(fd, buf, count);
			printf("buf = %s\n",buf);
		}
	} 
	close(fd);	
	return 0;
}


