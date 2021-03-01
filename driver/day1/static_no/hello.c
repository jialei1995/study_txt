#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
MODULE_DESCRIPTION("this is a ch340 driver, arm, litlle endian");
MODULE_AUTHOR("ZHANGSAN, 139xxxxxxxx, 123@qq.com");
MODULE_LICENSE("GPL");
int devno;
int ma=250;
int mi = 100;
int count = 1;
int  hello_init(void)
{
	int ret;
	ret = register_chrdev_region(MKDEV(ma,mi),count, "hello"); 
	if(ret < 0)
	{
		goto REGISTER_ERROR;
		
	}
	
	printk("hello, init_module\n");
	return 0;
REGISTER_ERROR:
	return -1;
}


void hello_cleanup(void)
{
	
	unregister_chrdev_region(MKDEV(ma,mi),count); 
	printk("hello, cleanup_module\n");
}
module_init(hello_init)
module_exit(hello_cleanup)
