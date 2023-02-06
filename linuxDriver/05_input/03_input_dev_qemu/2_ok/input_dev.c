
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
#include <linux/of_device.h>
#include <linux/of_gpio.h>

#include <linux/spinlock.h>

#define TOUCHSCREEN_POLL_TIME_MS 10

static struct qemu_ts_con{
	volatile unsigned int pressure;
	volatile unsigned int x;
	volatile unsigned int y;
	volatile unsigned int clean;
};

static struct input_dev *input_dev;
static int g_irq;
 
static struct qemu_ts_con *ts_con;
struct timer_list ts_timer;

static void ts_irq_timer(unsigned long _data)
{
	if (ts_con->pressure) // pressed
	{
		//after isr, if still pressed, keep update timer to update input data
		input_event(input_dev, EV_ABS, ABS_X, ts_con->x);
		input_event(input_dev, EV_ABS, ABS_Y, ts_con->y);
		input_sync(input_dev);

		mod_timer(&ts_timer,
			jiffies + msecs_to_jiffies(TOUCHSCREEN_POLL_TIME_MS));
	}

}

static irqreturn_t input_dev_demo_isr(int irq, void *dev)
{
	printk("input_dev_demo_isr\n");
    //read data
    
    //report data
    if(ts_con->pressure)
	{
		input_event(input_dev, EV_ABS, ABS_X, ts_con->x);
		input_event(input_dev, EV_ABS, ABS_Y, ts_con->y);
		
		input_event(input_dev, EV_KEY, BTN_TOUCH, 1);
		input_sync(input_dev);
		
		/* start timer */
		mod_timer(&ts_timer,
			jiffies + msecs_to_jiffies(TOUCHSCREEN_POLL_TIME_MS));
    }
	else
	{
		input_event(input_dev, EV_KEY, BTN_TOUCH, 0);
		input_sync(input_dev);
	}
    return IRQ_HANDLED;
}

static int input_dev_demo_probe(struct platform_device *pdev)
{
    printk("%s \n", __func__);
	struct resource *irq;
	int ret, err;
	struct resource *iomem;
	int gpio;
	
	gpio = of_get_gpio(pdev->dev.of_node, 0);
	
    //1. allocate input device
	input_dev = input_allocate_device();
    if (!input_dev) {
		printk("Unable to allocate the input device !!\n");
		return -ENOMEM;
	}
    
	//2. setting input device
    input_dev->name = "100ask_imx6ull TouchScreen";
	input_dev->id.bustype = BUS_HOST;
	input_dev->id.vendor  = 0x1;
	input_dev->id.product = 0x1;
	input_dev->id.version = 0x1;
	
    
    //2.1 which event type
    input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
    //__set_bit(EV_KEY, input_dev->evbit);
	//__set_bit(EV_ABS, input_dev->evbit);
    
    //2.2 which event
    __set_bit(BTN_TOUCH, input_dev->keybit);
	__set_bit(ABS_X, input_dev->absbit);
	__set_bit(ABS_Y, input_dev->absbit);
    
	//__set_bit(ABS_PRESSURE, input_dev->absbit);
	__set_bit(EV_SYN, input_dev->evbit);
	
	
	/* event params */
	input_set_abs_params(input_dev, ABS_MT_POSITION_X, 0, 0xFFFF, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y, 0, 0xFFFF, 0, 0);
	
	
    //3. register input device
	err = input_register_device(input_dev);

    //4. get irq-gpio from dts, gpio to irq
    //irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	//g_irq = irq->start;
	
	iomem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!iomem)
		return -ENOMEM;

	int len = resource_size(iomem);
	if (len < 0x100)
		printk("Invalid iomem size!\n");
	
	ts_con = ioremap(iomem->start, len);
	
	g_irq = gpio_to_irq(gpio);
    ret = request_irq(g_irq, input_dev_demo_isr, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, "input_dev_demo_isr", NULL);
    
	setup_timer(&ts_timer,
			ts_irq_timer, (unsigned long)NULL);
    return 0;
}

static int input_dev_demo_remove(struct platform_device *pdev)
{
	free_irq(g_irq, NULL);
    iounmap(ts_con);
	
    input_unregister_device(input_dev);
	
	input_free_device(input_dev);
	
	
	
	return 0;
}

static const struct of_device_id input_dev_demo_of_match[] = {
	{ .compatible = "100ask,input_dev_demo", },
	{},
};
MODULE_DEVICE_TABLE(of, input_dev_demo_of_match);

static struct platform_driver input_dev_demo_driver = {
	.driver = {
		   .name = "input_dev_demo",
		   .of_match_table = of_match_ptr(input_dev_demo_of_match),
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
