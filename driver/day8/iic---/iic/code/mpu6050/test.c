#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "mpu6050.h"

#define LSB_PER_DEG (16.384) 
#define LSB_PER_G (16384.0) 

int main(int argc, const char *argv[])
{
	int fd;
	union mpu6050_data data; 
	
	fd = open("/dev/mpu6050", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	while(1) {
		ioctl(fd, GET_ACCEL, &data);
		printf("acceleration data: x = %04x, y = %04x, z = %04x\n", 
				data.accel.x, data.accel.y, data.accel.z);
#if 0
		printf("acceleration data: x = %f, y = %f, z = %f\n", 
				(*(short*)&data.accel.x)/LSB_PER_G,
				(*(short*)&data.accel.y)/LSB_PER_G,
				(*(short*)&data.accel.z)/LSB_PER_G);
#endif
		ioctl(fd, GET_GYRO, &data);
		printf("gyroscope data: x = %04x, y = %04x, z = %04x\n", 
				data.accel.x, data.accel.y, data.accel.z);
#if 0
		printf("gyroscope data: x = %f, y = %f, z = %f\n", 
				(*(short*)&data.accel.x)/LSB_PER_DEG, 
				(*(short*)&data.accel.y)/LSB_PER_DEG, 
				(*(short*)&data.accel.z)/LSB_PER_DEG);
#endif		

		ioctl(fd, GET_TEMP, &data);
#if 0
		printf("temp data = %04x\n", data.temp);
#endif
		printf("temp data = %f\n", (*(short*)&data.temp)/340.0+36.53);

		sleep(5);
	}

	close(fd);

	return 0;
}
