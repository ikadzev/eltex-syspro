NAME = chardev
obj-m += $(NAME).o
PWD := $(CURDIR)

all: build insmod clean

insmod: $(NAME).ko
build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

insmod: $(NAME).ko
	sudo insmod $(NAME).ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

rmmod:
	sudo rmmod $(NAME).ko