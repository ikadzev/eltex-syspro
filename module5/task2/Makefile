NAME = hello-1
obj-m += $(NAME).o
PWD := $(CURDIR)

all: build insmod clean

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

insmod:
	sudo insmod $(NAME).ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

rmmod:
	sudo rmmod $(NAME).ko