#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
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
__used __section("__versions") = {
	{ 0xc1c10de8, "module_layout" },
	{ 0x4b6f8230, "wake_up_process" },
	{ 0x8cd3105c, "kthread_create_on_node" },
	{ 0x2a267427, "kernel_clone" },
	{ 0xa171de29, "current_task" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0xa5b1083, "put_pid" },
	{ 0xf37409c9, "do_wait" },
	{ 0xec2a0485, "find_get_pid" },
	{ 0x2d15d1b6, "do_execve" },
	{ 0xc5850110, "printk" },
	{ 0x85416d23, "getname_kernel" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "B519F09260994913A36A143");
