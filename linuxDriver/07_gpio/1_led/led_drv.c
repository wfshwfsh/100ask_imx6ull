//Reference: s3c2410fb.c
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
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <asm/div64.h>

#include <asm/mach/map.h>

static int major;
static struct class *led_drv_class;
static struct gpio_desc *led_gpio;

static int led_drv_open(struct inode *inode, struct file *file)
{
    printk("%s \n", __func__);
    
    /* set gpio direction */
	gpiod_direction_output(led_gpio, 0);
	
	return 0;
}

static ssize_t led_drv_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    printk("%s \n", __func__);
	int k_value;
	
    /* get led value */
    k_value = gpiod_get_value(led_gpio);

	return copy_to_user(buf, &k_value, sizeof(k_value));
}

static ssize_t led_drv_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    printk("%s \n", __func__);
	int err;
	int k_value;

	err = copy_from_user(&k_value, buf, sizeof(k_value));

	printk("k_value = %d \n", k_value);
    /* set led value */
	gpiod_set_value(led_gpio, k_value);
	
	return 0;
}


/* fops */
static const struct file_operations led_drv_fops = {
	.owner		= THIS_MODULE,
	//.unlocked_ioctl	= tw_chrdev_ioctl,
	.open		= led_drv_open,
    .read       = led_drv_read,
    .write      = led_drv_write,
	.release	= NULL,
};

static int led_drv_probe(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    
	/* get gpio_desc from dtb */
    led_gpio = gpiod_get(&pdev->dev, "led"/*label*/, 0);
    if (IS_ERR(led_gpio)) {
		dev_err(&pdev->dev, "Failed to get GPIO for led\n");
		return PTR_ERR(led_gpio);
	}

    /* register chardev */
	major = register_chrdev(0, "100ask_led", &led_drv_fops);    
	
    /* class create */
	led_drv_class = class_create(THIS_MODULE, "100ask_led_class");

	/* class device create */
	device_create(led_drv_class, NULL, MKDEV(major, 0), NULL, "100ask_led");
    
	
    
	return 0;
}

static int led_drv_remove(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    unregister_chrdev(major, "led_drv");
	device_destroy(led_drv_class, MKDEV(major, 0));
	class_destroy(led_drv_class);
	
	return 0;
}

static const struct of_device_id led_drv_of_match[] = {
	{ .compatible = "100ask,gpio_led", },
	{ },
};
MODULE_DEVICE_TABLE(of, led_drv_of_match);

static struct platform_driver led_drv_driver = {
	.driver		= {
		.name	= "led_drv",
		.of_match_table = led_drv_of_match,
	},
	.probe		= led_drv_probe,
	.remove		= led_drv_remove,
	// ??? .id_table	= mylcd_driver_ids, //???
};

static int __init led_drv_drv_init(void)
{
	printk("led_drv_drv_init \n");
    int ret;
	ret = platform_driver_register(&led_drv_driver);
	if (ret)
		return ret;

	return 0;
}

/* 2. 出口 */
static void __exit led_drv_drv_exit(void)
{
	platform_driver_unregister(&led_drv_driver);
}


module_init(led_drv_drv_init);
module_exit(led_drv_drv_exit);
//module_platform_driver(led_drv_driver);

MODULE_AUTHOR("will.Chen <wfshwfsh@gmail.com> ");
MODULE_DESCRIPTION("Framebuffer driver for the imx6ull");
MODULE_LICENSE("GPL");



