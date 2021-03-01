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
MODULE_DESCRIPTION("this is a ch340 driver, arm, litlle endian");
MODULE_AUTHOR("ZHANGSAN, 139xxxxxxxx, 123@qq.com");
MODULE_LICENSE("GPL");
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

int  hello_open(struct inode *node, struct file *f)
{
#if 0
	void adcinit()
{
		ADCMUX = 3;
		ADCCON = ADCCON & ~( 1 << 16);
		ADCCON = ADCCON | ( 1<<14);
		ADCCON = ADCCON & ~(0xff << 6) | (19 << 6);
		ADCCON = ADCCON & ~(1 << 2);
		ADCCON = ADCCON & ~(1 << 1);
}
#endif
#if 0
	unsigned int tmp;
	iowrite32(muxval, adcmux);
	tmp = ioread32(adccon);
	tmp = tmp & ~(1 << 16);
	tmp = tmp | ( 1<<14);
	tmp = (tmp & ~(0xff << 6)) | ( 19 << 6);
	tmp = tmp & ~(1 << 2);
	tmp = tmp & ~(1 << 1);
	iowrite32(tmp, adccon);
#endif

	printk("now, in hello_open\n");
	return 0;
}
int hello_close(struct inode *node, struct file *f)
{
	printk("now, in hello_close\n");
	return 0;
}
ssize_t hello_read (struct file *f, char __user *ubuf, size_t size, loff_t *pos)
{
	ssize_t len;
	if(size > KBUF_MAX)
		size = KBUF_MAX;

	len = copy_to_user(ubuf,kbuf, size);
	
	printk("now , in hello_read\n");
	return len;
}
ssize_t hello_write(struct file *f, const char __user *ubuf, size_t size, loff_t *pos)
{
	ssize_t len;
	
	if(size > KBUF_MAX)
		size = KBUF_MAX;
	memset(kbuf,0,10);
	len = copy_from_user(kbuf, ubuf, size);
	
	printk("now, in hello_write, kbuf = %s\n", kbuf);
	return len;
}
#if 0
int readadc()
{
	int ret;
	ADCCON = ADCCON | 1;
	while(!ADCDAT & ( 1 << 15)) ;                           // XXXXXXXXXXXXX1XXXXXXXXXXXXXXXXXX
	
	ret = ADCDAT & 0x3ff;
	return ret;
}
#endif

long hello_ioctl(struct file *f, unsigned int cmd, unsigned long para)
{
//	unsigned int tmp;
	
	printk("cmd = %d\t, para = %ld\n",cmd, para);
	switch(cmd)
	{
		case READ_ADC:
#if 0
			tmp = ioread32(adccon);
			tmp = tmp | 1;
			iowrite32(tmp, adccon);
			while(1)
			{
				tmp = ioread32(adccon);
				if(tmp & ( 1 << 15))
				{
					break;
				}
				
			}
			tmp = ioread32(adcdat);
			return tmp&0x3ff;
#endif
			break;
			
		case LED_ON:
			printk("led is on\n");
			break;
		case LED_OFF:
			printk("led is off\n");
			break;
		case LED_BLINK:
			printk("led is blink\n");
			break;
	}

	return 0;
}
struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.read = hello_read,
	.write = hello_write, 
	.open = hello_open, 
	.release = hello_close, 
	.unlocked_ioctl = hello_ioctl, 
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
	
	struct resource *pres,*pkres;
	int num;
	int i;
	pres = pDevice->resource;
	num = pDevice->num_resources;
#if 0
		muxval = *((unsigned int*)pDevice->dev.platform_data);
#endif
	for(i = 0; i < num; i++)
	{
		printk("%d resource :  start = %x, end = %x\n", i,pres[i].start, pres[i].end);
	}
//	printk("muxval = %d\n",  muxval);
	pkres = platform_get_resource(pDevice, IORESOURCE_IRQ, 0);
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
#if 0
	adccon = ioremap(pres[0].start, 4);
	adcdat = ioremap(pres[1].start, 4);
	adcmux = ioremap(pres[2].start, 4);
#endif
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
#if 0
	iounmap(adccon);
	iounmap(adcdat);
	iounmap(adcmux);
#endif
	free_irq(pDevice->resource[0].start, (void*)&keypara);
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	cdev_del(pdev); // 从内核移除cdev 结构体， 并释放内存
	unregister_chrdev_region(devno,count); //注销设备号
	printk("hello, cleanup_module\n");
}
//module_init(hello_init)
//module_exit(hello_cleanup)
