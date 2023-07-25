
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



static unsigned int pseudo_palette[16];

static inline u_int chan_to_field(u_int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static int s3c2410fb_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
    printk("s3c2410fb_setcolreg \n");
	struct s3c2410fb_info *fbi = info->par;
	unsigned int val;

	/* dprintk("setcol: regno=%d, rgb=%d,%d,%d\n",
		   regno, red, green, blue); */

	switch (info->fix.visual) {
	case FB_VISUAL_TRUECOLOR:
		/* true-colour, use pseudo-palette */

		if (regno < 16) {
			u32 *pal = info->pseudo_palette;

			val  = chan_to_field(red,   &info->var.red);
			val |= chan_to_field(green, &info->var.green);
			val |= chan_to_field(blue,  &info->var.blue);

			pal[regno] = val;
		}
		break;

	default:
		return 1;	/* unknown type */
	}

	return 0;
}


static struct fb_ops imx6ullfb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= s3c2410fb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};


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

static void OLED_DIsp_Set_Pos(int x, int y)
{
    oled_set_dc_pin(0);
 	oled_write_cmd_data(0xb0+y,OLED_CMD);
	oled_write_cmd_data((x&0x0f),OLED_CMD); 
	oled_write_cmd_data(((x&0xf0)>>4)|0x10,OLED_CMD);
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
        OLED_DIsp_Set_Pos(x,y);
		
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


static struct fb_info *myfb_info;
static unsigned int map_size;
static dma_addr_t phy_addr;
static struct task_struct *kTask_oled;

#define PAGE_BIT_SZ (128*8)

static int oled_fb_task(void *__unused)
{
    printk("%s \n", __func__);
	int i,j,k=0;
    char line;
    char *p[8];
    char data[8];
    char byte;
    char *oled_buf = kmalloc(PAGE_BIT_SZ, GFP_KERNEL);
    unsigned char *fb = myfb_info->screen_base;
    
    while(!kthread_should_stop())
    {
        //page: 0~7
        for(i=0;i<8;i++)
        {
            for(line=0;line<8;line++)
            {
                p[line] = fb + (128*i + 16*line);
            }
            
            for(j=0;j<16;j++)
            {
                for(line=0;line<8;line++)
                {
                    data[line] = *p[line];
                    p[line]++;
                }
                
                for(bit=0;bit<8;bit++)
                {
                    byte =  (((data[0]>>bit)&1) << bit) |
                            (((data[1]>>bit)&1) << bit) |
                            (((data[2]>>bit)&1) << bit) |
                            (((data[3]>>bit)&1) << bit) |
                            (((data[4]>>bit)&1) << bit) |
                            (((data[5]>>bit)&1) << bit) |
                            (((data[6]>>bit)&1) << bit) |
                            (((data[7]>>bit)&1) << bit);
                    
                    oled_buf[k++] = byte;
                }
            }
        }
        
        //spi transfer data
        for(i=0;i<8;i++)
        {
            //set to each page start pos
            OLED_DIsp_Set_Pos(0,i);
            
            oled_set_dc_pin(1);
            spi_write_datas(oled_buf, sizeof(oled_buf));
        }
        
        //wait a while
        schedule_timeout_interruptible(HZ);
    }
	
    kfree(oled_buf);
	return 0;
}


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
	
    
    //3.1 alloc fb
    myfb_info = framebuffer_alloc(0, NULL);
    if (!myfb_info)
		return -ENOMEM;
    
    //3.2 set fb param
	myfb_info->var.xres = 128;
	myfb_info->var.yres = 64;
	myfb_info->var.xres_virtual = 128;
	myfb_info->var.yres_virtual = 64;
	myfb_info->var.bits_per_pixel = 1; //black and white => 1 or 0
    
    myfb_info->fix.line_length  = myfb_info->var.xres * myfb_info->var.bits_per_pixel / 8;
    
    myfb_info->fix.smem_len     = myfb_info->var.xres * myfb_info->var.yres * myfb_info->var.bits_per_pixel / 8;
    
    /* fb virtual addr */
    map_size = PAGE_ALIGN(myfb_info->fix.smem_len);
    myfb_info->screen_base = dma_alloc_wc(NULL, map_size, &phy_addr, GFP_KERNEL);
    
    /* fb physical addr */
    myfb_info->fix.smem_start   = phy_addr;
    
    myfb_info->fix.type         = FB_TYPE_PACKED_PIXELS; //???
    myfb_info->fix.visual       = FB_VISUAL_MONO10;      //1:white, 0:black
    
	myfb_info->fbops = &imx6ullfb_ops;
    myfb_info->pseudo_palette = pseudo_palette;
    
    //3.3 register fb
    ret = register_framebuffer(myfb_info);
	if (ret < 0) {
		printk("Failed to register framebuffer device: %d\n");
	}
    
    //4. create kthread to sync oled & fb
    kTask_oled = kthread_run(oled_fb_task, NULL, "kTask_oled");
    
	return 0;
}

static int spi_oled_remove(struct spi_device *spi)
{
    unregister_framebuffer(myfb_info);
    dma_free_wc(NULL, map_size, myfb_info->screen_base, phy_addr);
    
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






