ifeq ($(KERNELRELEASE),)
#KERNELDIR ?= /lib/modules/$(shell uname -r)/build   
KERNELDIR ?= /home/farsight/rootfs/lib/modules/3.14.0/build   
PWD := $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
else
  obj-m := hello.o			防止递归，obj-m的名字不能与下面的名字有重复
  hello-objs := hehe0.o hehe1.o  
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* modules*


