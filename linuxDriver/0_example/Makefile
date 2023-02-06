KERN_DIR = /home/will/git/100ask_imx6ull_mini-sdk/Linux-4.9.88/

obj-m += example_drv.o

all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order


