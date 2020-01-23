#include <linux/cdev.h> 		/* For struct cdev, cdev_init, cdev_add */
#include <linux/fs.h>			/* For alloc_chrdev_region, unregister_chrdev_region */
#include <linux/init.h> 		/* For __init, __exit*/
#include <linux/kdev_t.h>		/* For MKDEV, MAJOR */
#include <linux/module.h> 		/* For MODULE_LICENSE, module_init, module_exit */
#include <linux/moduleparam.h>	/* For module_param */
#include <linux/types.h>		/* For dev_t */

#include "scull.h"

MODULE_LICENSE("Dual BSD/GPL");

static dev_t dev;
static int scull_major = SCULL_MAJOR;
static int scull_minor = SCULL_MINOR;
module_param(scull_major, int, 0444);
module_param(scull_minor, int, 0444);
static int scull_nr_devs = SCULL_DEV_COUNT;
static int scull_quantum = SCULL_QUANTUM;
static int scull_qset = SCULL_QSET;
module_param(scull_quantum, int, 0444);
module_param(scull_qset, int, 0444);

struct scull_qset
{
    void **data;
    struct scull_qset *next;
};

struct scull_dev
{
	struct scull_qset *data;
	int quantum;
	int qset;
	unsigned long size;
	unsigned int access_key;
	struct semaphore sem;
	struct cdev cdev;
} scull_device;

static int
scull_trim(struct scull_dev *dev)
{
    struct scull_qset *next, *dptr;
    int qset = dev->qset; /* "dev" is not NULL */
    int i;

    for (dptr = dev->data; dptr; dptr = next) { /* All the list items */
        if (dptr->data) {
            for (i = 0; i < qset; ++i) {
                kfree(dptr->data[i];
            }
            kfree(dptr->data);
            dptr->data = NULL;
        }
        next = dptr->next;
        kfree(dptr);
    }
    dev->size = 0;
    dev->quantum = scull_quantum;
    dev->qset = scull_qset;
    dev->data = NULL;
    return 0;
}

static int
scull_open(struct inode * inode, struct file * filp)
{
	struct scull_dev *dev;

	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev;

	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY ) {
		scull_trim(dev);
	}

	return 0;
}

static int
scull_release(struct inode *inode, struct file *filp)
{
    return 0;
}

struct file_operations scull_fops = {
	.owner 		= 	THIS_MODULE,
	.llseek 	=	scull_llseek,
	.read		= 	scull_read,
	.write		=	scull_write,
	.ioctl		= 	scull_ioctl,
	.open		=	scull_open,
	.release	= 	scull_release,
};

static void
scull_setup_cdev(struct scull_dev *dev, int index)
{
	int err, devno = MKDEV(scull_major, scull_minor + index);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	err = cdev_add(&dev->cdev, 1);
	if ( err < 0 ) {
		printk(KERN_NOTICE "Error %d adding scull%d\n", err, index);
	}
}

static int __init
scull_init(void)
{
	int result = 0;

	if ( scull_major ) {
		dev  = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, SCULL_DEV_NAME);
	} else {
		result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, SCULL_DEV_NAME);
		scull_major = MAJOR(dev);
	}

	if ( result < 0 ) {
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
	}
	
	scull_setup_cdev(&scull_device, 0);

	return result;
}

static void __exit
scull_exit(void)
{
	unregister_chrdev_region(dev, SCULL_DEV_COUNT);
}

module_init(scull_init);
module_exit(scull_exit);


