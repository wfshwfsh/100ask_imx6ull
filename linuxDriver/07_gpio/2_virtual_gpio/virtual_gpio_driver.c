/* Target: virtual GPIO Controller driver */

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/gpio/driver.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/bug.h>
#include <linux/gpio/driver.h>




static struct gpio_chip *g_virt_gpio;
static int g_gpio_val = 0;
static int g_gpio_dir = 0;



static int virt_gpio_dir_out(struct gpio_chip *gc, unsigned int offset, int val)
{
#if 0
	unsigned long flags;

	gc->set(gc, gpio, val);

	spin_lock_irqsave(&gc->bgpio_lock, flags);

	gc->bgpio_dir |= gc->pin2mask(gc, gpio);
	gc->write_reg(gc->reg_dir, gc->bgpio_dir);

	spin_unlock_irqrestore(&gc->bgpio_lock, flags);

#else
	printk("set pin %d as output %s\n", offset, val ? "high" : "low");
	g_gpio_dir &= ~(1<<offset);
#endif
	return 0;
}

static int virt_gpio_dir_in(struct gpio_chip *gc, unsigned int offset)
{
#if 0
	unsigned long flags;

	spin_lock_irqsave(&gc->bgpio_lock, flags);

	gc->bgpio_dir &= ~gc->pin2mask(gc, gpio);
	gc->write_reg(gc->reg_dir, gc->bgpio_dir);

	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
#else
	printk("set pin %d as input\n", offset);
	g_gpio_dir |= (1<<offset);
#endif
	return 0;
}

static int virt_gpio_get_dir(struct gpio_chip *gc, unsigned int offset)
{
	/* Return 0 if output, 1 of input */
	//return !(gc->read_reg(gc->reg_dir) & gc->pin2mask(gc, gpio));
	return ((g_gpio_dir & (1<<offset)) >> offset);
}


static void virt_gpio_set(struct gpio_chip *gc, unsigned int offset, int val)
{
#if 0
	unsigned long mask = gc->pin2mask(gc, gpio);
	unsigned long flags;

	spin_lock_irqsave(&gc->bgpio_lock, flags);

	if (val)
		gc->bgpio_data |= mask;
	else
		gc->bgpio_data &= ~mask;

	gc->write_reg(gc->reg_set, gc->bgpio_data);

	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
#else
	if(val)
		g_gpio_val |= (1<<offset);
	else
		g_gpio_val &= ~(1<<offset);
#endif
}

static int virt_gpio_get(struct gpio_chip *gc, unsigned int offset)
{
	//return !!(gc->read_reg(gc->reg_dat) & gc->pin2mask(gc, gpio));
	
	return (g_gpio_val & (1<<ofset));
}



static int virtual_gpio_probe(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    
    /* 1. alloc */
    g_virt_gpio = devm_kzalloc(&pdev->dev, sizeof(*g_virt_gpio), GFP_KERNEL);
	if(!g_virt_gpio)
		return -ENOMEM;
    
    /* 2. configure gpio_chip */
	
	//g_virt_gpio->request = mxc_gpio_request;
	//g_virt_gpio->free    = mxc_gpio_free;
	g_virt_gpio->parent  = &pdev->dev;
	g_virt_gpio->owner   = THIS_MODULE;

	//g_virt_gpio->label   = 
	//g_virt_gpio->base    = 
	//g_virt_gpio->ngpio   = 

	//g_virt_gpio->reg_set = set;
	g_virt_gpio->set = virt_gpio_set;
	g_virt_gpio->get = virt_gpio_get;
	//g_virt_gpio->set_multiple = bgpio_set_multiple_set;

	//g_virt_gpio->reg_dir = dirout;
	g_virt_gpio->direction_output = virt_gpio_dir_out;
	g_virt_gpio->direction_input = virt_gpio_dir_in;
	g_virt_gpio->get_direction = virt_gpio_get_dir;
	
	
    /* 3. register */
	devm_gpiochip_add_data(&pdev->dev, g_virt_gpio, NULL);
	
	return 0;
}

static int virtual_gpio_remove(struct platform_device *pdev)
{
    printk("%s \n", __func__);
    
	return 0;
}

static const struct of_device_id virtual_gpio_of_match[] = {
	{ .compatible = "100ask,virtual_gpio_ctrl", },
	{ },
};
MODULE_DEVICE_TABLE(of, virtual_gpio_of_match);

static struct platform_driver virtual_gpio_driver = {
	.driver		= {
		.name	= "virtual_gpio_ctrl",
		.of_match_table = virtual_gpio_of_match,
	},
	.probe		= virtual_gpio_probe,
	.remove		= virtual_gpio_remove,
	// ??? .id_table	= mylcd_driver_ids, //???
};

static int __init virtual_gpio_drv_init(void)
{
	printk("virtual_gpio_drv_init \n");
    int ret;
	ret = platform_driver_register(&virtual_gpio_driver);
	if (ret)
		return ret;

	return 0;
}

/* 2. 出口 */
static void __exit virtual_gpio_drv_exit(void)
{
	platform_driver_unregister(&virtual_gpio_driver);
}


module_init(virtual_gpio_drv_init);
module_exit(virtual_gpio_drv_exit);
//module_platform_driver(virtual_gpio_driver);

MODULE_AUTHOR("will.Chen <wfshwfsh@gmail.com> ");
MODULE_DESCRIPTION("Framebuffer driver for the imx6ull");
MODULE_LICENSE("GPL");



