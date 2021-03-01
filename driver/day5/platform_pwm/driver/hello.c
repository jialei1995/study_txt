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

void *pwmcon, *pwmtcfg0, *pwmtcfg1,*pwmtcntb0,*pwmtcmptb0,*pwmtcon;

int hello_open(struct inode *node, struct file *f)
{
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
	unsigned int temp;
	
	printk("cmd = %d\t, para = %ld\n",cmd, para);
	switch(cmd)
	{
		case READ_ADC:
			
			printk("begin xiang\n");
			temp = ioread32(pwmcon);
			temp |= 0x2;
			iowrite32(temp,pwmcon);
			//
			temp = ioread32(pwmtcfg0);
			temp = temp &~(0XFF) |24;
			iowrite32(temp,pwmtcfg0);
			//2JI FENPIN 4
			temp = ioread32(pwmtcfg1);
			temp = temp &~(0XF) | 0X2;
			iowrite32(temp,pwmtcfg1);
			
			//2500的波形个数实际就设置了输出的波形是400hz，1000000/400=2500。2500个1m的波形才能产生1个400hz的波形
			temp = ioread32(pwmtcntb0);
			temp = 2500;
			iowrite32(temp,pwmtcntb0);
		
			//2500*1/2---实际就设置了占空比为1/2
			temp = ioread32(pwmtcmptb0);
			temp = 1250;
			iowrite32(temp,pwmtcmptb0);
			
			temp = ioread32(pwmtcon);
			temp = temp |1<<3 |1<<1 |1  ;
			iowrite32(temp,pwmtcon);
			
			temp = ioread32(pwmtcon);
			temp = temp &~(1<<1) ;
			iowrite32(temp,pwmtcon);
			 
			while(1);
			return 0;
			
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
	for(i = 0; i < pDevice->num_resources; i++)
	{
		printk("%d resource :  start = %x, end = %x\n", i,pDevice->resource[i].start, pDevice->resource[i].end);
	}
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
	pwmcon = ioremap(pDevice->resource[0].start, 4);
	pwmtcfg0 = ioremap(pDevice->resource[1].start, 4);
	pwmtcfg1 = ioremap(pDevice->resource[2].start, 4);
	pwmtcntb0 = ioremap(pDevice->resource[3].start, 4);
	pwmtcmptb0 = ioremap(pDevice->resource[4].start, 4);
	pwmtcon = ioremap(pDevice->resource[5].start, 4);

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

	iounmap(pwmcon);
	iounmap(pwmtcfg0);
	iounmap(pwmtcfg1);
	iounmap(pwmtcntb0);
	iounmap(pwmtcmptb0);
	iounmap(pwmtcon);
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	cdev_del(pdev); // 从内核移除cdev 结构体， 并释放内存
	unregister_chrdev_region(devno,count); //注销设备号
	printk("hello, cleanup_module\n");
}
