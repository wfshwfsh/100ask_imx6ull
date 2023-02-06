//Reference: kernel
//  Documentation/gpio/consumer.txt
//  Documentation/gpio/board.txt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/io.h>
#include <linux/gpio/consumer.h>

#include <asm/div64.h>

#include <asm/mach/map.h>
#include <linux/uaccess.h>


static int major;
static struct class *gpio_key_class;
static struct gpio_desc *key1, *key2;

static int gpio_key_open(struct inode *inode, struct file *file)
{
    printk("%s \n", __func__);

    /* set gpio dir */
    gpiod_direction_input(key2);
	
	return 0;
}

static ssize_t gpio_key_read(struct file *file, char __user *buf,
                size_t size, loff_t *offset)
{
    printk("%s \n", __func__);
	int k_value;

	k_value = gpiod_get_value(key2);
	printk("k_value = %d\n", k_value);
	
	return copy_to_user(buf, &k_value, sizeof(k_value));
}

static ssize_t gpio_key_write(struct file *file, const char __user *data,
                size_t len, loff_t *ppos)
{
    printk("%s \n", __func__);

	return 0;
}

/* fops */
static const struct file_operations gpio_key_fops = {
	.owner		= THIS_MODULE,
	
	.open		= gpio_key_open,
    .read       = gpio_key_read,
    .write      = gpio_key_write,
	.release	= NULL,
};

static int gpio_key_probe(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    
	/* get gpio from dtb */
    key2 = gpiod_get_index(&pdev->dev, "key", 1, GPIOD_IN);//<gpio4, 14>
	if (IS_ERR(key2)) {
		printk("Failed to get GPIO for key\n");
		return PTR_ERR(key2);
	}

    /* register chardev */
	major = register_chrdev(0, "gpio_key", &gpio_key_fops);    
	
    /* class create */
	gpio_key_class = class_create(THIS_MODULE, "gpio_key_class");

	/* class device create */
	device_create(gpio_key_class, NULL, MKDEV(major, 0), NULL, "100ask_key%d", 1);
    //device_create(gpio_key_class, NULL, MKDEV(major, 0), NULL, "100ask_key%d", 2);
    
	///* gpio back-light */
	//btn_gpio = devm_gpiod_get(&pdev->dev, "gpio_key", 0);
    
	return 0;
}

static int gpio_key_remove(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    
    gpiod_put(key2);
    
	device_destroy(gpio_key_class, MKDEV(major, 0));
	class_destroy(gpio_key_class);
	unregister_chrdev(major, "gpio_key");
    
	return 0;
}

static const struct of_device_id gpio_key_of_match[] = {
	{ .compatible = "100ask,gpio_key", },
	{ },
};
MODULE_DEVICE_TABLE(of, gpio_key_of_match);

static struct platform_driver gpio_key_driver = {
	.driver		= {
		.name	= "gpio_key_drv",
		.of_match_table = gpio_key_of_match,
	},
	.probe		= gpio_key_probe,
	.remove		= gpio_key_remove,
	// ??? .id_table	= mylcd_driver_ids, //???
};

static int __init gpio_key_drv_init(void)
{
	printk("gpio_key_drv_init \n");
    int ret;
	ret = platform_driver_register(&gpio_key_driver);
	if (ret)
		return ret;

	return 0;
}

/* 2. 出口 */
static void __exit gpio_key_drv_exit(void)
{
	platform_driver_unregister(&gpio_key_driver);
}


module_init(gpio_key_drv_init);
module_exit(gpio_key_drv_exit);
//module_platform_driver(gpio_key_driver);

MODULE_AUTHOR("will.Chen <wfshwfsh@gmail.com> ");
MODULE_DESCRIPTION("Framebuffer driver for the imx6ull");
MODULE_LICENSE("GPL");



