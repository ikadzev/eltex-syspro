obj-m += kbleds.o
PWD := $(CURDIR)

all: build insmod clean

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

insmod:
	sudo insmod kbleds.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

rmmod:
	sudo rmmod kbleds.ko