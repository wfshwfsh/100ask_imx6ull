#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>


struct i2c_adapter *g_adapter;

static int i2c_bus_virtual_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[],
			    int num)
{
    int i;
    
    for(i=0;i<num;i++)
    {
        //do transfer msgs[i];
        
    }
    
    return i;
}

static u32 i2c_bus_virtual_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_NOSTART | I2C_FUNC_SMBUS_EMUL |
	       I2C_FUNC_SMBUS_READ_BLOCK_DATA |
	       I2C_FUNC_SMBUS_BLOCK_PROC_CALL |
	       I2C_FUNC_PROTOCOL_MANGLING;
}

static const struct i2c_algorithm i2c_bus_virtual_algo = {
	.master_xfer	= i2c_bus_virtual_xfer,
	.functionality	= i2c_bus_virtual_func,
};



static int i2c_bus_virtaul_probe(struct platform_device *pdev)
{
    printk("i2c_bus_virtaul_probe \n");
    
    g_adapter = kzalloc(sizeof(*g_adapter), GFP_KERNEL);
    
    adap->owner = THIS_MODULE;
	g_adapter->class = I2C_CLASS_HWMON | I2C_CLASS_SPD;//???
    g_adapter->nr = -1;
    g_adapter->algo = &i2c_bus_virtual_algo;
    snprintf(g_adapter->name, sizeof(g_adapter->name), "i2c-bus-virtual");
    
    /* i2c_add_adapter */
    ret = i2c_add_adapter(g_adapter);
    
	return 0;
}

static int i2c_bus_virtaul_remove(struct platform_device *pdev)
{
    printk("i2c_bus_virtaul_remove \n");
	i2c_del_adapter(g_adapter);
    
	return 0;
}

static const struct of_device_id i2c_adpter_virtaul_of_match[] = {
	{ .compatible = "i2c_adpter_virtaul", },
	{ },
};
MODULE_DEVICE_TABLE(of, i2c_adpter_virtaul_of_match);

static struct platform_driver i2c_bus_virtual_driver = {
	.driver		= {
		.name	= "i2c_adpter_virtaul",
        .owner = THIS_MODULE,
		.of_match_table = i2c_adpter_virtaul_of_match,
	},
	.probe		= i2c_bus_virtaul_probe,
	.remove		= i2c_bus_virtaul_remove,
	// ??? .id_table	= mylcd_driver_ids, //???
};

static int __init i2c_bus_virtual_init(void)
{
	printk("i2c_bus_virtual_init \n");
    int ret;
	ret = platform_driver_register(&i2c_bus_virtual_driver);
	if (ret)
		return ret;

	return 0;
}

/* 2. 出口 */
static void __exit i2c_bus_virtual_exit(void)
{
    printk("i2c_bus_virtual_exit \n");
	platform_driver_unregister(&i2c_bus_virtual_driver);
}


//module_init(test_drv_init);
//module_exit(test_drv_exit);

MODULE_AUTHOR("will.Chen <wfshwfsh@gmail.com> ");
MODULE_DESCRIPTION("Framebuffer driver for the imx6ull");
MODULE_LICENSE("GPL");