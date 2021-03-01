#include <linux/init.h>
#include <linux/mm.h>  
#include <linux/slab.h>
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
char *pkbuf;
#define KBUF_MAX 10
char kbuf[KBUF_MAX] = "abcdefg";
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
	if(size > KBUF_MAX)
		size = KBUF_MAX;
	copy_to_user(ubuf,kbuf, size);
	printk("now , in hello_read\n");
	return 0;
}
ssize_t hello_write(struct file *f, const char __user *ubuf, size_t size, loff_t *pos)
{
	
	if(size > KBUF_MAX)
		size = KBUF_MAX;
	printk(" pkbuf = %s\n", pkbuf);
	printk("now, in hello_write, pkbuf = %s\n", pkbuf);
	copy_from_user(pkbuf, ubuf, size);
	
	printk("now, in hello_write, pkbuf = %s\n", pkbuf);
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
//应用层传进来的mmap---->到内核成了vma结构体
int hello_mmap(struct file *f, struct vm_area_struct *vma)
{//vma 是内核传递来的用户空间的映射需求， 			多少页			end-start映射这么多
	remap_pfn_range(vma, vma->vm_start,virt_to_phys(pkbuf)>>12, vma->vm_end - vma->vm_start, vma->vm_page_prot);	// /4096
	return 0;						将将内核空间的pkbuf这个buf映射到用户空间了			1页4096byte
}
struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.read = hello_read,
	.write = hello_write, 
	.open = hello_open, 
	.release = hello_close, 
	.unlocked_ioctl = hello_ioctl,
	.mmap = hello_mmap, 
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



	pkbuf = kmalloc(4096, GFP_KERNEL);
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
	kfree(pkbuf);
	for(index = 0; index < count; index++)
		device_destroy(pcls, MKDEV(ma,mi+index));
	class_destroy(pcls);
	cdev_del(pdev); // 从内核移除cdev 结构体， 并释放内存
	unregister_chrdev_region(devno,count); //注销设备号
	printk("hello, cleanup_module\n");
}
module_init(hello_init)
module_exit(hello_cleanup)
