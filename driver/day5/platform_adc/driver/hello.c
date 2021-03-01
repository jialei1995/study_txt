#include <linux/init.h>
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

void *adccon, *adcdat, *adcmux;
unsigned int muxval=0;

int hello_open(struct inode *node, struct file *f)
{
	unsigned int tmp;
	iowrite32(muxval, adcmux);//初始化的时候muxval就给值了
	tmp = ioread32(adccon);
	tmp = tmp & ~(1 << 16);
	tmp = tmp | ( 1<<14);
	tmp = (tmp & ~(0xff << 6)) | ( 19 << 6);
	tmp = tmp & ~(1 << 2);
	tmp = tmp & ~(1 << 1);
	iowrite32(tmp, adccon);

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
	unsigned int tmp;
	
	printk("cmd = %d\t, para = %ld\n",cmd, para);
	switch(cmd)
	{
		case READ_ADC:
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
			
	}

	return 0;
}
struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.open = hello_open, 
	.release = hello_close, 
	.unlocked_ioctl = hello_ioctl, 
};
int  hello_init(struct platform_device *pDevice)
{
	int ret;
	int index;
	
	int i;
	muxval = *((unsigned int*)pDevice->dev.platform_data);
	for(i = 0; i < pDevice->num_resources; i++)
	{
		printk("%d resource :  start = %x, end = %x\n", i,pDevice->resource[i].start, pDevice->resource[i].end);
	}
	printk("muxval = %d\n",  muxval);
    /**以下是设备号的申请**/
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
	/**以下是设备文件的创建**/
	pcls = class_create(THIS_MODULE, "hello");
	if(IS_ERR(pcls))
		goto CLASS_CRE_ERR;
	 
	for(index = 0; index < count; index++)
	{
		pdevice = device_create(pcls, NULL,MKDEV(ma, mi+index), NULL, "hello%d",index); 
		if(IS_ERR(pdevice))
			goto DEVICE_CRE_ERR;
	}
	/**根据传进来的pdevice对adc寄存器初始化**/
	adccon = ioremap(pDevice->resource[0].start, 4);
	adcdat = ioremap(pDevice->resource[1].start, 4);
	adcmux = ioremap(pDevice->resource[2].start, 4);


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
	iounmap(adccon);
	iounmap(adcdat);
	iounmap(adcmux);
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	cdev_del(pdev); // 从内核移除cdev 结构体， 并释放内存
	unregister_chrdev_region(devno,count); //注销设备号
	printk("hello, cleanup_module\n");
}
