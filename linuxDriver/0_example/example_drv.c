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
#include <asm/div64.h>

#include <asm/mach/map.h>

static int major;
static struct class *example_class;
static struct device *example_class_device;
static struct gpio_desc *btn_gpio;

static int example_open(struct inode *inode, struct file *file)
{
    printk("%s \n", __func__);

	return 0;
}

static ssize_t example_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    printk("%s \n", __func__);
	
	return 0;
}


/* fops */
static const struct file_operations example_fops = {
	.owner		= THIS_MODULE,
	//.unlocked_ioctl	= tw_chrdev_ioctl,
	.open		= example_open,
    .read       = example_read,
	.release	= NULL,
};

static int example_probe(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    
	/* get gpio from dtb */
    
    
	
	///* gpio back-light */
	//btn_gpio = devm_gpiod_get(&pdev->dev, "example", 0);
    //
    ///* set gpio dir */
    //gpiod_direction_output(btn_gpio, 1);
    

    /* register chardev */
	major = register_chrdev(0, "example", &example_fops);    
	
    /* class create */
	example_class = class_create(THIS_MODULE, "example");

	/* class device create */
	example_class_device = device_create(example_class, NULL, MKDEV(major, 0), NULL, "example");
    
	
    
	return 0;
}

static int example_remove(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    unregister_chrdev(major, "example");
	device_destroy(example_class, MKDEV(major, 0));
	class_destroy(example_class);
	
	return 0;
}

static const struct of_device_id example_of_match[] = {
	{ .compatible = "100ask,example_drv", },
	{ },
};
MODULE_DEVICE_TABLE(of, example_of_match);

static struct platform_driver example_driver = {
	.driver		= {
		.name	= "example_drv",
		.of_match_table = example_of_match,
	},
	.probe		= example_probe,
	.remove		= example_remove,
	// ??? .id_table	= mylcd_driver_ids, //???
};

static int __init example_drv_init(void)
{
	printk("example_drv_init \n");
    int ret;
	ret = platform_driver_register(&example_driver);
	if (ret)
		return ret;

	return 0;
}

/* 2. 出口 */
static void __exit example_drv_exit(void)
{
	platform_driver_unregister(&example_driver);
}


module_init(example_drv_init);
module_exit(example_drv_exit);
//module_platform_driver(example_driver);

MODULE_AUTHOR("will.Chen <wfshwfsh@gmail.com> ");
MODULE_DESCRIPTION("Framebuffer driver for the imx6ull");
MODULE_LICENSE("GPL");



