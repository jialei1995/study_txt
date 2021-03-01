//操作集的定义，设备文件的创建
#include <linux/init.h>
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

unsigned int muxval=0;
void *ledcon,*leddat,*led1con,*led1dat;
void led_init(void)
{
	unsigned int temp;
	temp = ioread32(ledcon);
	temp &= ~(0xf<<28);
	temp |= (0x1<<28);
	iowrite32(temp,ledcon);
	
	
	temp = ioread32(led1con);
	temp &= ~(0xf);
	temp |= 0x01;
	iowrite32(temp,led1con);
	
}
void ledon(void)
{
	unsigned int temp;
	temp = ioread32(leddat);
	temp |= (0x1<<7);
	iowrite32(temp,leddat);
}
void ledoff(void)
{
	unsigned int temp;
	temp = ioread32(leddat);
	temp &= ~(0x1<<7);
	iowrite32(temp,leddat);
}

void led1on(void)
{
	unsigned int temp;
	temp = ioread32(led1dat);
	temp |= 0x1;
	iowrite32(temp,led1dat);
}
void led1off(void)
{
	unsigned int temp;
	temp = ioread32(led1dat);
	temp &= ~(0x1);
	iowrite32(temp,led1dat);
}
int  hello_open(struct inode *node, struct file *f)//***********8可以屏蔽吗
{
	led_init();
	printk("now, in hello_open\n");
	return 0;
}
int hello_close(struct inode *node, struct file *f)
{
	printk("now, in hello_close\n");
	return 0;
}

long hello_ioctl(struct file *f, unsigned int cmd, unsigned long para)
{
	printk("cmd = %d\t, para = %ld\n",cmd, para);
	switch(cmd)
	{
		case READ_ADC:
			
			while(1);
			break;
			
		case LED_ON:
			ledon();
			break;
		case LED_OFF:
			ledoff();
			break;
		case LED1_ON:
			led1on();
			break;
		case LED1_OFF:
			led1off();
			break;
		case LED_BLINK:
			printk("led is blink\n");
			break;
	}

	return 0;
}
struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.open = hello_open, 
	.release = hello_close, 
	.unlocked_ioctl = hello_ioctl, 
};
int  hello_init(struct platform_device *pDevice)//利用带过来的pdevice信息进行初始化
{
	int ret;
	int index;


	for(index = 0; index < pDevice->num_resources; index++)
	{
		printk("%d resource :  start = %x, end = %x\n", index,pDevice->resource[index].start, pDevice->resource[index].end);
	}

	ret = alloc_chrdev_region(&devno, mi,count,"hello");//动态设备号申请，devno只是该类第一个设备号
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

	pcls = class_create(THIS_MODULE, "hello");  //为了批量创建设备文件
	if(IS_ERR(pcls))
		goto CLASS_CRE_ERR;
	 
	for(index = 0; index < count; index++)
	{
		pdevice = device_create(pcls, NULL,MKDEV(ma, mi+index), NULL, "hello%d",index); 
		if(IS_ERR(pdevice))
			goto DEVICE_CRE_ERR;
	}

	ledcon = ioremap(pDevice->resource[0].start, 4);
	leddat = ioremap(pDevice->resource[1].start, 4);
	led1con = ioremap(pDevice->resource[2].start, 4);
	led1dat = ioremap(pDevice->resource[3].start, 4);

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

void hello_cleanup(struct platform_device *pDevice)//drv卸载的时候会调用
{
	int index;
	iounmap(ledcon);
	iounmap(ledoff);
	iounmap(led1con);
	iounmap(led1off);
	
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	cdev_del(pdev); // 从内核移除cdev 结构体， 并释放内存
	unregister_chrdev_region(devno,count); //注销设备号
	printk("hello, cleanup_module\n");
}
