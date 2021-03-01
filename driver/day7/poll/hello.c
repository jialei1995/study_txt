#include <linux/init.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include "mycmd.h"
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>
#include <linux/poll.h>
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
wait_queue_head_t   myqueue;//定义队列头

#define KBUF_MAX 10
char kbuf[KBUF_MAX] = "abcdefg";
int  canread = 0;// 0------ 表示设备空， 不能读     1----------- 设备可以读 
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
ssize_t hello_read (struct file *f, char __user *ubuf, size_t size, loff_t *pos)
{
	if(f->f_flags & O_NONBLOCK)//应用里打开的设备文件在虚文件系统层有对应的file*， 在驱动的操作方法里， 都带过来该file*
	{
		//应用里是非阻塞的操作
		if(canread == 0)
		{
			return -1;
		}
		
	}


	if(size > KBUF_MAX)
		size = KBUF_MAX;

	copy_to_user(ubuf,kbuf, size);
	
	canread = 0; //读完后， 该变量清零， 不可以再读
	
	printk("now , in hello_read\n");
	return 0;
}
ssize_t hello_write(struct file *f, const char __user *ubuf, size_t size, loff_t *pos)
{
	
	if(size > KBUF_MAX)
		size = KBUF_MAX;
	memset(kbuf,0,10);
	copy_from_user(kbuf, ubuf, size);



	canread = 1;
	wake_up(&myqueue);  //唤醒队列头， 就是让队列头的等待任务进入运行态
	
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



unsigned int hello_poll(struct file *f, struct poll_table_struct *tb)
{
	unsigned int  mask=0;

	poll_wait(f, &myqueue, tb);//添加当前的poll 到tb 里， 

	if(canread == 1)
	{
		mask |= POLLIN|POLLRDNORM;
	}
	else
	{
		
	}	

	return mask;
}



struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.read = hello_read,
	.write = hello_write, 
	.open = hello_open, 
	.release = hello_close, 
	.unlocked_ioctl = hello_ioctl,
	.poll = hello_poll,  
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

	init_waitqueue_head(&myqueue);//初始化队列头

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


void hello_cleanup(void)
{
	int index;
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	cdev_del(pdev); // 从内核移除cdev 结构体， 并释放内存
	unregister_chrdev_region(devno,count); //注销设备号
	printk("hello, cleanup_module\n");
}
module_init(hello_init)
module_exit(hello_cleanup)
