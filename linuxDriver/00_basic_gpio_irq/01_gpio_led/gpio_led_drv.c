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
static struct class *gpio_led_class;
static struct gpio_desc *led2;

static int gpio_led_open(struct inode *inode, struct file *file)
{
    printk("%s \n", __func__);

    /* set gpio dir */
    gpiod_direction_output(led2, 1);
	
	return 0;
}

static ssize_t gpio_led_read(struct file *file, char __user *buf,
                size_t size, loff_t *offset)
{
    printk("%s \n", __func__);
	
	return 0;
}

static ssize_t gpio_led_write(struct file *file, const char __user *data,
                size_t len, loff_t *ppos)
{
    printk("%s \n", __func__);
	int err;
	int status;
	
    err = copy_from_user(&status, data, sizeof(status));

	printk("len = %d\n", len);
	printk("status = %d\n", status);

	gpiod_set_value(led2, status);
	
	return 0;
}

/* fops */
static const struct file_operations gpio_led_fops = {
	.owner		= THIS_MODULE,
	
	.open		= gpio_led_open,
    .read       = gpio_led_read,
    .write      = gpio_led_write,
	.release	= NULL,
};

static int gpio_led_probe(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    
	/* get gpio from dtb */
    led2 = gpiod_get_index(&pdev->dev, "led", 0, GPIOD_OUT_LOW);
	if (IS_ERR(led2)) {
		printk("Failed to get GPIO for led\n");
		return PTR_ERR(led2);
	}

    /* register chardev */
	major = register_chrdev(0, "gpio_led", &gpio_led_fops);    
	
    /* class create */
	gpio_led_class = class_create(THIS_MODULE, "gpio_led_class");

	/* class device create */
	device_create(gpio_led_class, NULL, MKDEV(major, 0), NULL, "100ask_led%d", 0);
    
	///* gpio back-light */
	//btn_gpio = devm_gpiod_get(&pdev->dev, "gpio_led", 0);
    
	return 0;
}

static int gpio_led_remove(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    
    gpiod_put(led2);
    
	device_destroy(gpio_led_class, MKDEV(major, 0));
	class_destroy(gpio_led_class);
	unregister_chrdev(major, "gpio_led");
    
	return 0;
}

static const struct of_device_id gpio_led_of_match[] = {
	{ .compatible = "100ask,gpio_led", },
	{ },
};
MODULE_DEVICE_TABLE(of, gpio_led_of_match);

static struct platform_driver gpio_led_driver = {
	.driver		= {
		.name	= "gpio_led_drv",
		.of_match_table = gpio_led_of_match,
	},
	.probe		= gpio_led_probe,
	.remove		= gpio_led_remove,
	// ??? .id_table	= mylcd_driver_ids, //???
};

static int __init gpio_led_drv_init(void)
{
	printk("gpio_led_drv_init \n");
    int ret;
	ret = platform_driver_register(&gpio_led_driver);
	if (ret)
		return ret;

	return 0;
}

/* 2. 出口 */
static void __exit gpio_led_drv_exit(void)
{
	platform_driver_unregister(&gpio_led_driver);
}


module_init(gpio_led_drv_init);
module_exit(gpio_led_drv_exit);
//module_platform_driver(gpio_led_driver);

MODULE_AUTHOR("will.Chen <wfshwfsh@gmail.com> ");
MODULE_DESCRIPTION("Framebuffer driver for the imx6ull");
MODULE_LICENSE("GPL");



