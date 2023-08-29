KERNELDIR := /lib/modules/$(shell uname -r)/build

obj-m := matmul_kernel.o

all:
	make -C $(KERNELDIR) M=$(PWD) modules
	gcc matmul_user.c -o matmul_user

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm -f matmul_user
