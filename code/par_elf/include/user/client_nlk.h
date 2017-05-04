#ifndef CLIENT_NLK_H
#define CLIENT_NLK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include "../../include/unit/data.h"

struct CNLK //client_netlink
{
	int skfd;
	struct sockaddr_nl  local;
	struct sockaddr_nl dest_addr;
};

struct _my_msg
{
    struct nlmsghdr hdr;
    int8_t  data[MSG_LEN];
};

int init_cnlk(struct CNLK *cnlk);

int get_app_cmd_str(char **argv, char *buffer, int cnt);

int send_argv(char **argv, int argc, int envp_len, struct CNLK cnlk);

int count_envp(char **envp);

int send_envp(DATA_PACK data_pack, struct CNLK cnlk);

int send_start(struct CNLK cnlk);

#endif