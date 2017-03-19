#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct node{
    struct node *node;
    struct node *next;
}Node;

typedef struct list{
    Node *data;
    Node *next;
}List;

/* init list . */
bool list_init(List list);

/* insert data into list . */
bool list_insert(List list, Node node);

/* delete data from list . */
bool list_delete(List list, Node node);

/* update list old_node to be new_node value . */
bool list_update(List list, Node old_node, Node new_node);

/* select a node value from list . */
Node list_select(List list, Node node);


#endif LIST_H
