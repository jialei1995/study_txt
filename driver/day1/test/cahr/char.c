#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
int devno;
int ma=250;
int mi=1;
int count=1;
int hello_init(void)
{
	int ret;
	ret = register_chrdev_region(MKDEV(ma,mi),1,"uuuuu");
	if(ret != 0)
	{
		printk("register_chrdev_region\n");
		return -1;
	}
	printk("%s is called\n",__FUNCTION__);
	return 0;
}
void hello_exit(void)
{
	unregister_chrdev_region(MKDEV(ma,mi), 1);
	printk("%s is called\n",__FUNCTION__);
}
MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);
