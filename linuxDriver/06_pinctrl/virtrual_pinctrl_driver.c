#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/slab.h>
#include <linux/regmap.h>

#include "../core.h"
#include "pinctrl-imx.h"

static struct imx_pinctrl_soc_info *virtual_pinctrl_info;
static struct pinctrl_desc *pinctrl;
static struct pinctrl_dev *g_pinctrl_dev;

static const struct pinctrl_pin_desc pins[] = {
	{0, "pin0", NULL},
	{1, "pin1", NULL},
	{2, "pin2", NULL},
	{3, "pin3", NULL},
};

static unsigned long g_configs[4];


struct virtual_functions_desc {
	const char *func_name;
	const char **groups;
	int num_groups;
};

static struct virtual_functions_desc g_func_des[]={

	{"gpio", {"pin0", "pin1", "pin2", "pin3"}, 4},
	{"i2c",  {"pin0", "pin1"}, 2},
	{"uart", {"pin2", "pin3"}, 2},
};



static int virtual_get_groups_count(struct pinctrl_dev *pctldev)
{
	return pctldev->desc.npins;
}

static const char *virtual_get_group_name(struct pinctrl_dev *pctldev,
				       unsigned selector)
{
	return pctldev->desc.pins[selector].name;
}

static int virtual_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector,
				  const unsigned **pins,
				  unsigned *npins)
{
	if (selector >= pctldev->desc.npins)
	   return -EINVAL;

	*pins = pctldev->desc.pins[selector].number;
	*npins = 1;

	return 0;
}

static void  virtual_pin_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s,
		 unsigned offset)
{
	seq_printf(s, "%s", dev_name(pctldev->dev));
}

static int virtual_dt_node_to_map(struct pinctrl_dev *pctldev,
		 struct device_node *np,
		 struct pinctrl_map **map, unsigned *num_maps)
{
	
	return 0;
}

static void virtual_dt_free_map(struct pinctrl_dev *pctldev,
			 struct pinctrl_map *map, unsigned num_maps)
{
	kfree(map);
}


static const struct pinctrl_ops virtual_pctrl_ops = {
	.get_groups_count = virtual_get_groups_count,
	.get_group_name = virtual_get_group_name,
	.get_group_pins = virtual_get_group_pins,
	.pin_dbg_show = virtual_pin_dbg_show,
	.dt_node_to_map = virtual_dt_node_to_map,
	.dt_free_map = virtual_dt_free_map,

};



/*
 i2cgrp {
		 functions = "i2c", "i2c";
		 groups = "pin0", "pin1";
		 configs = <0x11223344	0x55667788>;
 };

 one pin ==> two pinctrl_map (one for mux, one for config)

 */


static int virtual_pmx_get_funcs_count(struct pinctrl_dev *pctldev)
{
	return ARRAYSIZE(g_func_des);
}

static const char *virtual_pmx_get_func_name(struct pinctrl_dev *pctldev,
					  unsigned selector)
{
	return g_func_des[selector].func_name;
}

static int virtual_pmx_get_groups(struct pinctrl_dev *pctldev, unsigned selector,
				 const char * const **groups,
				 unsigned * const num_groups)
{

	*groups = g_func_des[selector].groups;
	*num_groups = g_func_des[selector].num_groups;

	return 0;
}

static int virtual_pmx_set(struct pinctrl_dev *pctldev, unsigned selector,
			unsigned group)
{
	/*
	* Configure the mux mode for each pin in the group for a specific
	* function.
	*/
	printk("set %s as %s\n", pctldev->desc->pins[group].name, g_funcs_des[selector].func_name);

	return 0;
}


static const struct pinmux_ops virtual_pmx_ops = {
	.get_functions_count = virtual_pmx_get_funcs_count,
	.get_function_name = virtual_pmx_get_func_name,
	.get_function_groups = virtual_pmx_get_groups,
	.set_mux = virtual_pmx_set,
	//.gpio_set_direction = imx_pmx_gpio_set_direction,
};




static int virtual_pinconf_get(struct pinctrl_dev *pctldev,
			     unsigned pin_id, unsigned long *config)
{
	*config = g_configs[pin_id];
	return 0;
}

static int virtual_pinconf_set(struct pinctrl_dev *pctldev,
			     unsigned pin_id, unsigned long *configs,
			     unsigned num_configs)
{
	if(	num_configs != 1)
		return -EINVAL;

	g_configs[pin_id] = *config;
	printk("config %s as 0x%lx\n", pctldev->desc->pins[pin_id].name, *configs);
}

static void virtual_pinconf_dbg_show(struct pinctrl_dev *pctldev,
			struct seq_file *s, unsigned pin_id)
{
	seq_printf("0x%lx", g_configs[pin_id].config);
}

static void imx_pinconf_group_dbg_show(struct pinctrl_dev *pctldev,
					 struct seq_file *s, unsigned group)
{
	seq_printf("0x%lx", g_configs[pin_id].config);
}


static const struct pinconf_ops imx_pinconf_ops = {
	.pin_config_get = virtual_pinconf_get,
	.pin_config_set = virtual_pinconf_set,
	.pin_config_dbg_show = virtual_pinconf_dbg_show,
	.pin_config_group_dbg_show = imx_pinconf_group_dbg_show,
};


static int virtual_pinctrl_probe(struct platform_device *pdev)
{
	/* 1. 分配pinctrl_desc */
	pinctrl = devm_kzalloc(&pdev->dev, sizeof(*pinctrl), GFP_KERNEL);
	
	/* 2. 設置pinctrl_desc */
	pinctrl->name = dev_name(&pdev->dev);
	pinctrl->owner = THIS_MODULE;

	/* 2.1 set pins and groups */
	pinctrl->pins = pins;
	pinctrl->npins = ARRAYSIZE(pins);
	pinctrl->pctlops = &virtual_pctrl_ops;
	
	/* 2.2 pin_mux */
	pinctrl->pmxops = &virtual_pmx_ops;
	
	/* 2.3 pin_config */
	pinctrl->confops = &imx_pinconf_ops;
	
	/* 3. 註冊pinctrl_desc */
	g_pinctrl_dev = devm_pinctrl_register(pdev->dev, pinctrl, NULL);


	return 0;
}


static int virtual_pinctrl_remove(struct platform_device *pdev)
{
	//devm_pinctrl_unregister(pdev->dev, g_pinctrl_dev);
	printk("%s\n", __FUNCTION__);
	return 0;
}

static const struct of_device_id virtual_pinctrl_of_match[] = {
	{ .compatible = "100ask,virtual_pinctrl", },
	{ },
};

static struct platform_driver virtual_pinctrl_driver = {
	.driver = {
		.name = "virtual_pinctrl",
		.of_match_table = of_match_ptr(virtual_pinctrl_of_match),
	},

	.probe  = virtual_pinctrl_probe,
	.remove = virtual_pinctrl_remove,
};

static int __init virtual_pinctrl_init(void)
{
	printk("virtual_pinctrl_init \n");
    int ret;
	ret = platform_driver_register(&virtual_pinctrl_driver);
	if (ret)
		return ret;

	return 0;
}

/* 2. 出口 */
static void __exit virtual_pinctrl_exit(void)
{
	platform_driver_unregister(&virtual_pinctrl_driver);
}




module_init(virtual_pinctrl_init);
module_exit(virtual_pinctrl_exit);

MODULE_AUTHOR("will.Chen <wfshwfsh@gmail.com> ");
MODULE_DESCRIPTION("Framebuffer driver for the imx6ull");
MODULE_LICENSE("GPL");



