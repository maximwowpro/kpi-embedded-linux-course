
#include <linux/module.h>	// required by all modules
#include <linux/kernel.h>	// required for sysinfo
#include <linux/init.h>		// used by module_init, module_exit macros
#include <linux/jiffies.h>	// where jiffies and its helpers reside
#include <linux/kthread.h>  	// for threads
#include <linux/timer.h>
#include <linux/slab.h>		// for kmalloc()
#include <linux/list.h>		// for linked list

#define THREADS_RETVAL		(0)

MODULE_DESCRIPTION("Module works with Linux kernel threads, linked list, ");
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
LIST_HEAD(list_for_variables);
int global_var = 0;

struct list_node
{
	struct list_head next;
	int data;
};

int thread_handler (void *data)
{
	printk(KERN_INFO "Hello from thread\n");
	
	for (int i = 0; i < thread_inc_iterations; i++)
		(*((int *)data))++;
	
	struct list_node *node_ptr = kmalloc(sizeof(*node_ptr), GFP_KERNEL);
	if (NULL == node_ptr)
		printk(KERN_ERR "Can't allocate memory for list node\n");
	node_ptr->data = global_var;
	list_add(&(node_ptr->next), &list_for_variables);
	
	while (1) {
		if (kthread_should_stop())
			do_exit(THREADS_RETVAL);
		schedule();
	}
}

static int __init firstmod_init(void)
{
	printk(KERN_INFO "Hello from module");
	
	/* Allocate memory for threads handlsers */
	threads = kmalloc (num_threads * sizeof(*threads), GFP_KERNEL);
	if (NULL == threads) {
		printk(KERN_ERR "Can't allocate memory for thread's handlers"
		                 ", aborting\n");
		return -1;
	}
	
	/* Create threads */
	for (int i = 0; i < num_threads; i++) {
		threads[i] = kthread_create(thread_handler, &global_var, 
					    "test_thread");
		if ((ERR_PTR(-ENOMEM)) == threads[i]) {
			printk(KERN_ERR "Can't allocate memory for thread %i\n",
			       i);
			for (i=i; i != 0; i--)
				kfree(threads[i]);
			return -2;
		}
	}
	
	/* Run threads */
	for (int i = 0; i < num_threads; i++)
		wake_up_process(threads[i]);
	
	printk(KERN_INFO "Threads started\n");
	
	return 0;
}
 
static void __exit firstmod_exit(void)
{
	if (NULL != threads) {
		for (int i = 0; i < num_threads; i++) {
			int error_check = kthread_stop(threads[i]);
			if (THREADS_RETVAL == error_check)
				printk(KERN_INFO "Thread stopped\n");
			else
				printk(KERN_ERR "Some error occured while "
				       "trying to stop the thread\n");
		}
	}
	
	struct list_head *it;
	list_for_each(it, &list_for_variables) {
		int list_val = (container_of(it, struct list_node, next))->data;
		printk(KERN_INFO "List val is %i\n", list_val);
	}
	struct list_head *tmp;
	list_for_each_safe(it, tmp, &list_for_variables) {
		list_del(it);
	}
	
	printk(KERN_INFO "Final val is %i\n", global_var);
	
	printk(KERN_INFO "Long live the Kernel!\n");
}
 
module_init(firstmod_init);
module_exit(firstmod_exit);
