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


/**
 * @brief The init function - called when the module is loaded
 * @return 0 on success, negative error code on failure
 */
static int __init hellodriver_init(void)
{
    int i = 0;
    for (i = 0; i < cnt; i++)
        printk("hello world\n");
    return 0;
}
/**
 * @brief The exit function - called when the module is unloaded
 */
static void __exit hellodriver_exit(void)
{
    int i;
    for (i = 0; i < cnt; i++)
        printk("good bye\n");
}

module_init(hellodriver_init);  // Register the init function with the kernel
module_exit(hellodriver_exit);  // Register the exit function with the kernel
