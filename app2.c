#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define OUTPUT 1
#define INPUT 0
#define EDGE 3
#define IRQ_GPIO 13
#define OUTPUT_GPIO 0

typedef struct{
    uint8_t gpio_num;
    uint8_t value;
} gpio_set;


int main(int argc, char** argv){
	printf("start\n");
	int fp;
	char num, mode, value;

	gpio_set irq_gpio;
	gpio_set out_gpio;
	irq_gpio.gpio_num = IRQ_GPIO;
	out_gpio.gpio_num = OUTPUT_GPIO;

	if((fp = open("/dev/gpio",O_RDWR)) < 0){
		perror("open");
		exit(1);
	}

	if(ioctl(fp, OUTPUT, &out_gpio.gpio_num) < 0){
		printf("not support num(ioctl)\n");
		exit(1);
	}

	if(ioctl(fp, EDGE, &irq_gpio.gpio_num) < 0){
		printf("not support num(ioctl)\n");
		exit(1);
	}
	return 0;
}
