#include <asm-generic/ioctl.h>
#define MAGIC  'K'
#define LED_ON _IO(MAGIC, 0)
#define LED_OFF _IOR(MAGIC, 1, int)
#define LED1_ON _IO(MAGIC, 4)
#define LED1_OFF _IOR(MAGIC, 5, int)
#define LED_BLINK _IOW(MAGIC, 2, int)
#define READ_ADC  _IOR(MAGIC, 3, int)
