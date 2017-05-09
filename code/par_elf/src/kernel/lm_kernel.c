#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/string.h>
#include <linux/slab.h>
#include "../../include/kernel/lm_kernel.h"
#include "../../include/kernel/string.h"

MODULE_LICENSE("GPL");  
MODULE_AUTHOR("lm");  
MODULE_DESCRIPTION("lm_kernel_demo");

// extern long do_fork(unsigned long clone_flags,
//           unsigned long stack_start,
//           unsigned long stack_size,
//           int __user *parent_tidptr,
//           int __user *child_tidptr);

// extern static int do_execveat_common(int fd, struct filename *filename,
//                   struct user_arg_ptr argv,
//                   struct user_arg_ptr envp,
//                   int flags);

static struct sock *netlinkfd = NULL;

static char argv_buff[ARGV_LEN];
static char **argv;
static int argc;

static char envp_buff[ENVP_LEN];
static char **envp;
static int envc;

int count_arg(char *str, char delim);
int str_to_argv(char *str, char delim, char ***argv);
int free_argv(char ***argv, int cnt);
int atoi(char *str);

// static char **envp;

static int send_to_user(const char* data, unsigned int len, int type, int seq)
{
    int retval;
    int size;
    sk_buff_data_t old_tail;
    struct sk_buff *skb;
    struct nlmsghdr *nlhdr;        
    // printk("send_to_user start...\n");
    
    /*计算消息总长：消息首部加上数据长度*/
    size = NLMSG_SPACE(MAX_PLOAD);
    if(0 > size)
    {
        printk("error to NLMSG_SPACE\n");
        goto out;
    }

    /*分配一个新的套接字缓存*/
    skb = alloc_skb(size, GFP_ATOMIC);
    if(!skb)
    {
        printk("error to alloc_skb!\n");
        goto out_free_skb;
    }
    old_tail = skb->tail;

    /*初始化一个netlink消息首部*/
    nlhdr = nlmsg_put(skb, 0, 0, USER_MSG, (size - sizeof(*nlhdr)), 0);
    if(!nlhdr)
    {
        printk("error to nlmsg_put!\n");
        goto out;
    }
    /*跳过消息首部，指向数据区*/
    /*初始化数据区*/
    memset(NLMSG_DATA(nlhdr), 0, len);
    memcpy(NLMSG_DATA(nlhdr), data, len);

    nlhdr->nlmsg_len = skb->tail - old_tail;
    /*设置控制字段*/
    nlhdr->nlmsg_pid = 0;
    nlhdr->nlmsg_flags = 0;
    nlhdr->nlmsg_type = type;
    nlhdr->nlmsg_seq = seq;

    // printk("preto netlink_unicast.()\n");
    /*发送数据*/
    retval = netlink_unicast(netlinkfd, skb, USER_PORT, MSG_DONTWAIT);
    // if(NULL != skb)
    //     kfree_skb(skb);
    
    return retval;

    out_free_skb:
    kfree_skb(skb);
    out:
    return -1;
}

static void recv_cb(struct sk_buff *skb)
{
    struct nlmsghdr *nmh = NULL;
    void *data = NULL;
    int i;
    // int cnt;
    if(skb->len < nlmsg_total_size(0))
        return;

    nmh = nlmsg_hdr(skb);
    data = NLMSG_DATA(nmh);
    if(!data)
        return;

    if(T_ARGV == nmh->nlmsg_type)
    {
        // printk("================= ARGV =============\n\n");
        // printk("%s\n", (int8_t *)data);
        strcpy(argv_buff, data);
        // printk("argv_buff len: %d\n", (int)strlen(argv_buff));
        // printk("argv: %s\n", (char*)data);
        // argc = str_to_argv((char*)data, ' ', &argv);
        // for (i = 0; i < argc; i++)
        // {
        //     printk("argv[%d] = %s\n", i, argv[i]);
        //     // printk("i = %d", i);
        // }
        send_to_user(C_ARGV, strlen(C_ARGV), T_ARGV, nmh->nlmsg_seq);
        memset(envp_buff, 0, ENVP_LEN);
    }

   if(T_ENVP == nmh->nlmsg_type)
    {
        // // printk("================= T_ENVP =============\n\n");
        

        // if(0 == nmh->nlmsg_flags)
        // {
        //     printk("app args translation complete.\n");
        //     printk("envp_buff len: %d\n\n", (int)strlen(envp_buff));
        //     // printk("envp_buff:\n%s\n", envp_buff);
        // }
        // printk("nmh->nlmsg_type = %d, nmh->nlmsg_seq = %d, nmh->nlmsg_flags = %d\n", 
        //         nmh->nlmsg_type, nmh->nlmsg_seq, nmh->nlmsg_flags);
        strncat(envp_buff, (char*)data, strlen((char*)data));
        printk("envp_buff len = %ld\n", strlen(envp_buff));
        if(0 == nmh->nlmsg_flags)
        {
            // printk("\n\n envp_buff = %s\n", envp_buff);
            envc = str_to_argv((char*)data, DELIME, &envp);
            for (i = 0; i < envc; i++)
            {
                printk("envp[%d] = %s\n", i, envp[i]);
                // printk("i = %d", i);
            }
        }
        // printk("envp_buff:\n%s\n", envp_buff);
        // printk("data:\n%s\n", (char*)data);
       //  i = 0;
       // while(envp[i])
       // {
       //      printk("envp[%d] = %s\n", i, envp[i]);
       //      i++;
       // }

        send_to_user(C_ENVP, strlen(C_ENVP), T_ENVP, nmh->nlmsg_seq);
        // printk("================= T_ENVP =============\n\n");
    }
    // send_msg(data, nlmsg_len(nmh));
} 


struct netlink_kernel_cfg cfg = 
{
    .input = recv_cb,
};

static int __init lm_kernel_init(void)
{
    // int i;
    printk("init lm_kernel_demo!\n");
    memset(envp_buff, 0, ENVP_LEN);
    netlinkfd = netlink_kernel_create(&init_net, USER_MSG, &cfg);
    if(!netlinkfd)
    {
        printk(KERN_ERR "can not create a netlink socket!\n");
        return -1;
    }

    printk("lm_kernel_demo init ok!\n");
    // i = 0;
    // while(envp[i])
    // {
    //     printk("envp[%d] = %s\n", i, envp[i]);
    //     i++;
    // }
    return 0;
}

static void __exit lm_kernel__exit(void)
{
    free_argv(&argv, argc);
    free_argv(&envp, envc);
    sock_release(netlinkfd->sk_socket);
    printk(KERN_DEBUG "lm_kernel_demo exit\n");
}

module_init(lm_kernel_init);
module_exit(lm_kernel__exit);