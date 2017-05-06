#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#define NETLINK_USER 22
#define USER_MSG (NETLINK_USER + 1)
#define ARGV_LEN (1024 * 8)
#define ENVP_LEN (ARGV_LEN * 2)
#define MAX_PLOAD (ENVP_LEN)
#define USER_PORT 50

struct my_msg
{
    struct nlmsghdr hdr;
    int8_t  data[MSG_LEN];
};



int main(int argc, char **argv, char **envp)
{
    // check out argc
    if(1 == argc)
    {
        printf("argc less...\n");
        return 0;
    }

    int retval;
    int skfd;
    struct sockaddr_nl  local, dest_addr;
    struct nlmsghdr *nmh = NULL;
    struct my_msg info;

    // init sock
    skfd = socket(AF_NETLINK, SOCK_RAW, USER_MSG);
    if(skfd == -1)
    {
        printf("create socket error...%s\n", strerror(errno));
        return -1;
    }
   
    // bind sock
    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = USER_PORT; 
    local.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&local, sizeof(local)) != 0)
    {
        printf("bind() error\n");
        close(skfd);
        return -1;
    } 

    // init dest_addr
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // to kernel
    dest_addr.nl_groups = 0; 

    // init nlmsghdr
    nmh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nmh, 0, sizeof(struct nlmsghdr)); 

    // get argv_str
    // init argv_str nlmsghdr

    // get envp_str
    // init argv_str nlmsghdr 
       
    // send argv
    // check argv_echo
    

    // send envp
    // check envp_echo

    // send start
    // check start_echo
    
    return 0;
}