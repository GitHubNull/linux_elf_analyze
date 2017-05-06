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
#define T_ARGV 0
#define T_ENVP 1
#define T_START 2

struct my_msg
{
    struct nlmsghdr hdr;
    int8_t  data[MSG_LEN];
};

int count_srcv(char const **srcv);
int get_args_str(char const **srcv, int start, int stop, int count, char *dst, 
                    int len);

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
    char argv_buff[ARGV_LEN];
    char envp_buff[ENVP_LEN];
    char start[] = "010";

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
    nmh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nmh->nlmsg_flags = 0;
    nmh->nlmsg_pid = USER_PORT; //self port     

    // get argv_str
    retval = get_app_cmd_str(argv, 1, 0, argc, argv_buff, ARGV_LEN);
    if(0 > retval)
        goto out;
    // init argv_str nlmsghdr
    nmh->nlmsg_type = T_ARGV;
    nmh->nlmsg_seq = 0;   
    memcpy(NLMSG_DATA(nmh), argv_buff, strlen(argv_buff));

    // send argv
    retval = sendto(skfd, nmh, nmh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, 
                    sizeof(struct sockaddr_nl));
    if(!retval)
        goto out;

    // check argv_echo    
    printf("wait kernel argv_echo msg!\n");
    memset(&info, 0, sizeof(info));
    retval = recvfrom(skfd, &info, sizeof(struct my_msg), 0, 
                    (struct sockaddr *)&dest_addr, 
                    (socklen_t *)sizeof(dest_addr));
    if(!retval)
        goto out;

    memset(nmh, 0, sizeof(struct nlmsghdr));

    // get envp_str
    retval = get_app_cmd_str(envp, 0, 0, 0, envp_buff, ENVP_LEN);
    if(0 > retval)
        goto out;    
    // init envp_str nlmsghdr
    nmh->nlmsg_type = T_ENVP;
    nmh->nlmsg_seq = 1;     
    memcpy(NLMSG_DATA(nmh), envp_buff, strlen(envp_buff));
    
    // send envp
    retval = sendto(skfd, nmh, nmh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, 
                    sizeof(struct sockaddr_nl));
    if(!retval)
        goto out; 

    // check envp_echo
    printf("wait kernel envp_echo msg!\n");
    memset(&info, 0, sizeof(info));
    retval = recvfrom(skfd, &info, sizeof(struct my_msg), 0, 
                        (struct sockaddr *)&dest_addr, 
                        (socklen_t *)sizeof(dest_addr));
    if(!retval)
        goto out; 

    memset(nmh, 0, sizeof(struct nlmsghdr));
    // init start_str nlmsghdr
    nmh->nlmsg_type = T_START;
    nmh->nlmsg_seq = 2; 
    memcpy(NLMSG_DATA(nmh), start, strlen(start));          
    // send start_str
    retval = sendto(skfd, nmh, nmh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, 
                    sizeof(struct sockaddr_nl));
    if(!retval)
        goto out; 

    // check start_echo
    printf("wait kernel start_echo msg!\n");
    memset(&info, 0, sizeof(info));
    retval = recvfrom(skfd, &info, sizeof(struct my_msg), 0, 
                        (struct sockaddr *)&dest_addr, 
                        (socklen_t *)sizeof(dest_addr));
    if(!retval)
        goto out;     
    // check start_echo
    
    free(nmh);
    close(skfd);    
    return 0;

out:
    free(nmh);
    close(skfd);
    return -1;
}

int get_args_str(char const **srcv, int start, int stop, int count, char *dst, 
                    int len)
{
    int index;
    if(NULL == srcv || NULL == dst)
        return -1;
    if(0 == count)
        count = count_srcv(srcv);
    if(count < start)
        start = 0;
    if(0 == stop)
        stop = count - 1;
    index = start;
    memset(dst, 0, len);
    for(; index < stop; index++)
    {
        strcat(dst, srcv[index]);
        if(index < stop)
            strcat(dst, " ");
    }
    return ((stop - start) + 1);
}

int count_srcv(char const **srcv)
{
    int retval = 0;
    if(NULL == srcv)
        return -1;
    while(NULL != srcv[retval])
    {
        retval++;
    }
    return retval;
}