/**
 * @file Hello.c
 * @author Ahmed Moahmed (ahmed.moahmed.eng.25@gmail.com)
 * @brief Hello World Device Driver 
 * @version 0.1
 * @date 2022-08-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>

/******************************************************************************/
/*                     Function Declarations                                  */
/******************************************************************************/
/**
 * @brief The init function - called when the module is loaded
 * @return 0 on success, negative error code on failure
 */
static int __init hellodriver_init(void);

/**
 * @brief The exit function - called when the module is unloaded
 */
static void __exit hellodriver_exit(void);

/**
 * @brief device open function 
 * 
 * @param inode     the inode of the device - unused
 * @param file      the file structure of the device - contains the file operations
 * 
 * @return int    0 on success, negative error code on failure
 */
static int device_open(struct inode *inode, struct file *file);

/**
 * @brief device close function 
 * 
 * @param inode     the inode of the device - unused
 * @param file      the file structure of the device - contains the file operations
 * @return int      0 on success, negative error code on failure
 */
static int device_release(struct inode *inode, struct file *file);

/**
 * @brief device read function 
 * 
 * @param file      the file structure of the device - contains the file operations
 * @param buf       the buffer to read into
 * @param count     the number of bytes to read
 * @param ppos      the current position in the file
 * @return ssize_t  the number of bytes read, or negative error code on failure
 */
static ssize_t device_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);

/**
 * @brief device write function 
 * 
 * @param file      the file structure of the device - contains the file operations
 * @param buf       the buffer to write from
 * @param count     the number of bytes to write
 * @param ppos      the current position in the file
 * @return ssize_t  the number of bytes written, or negative error code on failure
 */
static ssize_t device_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

/******************************************************************************/
/*                      Module Meta Information                               */
/******************************************************************************/
// Module license information (GPL) - required for all modules in kernel space (not user space) to be distributed under the GPL license.
MODULE_LICENSE("GPL");     
MODULE_AUTHOR("Ahmed Abd El-Hamed ");                  // Author of the module 
MODULE_DESCRIPTION("A hello world - device driver");   // Description of the module
MODULE_VERSION("0.1");                                  // Version of the module

/******************************************************************************/
/*                      Module Parameters                                     */
/******************************************************************************/

int cnt = 0; // Global variable to count the number of times the module is loaded
module_param(cnt, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);  // Parameter to be passed to the module - cnt with read, write and group permissions for the user and group
MODULE_PARM_DESC(cnt, "An integer to count the number of times the module is loaded");   // Description of the parameter - cnt 

// set major number to 50 - this is the major number of the device driver 
static int major_num = 0;
module_param(major_num, int, S_IRUGO);
MODULE_PARM_DESC(major_num, "The major number of the device driver");

/******************************************************************************/
/*                      Module Global Variables                               */
/******************************************************************************/

dev_t dev_num; // Global variable to store the device number
struct cdev character_device; // Global variable to store the character device structure
struct class *dev_class; // Global variable to store the device class structure
struct device *dev_device; // Global variable to store the device structure

struct file_operations helloworld_fops={
    .owner = THIS_MODULE,       // Owner of the module - THIS_MODULE is a macro that is defined in the linux/module.h file
    .open = device_open,        // Open function of the device driver - device_open is a function defined in the file Hello.c
    .release = device_release,  // Release function of the device driver - device_release is a function defined in the file Hello.c
    .read = device_read,        // Read function of the device driver - device_read is a function defined in the file Hello.c
    .write = device_write       // Write function of the device driver - device_write is a function defined in the file Hello.c
}; // Structure to hold the file operations of the device driver



/************************************************************************************************/
/*                               Module Registration Functions                                  */
/************************************************************************************************/
module_init(hellodriver_init);  // Register the init function with the kernel
module_exit(hellodriver_exit);  // Register the exit function with the kernel



/******************************************************************************/
/*                      Function Definitions                                  */
/******************************************************************************/
static int __init hellodriver_init(void)
{
    int i = 0;          // Loop counter
    int returnval = 0;  // Return value of functions to handle errors
    for (i = -1; i < cnt; i++)       // Loop to print the number of times the module is loaded
        printk("************************Hello world Module**************************************\n");

    /*************************************************************************************************/
    /* Step 1: Register the device num */
    /*************************************************************************************************/
    // Allocate a device number - this is a macro defined in the linux/major.h file
    // get device major number dynamically - if the major number is not set in the module parameter
    if (major_num == 0)
    {
        // get the major number dynamically - this is the major number of the device driver
        returnval = alloc_chrdev_region(&dev_num, 0, 1, "helloworld");
        if (returnval < 0)
        {
            printk(KERN_ALERT "Failed to allocate a major number\n");
            printk(KERN_ALERT "\t Major number is %d, Minor number is %d\n", MAJOR(dev_num), MINOR(dev_num));
            return returnval;
        }else{
            printk(KERN_INFO "Successfully allocated a major number\n");
            printk(KERN_INFO "Major number is %d, Minor number is %d\n", MAJOR(dev_num), MINOR(dev_num));
        }
    }
    else
    {
        // get the major number statically - this is the major number of the device driver
        dev_num = MKDEV(major_num, 0);          // Create a device number - this is a macro defined in the linux/major.h file
        // Register the device number - this is a macro defined in the linux/major.h file
        returnval = register_chrdev_region(dev_num, 1, "helloworld");   
        if (returnval < 0)
        {
            printk(KERN_ALERT "Failed to register a major number\n");
            return returnval;
        }
    }

    /*************************************************************************************************/
    /* Step 2: Define the type of device driver (character device - Block device - Network device) */
    /*************************************************************************************************/
    // initialize the cdev structure - this is the structure that holds the file operations of the device driver
    cdev_init(&character_device, &helloworld_fops);
    // add the device to the system - this is the device driver
    returnval = cdev_add(&character_device, dev_num, 1);
    if (returnval < 0)
    {
        unregister_chrdev_region(dev_num, 1);       // Unregister the device number - this is a macro defined in the linux/major.h file
        printk(KERN_ALERT "Failed to add the device\n");
        return returnval;
    }
    /*************************************************************************************************/
    /* Step 3: Generate the File System                                                              */
    /*************************************************************************************************/
    dev_class = class_create(THIS_MODULE, "helloworld");        // Create a device class - this is a macro defined in the linux/fs.h file
    if (IS_ERR(dev_class))  // if the device class is not created successfully
    {
        // unregister the device number
        cdev_del(&character_device);             // Delete the device - this is a macro defined in the linux/major.h file
        unregister_chrdev_region(dev_num, 1);    // Unregister the device number - this is a macro defined in the linux/major.h file
        printk(KERN_ALERT "Failed to create the device class\n");
        return PTR_ERR(dev_class);
    }
    // create the device file - this is the device driver
    dev_device  = device_create(dev_class, NULL, dev_num, NULL, "helloworld");
    if (IS_ERR(dev_device))  // if the device is not created successfully
    {
        // unregister the device number
        class_destroy(dev_class);               // Destroy the device class - this is a macro defined in the linux/fs.h file
        cdev_del(&character_device);            // Delete the device - this is a macro defined in the linux/major.h file
        unregister_chrdev_region(dev_num, 1);   // Unregister the device number - this is a macro defined in the linux/major.h file
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(dev_device);
    }
    printk(KERN_INFO "Device driver created successfully\n");
    /*************************************************************************************************/
    /* Step 4: Print the module meta information */
    printk(KERN_INFO "-----------------------------------------------------\n");
    printk(KERN_INFO "Hello World Device Driver has been loaded\n");
    printk(KERN_INFO "Module name: %s\n", THIS_MODULE->name);
    printk(KERN_INFO "Module version: %s\n", THIS_MODULE->version);
    printk(KERN_INFO "Module author: Ahmed Abd El-Hamed \n");
    printk(KERN_INFO "-----------------------------------------------------\n");

    return 0;
}

static void __exit hellodriver_exit(void)
{
    int i = 0;
    for (i = 0; i < cnt; i++)
        printk("good bye\n");

    // remove the device from the system - this is the device driver
    cdev_del(&character_device);
    // destroy the device file - this is the device driver
    device_destroy(dev_class, dev_num);
    // destroy the device class - this is the device driver
    class_destroy(dev_class);
    // unregister the device number - this is the device driver
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "Hello World Device Driver has been unloaded\n");
    printk(KERN_INFO "-----------------------------------------------------\n");
}

static int device_open(struct inode *inode, struct file *file)
{
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    // print the function name
    printk(KERN_INFO "%s dev_nr - open was called\n", __FUNCTION__);
    printk(KERN_INFO "device_open(%p)\n", file);
    printk(KERN_INFO "File name: %s\n", file->f_path.dentry->d_iname);
    printk(KERN_INFO "Device opened successfully\n");
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)  {
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    // print the function name
    printk(KERN_INFO "%s dev_nr - release was called\n", __FUNCTION__);
    printk(KERN_INFO "device_release(%p)\n", file);
    printk(KERN_INFO "File name: %s\n", file->f_path.dentry->d_iname);
    printk(KERN_INFO "Device closed successfully\n");
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buf, size_t count, loff_t *ppos){
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    // print the function name
    printk(KERN_INFO "%s dev_nr - read was called\n", __FUNCTION__);
    printk(KERN_INFO "device_read(%p)\n", file);
    printk(KERN_INFO "File name: %s\n", file->f_path.dentry->d_iname);
    printk(KERN_INFO "Device read successfully\n");
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    return 0;
}

static ssize_t device_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos){
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    // print the function name
    printk(KERN_INFO "%s dev_nr - write was called\n", __FUNCTION__);
    printk(KERN_INFO "device_write(%p)\n", file);
    printk(KERN_INFO "File name: %s\n", file->f_path.dentry->d_iname);
    printk(KERN_INFO "Device write successfully\n");
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    return 0;
}