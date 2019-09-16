/**
 * Based on the thodnev's code example:
 * https://github.com/kpi-keoa/kpi-embedded-linux-course/blob/master/demo/lab1/firstmod.c
 */
#include <linux/module.h>	// required by all modules
#include <linux/kernel.h>	// required for sysinfo
#include <linux/init.h>		// used by module_init, module_exit macros
#include <linux/jiffies.h>	// where jiffies and its helpers reside
#include <linux/ktime.h>
#include <linux/timekeeping.h>

MODULE_DESCRIPTION("Basic module demo (extended): init, deinit, printk, jiffies, current time");
MODULE_AUTHOR("max_shvayuk");
MODULE_VERSION("0.228");
MODULE_LICENSE("Dual MIT/GPL");		// this affects the kernel behavior

static int __init firstmod_init(void)
{
	struct timespec human_readable_time = {0, 0};
	getnstimeofday(&human_readable_time);

	printk(KERN_INFO "Hello, $username!\njiffies = %lu\nseconds = %llu\n", jiffies, (unsigned long long)human_readable_time.tv_sec);
	return 0;
}
 
static void __exit firstmod_exit(void)
{
	printk(KERN_INFO "Long live the Kernel!\n");
}
 
module_init(firstmod_init);
module_exit(firstmod_exit);
