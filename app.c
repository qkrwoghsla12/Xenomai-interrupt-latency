#include <native/task.h>
#include <rtdk.h>
#include <native/timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <rtdm/rtdm.h>

#define OUTPUT 1
#define INPUT 0
#define EDGE 3
#define IRQ_GPIO 12
#define OUT_GPIO 15
#define PERIOD 1000000 //1ms
RT_TASK gpio_irq_task;
int quitFlag=0;

int fd;
void signal_handler(int sig){
    quitFlag = 1;
}

typedef struct{
    uint8_t gpio_num;
    uint8_t value;
} gpio_set;


void gpio_irq(void *arg)
{
    RTIME now, previous;
    int out = OUT_GPIO;
    int irq = IRQ_GPIO;

    if(rt_dev_ioctl(fd, OUTPUT, &out) < 0){
        rt_printf("not support num %d\n",out);
    }
    if(rt_dev_ioctl(fd, EDGE, &irq) < 0){
        rt_printf("not support num %d\n",irq);
    }
    while (1){
        rt_task_wait_period(NULL);
    }
}
int main(){
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    mlockall(MCL_CURRENT|MCL_FUTURE);
    rt_print_auto_init(1);

    if((fd = rt_dev_open("gpio_rtdm", 0)) < 0) {
        perror("rt_open");
        exit(-1);
    }
    rt_task_create(&gpio_irq_task, "rtdm_irq", 0, 99, 0);
    rt_task_set_periodic(&gpio_irq_task, TM_NOW, rt_timer_ns2ticks(PERIOD));
    rt_task_start(&gpio_irq_task, &gpio_irq, NULL);
    pause();
    rt_task_delete(&gpio_irq_task);
    return 0;
}
