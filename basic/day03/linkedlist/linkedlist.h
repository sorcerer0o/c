#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SAFE_ALLOC(ptr, error_flag,msg) \
    do{ \
        if ((ptr) == (error_flag)) { \
            perror(msg); \
            return msg; \
        } \
    } while(0) \

typedef int DataType;
typedef struct node {
    DataType data;
    struct node *next;
} Node;
typedef struct {
    Node *head;
    Node *tail;
    DataType size;
} LinkedList;

LinkedList *create_linked_list();
void destry_linked_list(LinkedList *list);
bool add_before_head(LinkedList *list,DataType new_data);
bool add_behind_tail(LinkedList *list,DataType new_data);
bool add_by_idx(LinkedList *list, int idx, DataType new_data);
bool delete_by_data(LinkedList *list, DataType data);
bool delete_by_idx(LinkedList *list, int idx);
Node *search_by_data(LinkedList *list, DataType data);
Node *search_by_idx(LinkedList *list, int idx);
void print_list(LinkedList *list);

#endif
