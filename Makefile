obj-m = kernelmodule.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	gcc client.c -o client

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm client
