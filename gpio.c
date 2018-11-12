#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>			// file file_operations 구조체 정의되어있다.
#include <linux/io.h>			// ioremap 함수 들어있다.
#include <asm/uaccess.h>  		// copy to user 같은 함수 들어있다.
#include <linux/gpio.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#define GPIO_MAJOR 200
#define GPIO_NAME "gpio"
#define IRQ_NAME "gpio_irq"

#define GPIO_IRQ 66
#define GPIO_OUT 67

static irq_handler_t irq_handle(int irq, void* ident){
        uint8_t val;
        val = gpio_get_value(GPIO_IRQ);
        gpio_set_value(GPIO_OUT,((val>0)?1:0));
        return (irq_handler_t)IRQ_HANDLED;
}

static int __init gpio_init(void){
	printk("gpio device driver init\n");
	gpio_direction_output(GPIO_OUT, 0);
	gpio_direction_input(GPIO_IRQ);
	request_irq(gpio_to_irq(GPIO_IRQ), &irq_handle, IRQ_TYPE_EDGE_BOTH, IRQ_NAME, NULL);
        enable_irq(gpio_to_irq(GPIO_IRQ));

	return 0;
}

static void __exit gpio_exit(void){
	printk("gpio device driver exit\n");
	disable_irq(GPIO_IRQ);
	free_irq(GPIO_IRQ,NULL);
} 

MODULE_LICENSE("GPL");
module_init(gpio_init);
module_exit(gpio_exit);
