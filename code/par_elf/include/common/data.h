#ifndef DATA_H
#define DATA_H

#define NETLINK_USER 22
#define USER_MSG    (NETLINK_USER + 1)
#define ARGV_LEN (1024 * 4)
#define MSG_LEN (ARGV_LEN)
#define ENVP_LEN (ARGV_LEN)
#define MAX_PLOAD (ENVP_LEN)
#define USER_PORT   50
#define T_ARGV 0 	//nlmsg_type
#define T_ENVP 1	//nlmsg_type
#define T_START 2	//nlmsg_type

// requst statu code
#define C_ARGV "01"		// complte send argv
#define E_ARGV "02"		//	send argv erro
#define C_ENVP "03"		// complte send envp
#define E_ENVP "04"		// send envp error
#define C_START "05"	// complte start app
#define E_START "06"	// start app erro

#define DELIME '|' 		// delime
#define STR_DELIME "|" 		// delime

#endif