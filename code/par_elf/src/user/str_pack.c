#include "../../include/user/str_pack.h"

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

int pack_str(char *str, DATA_PACK *data_pack)
{
	printf("pack_str...\n");
	int cnt;
	int i = 0;
	char *p;
	if(0 > (cnt = count_pack(str)))
		return -1;

	printf("cnt = %d\n", cnt);
	data_pack->cnt = cnt;
	p = str;
    data_pack->data = (char **)malloc(sizeof(char*) * cnt);
    memset(data_pack->data, 0, (sizeof(char*) * cnt));
	for(; i < cnt; i++){
		data_pack->data[i] = (char*)malloc(sizeof(char) * (MSG_LEN + 1));
		if(NULL == data_pack->data[i]){
			printf("erro!\n");
			goto out;
		}
		memset(data_pack->data[i], 0, (MSG_LEN + 1));
		strncpy(data_pack->data[i], (p + (i * MSG_LEN)), MSG_LEN);
		data_pack->data[i][MSG_LEN] = 0;
	}
	out:
	return 0;
}

int free_data_pack(DATA_PACK *data_pack)
{
	printf("free_data_pack...\n");
	if(NULL == data_pack || 0 == data_pack->cnt)
		goto out;
	int i = 0;
	int cnt = data_pack->cnt;
	for(; i < cnt; i++){
		free(data_pack->data[i]);
		data_pack->data[i] = NULL;
	}
out:
	return 0;
}