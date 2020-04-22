#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h> 
#include <linux/device.h>	/* device class */
#include <linux/cdev.h>    /* Needed for cdev struct */
#include <linux/slab.h>    /* Needed for kmalloc, kzalloc etc. */

#define   DEFAULT_MAJOR         0
#define   DEFAULT_MINOR         0
#define   DEFAULT_NUM_DEVICES   1
#define  MODULE_NODE_NAME   "cdev"
#define  MODULE_NAME     "asp_mycdev"
#define  MODULE_CLASS_NAME  "asp_mycdev_class"
#define  MAX_NODE_NAME_SIZE  10

static int mycdev_major = DEFAULT_MAJOR;
static int mycdev_minor = DEFAULT_MINOR;
static int max_devices = DEFAULT_NUM_DEVICES;
static struct class *asp_mycdev_class = NULL;
static struct asp_mycdev *mycdev_devices = NULL;

MODULE_LICENSE("GPL");

/* Device struct */
struct asp_mycdev
{
	int devID; /* device ID */
	char *ramdisk; /* device */
	size_t ramdiskSize; /* device size */
	struct mutex lock; /* mutex for this device */
	struct cdev cdev; /* char device struct */
	struct device *device; /* device node in sysfs */
	bool devReset; /* flag to indicate that the device is reset */
};


long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
printk(KERN_INFO "IOCTL !!!\n");
return 0;
}

/* fileops for asp_mycdev */

static struct file_operations mycdev_fileops = {
	.owner  = THIS_MODULE,
/*	.open   = asp_mycdev_open,
	.read   = asp_mycdev_read,
	.llseek = asp_mycdev_lseek,
	.write  = asp_mycdev_write,
	.release = asp_mycdev_release,*/
	.unlocked_ioctl = my_ioctl,
};



int init_module(void)
{
printk(KERN_INFO "Hello world [0002] !!!\n");



dev_t devNum = 0;
int retval = 0;

devNum = MKDEV(mycdev_major, mycdev_minor);

if(mycdev_major) {
		devNum = MKDEV(mycdev_major, mycdev_minor);
		retval = register_chrdev_region(devNum, max_devices, MODULE_NODE_NAME);
	}
	else {
		retval = alloc_chrdev_region(&devNum, mycdev_minor, max_devices, MODULE_NODE_NAME);
		mycdev_major = MAJOR(devNum);
	}
printk(KERN_DEBUG "[001] \n");
if(retval < 0){
		printk(KERN_WARNING "%s: Unable to allocate major %d\n", MODULE_NAME, mycdev_major);
		return retval;
}
printk(KERN_DEBUG "[002] \n");
asp_mycdev_class = class_create(THIS_MODULE, MODULE_CLASS_NAME);
	if(IS_ERR_OR_NULL(asp_mycdev_class)){
		printk(KERN_DEBUG "%s: Failed to Init Device Class %s\n",\
			MODULE_NAME, MODULE_CLASS_NAME);
		retval = -1;
		goto FAIL;
	}
	printk(KERN_DEBUG "%s: Created device class: %s\n", MODULE_NAME, MODULE_CLASS_NAME);
printk(KERN_DEBUG "[003] \n");
char nodeName[MAX_NODE_NAME_SIZE] = { 0 };

snprintf(nodeName, sizeof(nodeName), MODULE_NODE_NAME"%d", 0);

mycdev_devices = kzalloc(max_devices * sizeof(struct asp_mycdev), GFP_KERNEL);
	if(mycdev_devices == NULL){
		printk(KERN_DEBUG "kzalloc FAIL!!!!\n");
		retval = -ENOMEM;
		goto FAIL;
	}
printk(KERN_DEBUG " kzalloc sucsess\n");
mycdev_devices[0].device = device_create(asp_mycdev_class, NULL, MKDEV(mycdev_major, mycdev_minor), NULL, nodeName);


//-----------------------
int error = 0;
	dev_t devNo = 0;

	/* Device Number */
	devNo = MKDEV(mycdev_major, mycdev_minor);
	/* Init cdev */
	cdev_init(&mycdev_devices[0].cdev, &mycdev_fileops);
	mycdev_devices[0].cdev.owner = THIS_MODULE,
	mycdev_devices[0].cdev.ops = &mycdev_fileops;
	/* Add the device, NOTE:: This makes the device go live! */
	error = cdev_add(&mycdev_devices[0].cdev, devNo, 1);
	/* report error */
	if(error) {
		printk(KERN_DEBUG "%s: Error %d adding mycdev%d\n", MODULE_NAME, error);
		retval = -1;
	}
	return retval;

//-----------------------


FAIL:
	printk(KERN_DEBUG "[FAIL!!!!]\n");
	cleanup_module();
	
	return retval;










printk(KERN_DEBUG "%s: Requested Devices - %d, Major :- %d, Minor - %d\n",\
		MODULE_NAME, max_devices, mycdev_major, mycdev_minor);


return 0;
}

void cleanup_module(void)
{
printk(KERN_INFO "Goodbuy world !!!\n");



cdev_del(&mycdev_devices[0].cdev);

device_destroy(asp_mycdev_class, MKDEV(mycdev_major, mycdev_minor));


unregister_chrdev_region(MKDEV(mycdev_major, mycdev_minor), max_devices);

printk(KERN_INFO "%s: Cleanup Done!\n", MODULE_NAME);
}

















