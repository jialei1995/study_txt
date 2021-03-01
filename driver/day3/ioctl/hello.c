#include <linux/init.h>
#include <linux/kernel.h> 
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include "mycmd.h"
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#define KBUF_MAX 10
MODULE_LICENSE("GPL");
int devno;
int ma;
int mi = 100;
int count = 5;
struct cdev *pdev;
struct class *pcls;
struct device*pdevice;
char kbuf[KBUF_MAX] = "abcdefh";
void * adcdat,*adcmux,*adccon;
//先定义函数再写struct file_operations

int hello_open(struct inode* node,struct file*f)
{
	unsigned int tmp;
	iowrite32(3,adcmux);//其他位没影响，直接wrote就好
	tmp = ioread32(adccon);//初始化的adccon  adcmux 数据寄存器是用来读的不用初始化
	tmp = tmp &~(1<<16);//10bit fenbianlv
	tmp = tmp |(1<<14);//enable yufenpin
	tmp = tmp &~(0xff<<6)|(19<<6);//这几位默认是0xff，所以先都置0
	tmp = tmp &~(1<<2);//正常模式，还有个stand by mode
	tmp = tmp &~(1<<1);//有2中启动方式，0bit与1bit，   0bit与14bit配和
	iowrite32(tmp,adccon);
	printk("chushihua over\n");
	return 0;
}
int hello_close(struct inode*node,struct file*f)
{
	printk("in close\n");
	return 0;
}
ssize_t hello_read(struct file*f,char __user*ubuf,size_t size,loff_t * pos)
{
	ssize_t num = 0;
	if(size>KBUF_MAX)//用户要读的比内核空间的buf大
		size = KBUF_MAX;
	num  = copy_to_user(ubuf,kbuf,size);
	
	printk("in read ubuf=%s\n",ubuf);
	return num;//为什么要return num
}
ssize_t hello_write(struct file* f, const char __user* ubuf,size_t size,loff_t *pos)
{
	ssize_t num = 0;
	if(size>KBUF_MAX)
		size = KBUF_MAX;
	num = copy_from_user(kbuf,ubuf,size);
	printk("in write kbuf=%s\n",kbuf);
	return num;
}
long hello_ioctl(struct file *f, unsigned int cmd, unsigned long param)
{
	//unsigned int * ledcon,*leddata;//为啥这里就用uint*，adc的寄存器用void*
	unsigned int tmp;
#if 0    可以在真正ioctl控制之前初始化，可以在init函数初始化，也可以在open设备的时候初始化
	ledcon = ioremap(0x11000c40,4);
	leddata = ioremap(0x11000c44,4);//完成地址映射
	tmp = ioread32(ledcon);
	tmp = tmp & ~(0xf << 28);  //读数据，更改，写回去
	tmp = tmp | ( 1 << 28);    //0001 0000 0000 0000
	iowrite32(tmp,ledcon); 
#endif
	printk("cmd = %d\tparam = %ld\n",cmd,param);
	switch(cmd)
	{
#if 0
		case LED_ON:
			tmp = ioread32(leddata);
			tmp = tmp | ( 1<<7);
			iowrite32(tmp,leddata);	
			printk("led on\n");
			break;
		case LED_OFF:
			tmp = ioread32(leddata);
			tmp = tmp & ~( 1<<7);
			iowrite32(tmp,leddata);
			printk("led OFF\n");
			break;
		case LED_BLINK:
			printk("led BLINK\n");
			break;
#endif
		case READ_ADC:
			tmp = ioread32(adccon);
			tmp = tmp|1;//我们采用0bit启动，置1启动后自动清零
			iowrite32(tmp,adccon);
			while(1)
			{
				tmp = ioread32(adccon);//是否end conver
				if(tmp & (1<<15))break;
			}
			tmp = ioread32(adcdat);
			return tmp&0x3ff;
	}
	return 0;
}
struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.read = hello_read,			//定义一个函数，然后给函数写入自己的方法
	.write = hello_write,
	.open = hello_open,
	.release = hello_close,
	.unlocked_ioctl = hello_ioctl,
};
int hello_init(void)
{
	int ret;int index;
	ret = alloc_chrdev_region(&devno,mi,count,"hello");//动态创建设备号,有了设备，就有了inode结构体，inode结构提里面包含cdev结构提指针
	if(ret < 0)
	{
		goto REGISTER_ERROR;
	}
	ma = MAJOR(devno);
	pdev = cdev_alloc();//inode里面只有1个指针，现在要为他分配空间了
	if(pdev == NULL)
	{
		goto ALLOC_ERROR;
	}
	cdev_init(pdev,&hello_ops);//cdev创建好，先初始化,将自己定义的ops函数与底层函数关联起来
	ret = cdev_add(pdev,devno,count);//将cdev与设备号关联，插入内核
	if(ret < 0)
		goto ALLOC_ERROR;
	pcls = class_create(THIS_MODULE,"hello");//类 创建成功再创建设备文件
	if(IS_ERR(pcls))
		goto CLASS_CRE_ERROR;
	for(index = 0;index < count;index++)
 	{
		pdevice = device_create(pcls,NULL,MKDEV(ma,mi+index),NULL,"hello%d",index);
		if(IS_ERR(pdevice))
			goto DEVICE_CRE_ERROR;
	}
	adccon = ioremap(0x126c0000,4);
	adcdat = ioremap(0x126c000c,4);
	adcmux = ioremap(0x126c001c,4);


	printk("hello,init_modules\n");
	return 0;
DEVICE_CRE_ERROR:
	for(index = 0;index < count;index++)
		device_destroy(pcls,MKDEV(ma,mi+index));//这里就是销毁pcls
CLASS_CRE_ERROR:
	class_destroy(pcls);//销毁类
ALLOC_ERROR://顺序执行，跳到哪里，一直往后执行
	cdev_del(pdev);
REGISTER_ERROR://只是个标号，不会打断操作
	unregister_chrdev_region(devno,count);
	return -1;
}
void hello_cleanup(void)
{
	int index;
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	cdev_del(pdev);
	unregister_chrdev_region(devno,count);
	printk("exit module_hello\n");
}
module_init(hello_init);
module_exit(hello_cleanup);
