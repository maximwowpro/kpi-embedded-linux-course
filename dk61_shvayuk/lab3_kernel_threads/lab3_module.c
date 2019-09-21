
#include <linux/module.h>	// required by all modules
#include <linux/kernel.h>	// required for sysinfo
#include <linux/init.h>		// used by module_init, module_exit macros
#include <linux/jiffies.h>	// where jiffies and its helpers reside
#include <linux/kthread.h>  	// for threads
#include <linux/timer.h>
#include <linux/slab.h>		// for kmalloc()

MODULE_DESCRIPTION("Module works with Linux kernel threads");
MODULE_AUTHOR("max_shvayuk");
MODULE_VERSION("0.228");
MODULE_LICENSE("Dual MIT/GPL");		// this affects the kernel behavior

int num_threads = 0;
int thread_inc_iterations = 0;
module_param(num_threads, int, 0);
module_param(thread_inc_iterations, int, 0);

MODULE_PARM_DESC(num_threads, "Number of kernel threads");
MODULE_PARM_DESC(thread_inc_iterations, "The number of a global variable "
                 "iterations, each thread has to perform");

struct task_struct **threads;
int global_var = 0;

int thread_handler (void *data)
{
	printk(KERN_INFO "Hello from thread\n");
	int i = 0;
	for (i = 0; i < thread_inc_iterations; i++)
		(*((int *)data))++;
	
	printk(KERN_INFO "val is %i\n", (*((int *)data)));
	
	return 0;
}

static int __init firstmod_init(void)
{
	printk(KERN_INFO "Hello from module");
	
	/* Allocate memory for threads handlsers */
	threads = kmalloc (num_threads * sizeof(struct task_struct *), GFP_KERNEL);
	if (NULL == threads) {
		printk(KERN_ERR, "Can't allocate memory for thread's handlers"
		                 ", aborting\n");
		return -1;
	}
	
	/* Create threads */
	int i = 0;
	for (i = 0; i < num_threads; i++) {
		threads[i] = kthread_create(thread_handler, &global_var, 
					    "test_thread");
		if ((ERR_PTR(-ENOMEM)) == threads[i]) {
			printk(KERN_ERR, "Can't allocate memory for thread %i\n",
			       i);
			for (i; i != 0; i--)
				kfree(threads[i]);
			return -2;
		}
	}
	
	/* Run threads */
	for (i = 0; i < num_threads; i++)
		wake_up_process(threads[i]);
	
	printk(KERN_INFO "Threads started\n");
	
	return 0;
}
 
static void __exit firstmod_exit(void)
{
	if (NULL != threads) {
		int i = 0;
		for (i = 0; i < num_threads; i++) {
			kthread_stop(threads[i]);
			printk(KERN_INFO "Thread stopped\n");
		}
	}
	
	printk(KERN_INFO "Long live the Kernel!\n");
}
 
module_init(firstmod_init);
module_exit(firstmod_exit);
