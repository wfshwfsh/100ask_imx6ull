//Reference: kernel
//  Documentation/gpio/consumer.txt
//  Documentation/gpio/board.txt

#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>



struct imx6ull_key_irq{
    int gpio; //gpio_id
    struct gpio_desc *gpiod;
    int flag; //IRQF_	
    int irq;
};

static struct imx6ull_key_irq *key_irq;

static int major = 0;
static struct class *gpio_key_class;
static int g_key = 0;

static DECLARE_WAIT_QUEUE_HEAD(gpio_key_wait);

static ssize_t gpio_key_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    //printk("%s \n", __func__);
	int k_value, ret;
    
    wait_event_interruptible(gpio_key_wait, g_key);
    
	printk("g_key = %d\n", g_key);
	ret = copy_to_user(buf, &g_key, sizeof(g_key));
    g_key = 0;
    
	return ret;
}

static irqreturn_t imx6ull_key_isr(int irq, void *dev_id)
{
	//printk("%s \n", __func__);
	struct imx6ull_key_irq *_key_irq = dev_id;
	int k_value;

	k_value = gpiod_get_value(_key_irq->gpiod);
	printk("gpio = %d\n", _key_irq->gpio);
    
    printk("k_value = %d\n", k_value);
	g_key = k_value;
    wake_up_interruptible(&gpio_key_wait);
    
	return IRQ_HANDLED;
}




/* fops */
static const struct file_operations gpio_key_fops = {
	.owner		= THIS_MODULE,
	
    .read       = gpio_key_read,
	.release	= NULL,
};

static int gpio_key_probe(struct platform_device *pdev)
{
    printk("%s \n", __func__);
	enum of_gpio_flags flag;
	struct device_node *key_node = pdev->dev.of_node;
	int i, count;
	
	/* get gpio from dtb */
    count = of_gpio_count(key_node);
    printk("count = %d\n", count);
    if (!count)
	{
		printk("%s %s line %d, there isn't any gpio available\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
    
	key_irq = kzalloc(sizeof(struct imx6ull_key_irq)*count, GFP_KERNEL);
    if(NULL == key_irq)
    {
        printk("%s %s line %d, kzalloc failed \n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
    }
    
	/* gpio => irq */
	for(i=0;i<count;i++)
	{
		key_irq[i].gpio = of_get_gpio_flags(key_node, i, &flag);
        if(key_irq[i].gpio < 0){
            printk("%s %s line %d, of_get_gpio_flags fail\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
		key_irq[i].gpiod = gpio_to_desc(key_irq[i].gpio);
        key_irq[i].flag = flag & OF_GPIO_ACTIVE_LOW;
		key_irq[i].irq  = gpio_to_irq(key_irq[i].gpio);
	}
	
    /* request_irq */
    for(i=0;i<count;i++)
    {
        int retval = request_irq(key_irq[i].irq, imx6ull_key_isr,
                        IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                        "imx6ull_key_isr", &key_irq[i]);
        
        if (retval) {
            printk("unable to request irq %d\n", key_irq[i].irq);
            return -1;
        }
    }
	
    /* register chardev */
	major = register_chrdev(0, "gpio_key", &gpio_key_fops);    
	
    /* class create */
	gpio_key_class = class_create(THIS_MODULE, "gpio_key_class");
    
	/* class device create */
	device_create(gpio_key_class, NULL, MKDEV(major, 0), NULL, "100ask_key");
    
	return 0;
}

static int gpio_key_remove(struct platform_device *pdev)
{
    printk("%s \n", __func__);
	struct device_node *node = pdev->dev.of_node;
	int count;
	int i;
    
	device_destroy(gpio_key_class, MKDEV(major, 0));
	class_destroy(gpio_key_class);
	unregister_chrdev(major, "gpio_key");
    
	count = of_gpio_count(node);
	for (i = 0; i < count; i++)
	{
		free_irq(key_irq[i].irq, &key_irq[i]);
	}
	kfree(key_irq);

    
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

/* 2. ?ºå£ */
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



