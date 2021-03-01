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
void *ledcon, *leddata;
int  ledpin=0;
void ledinit(void)
{
	unsigned int tmp;
	tmp = ioread32(ledcon);
	tmp = tmp & ~(0xf << ledpin*4);
	tmp = tmp | (1 << ledpin*4);
	iowrite32(tmp, ledcon);
	
}

void led2on(void )
{
	iowrite32(ioread32(leddata)|(1<<ledpin) ,  leddata);	
}

void led2off(void)
{
	iowrite32(ioread32(leddata) & ~(1 << ledpin),leddata);
}
int  hello_open(struct inode *node, struct file *f)
{
	ledinit();

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

long hello_ioctl(struct file *f, unsigned int cmd, unsigned long para)
{
//	unsigned int tmp;
	
	printk("cmd = %d\t, para = %ld\n",cmd, para);
	switch(cmd)
	{
		case READ_ADC:
			break;
			
		case LED_ON:
			led2on();
			printk("led is on\n");
			break;
		case LED_OFF:
			led2off();
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
int  hello_init(struct platform_device *pDevice)//利用带过来的pdevice信息进行初始化
{
	int ret;
	int index;
	int i;
	ret = of_property_read_u32(pDevice->dev.of_node, "pin", &ledpin);// 这个dtb中的管脚参数是自己定义的，所以要用函数去获取
	printk("ledpin is %d\n",ledpin);

	for(i = 0; index < pDevice->num_resources; i++)
	{
		printk("%d resource :  start = %x, end = %x\n", i,pDevice->resource[index].start, pDevice->resource[index].end);
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
	leddata = ioremap(pDevice->resource[1].start, 4);

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
	iounmap(leddata);
	iounmap(ledcon);
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	cdev_del(pdev); // 从内核移除cdev 结构体， 并释放内存
	unregister_chrdev_region(devno,count); //注销设备号
	printk("hello, cleanup_module\n");
}
