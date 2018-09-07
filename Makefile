obj-m += rtdm.o

XENO ?= /zynq-rt/xenomai
XENO_CONFIG=$(shell PATH=$(XENO):$(XENO)/bin:$(PATH) which xeno-config 2>/dev/null)

PWD:= $(shell pwd)
KDIR := /lib/modules/$(shell uname -r)/build
CC := $(shell $(XENO_CONFIG) --skin=native --cc)
STD_CFLAGS  := $(shell $(XENO_CONFIG) --skin=native --cflags) -I.
STD_LDFLAGS := $(shell $(XENO_CONFIG) --skin=native --ldflags) -lrtdm

EXTRA_CFLAGS := -I${XENO}/include
EXTRA_CFLAGS += $(shell $(XENO_CONFIG) --skin=native --cflags)
EXTRA_CFLAGS += $(CFLAGS)


all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	make app

app: app.c
	$(CC) -o $@ $< $(STD_CFLAGS) $(STD_LDFLAGS)

install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules_install
	depmod -a

clean:
	rm -f *~ Module.markers Module.symvers modules.order
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	$(RM) -f *.o *~ gpio-irq-test
