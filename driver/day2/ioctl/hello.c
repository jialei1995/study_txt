#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include "mycmd.h"
#include <linux/device.h>
#include <linux/uaccess.h>
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
//先定义函数再写struct file_operations

int hello_open(struct inode* node,struct file*f)
{
	printk("in open\n");
	return 0;
}
int hello_close(struct inode*node,struct file*f)
{
	printk("in close\n");
	return 0;
}
ssize_t hello_read(struct file*f,char __user*ubuf,size_t size,loff_t * pos)
{
	if(size>KBUF_MAX)
		size = KBUF_MAX;
	copy_to_user(ubuf,kbuf,size);
	
	printk("in read ubuf=%s\n",ubuf);
	return 0;
}
ssize_t hello_write(struct file* f, const char __user* ubuf,size_t size,loff_t *pos)
{
	if(size>KBUF_MAX)
		size = KBUF_MAX;
	copy_from_user(kbuf,ubuf,size);
	printk("in write kbuf=%s\n",kbuf);
	return 0;
}
long hello_ioctl(struct file *f, unsigned int cmd, unsigned long param)
{
	printk("cmd = %d\tparam = %ld\n",cmd,param);
	switch(cmd)
	{
		case LED_ON:
			printk("led on\n");
			break;
		case LED_OFF:
			printk("led OFF\n");
			break;
		case LED_BLINK:
			printk("led BLINK\n");
			break;
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
	cdev_del(pdev);
	unregister_chrdev_region(devno,count);
	printk("exit module_hello\n");
}
module_init(hello_init);
module_exit(hello_cleanup);
