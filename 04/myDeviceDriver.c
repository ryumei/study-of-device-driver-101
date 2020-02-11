/*
 * 4. Dynamic driver loading
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/current.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define DRIVER_NAME "MyDevice"

#define NUM_BUFFER 256
/* minor number */
static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM  = 2;
/* major number (dynamic assigned) */
static unsigned int mydevice_major;
/* character device object */
static struct cdev mydevice_cdev;
static struct class *mydevice_class = NULL;
struct _mydevice_file_data {
    unsigned char buffer[NUM_BUFFER];
};

/* on open */
static int mydevice_open(struct inode *inode, struct file *file) {
    printk("mydevice_open\n");
    
    /* allocate */
    struct _mydevice_file_data *p = kmalloc(sizeof(struct _mydevice_file_data), GFP_KERNEL);
    if (p == NULL) {
        printk(KERN_ERR "kmalloc\n");
        return -ENOMEM;
    }
    /* initialize file intrinsic data */
    strlcat(p->buffer, "dummy", 5);

    /* keep pointers by fd of user side */
    file->private_data = p;

    return 0;
}

/* on close */
static int mydevice_close(struct inode *inode, struct file *file) {
    printk("mydevice_close\n");

    if (file->private_data) {
        kfree(file->private_data);
        file->private_data = NULL;
    }

    return 0;
}

/* on read */
static ssize_t mydevice_read(struct file *filep, char __user *buf, size_t count, loff_t *f_pos) {
    printk("mydevice_read\n");
    if (count > NUM_BUFFER) count = NUM_BUFFER;

    struct _mydevice_file_data *p = filep->private_data;
    if (raw_copy_to_user(buf, p->buffer, count) != 0) {
        return -EFAULT;
    }
    return count;
}

/* on write */
static ssize_t mydevice_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos) {
    printk("mydevice_write\n");

    struct _mydevice_file_data *p = filep->private_data;
    if (raw_copy_from_user(p->buffer, buf, count) != 0) {
        return -EFAULT;
    }
    /*printk("%s\n", stored_value);*/
    return count;
}

/* handler table for system calls */
struct file_operations s_mydevice_fops = {
    .open    = mydevice_open,
    .release = mydevice_close,
    .read    = mydevice_read,
    .write   = mydevice_write,
};

/* on load (insmod) */
static int mydevice_init(void) {
    printk("mydevice_init\n");

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
    cdev_init(&mydevice_cdev, &s_mydevice_fops);
    mydevice_cdev.owner = THIS_MODULE;

    /* 4. Register this driver (cdev) to the kernel */
    cdev_err = cdev_add(&mydevice_cdev, dev, MINOR_NUM);
    if (cdev_err != 0) {
        printk(KERN_ERR "cdev_add = %d\n", cdev_err);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    /* 5. register class for this device */
    mydevice_class = class_create(THIS_MODULE, "mydevice");
    if (IS_ERR(mydevice_class)) {
        printk(KERN_ERR "class_create\n");
        cdev_del(&mydevice_cdev);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    /* 6. Create /sys/class/mydevice/mydevice* */
    for (int minor = MINOR_BASE; minor < MINOR_BASE + MINOR_NUM; minor++) {
        device_create(mydevice_class, NULL, MKDEV(mydevice_major, minor), NULL, "mydevice%d", minor);
    }

    return 0;
}

/* on unload (rmmod) */
static void mydevice_exit(void) {
    printk("mydevice_exit\n");

    dev_t dev = MKDEV(mydevice_major, MINOR_BASE);

    /* 7. Delete /sys/class/mydevice/mydevice* */
    for (int minor = MINOR_BASE; minor < MINOR_BASE + MINOR_NUM; minor++) {
        device_destroy(mydevice_class, MKDEV(mydevice_major, minor));
    }

    /* 8. Remove class registration for this device */
    class_destroy(mydevice_class);

    /* 9. Remove this driver (cdev) from the kernel */
    cdev_del(&mydevice_cdev);

    /* 10. Remove registered major number for this driver */
    unregister_chrdev_region(dev, MINOR_NUM);
}

module_init(mydevice_init);
module_exit(mydevice_exit);

