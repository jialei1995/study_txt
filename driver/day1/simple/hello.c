#include <linux/init.h>
#include <linux/module.h>
int  init_module(void)
{
	printk("hello, init_module\n");
	return 0;
}


void cleanup_module(void)
{
	printk("hello, cleanup_module\n");
}
MODULE_LICENSE("GPL");
