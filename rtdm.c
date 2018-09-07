#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/types.h>
#include <rtdm/rtdm_driver.h>

#define RTDM_SUBCLASS_GPIO_IRQ       4711
#define DEVICE_NAME 				"gpio_rtdm"

#define OUTPUT 1
#define INPUT 0
#define EDGE 3

uint8_t gpio_irq;
uint8_t gpio_out;

typedef struct{
    uint8_t gpio_num;
    uint8_t value;
} gpio_set;

rtdm_irq_t rtdm_irq_handle;
rtdm_event_t gpio_event;
char name[10];


static int irq_handle(rtdm_irq_t *irq_handle){
	//printk("irq : %d %d\n",gpio_irq,gpio_out);
	uint8_t val = gpio_get_value(gpio_irq);
	gpio_set_value(gpio_out, val);

	return RTDM_IRQ_HANDLED;
}
int gpio_rtdm_open(struct rtdm_dev_context *context, rtdm_user_info_t *user_info, int oflags) {
	printk("gpio rtdm open\n");
	return 0;
}
int gpio_rtdm_close(struct rtdm_dev_context *context, rtdm_user_info_t *user_info){
	rtdm_irq_disable(&rtdm_irq_handle);
	rtdm_irq_free(&rtdm_irq_handle);
	printk("gpio rtdm close\n");
	return 0;
}
/*
static ssize_t gpio_rtdm_write(struct file *flip, const char *buf, size_t size, loff_t *offset){
	ssize_t ret;
	gpio_set recv_data;

	ret = copy_from_user(&recv_data, (gpio_set*)buf, sizeof(gpio_set));
	gpio_set_value(recv_data.gpio_num, recv_data.value);
	return ret;	
}

static ssize_t gpio_rtdm_read(struct file *flip,char *buf, size_t size, loff_t *offset){
	uint8_t val;
	ssize_t ret;
	gpio_set recv_data;

	ret = copy_from_user(&recv_data, (gpio_set*)buf, sizeof(gpio_set));
	val = gpio_get_value(recv_data.gpio_num);
	recv_data.value = val;

	//printk("read[%d] : %d\n",recv_data.gpio_num ,val);
	ret |= copy_to_user((gpio_set*)buf, &recv_data, sizeof(gpio_set));
	return ret;
}
*/
static ssize_t gpio_rtdm_ioctl(struct rtdm_dev_context* context, rtdm_user_info_t* user_info, unsigned int request, void __user* arg){
	int ret;
	char recv;
	printk(KERN_INFO "dawdwa\n");
	
	ret = rtdm_safe_copy_from_user(user_info, &recv, (char*)arg, 1);	// 사용자 공간 메모리 블록에 대한 읽기 엑세스가 안전한지 확인후에 저장
	printk("pinnum 2: %d, mode : %d\n",recv, request);

	switch(request){
		case OUTPUT:
			ret = gpio_direction_output(recv, 0);
			gpio_out = recv;
			printk("[%d] output\n", recv);
			break;
		case EDGE:
			ret = gpio_direction_input(recv);
			gpio_irq = recv;
			irq_set_irq_type(gpio_to_irq(recv), IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING);
			ret = rtdm_irq_request(&rtdm_irq_handle, gpio_to_irq(recv), &irq_handle, RTDM_IRQTYPE_EDGE, DEVICE_NAME, NULL);
			rtdm_irq_enable(&rtdm_irq_handle);

			if(ret >= 0){
				printk("[%d] irq request success\n", recv);
			}else{
				printk("irq request fail\n");
			}
			break;
		default:
			ret = -1;
			break;
	}
	return 0;
}

static struct rtdm_device device = {
 struct_version:         RTDM_DEVICE_STRUCT_VER,
 device_flags:           RTDM_NAMED_DEVICE,
 device_name:            DEVICE_NAME,
 open_rt:                NULL,
 open_nrt:               gpio_rtdm_open,

 ops:{
    close_rt:       NULL,				
    close_nrt:      gpio_rtdm_close,		// open, close 는 nrt task 인 main 에서 이루어 지므로 nrt 에 지정
  
    ioctl_rt:       gpio_rtdm_ioctl,
    ioctl_nrt:      NULL,
  
    read_rt:        NULL,
    read_nrt:       NULL,

    write_rt:       NULL,
    write_nrt:      NULL,   
    },

 device_class:           RTDM_CLASS_EXPERIMENTAL,
 device_sub_class:       RTDM_SUBCLASS_GPIO_IRQ,
 driver_name:            "gpio_irq_rtdm",
 driver_version:         RTDM_DRIVER_VER(0, 0, 0),
 peripheral_name:        "GPIO_IRQ RTDM",
 provider_name:          "aa",
 proc_name:              device.device_name,
};


static int __init gpio_rtdm_init(void){
	printk("gpio rtdm init\n");

	return rtdm_dev_register(&device);
}

static void __exit gpio_rtdm_exit(void){
	printk("gpio rtdm exit\n");
	rtdm_dev_unregister (&device, 1000);			// 인자 2 : 열려있는 장치 인스턴스를 반복적으로 검사하는 폴링 지연시간(ms), non blocking 의 경우 0
} 	

MODULE_LICENSE("GPL");
module_init(gpio_rtdm_init);
module_exit(gpio_rtdm_exit);
