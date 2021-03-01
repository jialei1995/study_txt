#include <linux/init.h>
#include <linux/module.h>
MODULE_DESCRIPTION("this is a ch340 driver, arm, litlle endian");
MODULE_AUTHOR("ZHANGSAN, 139xxxxxxxx, 123@qq.com");
MODULE_LICENSE("GPL");
int type = 0;
module_param(type, int, 0644);
char *p = "Abc";
module_param(p,charp, 0);
int  hello_init(void)
{
	printk("type = %d\n",type);
	printk("p = %s\n",p);
	printk("hello, init_module\n");
	return 0;
}


void hello_cleanup(void)
{
	printk("hello, cleanup_module\n");
}
module_init(hello_init)
module_exit(hello_cleanup)
