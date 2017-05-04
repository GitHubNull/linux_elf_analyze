#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/string.h>
#include "../../include/unit/data.h"
#include "../../include/kernel/str_unpack.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("lm");
MODULE_DESCRIPTION("netlink_demo");

// extern long do_fork(unsigned long clone_flags,
//           unsigned long stack_start,
//           unsigned long stack_size,
//           int __user *parent_tidptr,
//           int __user *child_tidptr);

// extern static int do_execveat_common(int fd, struct filename *filename,
//                   struct user_arg_ptr argv,
//                   struct user_arg_ptr envp,
//                   int flags);

// extern int str_to_argv(char *str, char delim, char ***argv);
// extern int free_argv(char ***argv, int cnt);

extern int unpack_argv(char *src);

extern int unpack_envp(char *src, int seq);

extern int unpack_start(char *src);

static struct sock *netlinkfd = NULL;


static int send_to_user(const char* data, unsigned int len, int type, int seq)
{
        int ret;
        int size;
        sk_buff_data_t old_tail;
        struct sk_buff *skb;
        struct nlmsghdr *nlhdr;
        struct cha *packet;
        /*计算消息总长：消息首部加上数据加度*/
        size = NLMSG_SPACE(len);

        /*分配一个新的套接字缓存*/
        skb = alloc_skb(size, GFP_ATOMIC);
        old_tail = skb->tail;

        /*初始化一个netlink消息首部*/
        nlhdr = nlmsg_put(skb, 0, 0, USER_MSG, (size - sizeof(*nlhdr)), 0);
        /*跳过消息首部，指向数据区*/
        packet = NLMSG_DATA(nlhdr);
        /*初始化数据区*/
        memset(packet, 0, len);
        memcpy(packet, data, len);

        nlhdr->nlmsg_len = skb->tail - old_tail;
        /*设置控制字段*/
        nlhdr->nlmsg_pid = 0;
        nlhdr->nlmsg_flags = 0;
        nlhdr->nlmsg_type = type;
        nlhdr->nlmsg_seq = seq;


        /*发送数据*/
        ret = netlink_unicast(netlinkfd, skb, USER_PORT, MSG_DONTWAIT);
        return ret;
}

int send_msg(int8_t *pbuf, uint16_t len)
{
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;

    int ret;

    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if(!nl_skb)
    {
        printk("netlink_alloc_skb error\n");
        return -1;
    }

    nlh = nlmsg_put(nl_skb, 0, 0, USER_MSG, len, 0);
    if(nlh == NULL)
    {
        printk("nlmsg_put() error\n");
        nlmsg_free(nl_skb);
        return -1;
    }
    memcpy(nlmsg_data(nlh), pbuf, len);

    ret = netlink_unicast(netlinkfd, nl_skb, USER_PORT, MSG_DONTWAIT);

    return ret;
}


static void recv_cb(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    void *data = NULL;
    int retval;

    if(skb->len <= nlmsg_total_size(0))
        return;

    nlh = nlmsg_hdr(skb);
    data = NLMSG_DATA(nlh);
    if(NULL != data)
        return;

    switch(nlh->nlmsg_type){
        case T_ARG:
            retval = unpack_argv((int8_t *)data);
            if(0 == retval){
                send_to_user(C_ARGV, strlen(C_ARGV), T_ARG, 1);
                printk("unpack_argv somplete.\n");
                break;
            }
            else{
                send_to_user(E_ARGV, strlen(E_ARGV), T_ARG, 1);
                printk("unpack_argv error.\n");
                break;  
            }          
        case T_ENVP:
                retval = unpack_envp((int8_t *)data, nlh->nlmsg_seq);
                if(0 == retval){
                    send_to_user(C_ENVP, strlen(C_ENVP), T_ENVP, nlh->nlmsg_seq);
                    printk("unpack_envp somplete.\n");
                    break;
                }
                else if(1 == retval){
                    char tmp[3];
                    tmp[0] = nlh->nlmsg_seq - 48;
                    tmp[1] = (nlh->nlmsg_seq + 1) - 48;
                    tmp[2] = 0;
                    send_to_user(tmp, strlen(tmp), T_ENVP, nlh->nlmsg_seq);
                    printk("unpack_envp continue...\n");
                    break;
                }
                else{
                    send_to_user(E_ENVP, strlen(E_ENVP), T_ENVP, nlh->nlmsg_seq);
                    printk("unpack_envp error.\n");
                    break; 
                }                   
        case T_START:
                retval = unpack_start((int8_t *)data);
                if(0 == retval){
                    send_to_user(C_START, strlen(C_START), T_START, 1);
                    printk("unpack_start complete.\n");
                    break;
                }
                else{
                    send_to_user(E_START, strlen(E_START), T_START, 1);
                    printk("unpack_start error.\n");
                    break;
                }
        default:
            break;               
    }
}


struct netlink_kernel_cfg cfg =
{
    .input = recv_cb,
};

static int __init pare_netlink_init(void)
{
    printk("init netlink_demo!\n");


    netlinkfd = netlink_kernel_create(&init_net, USER_MSG, &cfg);
    if(!netlinkfd)
    {
        printk(KERN_ERR "can not create a netlink socket!\n");
        return -1;
    }

    printk("netlink demo init ok!\n");
    return 0;
}

static void __exit pare_netlink_exit(void)
{
    sock_release(netlinkfd->sk_socket);
    printk(KERN_DEBUG "netlink exit\n!");
}

module_init(pare_netlink_init);
module_exit(pare_netlink_exit);
