#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO

int init_module(void)
{
    pr_info("Hello World!\n");
    return 0;    // Non-zero return means that the module couldn't be loaded.
}

void cleanup_module(void)
{
    pr_info("Goodbye World!\n"); 
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ikadzev");
MODULE_DESCRIPTION("Simple Hello World");