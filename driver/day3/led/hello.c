#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include "mycmd.h"
#include <linux/io.h>
dev_t major = 250;
dev_t minor = 0;
unsigned int count = 10;
struct cdev *pdev;
dev_t number;
struct class *pclass;
struct device *pdevice;
char kbuff[10]="abcdefg";
int haha_open(struct inode *p, struct file *file)
{
	printk("haha opened\n");
	return 0;

}
int haha_release(struct inode *p, struct file *file)
{
	printk("haha closed\n");
	return 0;

}
ssize_t haha_read(struct file *file, char __user *buf, size_t length, loff_t *pos)
{
	ssize_t num=0;
		
	//kbuff ---- > buf
	num = copy_to_user(buf,kbuff,length);
	printk("haha read...length = %d, num = %d\n",length, num);
	return num;
} 
ssize_t haha_write(struct file *file, const char __user *buf, size_t length, loff_t *pos)
{
	
	ssize_t num=0;

	//kbuff <----- buf
	if(length > sizeof(kbuff))
	{
		length = 10;
	}
	num = copy_from_user(kbuff,buf,length);
	printk("haha write ...length = %d, num = %d\n",length, num);
	printk("haha write  %s\n",kbuff);
	return num;
} 

long haha_ioctl(struct file *file, unsigned int cmd, unsigned long par)
{

	unsigned int *ledcon, *leddata;
	unsigned int tmp;
	ledcon = ioremap(0x11000c40,4);
	leddata = ioremap(0x11000c44, 4);

	tmp = ioread32(ledcon);
	tmp = tmp & ~(0xf << 28);  //0000 1111 1111 1111
	tmp = tmp | ( 1 << 28);    //0001 0000 0000 0000
	iowrite32(tmp,ledcon); 
	printk("cmd = %d\n", cmd);
#if 0
	unsigned int *ledcon = ioremap(11000400,4);
	unsigned int tmp;
	tmp = ioread32(ledcon);
	
	tmp = tmp | 1;
	iowrite32(tmp, ledcon);
#endif


	switch(cmd)
	{
		case CMD_LED_ON:
			tmp = ioread32(leddata);
			tmp = tmp | ( 1<<7);
			iowrite32(tmp,leddata);		
			printk("haha_ioctl,   led on ......\n");
			break;
		case CMD_LED_OFF:
			tmp = ioread32(leddata);
			tmp = tmp & ~( 1<<7);
			iowrite32(tmp,leddata);		
			printk("haha_ioctl,   led off ......\n");
			break;	
		default:
			printk("haha_ioctl, invalid command\n");
	}
	return 0;
}
struct file_operations hahaops = 
{
	.owner = THIS_MODULE,
	.open = haha_open,
	.release = haha_release,
	.read = haha_read,
	.write = haha_write,
	.unlocked_ioctl = haha_ioctl,
};
static int __init hello_init(void)
{
	int ret;
	int i;
//	ret = register_chrdev_region(MKDEV(250,minor), count, "haha");
	ret = alloc_chrdev_region(&number, minor,count, "haha");
	if(ret < 0)
		goto REGISTER_ERROR; 

	pdev = cdev_alloc();
	if(pdev == NULL)
		goto CDEV_ERROR;
	cdev_init(pdev, &hahaops);
	ret = cdev_add(pdev, number, count);
	if( ret < 0)
		goto CDEV_ADD_ERROR;

	pclass = class_create(THIS_MODULE, "hehe");
	if(IS_ERR(pclass))
		goto CDEV_ADD_ERROR;
	for(i = 0; i < count; i++)
	{
		pdevice = device_create(pclass, 0, MKDEV(MAJOR(number),minor+i) , 0, "hehe%d",i);
		if(IS_ERR(pdevice))
			goto DEVICE_CREATE_ERR;
	}
	
	printk("hello, enter module\n");
	return 0;
DEVICE_CREATE_ERR:
	class_destroy(pclass);
	
CDEV_ADD_ERROR:
	cdev_del(pdev);
CDEV_ERROR:
	unregister_chrdev_region(number,count);
	
REGISTER_ERROR:
	 return -1;
}
static void __exit hello_exit(void)
{
	int i;

	for(i = 0; i < count; i++)
	{
		device_destroy(pclass, MKDEV(MAJOR(number),i+minor));
	}
	class_destroy(pclass);
	cdev_del(pdev);
	unregister_chrdev_region(number,count);
	printk("hello, cleanup module\n");
}
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZHANG SAN, 139xxxxxxxx, xxx@163.com");
MODULE_DESCRIPTION("hlllo, test");
module_init(hello_init);
module_exit(hello_exit);
