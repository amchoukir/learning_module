#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("Dual BSD/GPL");

static char *whom = "world";
static int howmany = 1;
module_param(howmany, int, 0444);
module_param(whom, charp, 0444);

static int __init
hello_init(void)
{
	int i;
	for(i = 0; i < howmany; ++i)
    	printk(KERN_ALERT " Hello, %s!\n", whom);
    return 0;
}

static void __exit
hello_exit(void)
{
    printk(KERN_ALERT " Goodbye, cruel world!\n");
}

module_init(hello_init);
module_exit(hello_exit);
