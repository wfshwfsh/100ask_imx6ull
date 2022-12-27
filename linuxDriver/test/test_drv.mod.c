#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x3cb79823, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6c1ed2c6, __VMLINUX_SYMBOL_STR(cfb_imageblit) },
	{ 0x5f9d1175, __VMLINUX_SYMBOL_STR(cfb_copyarea) },
	{ 0x94f46b0f, __VMLINUX_SYMBOL_STR(cfb_fillrect) },
	{ 0xd6c4624b, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0xd49c9dee, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0xb077e70a, __VMLINUX_SYMBOL_STR(clk_unprepare) },
	{ 0x815588a6, __VMLINUX_SYMBOL_STR(clk_enable) },
	{ 0x6aa6ada8, __VMLINUX_SYMBOL_STR(gpiod_set_value) },
	{ 0x40354e1c, __VMLINUX_SYMBOL_STR(devm_ioremap_resource) },
	{ 0x2d9b5b90, __VMLINUX_SYMBOL_STR(platform_get_resource) },
	{ 0x5ca27c5, __VMLINUX_SYMBOL_STR(register_framebuffer) },
	{ 0x61a5bcd0, __VMLINUX_SYMBOL_STR(arm_dma_ops) },
	{ 0x7060bb1c, __VMLINUX_SYMBOL_STR(dma_alloc_from_coherent) },
	{ 0xdead2cd7, __VMLINUX_SYMBOL_STR(framebuffer_alloc) },
	{ 0x7c9a7371, __VMLINUX_SYMBOL_STR(clk_prepare) },
	{ 0x76d9b876, __VMLINUX_SYMBOL_STR(clk_set_rate) },
	{ 0x9dffe2c5, __VMLINUX_SYMBOL_STR(devm_clk_get) },
	{ 0xa19fd89, __VMLINUX_SYMBOL_STR(gpiod_direction_output) },
	{ 0x64e7ef20, __VMLINUX_SYMBOL_STR(devm_gpiod_get) },
	{ 0xcc7a7ea3, __VMLINUX_SYMBOL_STR(of_get_display_timings) },
	{ 0x93a764b5, __VMLINUX_SYMBOL_STR(of_property_read_variable_u32_array) },
	{ 0xaa2459a3, __VMLINUX_SYMBOL_STR(of_parse_phandle) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x2b9692ce, __VMLINUX_SYMBOL_STR(framebuffer_release) },
	{ 0x26993e97, __VMLINUX_SYMBOL_STR(unregister_framebuffer) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*C100ask,lcd_drv");
MODULE_ALIAS("of:N*T*C100ask,lcd_drvC*");
