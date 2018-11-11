#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/types.h>
#include <rtdm/rtdm_driver.h>

#define RTDM_SUBCLASS_GPIO_IRQ       4711
#define DEVICE_NAME 				"gpio_rtdm"

#define GPIO_IRQ 45 
#define GPIO_OUT 44

int ret;
rtdm_irq_t rtdm_irq_handle;
rtdm_event_t gpio_event;
char name[10];


static int irq_handle(rtdm_irq_t *irq_handle){
	uint8_t val = gpio_get_value(GPIO_IRQ);
	gpio_set_value(GPIO_OUT, val);

	return RTDM_IRQ_HANDLED;
}

/*
static struct rtdm_device device = {
 struct_version:         RTDM_DEVICE_STRUCT_VER,
 device_flags:           RTDM_NAMED_DEVICE,
 device_name:            DEVICE_NAME,
 open_rt:                NULL,
 open_nrt:               NULL,

 ops:{
    close_rt:       NULL,				
    close_nrt:      NULL,		// open, close 는 nrt task 인 main 에서 이루어 지므로 nrt 에 지정
  
    ioctl_rt:       NULL,
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
};
*/

static int __init gpio_rtdm_init(void){
	ret = gpio_direction_input(GPIO_IRQ);
	gpio_direction_output(GPIO_OUT,0);
	irq_set_irq_type(gpio_to_irq(GPIO_IRQ), IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING);
	ret = rtdm_irq_request(&rtdm_irq_handle, gpio_to_irq(GPIO_IRQ), &irq_handle, RTDM_IRQTYPE_EDGE, DEVICE_NAME, NULL);
	rtdm_irq_enable(&rtdm_irq_handle);


	printk("gpio rtdm init 1\n");

}

static void __exit gpio_rtdm_exit(void){
	rtdm_irq_disable(&rtdm_irq_handle);
	rtdm_irq_free(&rtdm_irq_handle);
	printk("gpio rtdm exit\n");
}

MODULE_LICENSE("GPL");
module_init(gpio_rtdm_init);
module_exit(gpio_rtdm_exit);
