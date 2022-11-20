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
	{ 0xd96e06c6, "module_layout" },
	{ 0x102c7fe7, "wake_up_process" },
	{ 0x10c0017e, "kthread_create_on_node" },
	{ 0x5eb7662b, "kernel_clone" },
	{ 0x606513a5, "current_task" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x14712b51, "put_pid" },
	{ 0xf37409c9, "do_wait" },
	{ 0xa7104b95, "find_get_pid" },
	{ 0x952664c5, "do_exit" },
	{ 0x2d15d1b6, "do_execve" },
	{ 0x85416d23, "getname_kernel" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "B49997BF12071C169B8AA99");
