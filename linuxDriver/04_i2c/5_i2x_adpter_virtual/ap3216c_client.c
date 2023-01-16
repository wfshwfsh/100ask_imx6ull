#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/property.h>
#include <linux/acpi.h>
#include <linux/i2c.h>
#include <linux/nvmem-provider.h>
#include <linux/regmap.h>
#include <linux/pm_runtime.h>
#include <linux/gpio/consumer.h>
#include <linux/uaccess.h>
#include <linux/fs.h>


struct i2c_client *ap3216c_client;
#if 0
static int __init ap3216c_init(void)
{
    struct i2c_adapter *i2c_adap;
	struct i2c_board_info board_info = {
        I2C_BOARD_INFO("ap3216c", 0x1e),
	};
	
    i2c_adap = i2c_get_adapter(0/*i2c-bus0*/);
    
	ap3216c_client = i2c_new_device(i2c_adap, &board_info);
	if (ap3216c_client == NULL) {
		printk("i2c_new_device failed");
	}
    
    i2c_put_adapter(i2c_adap);
	return 0;
}

#else
static const unsigned short normal_i2c[] = { 0x1e, I2C_CLIENT_END };

static int __init ap3216c_init(void)
{
    struct i2c_adapter *i2c_adap;
	struct i2c_board_info board_info = {
        I2C_BOARD_INFO("ap3216c", 0x1e),
	};
	
    i2c_adap = i2c_get_adapter(0/*i2c-bus0*/);
    
	ap3216c_client = i2c_new_probed_device(i2c_adap, &board_info,
						   normal_i2c, NULL);
	
	if (ap3216c_client == NULL) {
		printk("i2c_new_device failed");
	}
    
    i2c_put_adapter(i2c_adap);
	return 0;
}


#endif

module_init(ap3216c_init);

static void __exit ap3216c_exit(void)
{
	i2c_unregister_device(ap3216c_client);
}
module_exit(ap3216c_exit);


MODULE_AUTHOR("Will Chen");
MODULE_LICENSE("GPL");
