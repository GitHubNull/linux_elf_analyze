#ifndef STR_PACK_H
#define STR_PACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/unit/data.h"

int count_pack(char *str);

int pack_str(char *str, DATA_PACK *data_pack);

int free_data_pack(DATA_PACK *data_pack);

#endif