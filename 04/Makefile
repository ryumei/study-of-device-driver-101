#
# Based on https://qiita.com/iwatake2222/items/26d5f7f4894ccc4ce227
#
all: MyDeviceModule.ko

get_memory: get_memory.c
	gcc get_memory.c -L/opt/vc/lib -lbcm_host

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
	sudo rm -f /dev/mydevice[012]

.PHONY:
distclean: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

.PHONY: load
load: install-module /dev/mydevice0 /dev/mydevice1 /dev/mydevice2

.PHONY: install-module
install-module: MyDeviceModule.ko
	sudo insmod MyDeviceModule.ko

/dev/mydevice0:
	sudo mknod --mode=666 $@ c $(shell grep MyDevice /proc/devices | awk '{print $$1;}') 0
	sudo chmod 666 $@

/dev/mydevice1:
	sudo mknod --mode=666 $@ c $(shell grep MyDevice /proc/devices | awk '{print $$1;}') 1
	sudo chmod 666 $@

/dev/mydevice2:
	sudo mknod --mode=666 $@ c $(shell grep MyDevice /proc/devices | awk '{print $$1;}') 2

