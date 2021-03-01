#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/platform_device.h>


//完全描述硬件资源
void HelloPlatFromRelease(struct device *dev)// 平台设备的注销时， 调用的函数
{
        printk("device, HelloPlatFormReleasee\r\n");
}

struct resource myresource[6] = {
	{.start = 0x114000a0, .end = 0x114000a3,},
	{.start = 0x139D0000, .end = 0x139D0003,},
	{.start = 0x139D0004, .end = 0x139D0007,},
	{.start = 0x139D000C, .end = 0x139D000F,},
	{.start = 0x139D0010, .end = 0x139D0013,},
	{.start = 0x139D0008, .end = 0x139D000B,},
};

static struct platform_device g_stPlatFormDevice =
{
        .name           = "hehe1_platform",//平台设备的名字
        .dev            = {
                                .release = HelloPlatFromRelease,// 注销平台设备platform_device_unregister时， 调用该函数
		},
	.num_resources = 6, 
	.resource = myresource, 
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
        return iRet;

DEVICE_REGISTER_ERR:
        platform_device_unregister(&g_stPlatFormDevice);//如果出错， 注销该平台设备
        return iRet;
}

void HelloRelease(void)
{
        platform_device_unregister(&g_stPlatFormDevice);//注销平台设备
        printk("clean module exit\r\n");
}

module_exit(HelloRelease);
module_init(HelloInit);

MODULE_LICENSE("GPL");


