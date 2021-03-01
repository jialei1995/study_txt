#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include "mycmd.h"
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/platform_device.h>

int devno;
int ma;
int mi = 100;
int count = 5;
struct cdev *pdev;
struct class *pcls;
struct device *pdevice;

#define KBUF_MAX 10
char kbuf[KBUF_MAX] = "abcdefg";

//void *adccon, *adcdat, *adcmux;
unsigned int muxval=0;

/*
int  hello_open(struct inode *node, struct file *f)
{
	printk("now, in hello_open\n");
	return 0;
}

int hello_close(struct inode *node, struct file *f)
{
	printk("now, in hello_close\n");
	return 0;
}
*/


struct file_operations hello_ops = {
	.owner = THIS_MODULE,
};
int keypara = 100;
irqreturn_t  keyhandler(int no,  void *para)
{
	printk("no = %d, para = %d\n",no, *(int*)para);
	return IRQ_HANDLED;
}
int  hello_init(struct platform_device *pDevice)
{
	int ret;
	int index;
	
	struct resource *pres,*pkres;//这两个类型是一样的，都是struct资源
	int num;
	int i;
	pres = pDevice->resource;
	num = pDevice->num_resources;
	for(i = 0; i < num; i++)
	{
		printk("%d resource :  start = %x, end = %x\n", i,pres[i].start, pres[i].end);//这里是打印的所有资源，可能有寄存器....
	}
	pkres = platform_get_resource(pDevice, IORESOURCE_IRQ, 0); //这里是获取中断号，这个宏是一种提取。
	printk("pkres->start = %d, pkres->end = %d\n",pkres->start, pkres->end);
	
	ret = alloc_chrdev_region(&devno, mi,count,"hello");//动态设备号申请，
	if(ret < 0)
	{
		goto REGISTER_ERROR;
		
	}
	ma = MAJOR(devno);	
	pdev = cdev_alloc();// 申请cdev 结构体 ，
	if(pdev == NULL)
		goto ALLOC_ERROR;
	cdev_init(pdev,&hello_ops);//cdev 结构体初始化，让cdev 结构体 记住设备的操作

	ret = cdev_add(pdev, devno, count);// cdev 记住自己的设备号， 注册该cdev 结构体 到内核 	
	if(ret < 0)
		goto ALLOC_ERROR;

	pcls = class_create(THIS_MODULE, "hello");
	if(IS_ERR(pcls))
		goto CLASS_CRE_ERR;
	 
	for(index = 0; index < count; index++)
	{
		pdevice = device_create(pcls, NULL,MKDEV(ma, mi+index), NULL, "hello%d",index); 
		if(IS_ERR(pdevice))
			goto DEVICE_CRE_ERR;
	}
//中断注册
	ret = request_irq(pres[0].start, keyhandler, pres[0].flags, "key-int", (void*)&keypara);	
	printk("hello, init_modul`e\n");
	return 0;

DEVICE_CRE_ERR:
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
CLASS_CRE_ERR:
	class_destroy(pcls);
ALLOC_ERROR:
	cdev_del(pdev);
REGISTER_ERROR:
	unregister_chrdev_region(devno,count); 
	return -1;
}


void hello_cleanup(struct platform_device *pDevice)
{
	int index;
	free_irq(pDevice->resource[0].start, (void*)&keypara);
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	cdev_del(pdev); // 从内核移除cdev 结构体， 并释放内存
	unregister_chrdev_region(devno,count); //注销设备号
	printk("hello, cleanup_module\n");
}
