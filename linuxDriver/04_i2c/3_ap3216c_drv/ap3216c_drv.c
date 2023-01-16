#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/pci.h>
#include <linux/acpi.h>


static struct class *ap3216c_class;

struct i2c_client *ap3216c_client;



static int ap3216c_open(struct inode *inode, struct file *file)
{
	/* reset */
	i2c_smbus_write_byte_data(ap3216c_client, 0, 0x4);
	
	/* enable */
	i2c_smbus_write_byte_data(ap3216c_client, 0, 0x3);

    
	return 0;
}

static ssize_t ap3216c_read(struct file *file, char __user *buf, size_t count,
		loff_t *offset)
{
	unsigned char k_buf[6];
	int val;
    
    if (count != 6)
		return -EINVAL;
    
	/* read IR */
	val = i2c_smbus_read_word_data(ap3216c_client, 0xC);
	k_buf[0] = (val & 0xff);
	k_buf[1] = ((val>>8) & 0xff);
	
	/* read light */
	val = i2c_smbus_read_word_data(ap3216c_client, 0xC);
	k_buf[2] = (val & 0xff);
	k_buf[3] = ((val>>8) & 0xff);
	
	/* read dist */
	val = i2c_smbus_read_word_data(ap3216c_client, 0xE);
	k_buf[4] = (val & 0xff);
	k_buf[5] = ((val>>8) & 0xff);

	err = copy_to_user(buf, k_buf, count);
	return count;
}

static int ap3216c_release(struct inode *inode, struct file *file)
{
	return 0;
}



static const struct file_operations ap3216c_fops = {
	.owner		= THIS_MODULE,
	.open		= ap3216c_open,
	.read		= ap3216c_read,
	.release	= ap3216c_release,
};


static int ap3216c_probe(struct i2c_client *client, const struct i2c_device_id *devid)
{
    ap3216c_client = client; //??? client何時被宣告出來?
    
    major = register_chrdev(0, "ap3216c", &ap3216c_fops);
    
    ap3216c_class = class_create(THIS_MODULE, "ap3216c_class");
    
    device_create(ap3216c_class, NULL, MKDEV(major, 0), NULL, "ap3216c");
    
    
    return 0;
}

static int ap3216c_remove(struct i2c_client *client)
{
	device_destroy(ap3216c_class, MKDEV(major, 0));
	
    class_destroy(ap3216c_class);

	unregister_chrdev(major, "ap3216c");
	
    
	return 0;
}


static const struct i2c_device_id ap3216c_id[] = {
	{ "ap3216c", 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, ap3216c_id);

static const struct of_device_id i2c_exmaple_of_match[] = {
	{ .compatible = "i2c,example", },
	{},
};
MODULE_DEVICE_TABLE(of, i2c_exmaple_of_match);

static struct i2c_driver ap3216c_driver = {
	.driver = {
		   .name = "ap3216c",
		   .of_match_table = of_match_ptr(i2c_exmaple_of_match),
	},
	.probe  = ap3216c_probe,
    .remove = ap3216c_remove,
	.id_table = ap3216c_id,
};

static int __init ap3216c_init(void)
{
	return i2c_add_driver(&ap3216c_driver);
}
module_init(ap3216c_init);

static void __exit ap3216c_exit(void)
{
	i2c_del_driver(&ap3216c_driver);
}
module_exit(ap3216c_exit);

MODULE_AUTHOR("Will Chen");
MODULE_LICENSE("GPL");
