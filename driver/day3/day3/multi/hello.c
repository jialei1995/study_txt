#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include "mycmd.h"
#include <linux/device.h>
#include <linux/uaccess.h>
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

typedef struct {
	char buf0[10];
	char buf1[10];
	char buf2[10];
	char buf3[10];
	char buf4[10];
	struct cdev  multidev;
}MYDEV;
MYDEV mydev = {
	.buf0 = "abcdefg",
	.buf1 = "hijklmn",
	.buf2 = "opqrst",
	.buf3 = "uvwxya",
	.buf4 = "111222333",
};


#define KBUF_MAX 10
char kbuf[KBUF_MAX] = "abcdefg";
int hello_open(struct inode *node, struct file *f)
{
	printk(" 当前文件的设备号  major = %d, minor = %d\n", MAJOR(node->i_rdev), MINOR(node->i_rdev));

	switch(MINOR(node->i_rdev))
	{
		case 100+0:
			f->private_data = mydev.buf0;
			break;
		case 100+1:
			f->private_data = mydev.buf1;
			break;
		case 100+2:
			f->private_data = mydev.buf2;
			break;
		case 100+3:
			f->private_data = mydev.buf3;
			break;
		case 100+4:
			f->private_data = mydev.buf4;
			break;
			
	}
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
	if(size > KBUF_MAX)
		size = KBUF_MAX;

	copy_to_user(ubuf,f->private_data, size);
	
	printk("now , in hello_read\n");
	return 0;
}
ssize_t hello_write(struct file *f, const char __user *ubuf, size_t size, loff_t *pos)
{
	
	if(size > KBUF_MAX)
		size = KBUF_MAX;
	memset(kbuf,0,10);
	copy_from_user(f->private_data, ubuf, size);
	
	printk("now, in hello_write, kbuf = %s\n", kbuf);
	return 0;
}
long hello_ioctl(struct file *f, unsigned int cmd, unsigned long para)
{
	
	printk("cmd = %d\t, para = %ld\n",cmd, para);
	switch(cmd)
	{
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
int  hello_init(void)
{
	int ret;
	int index;
	ret = alloc_chrdev_region(&devno, mi,count,"hello");//动态设备号申请，
	if(ret < 0)
	{
		goto REGISTER_ERROR;
		
	}
	ma = MAJOR(devno);	
#if 0
	pdev = cdev_alloc();// 申请cdev 结构体 ，
	if(pdev == NULL)
		goto ALLOC_ERROR;
#endif
	cdev_init(&mydev.multidev,&hello_ops);//cdev 结构体初始化，让cdev 结构体 记住设备的操作

	ret = cdev_add(&mydev.multidev, devno, count);// cdev 记住自己的设备号， 注册该cdev 结构体 到内核 	
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




	printk("hello, init_modul`e\n");
	return 0;

DEVICE_CRE_ERR:
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
CLASS_CRE_ERR:
	class_destroy(pcls);
ALLOC_ERROR:
	cdev_del(&mydev.multidev);
REGISTER_ERROR:
	unregister_chrdev_region(devno,count); 
	return -1;
}


void hello_cleanup(void)
{
	int index;
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	//cdev_del(pdev); // 从内核移除cdev 结构体， 并释放内存
	cdev_del(&mydev.multidev);
	unregister_chrdev_region(devno,count); //注销设备号
	printk("hello, cleanup_module\n");
}
module_init(hello_init)
module_exit(hello_cleanup)
