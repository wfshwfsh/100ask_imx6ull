
#include <linux/module.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>


static struct input_dev *input_dev;
static int g_irq;


static irqreturn_t input_dev_demo_isr(int irq, void *dev_instance)
{
	printk("input_dev_demo_isr \n");
    //read data
    
    //report data
    
	
    return IRQ_HANDLED;
}

static int input_dev_demo_probe(struct i2c_client *client, const struct i2c_device_id *devid)
{
    printk("%s \n", __func__);
	struct resource *irq;
	int ret, err;
	
    //1. allocate input device
	input_dev = input_allocate_device();
    if (!input_dev) {
		dev_err(dev, "Unable to allocate the input device !!\n");
		ret = -ENOMEM;
		goto err_iomap;
	}
    
	//2. setting input device
    input_dev->name = "100ask_imx6ull TouchScreen";
	input_dev->id.bustype = BUS_HOST;
	input_dev->id.vendor = 0x1;
	input_dev->id.product = 0x1;
	input_dev->id.version = 0x1;
	
    
    //2.1 which event type
    input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	
    
    //2.2 which event
    input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	input_dev->absbit[BIT_WORD()]

	__set_bit(EV_ABS, touch->idev->evbit);
	__set_bit(ABS_X, touch->idev->absbit);
	__set_bit(ABS_Y, touch->idev->absbit);
	__set_bit(ABS_PRESSURE, touch->idev->absbit);
	
	__set_bit(EV_SYN, touch->idev->evbit);
	__set_bit(EV_KEY, touch->idev->evbit);
	__set_bit(BTN_TOUCH, touch->idev->keybit);

	
	
	input_set_abs_params(input_dev, ABS_MT_POSITION_X, 0, 0xFFFF, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y, 0, 0xFFFF, 0, 0);
	
	
    
    //3. register input device
	err = input_register_device(input_dev);

    //4. get irq-gpio from dts, gpio to irq
    irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	g_irq = irq->start;
    
    ret = request_irq(g_irq, input_dev_demo_isr, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "input_dev_demo_isr", NULL)
    
    return 0;
}

static int input_dev_demo_remove(struct i2c_client *client)
{
	free_irq(BUTTON_IRQ, button_interrupt);
    
    input_unregister_device(input_dev);
	
	input_free_device(input_dev);
	
	
	
	return 0;
}


static const struct i2c_device_id input_dev_demo_id[] = {
	{ "input_dev_demo", 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, input_dev_demo_id);

static const struct of_device_id i2c_exmaple_of_match[] = {
	{ .compatible = "100ask,input_dev_demo", },
	{},
};
MODULE_DEVICE_TABLE(of, i2c_exmaple_of_match);

static struct platform_driver input_dev_demo_driver = {
	.driver = {
		   .name = "input_dev_demo",
		   .of_match_table = of_match_ptr(i2c_exmaple_of_match),
	},
	.probe  = input_dev_demo_probe,
    .remove = input_dev_demo_remove,
};

static int __init input_dev_demo_init(void)
{
	return platform_driver_register(&input_dev_demo_driver);
}

static void __exit input_dev_demo_exit(void)
{
	platform_driver_unregister(&input_dev_demo_driver);
}

module_init(input_dev_demo_init);
module_exit(input_dev_demo_exit);

MODULE_AUTHOR("Will Chen");
MODULE_LICENSE("GPL");
