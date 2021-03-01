#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/delay.h>

#include <asm/uaccess.h>

#include "mpu6050.h"

MODULE_LICENSE("GPL");

#define SMPLRT_DIV		0x19
#define CONFIG			0x1A
#define GYRO_CONFIG		0x1B
#define ACCEL_CONFIG	0x1C
#define ACCEL_XOUT_H	0x3B
#define ACCEL_XOUT_L	0x3C
#define ACCEL_YOUT_H	0x3D
#define ACCEL_YOUT_L	0x3E
#define ACCEL_ZOUT_H	0x3F
#define ACCEL_ZOUT_L	0x40
#define TEMP_OUT_H		0x41
#define TEMP_OUT_L		0x42
#define GYRO_XOUT_H		0x43
#define GYRO_XOUT_L		0x44
#define GYRO_YOUT_H		0x45
#define GYRO_YOUT_L		0x46
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48
#define PWR_MGMT_1		0x6B

static int minor_base = 0;
static int minor_count = 1;
static char name[] = "mpu6050";
static struct class *pcls = NULL;
static struct device *pdevice = NULL;
static dev_t devno;
static int major;
struct cdev *pdev;

struct i2c_client *client;

static int mpu6050_read_byte(struct i2c_client *pclient, unsigned char reg)
{
	int ret;

	char txbuf[1] = { reg };
	char rxbuf[1];

	struct i2c_msg msg[2] = {  //Ã¿¸öÔªËØ¶¼ÊÇ´ÓstartÐÅºÅ£¬ ÒÔstop ÐÅºÅ½áÊøµÄ
		{client->addr, 0, 1, txbuf},     //·¢ËÍ²Ù×÷£¬·¢´ÓiicÉè±¸µØÖ·£¬ 0 Ð´¶¯×÷£¬ Ö®ºó£¬ ¼ÌÐø·¢ËÍÒª¶ÁµÄreg 
		{client->addr, 1, 1, rxbuf}  // ¶Á¶¯×÷£¬ ·¢ËÍÒª¶ÁµÄ´Óiic Éè±¸µÄ7Î»µØÖ·£¬¸Ä³É½ÓÊÕÄ£Ê½£¬ ½ÓÊÕµ½1¸ö×Ö½ÚµÄÊý¾Ý ±£´æÔÚ  rxbuf
	};

	ret = i2c_transfer(pclient->adapter, msg, ARRAY_SIZE(msg));// µ÷ÓÃºËÐÄ²ãµÄi2c_transfer À´ºÍ´Óiic Éè±¸Í¨Ñ¶£¬  µÚÒ»¸ö²ÎÊý¾ÍÊÇ´ÓÉè±¸ËùÔÚµÄ×ÜÏß£¬
	// µÚ¶þ¸ö²ÎÊýÊÇÒª·¢µÄÊý¾Ýmsg£¬ µÚÈý¸ö²ÎÊýÊÇÓÐ¼¸×éÊý×é.
	if (ret < 0) {
		printk("ret = %d\n", ret);
		return ret;
	}

	return rxbuf[0];
}

static int mpu6050_write_byte(struct i2c_client *pclient, unsigned char reg, unsigned char val)
{
	char txbuf[2] = {reg, val};

	struct i2c_msg msg[1] = {
		{client->addr, 0, 2, txbuf},// ÒÀ´ÎÊÇiic Ð¾Æ¬µÄ7Î»µØÖ·£¬ 0-- Ð´²Ù×÷£¬ 2 --- ·¢ËÍµÄÊý¾ÝÓÐ2¸ö£¬  
		//txbuf --- Òª·¢ËÍµÄÊý¾Ý(2¸ö×Ö½Ú£© 
	};

	i2c_transfer(pclient->adapter, msg, ARRAY_SIZE(msg));// µ÷ÓÃºËÐÄ²ãµÄi2c_transfer À´ºÍ´Óiic Éè±¸Í¨Ñ¶£¬  µÚÒ»¸ö²ÎÊý¾ÍÊÇ´ÓÉè±¸ËùÔÚµÄ×ÜÏß£¬
	// µÚ¶þ¸ö²ÎÊýÊÇÒª·¢µÄÊý¾Ýmsg£¬ µÚÈý¸ö²ÎÊýÊÇÓÐ¼¸×éÊý×é.

	return 0;
}


static int mpu6050_open(struct inode *inode, struct file *file) 
{
	return 0;
}

static int mpu6050_release(struct inode *inode, struct file *file) 
{
	return 0;
}

static long mpu6050_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	union mpu6050_data data;

	switch(cmd) {
	case GET_ACCEL://Ó¦ÓÃ·¢µÄ¶Á¼ÓËÙ¶ÈµÄÃüÁî 
			//¶ÁxÖáµÄ¼ÓËÙ¶È
		data.accel.x = mpu6050_read_byte(client, ACCEL_XOUT_L);
		data.accel.x |= mpu6050_read_byte(client, ACCEL_XOUT_H) << 8;
			//¶ÁyÖáµÄ¼ÓËÙ¶È
		data.accel.y = mpu6050_read_byte(client, ACCEL_YOUT_L);
		data.accel.y |= mpu6050_read_byte(client, ACCEL_YOUT_H) << 8;
//¶ÁzÖáµÄ¼ÓËÙ¶È
		data.accel.z = mpu6050_read_byte(client, ACCEL_ZOUT_L);
		data.accel.z |= mpu6050_read_byte(client, ACCEL_ZOUT_H) << 8;
		break;

	case GET_GYRO://¶Á½ÇËÙ¶È

		data.gyro.x = mpu6050_read_byte(client, GYRO_XOUT_L);
		data.gyro.x |= mpu6050_read_byte(client, GYRO_XOUT_H) << 8;

		data.gyro.y = mpu6050_read_byte(client, GYRO_YOUT_L);
		data.gyro.y |= mpu6050_read_byte(client, GYRO_YOUT_H) << 8;

		data.gyro.z = mpu6050_read_byte(client, GYRO_ZOUT_L);
		data.gyro.z |= mpu6050_read_byte(client, GYRO_ZOUT_H) << 8;
		break;

	case GET_TEMP:	//¶ÁÎÂ¶È
		data.temp = mpu6050_read_byte(client, TEMP_OUT_L);
		data.temp |= mpu6050_read_byte(client, TEMP_OUT_H) << 8;
		break;

	default:
		printk("invalid argument\n");
		return -EINVAL;
	}
//¸úcÓïÑÔÓ¦ÓÃ³ÌÐòÖÐµÄº¯ÊýgetÖµÒ»Ñù£¬argÔÚÕâÀï½«dataµÄÖµ¾ÍµÃµ½ÁË
	if (copy_to_user((void *)arg, &data, sizeof(data)))
		return -EFAULT;

	return sizeof(data);
}

struct file_operations mpu6050_fops = {// Çý¶¯Àïmpu6050 µÄ²Ù×÷·½·¨£¬ 
	.owner 		= THIS_MODULE,
	.open		= mpu6050_open,
	.release 	= mpu6050_release,
	.unlocked_ioctl = mpu6050_ioctl,
};

static int mpu6050_probe(struct i2c_client *pclient, const struct i2c_device_id *id)
{//Ì½²âµ½µÄiic Éè±¸ pclient 
	int ret;
	printk("hello, i2c module\n");
	
	client = pclient;//pclient  ---- ¾ÍÊÇÉè±¸Ê÷ÀïµÄ invensense,mpu6050 iic ´ÓÉè±¸£¬ µÃµ½¸ÃÉè±¸µÄiicµØÖ·£¬ ÖÐ¶Ï
					把他赋给全局变量，别的函数也要用呢，只能从这里拿它
	ret = alloc_chrdev_region(&devno, minor_base, minor_count, name);
	if (ret < 0) {
		printk("failed to alloc char device region!\n");
		goto err1;
	}
	major = MAJOR(devno);
	pdev = cdev_alloc();

	cdev_init(pdev, &mpu6050_fops);
	ret = cdev_add(pdev, devno, 1);
	if (ret < 0) {
		printk("failed to add device\n");
		goto err2;
	}
	

	pcls = class_create(THIS_MODULE, name);
	if (IS_ERR(pcls)) {
		printk("class create error\n");
		ret = PTR_ERR(pcls);
		goto err2;
	}

	pdevice = device_create(pcls, NULL, MKDEV(major, minor_base), NULL, name);	
	if(IS_ERR(pdevice)) {
		printk("device create error\n");
		ret = PTR_ERR(pdevice);
		goto err3;
	}
//¿ØÖÆmpu6050 Ð¾Æ¬£¬·¢ÃüÁî¸ømpu6050£¬ £¨ ¾ÍÊÇÐ´6050 µÄ¼Ä´æÆ÷£© 
// Í¨¹ýi2c Í¨Ñ¶£¬ ÈÃmpu6050 ÉèÖÃÏàÓ¦µÄ¼Ä´æÆ÷Îª ÏàÓ¦µÄÖµ£¬ ±£Ö¤mpu6050 °´ÎÒÃÇÒªÇó¹¤×÷¡£
	mpu6050_write_byte(client, PWR_MGMT_1, 0x00);
	mpu6050_write_byte(client, SMPLRT_DIV, 0x07);
	mpu6050_write_byte(client, CONFIG, 0x06);
	mpu6050_write_byte(client, GYRO_CONFIG, 0x18);
	mpu6050_write_byte(client, ACCEL_CONFIG, 0x00);
	对6050发数据按照人家的手册，这就初始化好了
	printk("mpu6050 init\n");

	return 0;
err3:
	class_destroy(pcls);
err2:
	cdev_del(pdev);
err1:
	unregister_chrdev_region(devno, 1);
	return ret;
}

static int mpu6050_remove(struct i2c_client *client)
{
	device_destroy(pcls, MKDEV(major, minor_base));
	class_destroy(pcls);
	cdev_del(pdev);
	unregister_chrdev_region(devno, 1);
	printk("Good bye, i2c module\n");
	return 0;
}

static const struct i2c_device_id mpu6050_id[] = {
//	{ "mpu6050", 0},
	{}
}; 

static struct of_device_id mpu6050_dt_match[] = {
	{.compatible = "invensense,mpu6050" },// mpu6050_driver  Ö§³ÖÉè±¸Ê÷ÀïµÄinvensense,mpu6050
	{/*northing to be done*/},
};

struct i2c_driver mpu6050_driver = {
	.driver = {
		.name 			= "mpu6050",       //Ö§³ÖÉè±¸Ãû
		.owner 			= THIS_MODULE,
		.of_match_table = of_match_ptr(mpu6050_dt_match),  //¼æÈÝµÄÉè±¸Ê÷ÀïµÄÉè±¸
	},
	.probe 		= mpu6050_probe,     //Ì½²âµ½Éè±¸invensense,mpu6050£¬ Ö´ÐÐprobe º¯Êý
	.remove 	= mpu6050_remove,   //×¢Ïúiic Çý¶¯Ê±£¬ Ö´ÐÐ¸Ãº¯Êý
	.id_table 	= mpu6050_id,// ×¢ÏúÇý¶¯Ê±£¬ Ö´ÐÐ¸Ãº¯Êý
};
static int __init mpu6050_init(void)
{
	i2c_register_driver(THIS_MODULE,&mpu6050_driver);
	//×¢²áÁËÒ»¸öiic Éè±¸µÄÇý¶¯
	
	return 0;
}
static void __exit mpu6050_exit(void)
{
	i2c_del_driver(&mpu6050_driver);
}
module_init(mpu6050_init);
module_exit(mpu6050_exit);
