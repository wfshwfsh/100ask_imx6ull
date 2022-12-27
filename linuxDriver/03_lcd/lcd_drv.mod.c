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
	{ 0x9a454969, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x8e53af03, __VMLINUX_SYMBOL_STR(cfb_imageblit) },
	{ 0x84ba22c1, __VMLINUX_SYMBOL_STR(cfb_copyarea) },
	{ 0xfc43a3ff, __VMLINUX_SYMBOL_STR(cfb_fillrect) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0x19800b3e, __VMLINUX_SYMBOL_STR(framebuffer_release) },
	{ 0xf1b523a5, __VMLINUX_SYMBOL_STR(unregister_framebuffer) },
	{ 0x42ecf546, __VMLINUX_SYMBOL_STR(ioremap) },
	{ 0xd1c954a3, __VMLINUX_SYMBOL_STR(register_framebuffer) },
	{ 0xf83e96c2, __VMLINUX_SYMBOL_STR(arm_dma_ops) },
	{ 0x3ac8ded9, __VMLINUX_SYMBOL_STR(dma_alloc_from_coherent) },
	{ 0x16d5162e, __VMLINUX_SYMBOL_STR(framebuffer_alloc) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "E078000FE7301F0E5A45136");
