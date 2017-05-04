#ifndef CLIENT_C
#define CLIENT_C

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include "../../include/user/client_nlk.h"
#include "../../include/user/str_pack.h"

// int get_app_cmd_str(char **argv, char *buffer, int cnt);
// int count_envp(char **envp);

int main(int argc, char **argv, char **envp)
{
#if 0
    struct sockaddr_nl  local, dest_addr;

    int skfd;
    struct nlmsghdr *nlh = NULL;
    struct _my_msg info;
    int ret;
    char buffer[MSG_LEN];
    char *p = buffer;

    if(1 == argc){
        printf("Usage: pes your app ...\n");
        goto out;
    }

    if(2 < argc){
        if(0 != get_app_cmd_str(argv, p, argc - 1))
           goto out;
    }
    if(2 == argc){
        memset(p, '\0', MSG_LEN);
        strcpy(p, argv[1]);
    }
    printf("%s, len = %ld\n", p, strlen(p));

    skfd = socket(AF_NETLINK, SOCK_RAW, USER_MSG);
    if(skfd == -1)
    {
        printf("create socket error...%s\n", strerror(errno));
       goto out;
    }

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = 50; 
    local.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&local, sizeof(local)) != 0)
    {
        printf("bind() error\n");
        close(skfd);
        goto out;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // to kernel
    dest_addr.nl_groups = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = local.nl_pid; //self port

    get_app_cmd_str(argv, p, argc - 1);

    memcpy(NLMSG_DATA(nlh), p, strlen(p));
    ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_nl));

    if(!ret)
    {
        perror("sendto error1\n");
        close(skfd);
        goto out;
    }
    printf("wait kernel msg!\n");
    memset(&info, 0, sizeof(info));
    ret = recvfrom(skfd, &info, sizeof(struct _my_msg), 0, (struct sockaddr *)&dest_addr, (socklen_t *)sizeof(dest_addr));
    if(!ret)
    {
        perror("recv form kernel error\n");
        close(skfd);
        goto out;
    }

    printf("msg receive from kernel:%s\n", info.data);

out:
    close(skfd);

    free((void *)nlh);
    return 0;
#endif
    struct CNLK cnlk;
    int retval = init_cnlk(&cnlk);
    if(0 != retval)
        goto out1;
    char *tmp = (char *)malloc(sizeof(char) * 8092 * 3);
    memset(tmp, 0, sizeof(char) * 8092 * 3);

    retval = count_envp(envp);
    if(0 > retval)
        goto out2;
    int i = 0;

#if 1
    for(; i < retval; i++){
        printf("envp[%d] = %s\n", i, envp[i]);
    }
#endif

    retval = get_app_cmd_str(envp, tmp, retval);
    if(0 != retval)
        goto out2;
    int envp_len = strlen(tmp);

    retval = send_argv(argv, argc, envp_len, cnlk);
    if(1 != retval){
         goto out2;
    }

    DATA_PACK data_pack;
    retval = pack_str(tmp, &data_pack);
    if(0 != retval){
        printf("pack_str erro...\n");
        goto out3;
    }
    retval = send_envp(data_pack, cnlk);
    if(0 != retval){
        printf("send_envp erro...\n");
        goto out3;
    }
    
    retval = send_start(cnlk);
    if(0 != retval){
        printf("send_start erro...\n");
        goto out3;
    }

    free(tmp);
    return 0;

out1:
    return -1;
out2:
    free(tmp);
    return -2;
out3:
    free(tmp);
    free_data_pack(&data_pack);
    return -2; 
}

#endif