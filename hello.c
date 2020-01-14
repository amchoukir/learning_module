#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");
//MODULE_INFO(intree, "Y"); This was not needed the tainting
// was a result of the lack of signature in the module
// Disabled the kernel signature verification

static int __init
hello_init(void)
{
    printk(KERN_ALERT " Hello, World!\n");
    return 0;
}

static void __exit
hello_exit(void)
{
    printk(KERN_ALERT " Goodbye, cruel world!\n");
}

module_init(hello_init);
module_exit(hello_exit);
