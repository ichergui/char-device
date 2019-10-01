KDIR := /lib/modules/$(shell uname -r)/build

obj-m += chrdev_kernel.o

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko .cache.mk modules.* Module.* .*.mod.o.* .*.cmd *.mod.* .tmp_versions/
