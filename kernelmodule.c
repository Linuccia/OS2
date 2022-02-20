#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/mount.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include "kernelmodule.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Polina Stephanskaya");
MODULE_DESCRIPTION("Kernel module for getting vm_area_struct and vfsmount structs informatipn via debugfs driver");
MODULE_VERSION("1.0");

/* debugfs directory and files */
static struct dentry *debugfs_dir;
static struct dentry *debugfs_vma_file;
static struct dentry *debugfs_vma_arg_file;
static struct dentry *debugfs_vfs_file;

/* needed structs and arguments */
static struct vm_area_struct *vma;
static pid_t pid_num;
static struct vfsmount *vfs;

static char buffer_k[BUFFER_SIZE] = {NULL};
static bool init_error = false;

/* function for getting vm_area_struct */
static struct vm_area_struct * get_vma( pid_t nr ) {
    struct task_struct *ts = get_pid_task(find_get_pid(nr), PIDTYPE_PID);
    struct mm_struct *mm = ts->mm;
    struct vm_area_struct *vma = mm->mmap;
    return vma;
}

/* function for getting vm_area_struct */
static struct vfsmount * get_vfs(void) {
    struct vfsmount *vfs = kern_mount(get_fs_type("bdev"));
    return vfs;
}

/* utility function for sending message to user */
static void to_user( char __user *buff, size_t count, loff_t *fpos ) {
    count = strlen(buffer_k) + 1;
    copy_to_user(buff, buffer_k, count);
    *fpos += count;
}

/* overrode function to send vm_area_struct to user when he tries to read debugfs file */
static ssize_t vma_to_user(struct file *filp, char __user *buff, size_t count, loff_t *fpos ) {
    if ( *fpos > 0 )  {
        return -EFAULT;
    }
    if ( init_error ) {
        sprintf(buffer_k, KERNEL_ERROR_MSG);
    } else {
        printk(KERN_INFO "Trying to get vm_area_struct\n");
        vma = get_vma(pid_num);
        if ( vma == NULL ) {
            printk(KERN_WARNING "Cannot get vm_area_struct\n");
            sprintf(buffer_k, KERNEL_ERROR_MSG);
        } else {
            sprintf(buffer_k, "vm_start = %d\nvm_end = %d\nvm_flags = %d\n", vma->vm_start, vma->vm_end, vma->vm_flags);
        }
    }
    to_user(buff, count, fpos);
    return *fpos;
}

/* overrode function to get PID from user when he tries to write it to debugfs file */
static ssize_t pid_from_user(struct file *filp, const char __user *buff, size_t count, loff_t *fpos) {
    if ( BUFFER_SIZE < count ) {
        printk(KERN_WARNING "Buffer size is not enough\n");
        sprintf(buffer_k, KERNEL_ERROR_MSG);
        to_user(buff, count, fpos);
        return -EFAULT;
    }
    if ( *fpos > 0 )  {
        return -EFAULT;
    }
    printk(KERN_INFO "Trying to get PID from user...\n");
    copy_from_user(buffer_k, buff, count);
    if ( sscanf(buffer_k, "%d", &pid_num) < 1 ) {
    	printk(KERN_WARNING "Cannot read PID properly\n");
    	return -EFAULT;
    }
    *fpos = strlen(buffer_k);
    return *fpos;
}

/* overrode function to send vfsmount struct to user when he tries to read debugfs file */
static ssize_t vfs_to_user( struct file *filp, char __user *buff, size_t count, loff_t *fpos ) {
    if ( *fpos > 0 )  {
        return -EFAULT;
    }
    if ( init_error ) {
        sprintf(buffer_k, KERNEL_ERROR_MSG);
    } else {
   	printk(KERN_INFO "Trying to get vfsmount struct...\n");
        vfs = get_vfs();
        if ( vfs == NULL ) {
            printk(KERN_WARNING "Cannot get vfsmount struct\n");
            sprintf(buffer_k, KERNEL_ERROR_MSG);
        } else {
            sprintf(buffer_k, "mnt_flags = %d\n", vfs->mnt_flags);
        }
    }
    to_user(buff, count, fpos);
    return *fpos;
}

/* overriding read and write functions for debugfs files */
static const struct file_operations vma_file_op = {
        .read = vma_to_user
};
static const struct file_operations vma_arg_file_op = {
        .write = pid_from_user
};
static const struct file_operations vfs_file_op = {
        .read = vfs_to_user
};

/* module initialization */
static int __init kernel_module_init( void ) {
    printk(KERN_INFO "Initializing kernel module...\n");
    debugfs_dir = debugfs_create_dir(DEBUGFS_DIR_NAME, NULL);
    if ( !debugfs_dir ) {
        printk(KERN_WARNING "Cannot create debugfs directory\n");
        init_error = true;
        return -1;
    }
    debugfs_vma_file = debugfs_create_file(DEBUGFS_VMA_FILE_NAME, 0666, debugfs_dir, NULL, &vma_file_op);
    if ( !debugfs_vma_file ) {
        printk(KERN_WARNING "Cannot create debugfs file\n");
        init_error = true;
        return -1;
    }
    debugfs_vma_arg_file = debugfs_create_file(DEBUGFS_VMA_ARG_FILE_NAME, 0666, debugfs_dir, NULL, &vma_arg_file_op);
    if ( !debugfs_vma_arg_file ) {
        printk(KERN_WARNING "Cannot create debugfs arg file\n");
        init_error = true;
        return -1;
    }
    debugfs_vfs_file = debugfs_create_file(DEBUGFS_VFS_FILE_NAME, 0666, debugfs_dir, NULL, &vfs_file_op);
    if ( !debugfs_vfs_file ) {
        printk(KERN_WARNING "Cannot create debugfs file\n");
        init_error = true;
        return -1;
    }
    printk(KERN_INFO "Kernel module initialized successfully\n");
    return 0;
}

/* module cleanup */
static void __exit kernel_module_exit( void ) {
    debugfs_remove(debugfs_vma_file);
    debugfs_remove(debugfs_vma_arg_file);
    debugfs_remove(debugfs_vfs_file);
    debugfs_remove(debugfs_dir);
}

/* registering module init and exit functions */
module_init(kernel_module_init);
module_exit(kernel_module_exit);

