#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xcc47d0ba, "module_layout" },
	{ 0xc28cb3b4, "param_ops_int" },
	{ 0x37a0cba, "kfree" },
	{ 0xf1c3635, "device_destroy" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x3458b049, "class_destroy" },
	{ 0x1cd2a2fa, "device_create" },
	{ 0x2b738d60, "cdev_del" },
	{ 0x1e46323e, "__class_create" },
	{ 0x93de3581, "cdev_add" },
	{ 0xcf45f7c7, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xc0429d73, "kmem_cache_alloc_trace" },
	{ 0xede28f84, "kmalloc_caches" },
	{ 0x3fd78f3b, "register_chrdev_region" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0xcf2a6966, "up" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x81b395b3, "down_interruptible" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2A9ED7D57120FE961487B47");
