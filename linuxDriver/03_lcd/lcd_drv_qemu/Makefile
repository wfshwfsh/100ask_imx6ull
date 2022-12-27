KERN_DIR = /home/will/git/100ask_imx6ull-qemu/linux-4.9.88/

obj-m += lcd_drv.o

all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order


