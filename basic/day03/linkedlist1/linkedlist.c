#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>

// 创建一个空的链表
LinkedList *create_linked_list() {
    return calloc(1, sizeof(LinkedList));
}
// 销毁链表
void destroy_linked_list(LinkedList *list) {
    // 遍历整条链表,包括最后一个结点,然后逐一free
    Node *curr = list->head;
    while (curr != NULL) {
        Node *tmp = curr->next;
        free(curr);
        curr = tmp;
    }
    free(list);
}

// 头部插入结点
bool add_before_head(LinkedList *list, DataType data) {
    // 1.分配一个新结点,初始化它
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("error: malloc failed in add_before_head.\n");
        return false;
    }
    new_node->data = data;
    // 2.让新结点指向原本的第一个结点
    new_node->next = list->head;

    // 3.更新头指针
    list->head = new_node;

    // 考虑特殊情况
    //if(list->size == 0)
    if (list->tail == NULL) {
        // 说明头插之前链表为空, 更新尾指针
        list->tail = new_node;
    }
    list->size++;
    return true;
}
// 尾部插入结点
bool add_behind_tail(LinkedList *list, DataType data) {
    // 1.分配一个新结点,初始化它
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("error: malloc failed in add_behind_tail.\n");
        return false;
    }
    new_node->data = data;
    new_node->next = NULL;

    // 2.判断链表插入之前是否为空
    if (list->size == 0) {
        // 尾插之前链表为空
        list->head = new_node;
        list->tail = new_node;
        list->size++;
        return true;
    }
    // 链表尾插之前不为空
    list->tail->next = new_node;
    list->tail = new_node;
    list->size++;
    return true;
}
// 按索引插入结点
/*
    对于链表来说,第一个结点的索引规定是0,那么链表的结点索引合法范围是[0, size-1]
    分析:
    1.参数校验,对错误的参数进行错误的处理
    index在这个函数中,合法的取值范围是[0, size]
    其中0表示头插, size表示尾插

    2.处理特殊值,尾插和头插直接调用已实现的函数
    3.在index合法,且不是头插尾插,即在中间插入结点时
*/
bool add_by_idx(LinkedList *list, int index, DataType data) {
    if (index < 0 || index > list->size) {
        printf("Illegal Arguments: index = %d\n", index);
        return false;
    }
    if (index == 0) {
        return add_before_head(list, data);
    }
    if (index == list->size) {
        return add_behind_tail(list, data);
    }

    // 处理中间插入结点的情况
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("error: malloc failed in add_index.\n");
        return false;
    }
    new_node->data = data;

    // 遍历链表,找到index-1位置的结点
    Node *prev = list->head;
    for (int i = 0; i < index - 1; i++) {
        prev = prev->next;
    }   // for循环结束时,prev指针指向index索引的前驱结点

    // 让新结点指向prev的后继结点
    new_node->next = prev->next;
    // 让prev指向新结点
    prev->next = new_node;
    list->size++;
    return true;
}

// 根据索引搜索结点
Node *search_by_idx(LinkedList *list, int index) {
    if (index < 0 || index > list->size - 1) {  // size-1就是最后一个结点
        printf("Illegal Arguments: index = %d\n", index);
        return NULL;
    }
    // 索引合法
    Node *curr = list->head;
    for (size_t i = 0; i < index; i++)  // 小于index就表示寻找index索引的结点
    {
        curr = curr->next;
    }   // for循环结束时,curr指针指向index索引位置的结点
    return curr;
}

// 根据数据搜索结点
Node *search_by_data(LinkedList *list, DataType data) {
    // 惯用法遍历每一个结点,遍历过程中判断即可
    Node *curr = list->head;
    while (curr != NULL) {
        if (curr->data == data) {
            // 找到了
            return curr;
        }
        curr = curr->next;
    }
    // 遍历结束都没有return 说明没有找到
    return NULL;
}

// 根据数据删除结点
/*
    对于一条单链表来说, 在一个确定的结点的后面进行删除/新增操作, 它的时间复杂度是O(1)
    链表的结点删除,哪些是O(1)的呢?
    1.删除第一个结点是不是? 是,因为这个过程基本只需要改变头指针的指向就可以了
    2.删除最后一个结点是不是呢? 不是, 因为删除最后一个结点,需要找到倒数第二个结点,这需要遍历数组,时间复杂度是O(n)
    3.删除中间的结点是不是呢? 肯定不是,因为需要遍历找到这个结点
    于是将单链表的删除分解成两个步骤:
    1.假如删除的是第一个结点
    2.如果删除的是第一个结点外的其它结点

*/
bool delete_by_data(LinkedList *list, DataType data) {
    // 校验: 如果链表为空,不删除了
    if (list->head == NULL) {
        printf("链表为空,无法删除!\n");
        return false;
    }
    Node *curr = list->head;    // 该指针用于遍历整条单链表
    // 1.处理删除的是第一个结点的情况
    if (curr->data == data) {
        list->head = curr->next;    // 这步操作不论链表是否只有一个结点 都是要做的
        if (list->head == NULL) {
            // 说明链表仅有一个结点,删除后,要同步更新尾指针
            list->tail = NULL;
        }
        free(curr);
        list->size--;
        return true;
    }

    // 2.处理其他结点的删除情况
    // 已经有一个指针,叫curr,它目前指向第一个结点
    Node *prev = curr;
    curr = curr->next;

    while (curr != NULL) {  // 从第二个结点开始,遍历完整条单链表
        if (curr->data == data) {
            // 找到了待删除目标结点,curr指向它,此时prev指向它的前驱
            prev->next = curr->next;
            if (curr->next == NULL) {
                // 待删除结点是尾结点
                list->tail = prev;
            }
            free(curr);
            list->size--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    } // while循环结束时,curr指针指向NULL,prev指向链表的最后一个结点

    // while循环结束都没有return, 说明没找到目标data的结点,删除失败
    return false;
}

// 根据索引删除结点
bool delete_by_idx(LinkedList *list, int idx) {
    if (list->head == NULL) {
        printf("链表为空,无法删除!\n");
        return false;
    }

    // 检查链表是否为空或索引是否越界
    if (idx < 0 || idx >= list->size) {
        printf("Illegal Arguments: index = %d\n", idx);
        return false;
    }

    Node *curr = list->head;
    Node *prev = NULL;

    // 如果要删除的是第一个结点
    if (idx == 0) {
        list->head = curr->next;  // 更新头指针
        // 若删除的结点是唯一的结点
        if (list->size == 1) {
            list->tail = NULL;  // 更新尾指针
        }
        free(curr);
        list->size--;
        return true;
    }

    // 如果删除的结点不是第一个结点
    for (int i = 0; i < idx; i++) {
        prev = curr;
        curr = curr->next;
    }

    // 让前驱结点指向后继结点
    prev->next = curr->next;

    // 如果要删除的是尾结点, 则需要更新尾指针
    if (idx == list->size - 1) {
        list->tail = prev;
    }

    free(curr);
    list->size--;
    return true;
}

// 打印单链表 格式为：1 -> 2 -> 3 ->...
void print_list(LinkedList *list) {
    Node *curr = list->head;
    // 遍历此单链表
    while (curr != NULL) {
        printf("%d", curr->data);
        // 如果不是链表的最后一个结点，就打印箭头
        if (curr->next != NULL) {
            printf(" -> ");
        }
        curr = curr->next;
    }
    printf("\n");
}
