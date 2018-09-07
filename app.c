#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define OUTPUT 1
#define INPUT 0

typedef struct{
    uint8_t gpio_num;
    uint8_t value;
} gpio_set;


int main(int argc, char** argv){
	if(argc != 4){
		printf("usage : ./a pinnum value mode(1: output, 0: input)\n");
		exit(1);
	}
	printf("start\n");
	int fp;
	char num, mode, value;
	num   = atoi(argv[1]);
	value = atoi(argv[2]);
	mode  = atoi(argv[3]);
	gpio_set cmd;

	cmd.gpio_num = num;
	cmd.value    = value;


	if((fp = open("/dev/gpio",O_RDWR)) < 0){
		perror("open");
		exit(1);
	}

	if(ioctl(fp, mode, &num) < 0){
		printf("not support num(ioctl)\n");
		exit(1);
	}

	if(mode == OUTPUT){
		if(write(fp, &cmd, sizeof(cmd)) < 0){
			printf("not support num2\n");
			exit(1);
		}
		printf("GPIO : %d, mode : OUTPUT,  Write Data : %d\n",cmd.gpio_num, cmd.value);
	}
	else{
		if(read(fp, &cmd, sizeof(cmd)) < 0){
			printf("not support num2\n");
			exit(1);
		}
		printf("GPIO : %d, mode : INPUT, Read Data : %d\n",cmd.gpio_num, cmd.value);
	}

	return 0;
}
