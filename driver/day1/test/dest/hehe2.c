#include <linux/init.h>
#include <linux/module.h>
extern int g_var;
extern int add(int a,int b);
int hehe_init(void)
{
	printk("%s is called\n",__FUNCTION__);
	printk("%d\n",g_var);
	printk("%d\n",add(100,200));
	return 0;
}
void hehe_exit(void)
{
	printk("%s is called\n",__FUNCTION__);
}
MODULE_LICENSE("GPL");
module_init(hehe_init);
module_exit(hehe_exit);
