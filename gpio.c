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

#define OUTPUT 1
#define INPUT 0
#define EDGE 3

uint8_t gpio_irq[10];
uint8_t gpio_out[10];
uint8_t gpio_in[10];
int i=0,ii=0,iii=0;

typedef struct{
    uint8_t gpio_num;
    uint8_t value;
} gpio_set;

static irq_handler_t irq_handle(int irq, void* ident){
        uint8_t val;
        val = gpio_get_value(gpio_irq[0]);
        gpio_set_value(gpio_out[0],((val>0)?1:0));
//	printk("int\n");
        return (irq_handler_t)IRQ_HANDLED;
}
static int gpio_open(struct inode *inode, struct file *filp){
	printk("gpio open\n");
	return 0;
}

static int gpio_release(struct inode *inode, struct file *filp){
	printk("gpio release\n");
	return 0;
}

static ssize_t gpio_write(struct file *flip, const char *buf, size_t size, loff_t *offset){
	ssize_t ret;
	gpio_set recv_data;

	ret = copy_from_user(&recv_data, (gpio_set*)buf, sizeof(gpio_set));
	gpio_set_value(recv_data.gpio_num, recv_data.value);
	return ret;	
}

static ssize_t gpio_read(struct file *flip,char *buf, size_t size, loff_t *offset){
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

static int gpio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	int ret;
	char recv;
	ret = copy_from_user(&recv ,(char*)arg, 1);
	printk("pinnum : %d, mode : %d\n",recv, cmd);

	switch(cmd){
        case INPUT:
            ret = gpio_direction_input(recv);
            gpio_in[i] = recv;
            printk("[%d] input\n", recv);
            i++;
            break;
        case OUTPUT:
            ret = gpio_direction_output(recv, 0);
            gpio_out[ii] = recv;
            printk("[%d] output\n", recv);
            ii++;
            break;
	case EDGE:
	    gpio_direction_input(recv);
 	    //irq_set_irq_type(gpio_to_irq(recv), IRQ_TYPE_EDGE_BOTH);
	    ret = request_irq(gpio_to_irq(recv), irq_handle, IRQ_TYPE_EDGE_BOTH, IRQ_NAME, NULL);
            enable_irq(gpio_to_irq(recv));
            gpio_irq[iii] = recv;
            iii++;
            printk("[%d] enterrupt\n", recv);
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
}

static struct file_operations gpio_fops = {
	.owner   		= THIS_MODULE,
	.open    		= gpio_open,
	.release 		= gpio_release,
	.write   		= gpio_write,
	.read	 		= gpio_read,
	.unlocked_ioctl = gpio_ioctl,
};

static int __init gpio_init(void){
	printk("gpio device driver init\n");
/*
	gpio_request(44 , "GPIO");
	gpio_export(44 , 1);
	gpio_request(67 , "GPIO");
	gpio_export(67 , 1);
	gpio_direction_output(67, 0);
	gpio_direction_input(44);

	request_irq(gpio_to_irq(44), &irq_handle, NULL, "gpio_irq", NULL);
	irq_set_irq_type(gpio_to_irq(44), IRQ_TYPE_EDGE_BOTH);
*/
	register_chrdev(GPIO_MAJOR, GPIO_NAME, &gpio_fops);
	return 0;
}

static void __exit gpio_exit(void){
	printk("gpio device driver exit\n");
	//int z=0;
	//for(z=0; z<=i-1; z++){
	//	free_irq(44,NULL);
	//	disable_irq(44);
	//}
	unregister_chrdev(GPIO_MAJOR, GPIO_NAME);
} 

MODULE_LICENSE("GPL");
module_init(gpio_init);
module_exit(gpio_exit);
