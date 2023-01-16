#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/pci.h>
#include <linux/acpi.h>



static int i2c_example_probe(struct i2c_client *client, const struct i2c_device_id *devid)
{
    
    return 0;
}

static int i2c_example_remove(struct i2c_client *client)
{
	
	return 0;
}


static const struct i2c_device_id i2c_example_id[] = {
	{ "i2c_example", 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, i2c_example_id);

static const struct of_device_id i2c_exmaple_of_match[] = {
	{ .compatible = "i2c,example", },
	{},
};
MODULE_DEVICE_TABLE(of, i2c_exmaple_of_match);

static struct i2c_driver i2c_example_driver = {
	.driver = {
		   .name = "i2c_exmaple",
		   .of_match_table = of_match_ptr(i2c_exmaple_of_match),
	},
	.probe  = i2c_example_probe,
    .remove = i2c_example_remove,
	.id_table = i2c_example_id,
};

static int __init i2c_example_init(void)
{
	return i2c_add_driver(&i2c_example_driver);
}
module_init(i2c_example_init);

static void __exit i2c_example_exit(void)
{
	i2c_del_driver(&i2c_example_driver);
}
module_exit(i2c_example_exit);

MODULE_AUTHOR("Will Chen");
MODULE_LICENSE("GPL");
