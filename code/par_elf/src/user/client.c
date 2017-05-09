#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include "../../include/user/client.h"


int count_srcv(char const **srcv);
int get_args_str(char const **srcv, int start, int stop, int count, char *dst,
                    int len);

int count_pack(char *str);
int pack_str(char *str, struct nlk p_nlk, struct pack *pack, int type, int seq);
int free_data_pack(struct pack *pack);
int init_nlk(struct nlk *cnlk);
int send_to_kernel(struct pack pack);

int main(int argc, char const **argv, char const **envp)
{
    // check out argc
    if(1 == argc)
    {
        printf("argc less...\n");
        return 0;
    }
    printf("main...\n");
    int retval;
    char argv_buff[MSG_LEN];
    char envp_buff[MSG_LEN * 8];
    struct nlk p_nlk;
    struct pack a_pack;
    struct pack e_pack;

    retval = init_nlk(&p_nlk);
    if(0 != retval)
    {
        printf("init_nlk( erro. retval = %d\n", retval);
        goto out;
    }

    retval = get_args_str(argv, 0, 0, argc, argv_buff, MSG_LEN);
    if(0 > retval)
    {
        printf("get_args_str(argv erro. retval = %d\n", retval);
        goto out;
    }

    retval = pack_str(argv_buff, p_nlk, &a_pack, T_ARGV, 1);
    if(0 != retval)
    {
        printf("pack_str(argv_buff erro. retval = %d\n", retval);
        goto out;
    }

    retval = send_to_kernel(a_pack);
    if(0 != retval)
    {
        printf("send_to_kernel(a_pack) erro. retval = %d\n", retval);
        goto out;
    }

    retval = get_args_str(envp, 0, 0, 0, envp_buff, (MSG_LEN * 8));
    if(0 > retval)
    {
        printf("get_args_str(envp erro. retval = %d\n", retval);
        goto out;
    }

    retval = pack_str(envp_buff, p_nlk, &e_pack, T_ENVP, 2);
    if(0 != retval)
    {
        printf("pack_str(envp_buff erro. retval = %d\n", retval);
        goto out;
    }

    //sleep(15);
    retval = send_to_kernel(e_pack);
    if(0 != retval)
    {
        printf("send_to_kernel(e_pack) erro. retval = %d\n", retval);
        goto out;
    }

    free_data_pack(&a_pack);
    free_data_pack(&e_pack);
    return 0;

    out:
    free_data_pack(&a_pack);
    free_data_pack(&e_pack);

    return -1;
}

int get_args_str(char const **srcv, int start, int stop, int count, char *dst,
                    int len)
{
    printf("get_args_str...\n");
    int index;
    if(NULL == srcv || NULL == dst)
        return -1;
    if(0 == count)
        count = count_srcv(srcv);
    if(count < start)
        start = 0;
    if(0 == stop)
        stop = count;
    index = start;
    memset(dst, 0, len);
    for(; index < stop; index++)
    {
        strcat(dst, srcv[index]);
        if(index < stop)
            strcat(dst, STR_DELIME);
    }
    return ((stop - start) + 1);
}

int count_srcv(char const **srcv)
{
    printf("count_srcv...\n");
    int retval = 0;
    if(NULL == srcv)
        return -1;
    while(NULL != srcv[retval])
    {
        retval++;
    }
    return retval;
}

int count_pack(char *str)
{
    printf("count_pack...\n");
    int len;
    int q;
    int p;
    if(NULL == str)
        return -1;
    len = strlen(str);
    if(0 == len)
        return -2;
    q = len/MSG_LEN;
    p = len%MSG_LEN;
    if(0 < p)
        return (q + 1);
    else
        return q;
}

int pack_str(char *str, struct nlk p_nlk, struct pack *pack, int type, int seq)
{
    printf("pack_str...\n");
    int cnt;
    int i = 0;
    char tmp_buff[MSG_LEN];
    char *p;
    int flag = 0;
    if(0 > (cnt = count_pack(str)))
        return -1;
    printf("type = %d, cnt = %d, str_len = %d\n", type, cnt, (int)strlen(str));
    pack->cnt = cnt;
    pack->p_nlk = p_nlk;
    if(1 == cnt)
    {
        pack->nmhd = (struct nlmsghdr *)malloc(sizeof(struct nlmsghdr));
        memset(pack->nmhd, 0, sizeof(struct nlmsghdr));
        pack->nmhd->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
        pack->nmhd->nlmsg_flags = flag;
        pack->nmhd->nlmsg_pid = USER_PORT; //self port
        pack->nmhd->nlmsg_type = type;
        pack->nmhd->nlmsg_seq = seq;
        memcpy(NLMSG_DATA(pack->nmhd), str, strlen(str));
    }
    else
    {
        pack->nmhd = (struct nlmsghdr *)malloc(sizeof(struct nlmsghdr) * cnt);
        if(!pack->nmhd)
        {
            printf("malloc error!\n");
           return -2;
        }
        for(; i < cnt; i++)
        {
            memset(&(pack->nmhd[i]), 0, sizeof(struct nlmsghdr));
            pack->nmhd[i].nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
            if(i != cnt - 1)
                flag = 1;
            else
                flag = 0;
            pack->nmhd[i].nlmsg_flags = flag;
            pack->nmhd[i].nlmsg_pid = USER_PORT; //self port
            pack->nmhd[i].nlmsg_type = type;
            pack->nmhd[i].nlmsg_seq = seq + i;
            if(i != cnt)
            {
                memset(tmp_buff, 0, MSG_LEN);
                p = str;
                p = p + (i * MSG_LEN);
                // printf("i = %d\n p = %s\n\n", i, p);
                // strncpy(tmp_buff, p, MSG_LEN);
                // tmp_buff[MSG_LEN - 1] = 0;
                // printf("tmp_buff = %s\n", tmp_buff);
                memcpy(NLMSG_DATA(&pack->nmhd[i]), p, MSG_LEN);
                printf("i = %d\n p = %s\n\n", i, NLMSG_DATA(&pack->nmhd[i]));
            }
            else
            {
                memset(tmp_buff, 0, MSG_LEN);
                p = p + (i * MSG_LEN);
                // strncpy(tmp_buff, p, strlen(p));
                // tmp_buff[strlen(p)] = 0;
                // printf("tmp_buff = %s\n", tmp_buff);
                memcpy(NLMSG_DATA(&pack->nmhd[i]), p, MSG_LEN);
            }
        }
    }
    printf("pack_str============================>>>>>>>>>>>>\n");
    return 0;
}

int free_data_pack(struct pack *pack)
{
    printf("free_data_pack...\n");
    if(NULL == pack)
        return -1;
    int i = 0;
    for(; i < pack->cnt; i++)
    {
        if(NULL != &(pack->nmhd[i]))
        {
            printf("%d\n", i);
            free(&(pack->nmhd[i]));
        }
        // pack->nmhd[i] = NULL;
    }
    // free(&pack->nmhd);
    return 0;
}

int init_nlk(struct nlk *cnlk)
{
    printf("init_cnlk start...\n");
    cnlk->skfd = socket(AF_NETLINK, SOCK_RAW, USER_MSG);
    if(cnlk->skfd == -1)
    {
        printf("create socket error...%s\n", strerror(errno));
       goto out_one;
    }

    memset(&(cnlk->local), 0, sizeof(cnlk->local));
    cnlk->local.nl_family = AF_NETLINK;
    cnlk->local.nl_pid = USER_PORT;
    cnlk->local.nl_groups = 0;
    if(bind(cnlk->skfd, (struct sockaddr *)&(cnlk->local), sizeof(cnlk->local)) != 0)
    {
        printf("bind() error\n");
        goto out_two;
    }

    memset(&(cnlk->dest_addr), 0, sizeof(cnlk->dest_addr));
    cnlk->dest_addr.nl_family = AF_NETLINK;
    cnlk->dest_addr.nl_pid = 0; // to kernel
    cnlk->dest_addr.nl_groups = 0;

    return 0;

out_one:
    return -1;
out_two:
    return -2;
}

int send_to_kernel(struct pack pack)
{
    printf("send_to_kernel...\n");
    if(NULL == pack.nmhd || 0 > pack.p_nlk.skfd)
        return -1;
    int i = 0;
    int retval;
    for(; i < pack.cnt; i++)
    {
        if(1 == pack.nmhd[i].nlmsg_type)
        {
            printf("i = %d\n", i);
            // printf("%s\n", NLMSG_DATA(&pack.nmhd[i]));
        }
        retval = sendto(pack.p_nlk.skfd, &pack.nmhd[i], pack.nmhd[i].nlmsg_len,
                        0, (struct sockaddr *)&(pack.p_nlk.dest_addr),
                        sizeof(struct sockaddr_nl));
        if(!retval)
        {
            perror("error infomation:");
            return -2;
        }
        struct my_msg info;
        memset(&info, 0, sizeof(info));
        retval = recvfrom(pack.p_nlk.skfd, &info, sizeof(struct my_msg),
                            MSG_WAITALL,
                            (struct sockaddr *)&(pack.p_nlk.dest_addr),
                            (socklen_t *)sizeof(pack.p_nlk.dest_addr));
        if(!retval)
        {
            perror("error infomation:");
            return -3;
        }
        if(info.nmhd.nlmsg_seq != pack.nmhd[i].nlmsg_seq)
        {
            printf("info.nmhd.nlmsg_seq = %d\n", info.nmhd.nlmsg_seq);
            return -4;
        }
    }
    return 0;
}
