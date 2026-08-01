#include "linkedlist.h"

LinkedList *create_linked_list() {
    return calloc(1,sizeof(LinkedList));
}
void destry_linked_list(LinkedList *list) {
    Node *curr = list->head;
    while (curr != NULL) {
        Node *temp = curr->next;
        free(curr);
        curr = temp;
    }
    free(list);
}
bool add_before_head(LinkedList *list,DataType new_data) {
    Node *new_node = calloc(1, sizeof(Node));
    SAFE_ALLOC(new_node,NULL,false);
    new_node->data = new_data;
    // Make the new node point to the first node.
    new_node->next = list->head;
    list->head = new_node;
    // linkedlist == NULL;
    if (list->tail == NULL) {
        list->tail = new_node;
    }
    list->size++;
    return true;
}
bool add_behind_tail(LinkedList *list,DataType new_data) {
    Node *new_node = calloc(1, sizeof(Node));
    SAFE_ALLOC(new_node,NULL,false);
    new_node->data = new_data;
    // The next pointer of the last node points to null. 
    new_node->next = NULL;
    // if linkedlist == NULL;
    if(list->size == 0) {
        list->head = new_node;
        list->tail = new_node;
        list->size++;
        return true;
    }
    // Make the old tail node point to the inserted node
    new_node->next = list->tail;
    list->tail = new_node;
    list->size++;
    return true;
}
bool add_by_idx(LinkedList *list, int idx, DataType new_data) {
    if(idx < 0 || idx > list->size) {
        return false;
    }
    if(idx == 0) {
        add_before_head(list, new_data);
    }
    if(idx == list->size) {
        add_behind_tail(list, new_data);
    }
    Node *new_node = calloc(1, sizeof(Node));
    SAFE_ALLOC(new_node,NULL,false);
    new_node->data = new_data;
    Node *prev = list->head;
    for (int i = 0; i < idx - 1; i++) {
        prev = prev->next;
    }
    new_node->next = prev->next;
    prev->next = new_node;
    list->size++;
    return true;
}
Node *search_by_idx(LinkedList *list, int index) {
    if (index < 0 || index > list->size - 1) {  
        printf("Illegal Arguments: index = %d\n", index);
        return NULL;
    }
    Node *curr = list->head;
    for (size_t i = 0; i < index; i++)  
    {
        curr = curr->next;
    }   
    return curr;
}

Node *search_by_data(LinkedList *list, DataType data) {
    Node *curr = list->head;
    while (curr != NULL) {
        if (curr->data == data) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}
void print_list(LinkedList *list) {
    Node *curr = list->head;
    while (curr != NULL) {
        printf("%d", curr->data);
        if (curr->next != NULL) {
            printf(" -> ");
        }
        curr = curr->next;
    }
    printf("\n");
}
