#include <asm-generic/ioctl.h>
#define MAGIC  'K'
#define LED_ON _IO(MAGIC, 0)
#define LED_OFF _IOR(MAGIC, 1, int)
#define LED_BLINK _IOW(MAGIC, 2, int)
#define READ_ADC  _IOR(MAGIC, 3, int)
