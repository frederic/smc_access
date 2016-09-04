/*
 * drivers/amlogic/smc_access/smc_access.c
 * github.com/frederic/smc_access
 *
 * Copyright (C) 2015 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
*/



/* Standard Linux headers */
#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/wakelock.h>
#include <linux/debugfs.h>
#include <linux/io.h>
#include <linux/uaccess.h>

static struct dentry *debugfs_root = NULL;

static ssize_t smc_write_file(struct file *file, const char __user *userbuf,
				   size_t count, loff_t *ppos)
{
	register uint64_t x0 asm("x0") = 0;
	register uint64_t x1 asm("x1") = 0;
	register uint64_t x2 asm("x2") = 0;
	register uint64_t x3 asm("x3") = 0;
	register uint64_t x4 asm("x4") = 0;
	char buf[80];
	uint64_t arg0, arg1, arg2, arg3, arg4;

	count = min_t(size_t, count, (sizeof(buf)-1));
	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[count] = 0;

	sscanf(buf, "%llx %llx %llx %llx %llx", &arg0, &arg1, &arg2, &arg3, &arg4);
	
	x0 = arg0;
	x1 = arg1;
	x2 = arg2;
	x3 = arg3;
	x4 = arg4;
	
	asm __volatile__("" : : : "memory");

	do {
		asm volatile(
		    __asmeq("%0", "x0")
		    __asmeq("%1", "x0")
		    __asmeq("%2", "x1")
		    __asmeq("%3", "x2")
		    __asmeq("%4", "x3")
		    __asmeq("%5", "x4")
		    "smc #0\n"
		    : "=r"(x0)
		    : "r"(x0), "r"(x1), "r"(x2),"r"(x3),"r"(x4));
	} while (0);
	
	arg1 = x0;
	
	printk(KERN_ALERT "smc_access: SMC call %llx returns: %llx\n", arg0, arg1);

	return count;
}

static const struct file_operations smc_file_ops = {
	.owner		= THIS_MODULE,
	.open		= simple_open,
	.write		= smc_write_file,
};

static int __init smc_access_init(void)
{
	struct dentry *debugfs_file;
	
	debugfs_root = debugfs_create_dir("aml_smc", NULL);
	if (IS_ERR(debugfs_root) || !debugfs_root) {
		pr_warn("smc_access: failed to create smc_access debugfs directory\n");
		debugfs_root = NULL;
		return -1;
	}

	debugfs_file = debugfs_create_file("smc", S_IFREG | S_IRUGO,
			    debugfs_root, NULL, &smc_file_ops);
	if (!debugfs_file) {
		printk(KERN_ALERT "smc_access: failed to create smc_access debugfs file\n");
		return -1;
    }
    
	return 0;
}

static void __exit smc_access_exit(void)
{
	printk(KERN_ALERT "smc_access: exiting...\n");
	if(debugfs_root)
		debugfs_remove_recursive(debugfs_root);
}


module_init(smc_access_init);
module_exit(smc_access_exit);

MODULE_DESCRIPTION("SMC module");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Xing Xu <xing.xu@amlogic.com>");
