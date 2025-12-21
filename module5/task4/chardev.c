#include <linux/cdev.h> 
#include <linux/delay.h> 
#include <linux/device.h> 
#include <linux/fs.h> 
#include <linux/init.h> 
#include <linux/irq.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/poll.h> 

#define SUCCESS 0 
#define DEVICE_NAME "chardev"
#define BUF_LEN 1024

static int major;
static short readPos = 0;
enum { 
    CDEV_NOT_USED = 0, 
    CDEV_EXCLUSIVE_OPEN = 1, 
}; 

static char msg[BUF_LEN] = {0};
static size_t buffer_size = 0;
static struct class *cls = NULL;

static ssize_t device_write(struct file *filp, const char __user *buffer, size_t length, loff_t *off){ 
    size_t count = min(length, BUF_LEN);
    memset(msg, 0, BUF_LEN);
    if (copy_from_user(msg, buffer, count)) return -EFAULT;
    buffer_size = count;

    printk(KERN_INFO "chardev: recived %zu bytes\n", count);
    // printk(KERN_DEBUG "chardev: - buffer_size: %zu\n", buffer_size);
    // printk(KERN_DEBUG "chardev: - offset: %lld\n", *off);
    // printk(KERN_DEBUG "chardev: - len: %zu\n", length);
    // printk(KERN_DEBUG "chardev: - count: %zu\n", count);
    return count;
} 

static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *off) {
    if (*off >= buffer_size) {
        printk(KERN_INFO "chardev: end of file reached\n"); 
        return 0;
    }
    size_t count = min(length, buffer_size - *off);
    if (copy_to_user(buffer, msg + *off, count)) return -EFAULT;
    *off += count;

    printk(KERN_INFO "chardev: sent %zu bytes\n", count);
    // printk(KERN_DEBUG "chardev: - buffer_size: %zu\n", buffer_size);
    // printk(KERN_DEBUG "chardev: - offset: %lld\n", *off);
    // printk(KERN_DEBUG "chardev: - len: %zu\n", length);
    // printk(KERN_DEBUG "chardev: - count: %zu\n", count);
    return count;
} 

static int device_open(struct inode *inode, struct file *file){ 
    printk(KERN_INFO "chardev: device opened\n");
    return SUCCESS; 
} 

static int device_release(struct inode *inode, struct file *file){ 
    printk(KERN_INFO "chardev: device closed\n");
    return SUCCESS; 
} 

static struct file_operations chardev_fops = { 
    .read = device_read, 
    .write = device_write, 
    .open = device_open, 
    .release = device_release, 
}; 

static int __init chardev_init(void) { 
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops); 
    if (major < 0) { 
        printk(KERN_ALERT "chardev: registering char device failed with %d\n", major); 
        return major; 
    } 
    printk(KERN_INFO "chardev: assigned major number %d.\n", major); 

    cls = class_create(DEVICE_NAME);
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME); 
    printk(KERN_INFO "chardev: loaded %s\n", DEVICE_NAME); 
    return SUCCESS; 
} 

static void __exit chardev_exit(void){ 
    device_destroy(cls, MKDEV(major, 0)); 
    class_destroy(cls); 
    unregister_chrdev(major, DEVICE_NAME); 
    printk(KERN_INFO "chardev: unloaded\n");
} 

module_init(chardev_init); 
module_exit(chardev_exit); 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ikadzev");
MODULE_DESCRIPTION("chardeath: simple character device driver (once caused kernel panic)");