/*
 * 3. Dynamic driver loading
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define DRIVER_NAME "MyDevice"

/* minor number */
static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM  = 2;

/* major number (dynamic assigned) */
static unsigned int mydevice_major;

/* character device object */
static struct cdev mydevice_cdev;

/* on open */
static int myDevice_open(struct inode *inode, struct file *file) {
    printk("myDevice_open\n");
    return 0;
}

/* on close */
static int myDevice_close(struct inode *inode, struct file *file) {
    printk("myDevice_close\n");
    return 0;
}

/* on read */
static ssize_t myDevice_read(struct file *filep, char __user *buf, size_t count, loff_t *f_pos) {
    printk("myDevice_read\n");
    /* NOTE: will trigger segmentation fault, MUST use copy_to_user. see also '04' part and https://stackoverflow.com/questions/39515407/how-to-handle-a-page-domain-fault-in-a-self-written-character-device-kernel-modu */
    buf[0] = 'A';
    return 1;
}

/* on write */
static ssize_t myDevice_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos) {
    printk("myDevice_write\n");
    return 1;
}

/* handler table for system calls */
struct file_operations s_myDevice_fops = {
    .open    = myDevice_open,
    .release = myDevice_close,
    .read    = myDevice_read,
    .write   = myDevice_write,
};

/* on load (insmod) */
static int myDevice_init(void) {
    printk("myDevice_init\n");

    int alloc_ret = 0;
    int cdev_err = 0;
    dev_t dev;

    /* 1. allocate available major number */
    alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DRIVER_NAME);
    if (alloc_ret != 0) {
        printk(KERN_ERR "alloc_chrdev_region = %d\n", alloc_ret);
        return -1;
    }

    /* 2. Extract major number */
    mydevice_major = MAJOR(dev);
    dev = MKDEV(mydevice_major, MINOR_BASE); /* reconfirm (no needed?) */
  
    /* 3. init cdev struct and register systemcall handler */
    cdev_init(&mydevice_cdev, &s_myDevice_fops);
    mydevice_cdev.owner = THIS_MODULE;

    /* 4. Register this driver (cdev) to the kernel */
    cdev_err = cdev_add(&mydevice_cdev, dev, MINOR_NUM);
    if (cdev_err != 0) {
        printk(KERN_ERR "cdev_add = %d\n", cdev_err);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    return 0;
}

/* on unload (rmmod) */
static void myDevice_exit(void) {
    printk("myDevice_exit\n");

    dev_t dev = MKDEV(mydevice_major, MINOR_BASE);

    /* 5. Remove this driver (cdev) from the kernel */
    cdev_del(&mydevice_cdev);

    /* 6. Remove registered major number for this driver */
    unregister_chrdev_region(dev, MINOR_NUM);
}

module_init(myDevice_init);
module_exit(myDevice_exit);

