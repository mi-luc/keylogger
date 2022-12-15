//TEST MODUL KERNEL

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dumi si Luca");
MODULE_DESCRIPTION("kernel module");
MODULE_VERSION("1");
static int __init init(void) {
 printk(KERN_INFO "Hello, World!\n");
 return 0;
}
static void __exit exit(void) {
 printk(KERN_INFO "Goodbye, World!\n");
}
module_init(init);
module_exit(exit);
