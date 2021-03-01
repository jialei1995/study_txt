#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
//都是硬件资源相关


void HelloPlatFromRelease(struct device *dev)// 平台设备的注销时， 调用的函数
{
        printk("device, HelloPlatFormReleasee\r\n");
}

struct resource myresource[3] = {			//硬件资源初始化，有3个adc，控制寄存器地址分别是。。
	{.start = 0x126c0000, .end = 0x126c0003,},
	{.start = 0x126c000c, .end = 0x126c000f,},
	{.start = 0x126c001c, .end = 0x126c001f,},
};
static struct platform_device g_stPlatFormDevice =//定义1个平台设备，初始化的时候用。
{
        .name           = "hehe_platform",		//平台设备的名字
        .dev            = {
                                .release = HelloPlatFromRelease,// 注销平台设备时，调用该函数
        },
	.num_resources = 3, 
	.resource = myresource, //传资源地址
};

int HelloInit(void)
{
        int iRet = 0;
        printk("device, module init enter\r\n");
        iRet = platform_device_register(&g_stPlatFormDevice);//注册了一个平台设备
		if (0 != iRet)
        {
                printk("platform_device_register error");
                goto DEVICE_REGISTER_ERR;
        }

        printk("device, module init exit\r\n");
        return iRet;

DEVICE_REGISTER_ERR:
        platform_device_unregister(&g_stPlatFormDevice);//如果出错， 注销该平台设备
        return iRet;
}

void HelloRelease(void)//出口
{
        printk("clean module enter\r\n");
        platform_device_unregister(&g_stPlatFormDevice);//注销平台设备
        printk("clean module exit\r\n");
}

module_exit(HelloRelease);
module_init(HelloInit);

MODULE_LICENSE("GPL");


