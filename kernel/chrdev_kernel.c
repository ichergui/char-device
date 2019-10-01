#include <linux/init.h> 
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>

struct cdev_data {
    struct cdev cdev;
};

static int cdev_major = 0;
static struct class *mycdev_class = NULL;
static struct cdev_data mycdev_data;

static int cdev_open (struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "Entering: %s\n", __func__);
    return 0;
}

static int cdev_release (struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "Entering: %s\n", __func__);
    return 0;
}

static long cdev_ioctl (struct file *file, unsigned int cmd, unsigned long arg) {
    printk(KERN_DEBUG "Entering: %s\n", __func__);
    return 0;
}

static ssize_t cdev_read (struct file *file, char __user *buf, size_t count, loff_t *offset) {
    printk(KERN_DEBUG "Entering: %s\n", __func__);
    return 0;
}

static ssize_t cdev_write (struct file *file, const char __user *buf, size_t count, loff_t *offset) {
    printk(KERN_DEBUG "Entering: %s\n", __func__);
    return count;
}

static const struct file_operations cdev_fops = {
    .owner    = THIS_MODULE,
    .open     = cdev_open,
    .release  = cdev_release,
    .unlocked_ioctl = cdev_ioctl,
    .read    = cdev_read,
    .write   = cdev_write
};

int init_module ( void ) {
    int err;
    struct device *dev_ret;
    dev_t dev;

    printk(KERN_DEBUG "Entering: %s\n", __func__);

    err = alloc_chrdev_region(&dev, 0, 1, "mycdev");
    if ( err < 0 ) {
        printk(KERN_ERR "Allocate a range of char device numbers failed.\n");
        return err;
    }

    cdev_major = MAJOR(dev);
    printk(KERN_DEBUG "device major number is: %d\n", cdev_major);
    if (IS_ERR(mycdev_class = class_create(THIS_MODULE, "mycdev"))) {
        unregister_chrdev_region(MKDEV(cdev_major, 0), 1);
        return PTR_ERR(mycdev_class);
    }
    
    if (IS_ERR(dev_ret = device_create(mycdev_class, NULL, MKDEV(cdev_major, 0), NULL, "mycdev-0"))) {
        class_destroy(mycdev_class);
        unregister_chrdev_region(MKDEV(cdev_major, 0), 1);
        return PTR_ERR(dev_ret);
    }
    
    cdev_init(&mycdev_data.cdev, &cdev_fops);
    mycdev_data.cdev.owner = THIS_MODULE;
 
    err = cdev_add(&mycdev_data.cdev, MKDEV(cdev_major, 0), 1);
    if ( err < 0 ) {
        printk (KERN_ERR "Unable to add a char device\n");
        device_destroy(mycdev_class, MKDEV(cdev_major, 0));
        class_unregister(mycdev_class);
        class_destroy(mycdev_class);
        unregister_chrdev_region(MKDEV(cdev_major, 0), 1);        
        return err;
    }

    return 0;
}

void cleanup_module ( void ) {
    printk(KERN_DEBUG "Entering: %s\n", __func__);
    device_destroy(mycdev_class, MKDEV(cdev_major, 0));
    class_unregister(mycdev_class);
    class_destroy(mycdev_class);
    unregister_chrdev_region(MKDEV(cdev_major, 0), 1);
}

MODULE_DESCRIPTION("A simple Linux char driver");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Ilies CHERGUI <ilies.chergui@gmail.com>");
MODULE_LICENSE("GPL");

