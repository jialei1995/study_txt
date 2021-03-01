#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#define BUF_SIZE  32
#define SIZE 4096

int main()
{
	char buf[BUF_SIZE] = {0};
	int count;
	char *addr;
	char *str = " a very good test!";
	int fd = open("/dev/hello0", O_RDWR);
	if (fd == -1 ) {
		perror("open");
		printf("error: can't open file /dev/hello2\n");
	}
	//从fd 进行内存的映射 ， 
	addr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,	映射过去可读可写
					MAP_SHARED, fd, 0);		是shared共享的

	if (addr == MAP_FAILED){
			perror("mmap");
			return -1;
	}
	//操作addr， 往内核中的buf中写个字符串 
	strncpy(addr, str, strlen(str)+1); 
	getchar();
	char *myqq = "123";
	count = write(fd,myqq,3);		//为了调用到这句话，能在显示ping看到printk(" pkbuf = %s\n", pkbuf);
	munmap(addr, SIZE);//解除映射 
	close(fd);	
	return 0;
}
