
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/acpi.h>

#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include <linux/uaccess.h>
#include <linux/gpio/consumer.h>


#define OLED_CMD 	0
#define OLED_DATA 	1

#define OLED_IOC_INIT 			123
#define OLED_IOC_SET_POS 		124



static int major;

static struct gpio_desc *dc_gpio;

static struct spi_device *oled;

static struct class *spi_oled_class;
static struct device *spi_oled_dev;



static void dc_pin_init(void)
{
	//set dc pin direct to output
	gpiod_direction_output(dc_gpio, 1);
}

static void oled_set_dc_pin(int val)
{
	gpiod_set_value(dc_gpio, val);
}

static void spi_write_datas(const unsigned char *buf, int len)
{
	spi_write(oled, buf, len);
}



/**********************************************************************
	 * ?½æ•°?ç§°ï¼?oled_write_cmd
	 * ?Ÿèƒ½?è¿°ï¼?oled?‘ç‰¹å®šåœ°?€?™å…¥?°æ®?–è€…å‘½ä»?	 * è¾“å…¥?‚æ•°ï¼š@uc_data :è¦å??¥ç??°æ®
	 			@uc_type:ä¸??™è¡¨ç¤ºå??¥æ•°?®ï?ä¸?è¡¨ç¤º?™å…¥?½ä»¤
	 * è¾“å‡º?‚æ•°ï¼šæ?
	 * è¿????¼ï? ??	 * ä¿®æ”¹?¥æ? 	   ?ˆæœ¬?? ä¿®æ”¹äº?	  ä¿®æ”¹?…å®¹
	 * -----------------------------------------------
	 * 2020/03/04		 V1.0	  ?¯æ?		  ?›å»º
 ***********************************************************************/
void oled_write_cmd_data(unsigned char uc_data,unsigned char uc_type)
{
	if(uc_type==0)
	{
        oled_set_dc_pin(OLED_CMD);
	}
	else
	{
        oled_set_dc_pin(OLED_DATA);
	}
    
    spi_write_datas(&uc_data, 1);
}
/**********************************************************************
	 * ?½æ•°?ç§°ï¼?oled_init
	 * ?Ÿèƒ½?è¿°ï¼?oled_init?„å?å§‹å?ï¼Œå??¬SPI?§åˆ¶?¨å??å???	 * è¾“å…¥?‚æ•°ï¼šæ?
	 * è¾“å‡º?‚æ•°ï¼??å??–ç?ç»“æ?
	 * è¿????¼ï? ?å??™è???ï¼Œå¦?™è???1
	 * ä¿®æ”¹?¥æ? 	   ?ˆæœ¬?? ä¿®æ”¹äº?	  ä¿®æ”¹?…å®¹
	 * -----------------------------------------------
	 * 2020/03/15		 V1.0	  ?¯æ?		  ?›å»º
 ***********************************************************************/
static int oled_init(void)
{
	oled_write_cmd_data(0xae,OLED_CMD);//?³é—­?¾ç¤º

	oled_write_cmd_data(0x00,OLED_CMD);//è®¾ç½® lower column address
	oled_write_cmd_data(0x10,OLED_CMD);//è®¾ç½® higher column address

	oled_write_cmd_data(0x40,OLED_CMD);//è®¾ç½® display start line

	oled_write_cmd_data(0xB0,OLED_CMD);//è®¾ç½®page address

	oled_write_cmd_data(0x81,OLED_CMD);// contract control
	oled_write_cmd_data(0x66,OLED_CMD);//128

	oled_write_cmd_data(0xa1,OLED_CMD);//è®¾ç½® segment remap

	oled_write_cmd_data(0xa6,OLED_CMD);//normal /reverse

	oled_write_cmd_data(0xa8,OLED_CMD);//multiple ratio
	oled_write_cmd_data(0x3f,OLED_CMD);//duty = 1/64

	oled_write_cmd_data(0xc8,OLED_CMD);//com scan direction

	oled_write_cmd_data(0xd3,OLED_CMD);//set displat offset
	oled_write_cmd_data(0x00,OLED_CMD);//

	oled_write_cmd_data(0xd5,OLED_CMD);//set osc division
	oled_write_cmd_data(0x80,OLED_CMD);//

	oled_write_cmd_data(0xd9,OLED_CMD);//ser pre-charge period
	oled_write_cmd_data(0x1f,OLED_CMD);//

	oled_write_cmd_data(0xda,OLED_CMD);//set com pins
	oled_write_cmd_data(0x12,OLED_CMD);//

	oled_write_cmd_data(0xdb,OLED_CMD);//set vcomh
	oled_write_cmd_data(0x30,OLED_CMD);//

	oled_write_cmd_data(0x8d,OLED_CMD);//set charge pump disable 
	oled_write_cmd_data(0x14,OLED_CMD);//

	oled_write_cmd_data(0xaf,OLED_CMD);//set dispkay on

	return 0;
}		  			 		  						  					  				 	   		  	  	 	  


static int spi_oled_open(struct inode *inode, struct file *filp)
{
	//dc_pin_init 
	//dc_pin_init();
	
	//init oled boot seq
	//oled_init();
	
	return 0;
}

static int spi_oled_release(struct inode *inode, struct file *filp)
{
	
	return 0;
}

static long
spi_oled_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int x, y;
	
	//init
	switch (cmd){
	case OLED_IOC_INIT:
		//dc_pin_init 
		dc_pin_init();
		//init oled boot seq
		oled_init();
		break;

	case OLED_IOC_SET_POS:

		x = (arg & 0x7f);
		y = ((arg >> 8) & 0x7);
		
		//set position
		oled_set_dc_pin(0);
		oled_write_cmd_data(0xb0+y,OLED_CMD);
		oled_write_cmd_data((x&0x0f),OLED_CMD); 
		oled_write_cmd_data(((x&0xf0)>>4)|0x10,OLED_CMD);
		
		break;
	}

	
	return 0;
}


static ssize_t
spi_oled_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	unsigned char *kbuf;
	int err;

	kbuf = kmalloc(count, GFP_KERNEL);
	err = copy_from_user(kbuf, buf, count);

	oled_set_dc_pin(1);
	spi_write_datas(kbuf, count);
	
	kfree(kbuf);
	return count;
}


static const struct file_operations spi_oled_fops = {

	.owner =	THIS_MODULE,	
	.open = spi_oled_open,	
	.release = spi_oled_release,

	.unlocked_ioctl = spi_oled_ioctl,
	.write = spi_oled_write,
};





static const struct of_device_id spi_oled_dt_ids[] = {
	{ .compatible = "100ask,oled" },
	{},
};

MODULE_DEVICE_TABLE(of, spi_oled_dt_ids);


static int spi_oled_probe(struct spi_device *spi)
{
	printk("%s\n", __func__);
	
	/* 1. è®°å?spi_device */
	oled = spi;

	/* 2. æ³¨å?å­—ç¬¦è®¾å? */
	major = register_chrdev(0, "100ask_oled", &spi_oled_fops);

	spi_oled_class = class_create(THIS_MODULE, "100ask_oled");
	spi_oled_dev = device_create(spi_oled_class, NULL, MKDEV(major, 0), NULL, "100ask_oled");	

	//get dc_pin from dts
	dc_gpio = gpiod_get(&oled->dev, "dc", 0);
	
	return 0;
}

static int spi_oled_remove(struct spi_device *spi)
{
	gpiod_put(dc_gpio);
	
	device_destroy(spi_oled_class, MKDEV(major, 0));
	class_destroy(spi_oled_class);

	unregister_chrdev(major, "100ask_oled");
	return 0;
}


static struct spi_driver spi_oled_drv = {
	.driver = {
		.name = "100ask,oled",
		.of_match_table = of_match_ptr(spi_oled_dt_ids),
	},

	.probe =	spi_oled_probe,
	.remove =	spi_oled_remove,
};

static int __init oled_drv_init(void)
{
	int status;
	status = spi_register_driver(&spi_oled_drv);

	return status;
}

static void __exit oled_drv_exit(void)
{
	spi_unregister_driver(&spi_oled_drv);
}

module_init(oled_drv_init);
module_exit(oled_drv_exit);


MODULE_LICENSE("GPL");






