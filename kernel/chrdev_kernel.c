#include <linux/init.h> 
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#define MAX_BUF_SIZE 256

struct cdev_data {
    struct cdev cdev;
};

static int cdev_major = 0;
static struct class *mycdev_class = NULL;
static struct cdev_data mycdev_data;
static unsigned char *user_data;

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
    size_t udatalen;

    printk(KERN_DEBUG "Entering: %s\n", __func__);
    udatalen = strlen(user_data);
    printk(KERN_DEBUG "user data len: %zu\n", udatalen);
    if (count > udatalen)
        count = udatalen;

    if (copy_to_user(buf, user_data, count) != 0) {
        printk(KERN_ERR "Copy data to user failed\n");
        return -EFAULT;
    }

    return count;
}

static ssize_t cdev_write (struct file *file, const char __user *buf, size_t count, loff_t *offset) {
    size_t udatalen = MAX_BUF_SIZE;
    size_t nbr_chars = 0;

    printk(KERN_DEBUG "Entering: %s\n", __func__);
    if (count < udatalen)
        udatalen = count;

    nbr_chars = copy_from_user(user_data, buf, udatalen);
    if (nbr_chars == 0) {
        printk(KERN_DEBUG "Copied %zu bytes from the user\n", udatalen);
        printk (KERN_DEBUG "Receive data from user: %s", user_data);
    } else {
        printk(KERN_ERR "Copy data from user failed\n");
        return -EFAULT;
    }

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

    user_data = (unsigned char*) kzalloc(MAX_BUF_SIZE, GFP_KERNEL);
    if (user_data == NULL) {
        printk (KERN_ERR "Allocation memory for data buffer failed\n");
        device_destroy(mycdev_class, MKDEV(cdev_major, 0));
        class_unregister(mycdev_class);
        class_destroy(mycdev_class);
        unregister_chrdev_region(MKDEV(cdev_major, 0), 1);
        return -ENOMEM;
    }

    return 0;
}

void cleanup_module ( void ) {
    printk(KERN_DEBUG "Entering: %s\n", __func__);
    device_destroy(mycdev_class, MKDEV(cdev_major, 0));
    class_unregister(mycdev_class);
    class_destroy(mycdev_class);
    unregister_chrdev_region(MKDEV(cdev_major, 0), 1);
    if (user_data != NULL)
        kfree(user_data);
}

MODULE_DESCRIPTION("A simple Linux char driver");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Ilies CHERGUI <ilies.chergui@gmail.com>");
MODULE_LICENSE("GPL");

