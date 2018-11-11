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
