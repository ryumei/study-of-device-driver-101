#
# Based on https://qiita.com/iwatake2222/items/580ec7db2e88beeac3de
#
CFILES = myDeviceDriver.c
DRIVER_MAJOR = 63

obj-m := MyDeviceModule.o
MyDeviceModule-objs := $(CFILES:.c=.o)

MyDeviceModule.ko: $(CFILES)
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

.PHONY: clean
clean:
	sudo rmmod MyDeviceModule
	sudo rm /dev/myDevice

.PHONY:
distclean: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

.PHONY: load
load: 
	sudo insmod MyDeviceModule.ko
	cat /proc/devices | grep $(DRIVER_MAJOR)

/dev/myDevice: MyDeviceModule.ko
	sudo mknod /dev/myDevice c $(DRIVER_MAJOR) 1
	sudo chmod 666 $@
	ls -la $@

