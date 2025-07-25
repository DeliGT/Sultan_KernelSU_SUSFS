// SPDX-License-Identifier: GPL-2.0
// PR_SET_PROPERTY syscall and init.d support for Android kernels

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/prctl.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/namei.h>

#define PR_SET_PROPERTY 0x53504F4F // "SPOO" in hex
#define PROP_NAME_MAX 32
#define PROP_VALUE_MAX 92

// Simple property spoofing handler
static int handle_pr_set_property(unsigned long arg2, unsigned long arg3) {
    char prop_name[PROP_NAME_MAX];
    char prop_value[PROP_VALUE_MAX];
    if (copy_from_user(prop_name, (char __user *)arg2, PROP_NAME_MAX - 1))
        return -EFAULT;
    if (copy_from_user(prop_value, (char __user *)arg3, PROP_VALUE_MAX - 1))
        return -EFAULT;
    prop_name[PROP_NAME_MAX - 1] = '\0';
    prop_value[PROP_VALUE_MAX - 1] = '\0';
    pr_info("PRCTL_PROPERTY: %s=%s\n", prop_name, prop_value);
    // You could add extra hooks here, e.g. sysfs write or custom kernel property
    return 0;
}

// Hook into prctl syscall (must be registered properly in kernel)
SYSCALL_DEFINE5(prctl, int, option, unsigned long, arg2, unsigned long, arg3,
                unsigned long, arg4, unsigned long, arg5)
{
    if (option == PR_SET_PROPERTY) {
        return handle_pr_set_property(arg2, arg3);
    }
    // Original prctl logic here (call original implementation if available)
    return -EINVAL;
}

// Optionally: init.d runner (for Android boot scripts)
static int __init run_initd_scripts(void)
{
    struct file *filp;
    struct dir_context ctx;
    const char *initd_dir = "/system/etc/init.d";
    // This is a stub -- in practice, init.d support is best done in init (user space),
    // but you can use this as a template to run scripts at boot if needed.
    pr_info("Checking for init.d support at %s\n", initd_dir);
    // You could call usermodehelper to execute scripts from this directory
    // Example: call_usermodehelper("/system/etc/init.d/00firstscript", ...);
    return 0;
}

module_init(run_initd_scripts);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DeliGT & Copilot");
MODULE_DESCRIPTION("PR_SET_PROPERTY syscall & init.d support for spoofing");
