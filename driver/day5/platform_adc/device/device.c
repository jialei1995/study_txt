#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/platform_device.h>


//完全描述硬件资源
void HelloPlatFromRelease(struct device *dev)// 平台设备的注销时， 调用的函数
{
        printk("device, HelloPlatFormReleasee\r\n");
}

struct resource myresource[3] = {
	{.start = 0x126c0000, .end = 0x126c0003,},// adc 控制寄存器
	{.start = 0x126c000c, .end = 0x126c000f,},// adc 数据寄存器
	{.start = 0x126c001c, .end = 0x126c001f,},// adc 通道选择寄存器
};
unsigned int heheAdcMux[1] = { 3};

static struct platform_device g_stPlatFormDevice =
{
        .name           = "hehe1_platform",//平台设备的名字
        .dev            = {
                                .release = HelloPlatFromRelease,// 注销平台设备platform_device_unregister时， 调用该函数
								.platform_data = (void*)heheAdcMux,//该平台设备的其他资源， 比如通道  
        },
	.num_resources = 3, 
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


