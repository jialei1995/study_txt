#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>

#define LEN 32
static char buf[LEN];
static int fd;

void handler(int signo)
{
	printf("got signal %d\n", signo);
	read(fd, buf, LEN-1);// 
	buf[LEN-1] = '\0';
	printf("buf = %s\n", buf);
}
int main()
{
	int oflags;
	fd = open("/dev/hello0", O_RDWR);   
	signal(SIGIO, handler);				// 
	fcntl(fd, F_SETOWN, getpid());	//
	oflags = fcntl(fd, F_GETFL);    //
	fcntl(fd, F_SETFL, oflags | FASYNC); //
	while(1); 
	return 0;
}



