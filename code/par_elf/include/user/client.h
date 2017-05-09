#ifndef CLIENT_H
#define CLIENT_H

#include "../common/data.h"

struct my_msg
{
    struct nlmsghdr nmhd;
    int8_t  data[MSG_LEN];
};

struct nlk
{
	int skfd;
	struct sockaddr_nl  local;
	struct sockaddr_nl dest_addr;
};

struct pack
{
	int cnt;
	struct nlmsghdr *nmhd;
	struct nlk p_nlk;
};

#endif