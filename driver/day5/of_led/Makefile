ifeq ($(KERNELRELEASE),)
#KERNELDIR ?= /lib/modules/$(shell uname -r)/build  
#KERNELDIR ?= /source/rootfs/lib/modules/3.14.0/build  
KERNELDIR ?= /home/linux/Downloads/linux-3.14 

PWD = $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
else
    obj-m := driver.o
    driver-objs := plat_driver.o hello.o
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* modules*


