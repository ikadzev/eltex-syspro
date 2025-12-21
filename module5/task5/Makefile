NAME = netlink_monitor
obj-m += $(NAME).o

all: rmmod insmod build_client clean

build_client: client.c
	gcc -o client client.c

$(NAME).ko:
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) modules

insmod: $(NAME).ko
	sudo insmod $(NAME).ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean

rmmod:
	-sudo rmmod $(NAME)