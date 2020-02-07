#
# Based on https://qiita.com/iwatake2222/items/1fdd2e0faaaa868a2db2
#
obj-m := test.o

all: run

test.ko:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

.PHONY: clean
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

.PHONY: run
run: test.ko
	sudo insmod test.ko
	sudo rmmod test.lo
	dmesg
