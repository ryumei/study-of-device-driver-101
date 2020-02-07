/*
 *
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

#define DRIVER_NAME "MyDevice_NAME"
#define DRIVER_MAJOR 63

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
    /* register this driver */
    register_chrdev(DRIVER_MAJOR, DRIVER_NAME, &s_myDevice_fops);
    return 0;
}

/* on unload (rmmod) */
static void myDevice_exit(void) {
    printk("myDevice_exit\n");
    unregister_chrdev(DRIVER_MAJOR, DRIVER_NAME);
}

module_init(myDevice_init);
module_exit(myDevice_exit);

