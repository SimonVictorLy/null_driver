#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
//#include <asm/uaccess.h> // API for checking valid input
#include <linux/uaccess.h>

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class

static char c;

static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0; // returns 0 on success
}
static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0; // returns 0 on success
}

// READING THE DEVICE
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	// __user *buf is the buffer we write to with size of len
	//	off is to stop cat /dev/mynull from infinitly reading the same character
	// return negative for errors, otherwise, bytes read
	
    printk(KERN_INFO "Driver: read()\n");
	
	if(*off == 0){
		// check correct input
		if(copy_to_user(buf, &c,1)!=0)
			return -EFAULT; // error	
		else{
			(*off)++;
			return 1;
		}	
	}
	return 0;
	

}

// WRITING TO THE DEVICE
static ssize_t my_write(struct file *f, const char __user *buf, size_t len,
    loff_t *off)
{
	// __user* buf is read with a size len
	// return negative for errors, otherwise, bytes written
	
    printk(KERN_INFO "Driver: write()\n");
 	
	// len includes the '\0' string terminating character
	if(copy_from_user(&c,buf+len-2,1)!=0)
		return -EFAULT;
	else
		return len; 
}

static struct file_operations pugs_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};



// START AT __init function
static int __init ofcd_init(void) /* Constructor */
{
    int ret;
    struct device *dev_ret;
	
    printk(KERN_INFO "Namaskar: ofcd registered");

	// Allocate character device number to first
    if ((ret = alloc_chrdev_region(&first, 0, 1, "Shweta")) < 0)
    {
        return ret;
    }
	// Create a device class
    if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv")))
    {
        unregister_chrdev_region(first, 1); // deallocate char device number
        return PTR_ERR(cl);
    }
	
	// Create a device using the device class and the allocated number
    if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "mynull")))
    {
        class_destroy(cl); // deallocate class
        unregister_chrdev_region(first, 1); // deallocate char device number
        return PTR_ERR(dev_ret);
    }

	// Virtual File System (VFS)
	// 1. initialize c_dev with pugs_fops which holds all the functions: open,close,read,write. (look above)
	// 2. Add the CDF major and minor number to the c_dev, character device structure

    cdev_init(&c_dev, &pugs_fops);
    if ((ret = cdev_add(&c_dev, first, 1)) < 0)
    {
        device_destroy(cl, first); // deallocate device
        class_destroy(cl); // deallocate class
        unregister_chrdev_region(first, 1); // deallocate char device number
        return ret;
    }
    return 0;
}

static void __exit ofcd_exit(void) /* Destructor */
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Alvida: ofcd unregistered");
}

module_init(ofcd_init);
module_exit(ofcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anil Kumar Pugalia <email@sarika-pugs.com>");
MODULE_DESCRIPTION("Our First Character Driver");
