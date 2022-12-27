//Reference: s3c2410fb.c

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/io.h>

#include <asm/div64.h>

#include <asm/mach/map.h>

struct lcd_regs {
	volatile unsigned int fb_base_phys;
	volatile unsigned int fb_xres;
	volatile unsigned int fb_yres;
	volatile unsigned int fb_bpp;	
};

static struct lcd_regs *mylcd_regs;

static struct fb_info *myfb_info;

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






static int mylcd_probe(struct platform_device *pdev)
{
	int ret;
    dma_addr_t phy_addr;
	printk("imx6ull_fb_init \n");
    
	/* 1. 分配fb_info - no need private data space*/
    myfb_info = framebuffer_alloc(0, NULL);
    if (!myfb_info)
		return -ENOMEM;
    
    /* 2. 設置fb_info */
    /* var */

	myfb_info->var.xres = 500;
	myfb_info->var.yres = 300;
	myfb_info->var.xres_virtual = 500;
	myfb_info->var.yres_virtual = 300;
	myfb_info->var.bits_per_pixel = 16; //RGB565

    /* RGB565 */
	myfb_info->var.red.length = 5;
	myfb_info->var.red.offset = 11; //??? 0;

	myfb_info->var.green.length = 6;
	myfb_info->var.green.offset = 5;
    
	myfb_info->var.blue.length = 5;
	myfb_info->var.blue.offset = 0; //??? 11;
    
    /* fix */	
    myfb_info->fix.line_length  = myfb_info->var.xres * myfb_info->var.bits_per_pixel / 8;
    
    myfb_info->fix.smem_len     = myfb_info->var.xres * myfb_info->var.yres * myfb_info->var.bits_per_pixel / 8;
    if(myfb_info->var.bits_per_pixel == 24)
        myfb_info->fix.smem_len = myfb_info->var.xres * myfb_info->var.yres * 4;//actual 32bit => 32/8=4
    
    /* fb的虚拟地址 */
    unsigned int map_size = PAGE_ALIGN(myfb_info->fix.smem_len);
    myfb_info->screen_base = dma_alloc_wc(NULL, map_size, &phy_addr, GFP_KERNEL);
    
    /* fb的物理地址 */
    myfb_info->fix.smem_start   = phy_addr;
    
    myfb_info->fix.type         = FB_TYPE_PACKED_PIXELS;
    myfb_info->fix.visual       = FB_VISUAL_TRUECOLOR;
    
    
	// ??? myfb_info->fix.mmio_start   = 
    // ??? myfb_info->fix.mmio_len     = 
    
    // ??? myfb_info->fix.capabilities = 
    
    
    /* fbops */
	myfb_info->fbops = &imx6ullfb_ops;
    myfb_info->pseudo_palette = pseudo_palette;
    
    /* 3. 註冊fb_info */
    ret = register_framebuffer(myfb_info);
	if (ret < 0) {
		printk("Failed to register framebuffer device: %d\n");
	}
    
    /* 硬件操作 */
	mylcd_regs = ioremap(0x021C8000, sizeof(struct lcd_regs));
	mylcd_regs->fb_base_phys = phy_addr;
	mylcd_regs->fb_xres = 500;
	mylcd_regs->fb_yres = 300;
	mylcd_regs->fb_bpp  = 16;
    
	return 0;
}

static int mylcd_remove(struct platform_device *pdev)
{
	/* unregister */
	unregister_framebuffer(myfb_info);
	
    /* release fb_info */
    framebuffer_release(myfb_info);
    
    iounmap(mylcd_regs);
	
	return 0;
}

static const struct of_device_id mylcd_of_match[] = {
	{ .compatible = "100ask,lcd_drv", },
	{ },
};
MODULE_DEVICE_TABLE(of, mylcd_of_match);

static struct platform_driver mylcd_driver = {
	.driver		= {
		.name	= "mylcd",
		.of_match_table = mylcd_of_match,
	},
	.probe		= mylcd_probe,
	.remove		= mylcd_remove,
	.id_table	= mylcd_driver_ids, //???
};

static int __init mylcd_init(void)
{
	return platform_driver_register(&mylcd_driver);
}

static void __exit mylcd_exit(void)
{
	platform_driver_unregister(&mylcd_driver);
}

module_init(mylcd_init);
module_exit(mylcd_exit);

MODULE_AUTHOR(" will.Chen <wfshwfsh@gmail.com> ");
MODULE_DESCRIPTION("Framebuffer driver for the imx6ull");
MODULE_LICENSE("GPL");



