#include <linux/init.h>
#include <linux/module.h>
int g_var = 100;
EXPORT_SYMBOL(g_var);
int add(int a,int b)
{
	return a+b;
}
EXPORT_SYMBOL(add);
int hello_init(void)
{
	printk("%s is called\n",__FUNCTION__);
	return 0;
}
void hello_exit(void)
{
	printk("%s is called\n",__FUNCTION__);
}
MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);
