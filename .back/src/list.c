#include "../lib/list.h"

bool list_init(List list)
{
    list.data = NULL;
    list.next = NULL;
    return true;
}

bool list_insert(List list, Node node)
{
    list.next = &node;
    node.next = NULL;
    return true;
}

