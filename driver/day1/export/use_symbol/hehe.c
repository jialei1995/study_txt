#include <linux/init.h>
#include <linux/module.h>
MODULE_DESCRIPTION("test");
MODULE_AUTHOR("ZHANGSAN, 139xxxxxxxx, 123@qq.com");
MODULE_LICENSE("GPL");
extern int mysub(int,int);
extern int g_var;
int  hehe_init(void)
{
	printk("result for mysub = %d\n",mysub(7,4));
	printk("g_var = %d\n", g_var);
	printk("hehe, init_module\n");
	return 0;
}


void hehe_cleanup(void)
{
	printk("hehe, cleanup_module\n");
}
module_init(hehe_init)
module_exit(hehe_cleanup)
