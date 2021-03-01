#include <linux/init.h>
#include <linux/module.h>


void hello_cleanup(void)
{
	printk("hello, cleanup_module\n");
}
module_exit(hello_cleanup);
