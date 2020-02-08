#
# Based on https://qiita.com/iwatake2222/items/6b02494a3668f79800e6
#
CFILES = myDeviceDriver.c
#DRIVER_MAJOR = 63

obj-m := MyDeviceModule.o
MyDeviceModule-objs := $(CFILES:.c=.o)

ccflags-y += -std=gnu99 -Wall -Wno-declaration-after-statement

MyDeviceModule.ko: $(CFILES)
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) C=1 modules

.PHONY: clean
clean:
	sudo rmmod MyDeviceModule
	sudo rm /dev/mydevice0
	sudo rm /dev/mydevice1
	sudo rm /dev/mydevice2

.PHONY:
distclean: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

.PHONY: load
load: install-module /dev/mydevice0 /dev/mydevice1 /dev/mydevice2

.PHONY: install-module
install-module: 
	sudo insmod MyDeviceModule.ko

/dev/mydevice0:
	sudo mknod --mode=666 $@ c $(shell grep MyDevice /proc/devices | awk '{print $$1;}') 0
/dev/mydevice1:
	sudo mknod --mode=666 $@ c $(shell grep MyDevice /proc/devices | awk '{print $$1;}') 1
/dev/mydevice2:
	sudo mknod --mode=666 $@ c $(shell grep MyDevice /proc/devices | awk '{print $$1;}') 2

