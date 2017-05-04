#include "../../include/user/client_nlk.h"

int init_cnlk(struct CNLK *cnlk)
{
	cnlk->skfd = socket(AF_NETLINK, SOCK_RAW, USER_MSG);
	if(cnlk->skfd == -1)
    {
        printf("create socket error...%s\n", strerror(errno));
       goto out_one;
    }

    memset(&(cnlk->local), 0, sizeof(cnlk->local));
    cnlk->local.nl_family = AF_NETLINK;
    cnlk->local.nl_pid = 50; 
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

int get_app_cmd_str(char **argv, char *buffer, int cnt)
{
    memset(buffer, 0, MSG_LEN);
    int i = 0;
    for(; i < cnt; i++){
        strcat(buffer, argv[1 + i]);
        if(i != cnt - 1)
            strcat(buffer, " ");
    }
    return 0;
}

int send_argv(char **argv, int argc, int envp_len, struct CNLK cnlk)
{
	char msg[MSG_LEN];
	char *p = msg;
	int ret;
	struct _my_msg info;
    if(1 == argc){
        printf("Usage: pes your app ...\n");
        return 0;
    }
    if(2 == argc){
        memset(p, '\0', MSG_LEN);
        strcpy(p, argv[1]);
    }else{
		if(0 != get_app_cmd_str(argv, p, argc - 1)){
			printf("get_app_cmd_str erro...\n");
			return -1;
		}
    }

	char num[6];
	sprintf(num, "%d", envp_len);

	strcat(msg, " ");
	strcat(msg, num);

	struct nlmsghdr *nlh = NULL;
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = T_ARG;
    nlh->nlmsg_seq = 1;
    nlh->nlmsg_pid = cnlk.local.nl_pid; //self port	

    memcpy(NLMSG_DATA(nlh), p, strlen(p));
    ret = sendto(cnlk.skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&(cnlk.dest_addr), 
    				sizeof(struct sockaddr_nl));
    if(!ret)
    {
        perror("sendto error1\n");
        free(nlh);
        return -2;
    }

	memset(&info, 0, sizeof(info));
    ret = recvfrom(cnlk.skfd, &info, sizeof(struct _my_msg), 0, (struct sockaddr *)&(cnlk.dest_addr),
    				 (socklen_t *)sizeof(cnlk.dest_addr));
    if(!ret)
    {
        perror("recv form kernel error\n");
        free(nlh);
        return -3;
    } 
    if(0 == (strcmp(C_ARGV, info.data))){
    	printf("argv translate complte!\n");
    	return 1;
    }else{
		printf("argv translate erro!\n");
    	return -4; 	
    }
}

int count_envp(char **envp)
{
    int retval = 0;
    if(NULL == envp)
        return -1;
    while(NULL != envp[retval]){
        retval++;
    }
    return retval;
}

int send_envp(DATA_PACK data_pack, struct CNLK cnlk)
{
    int i = 0;
    int retval;
    struct nlmsghdr *nlh = NULL;
    struct _my_msg info;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    for(; i < data_pack.cnt; i++){
        memset(nlh, 0, sizeof(struct nlmsghdr));
        nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
        nlh->nlmsg_flags = 0;
        nlh->nlmsg_type = T_ENVP;

        if((i + 1) == data_pack.cnt)
            nlh->nlmsg_seq = 0;
        else
            nlh->nlmsg_seq = i + 1;
        
        nlh->nlmsg_pid = cnlk.local.nl_pid; //self port 

        memcpy(NLMSG_DATA(nlh),  data_pack.data[i], strlen( data_pack.data[i]));
        retval = sendto(cnlk.skfd, nlh, nlh->nlmsg_len, 0, 
                        (struct sockaddr *)&(cnlk.dest_addr), 
                        sizeof(struct sockaddr_nl));
        if(!retval){
            perror("sendto error1\n");
           goto out1;
        } 


        memset(&info, 0, sizeof(info));
        retval = recvfrom(cnlk.skfd, &info, sizeof(struct _my_msg), 0, (struct sockaddr *)&(cnlk.dest_addr),
                         (socklen_t *)sizeof(cnlk.dest_addr));
        if(!retval)
        {
            perror("recv form kernel error\n");
            goto out1;
        } 
        
        char status[3];
        int curi = i + 1;
        int nexi = i + 2;
        sprintf(status, "%d%d", curi, nexi);

        if(0 == (strcmp(status, info.data)))
            continue;
        else if(0 == (strcmp(C_ENVP, info.data)))
            goto out;
        else
            goto out1;
    }

out:
    free(nlh);
    nlh = NULL;
    return 0;

out1: 
    free(nlh);
    nlh = NULL;
    return -1;
}

int send_start(struct CNLK cnlk)
{
    int retval;
    struct _my_msg info;
    char start_str[] = "010";
    struct nlmsghdr *nlh = NULL;
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = T_START;
    nlh->nlmsg_seq = 1;
    nlh->nlmsg_pid = cnlk.local.nl_pid; //self port 

    memcpy(NLMSG_DATA(nlh), start_str, strlen(start_str));
    retval = sendto(cnlk.skfd, nlh, nlh->nlmsg_len, 0, 
                    (struct sockaddr *)&(cnlk.dest_addr), 
                    sizeof(struct sockaddr_nl));

    if(!retval)
    {
        perror("sendto error1\n");
       goto out1;
    }

    memset(&info, 0, sizeof(info));
    retval = recvfrom(cnlk.skfd, &info, sizeof(struct _my_msg), 0, 
                    (struct sockaddr *)&(cnlk.dest_addr),
                     (socklen_t *)sizeof(cnlk.dest_addr));

    if(!retval)
    {
        perror("recv form kernel error\n");
        goto out2;
    }

    if(0 == (strcmp(C_START, info.data))){
        printf("app start complte!\n");
       goto out;
    }else{
        printf("app start erro!\n");
        goto out3;
    }	

out:
    free(nlh);
    return 0;

out1:
    free(nlh);
    return -1;

out2:
    free(nlh);
    return -2;

out3:
    free(nlh);
    return -3;
}