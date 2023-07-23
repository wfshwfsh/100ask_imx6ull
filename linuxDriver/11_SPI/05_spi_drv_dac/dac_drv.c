
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/acpi.h>

#include <linux/uaccess.h>

#include <linux/spi/spi.h>



static int major;

static struct spi_device *dac;

static struct class *spi_dac_class;
static struct device *spi_dac_dev;


/*
static int 100ask_spi_dac_open(struct inode *inode, struct file *filp)
{
	struct spidev_data	*spi_data;

	if (!spi_data->tx_buffer) {
		spi_data->tx_buffer = kmalloc(bufsiz, GFP_KERNEL);
		
		if (!spi_data->tx_buffer) {
			printk(, "open/ENOMEM\n");
			status = -ENOMEM;
			goto err_alloc_tx_buf;
		}
	}

	if (!spi_data->rx_buffer) {
		spi_data->rx_buffer = kmalloc(bufsiz, GFP_KERNEL);
		if (!spi_data->rx_buffer) {
			printk(, "open/ENOMEM\n");
			status = -ENOMEM;
			goto err_alloc_rx_buf;
		}
	}

err_alloc_tx_buf:

err_alloc_rx_buf:

	return 0;
}

static int 100ask_spi_dac_release(struct inode *inode, struct file *filp)
{



}
*/


static long spi_dac_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct spi_transfer	xfer;
	struct spi_message	m;
	unsigned char tx_buf[2];
	unsigned char rx_buf[2];
	
	int val;
	int err;

	err = copy_from_user(&val, (const void __user *)arg, sizeof(val));

	printk("tx val = %d\n", val);

	val = (val<<2);
	val = (val & 0xffc);
	
	tx_buf[1] = (val & 0xff);
	tx_buf[0] = (val >> 8);
	
	xfer.tx_buf = tx_buf;
	xfer.rx_buf = rx_buf;
	xfer.len = 2;
	
	spi_message_init(&m);
	spi_message_add_tail(&xfer, &m);

	int r_len = spi_sync(dac, &m);

	printk("rx: %02x %02x\n", rx_buf[0], rx_buf[1]);
	val = (((rx_buf[0] | rx_buf[1] ) & 0xffc) >> 2);
	printk("ret_val = %d\n", val);
	
	err = copy_to_user((void __user *)arg, &val, sizeof(val));

	return 0;
}


static const struct file_operations spi_dac_fops = {

	.owner =	THIS_MODULE,	
//	.open = 100ask_dac_open,	
//	.release = 100ask_spi_dac_release,
	.unlocked_ioctl = spi_dac_ioctl,
	
};





static const struct of_device_id spidac_dt_ids[] = {
	{ .compatible = "100ask,dac" },
	{},
};

MODULE_DEVICE_TABLE(of, spidac_dt_ids);

static int spi_dac_probe(struct spi_device *spi)
{
	/* 1. 记录spi_device */
	dac = spi;

	/* 2. 注册字符设备 */
	major = register_chrdev(0, "100ask_dac", &spi_dac_fops);

	spi_dac_class = class_create(THIS_MODULE, "100ask_dac");
	spi_dac_dev = device_create(spi_dac_class, NULL, MKDEV(major, 0), NULL, "100ask_dac");	

	return 0;
}

static int spi_dac_remove(struct spi_device *spi)
{
	device_destroy(spi_dac_class, MKDEV(major, 0));
	class_destroy(spi_dac_class);

	unregister_chrdev(major, "100ask_dac");
	return 0;
}


static struct spi_driver spi_dac_drv = {
	.driver = {
		.name = "100ask,dac",
		.of_match_table = of_match_ptr(spidac_dt_ids),
	},

	.probe =	spi_dac_probe,
	.remove =	spi_dac_remove,
};

static int __init dac_drv_init(void)
{
	int status;
	status = spi_register_driver(&spi_dac_drv);

	return status;
}

static void __exit dac_drv_exit(void)
{
	spi_unregister_driver(&spi_dac_drv);
}

module_init(dac_drv_init);
module_exit(dac_drv_exit);


MODULE_LICENSE("GPL");






