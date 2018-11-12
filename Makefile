obj-m := gpio.o
KDIR := /lib/modules/$(shell uname -r)/build/ 

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
#	gcc -o app app.c
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf Module.symvers
	rm -rf modules.order
	
re:
	make clean
	make
