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

/**
 * @brief device open function 
 * 
 * @param inode 
 * @param file 
 * @return int 
 */
static int device_open(struct inode *inode, struct file *file)
{
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    // print the function name
    printk(KERN_INFO "%s dev_nr - open was called\n", __FUNCTION__);
    printk(KERN_INFO "device_open(%p)\n", file);
    printk(KERN_INFO "Device opened successfully\n");
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    return 0;
}

/**
 * @brief device close function 
 * 
 * @param inode 
 * @param file 
 * @return int 
 */
static int device_release(struct inode *inode, struct file *file)  {
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    // print the function name
    printk(KERN_INFO "%s dev_nr - release was called\n", __FUNCTION__);
    printk(KERN_INFO "device_release(%p)\n", file);
    printk(KERN_INFO "Device closed successfully\n");
    // print seperator
    printk(KERN_INFO "-----------------------------------------------------\n");
    return 0;
}

/******************************************************************************/
/*                      Module Meta Information                               */
/******************************************************************************/
// Module license information (GPL) - required for all modules in kernel space (not user space) to be distributed under the GPL license.
MODULE_LICENSE("GPL");     
MODULE_AUTHOR("Ahmed Abd El-Hamed ");                  // Author of the module 
MODULE_DESCRIPTION("A hello world - device driver");   // Description of the module

int cnt = 0; // Global variable to count the number of times the module is loaded
module_param(cnt, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);  // Parameter to be passed to the module - cnt with read, write and group permissions for the user and group
MODULE_PARM_DESC(cnt, "An integer to count the number of times the module is loaded");   // Description of the parameter - cnt 

// set major number to 50 - this is the major number of the device driver 
static int major_num = 0;
module_param(major_num, int, S_IRUGO);
MODULE_PARM_DESC(major_num, "The major number of the device driver");

dev_t dev_num; // Global variable to store the device number
struct cdev character_device; // Global variable to store the character device structure
struct class *dev_class; // Global variable to store the device class structure
struct device *dev_device; // Global variable to store the device structure

struct file_operations helloworld_fops={
    .owner = THIS_MODULE,       // Owner of the module - THIS_MODULE is a macro that is defined in the linux/module.h file
    .open = device_open,        // Open function of the device driver - device_open is a function defined in the file Hello.c
    .release = device_release   // Release function of the device driver - device_release is a function defined in the file Hello.c
    // .read = device_read,
    // .write = device_write,
}; // Structure to hold the file operations of the device driver



/**
 * @brief The init function - called when the module is loaded
 * @return 0 on success, negative error code on failure
 */
static int __init hellodriver_init(void)
{
    int i = 0, returnval = 0;
    for (i = 0; i < cnt; i++)
        printk("hello world\n");
    /*************************************************************************************************/
    /* Step 1: Register the device num */
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
        dev_num = MKDEV(major_num, 0);
        returnval = register_chrdev_region(dev_num, 1, "helloworld");
        if (returnval < 0)
        {
            printk(KERN_ALERT "Failed to register a major number\n");
            return returnval;
        }
    }
    /*************************************************************************************************/
    /* Step 2: Define the type of device driver (character device - Block device - Network device) */

    // initialize the cdev structure - this is the structure that holds the file operations of the device driver
    cdev_init(&character_device, &helloworld_fops);
    // add the device to the system - this is the device driver
    returnval = cdev_add(&character_device, dev_num, 1);
    if (returnval < 0)
    {

        // unregister the device number
        unregister_chrdev_region(dev_num, 1);

        printk(KERN_ALERT "Failed to add the device\n");
        return returnval;
    }
    /*************************************************************************************************/
    /* Step 3: Generate the File */
    dev_class = class_create(THIS_MODULE, "helloworld");
    if (IS_ERR(dev_class))  // if the device class is not created successfully
    {
        // unregister the device number
        cdev_del(&character_device);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ALERT "Failed to create the device class\n");
        return PTR_ERR(dev_class);
    }
    // create the device file - this is the device driver
    dev_device  = device_create(dev_class, NULL, dev_num, NULL, "helloworld");
    if (IS_ERR(dev_device))  // if the device is not created successfully
    {
        // unregister the device number
        class_destroy(dev_class);
        cdev_del(&character_device);
        unregister_chrdev_region(dev_num, 1);
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
    printk(KERN_INFO "-----------------------------------------------------\n");







    return 0;
}
/**
 * @brief The exit function - called when the module is unloaded
 */
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



}

module_init(hellodriver_init);  // Register the init function with the kernel
module_exit(hellodriver_exit);  // Register the exit function with the kernel
