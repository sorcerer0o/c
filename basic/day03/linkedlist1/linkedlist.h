// 头文件保护语法
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

// 包含linked_list.h头文件也会同步包含它包含的其它头文件
// 根据这一特点, 可以选择将一些共用的头文件包含在此头文件中
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef int DataType;

// 定义链表结点结构
typedef struct node {
	DataType data;		// 数据域
	struct node *next;  // 指针域
} Node;

// 定义链表结构本身
typedef struct {
	Node *head;		// 头指针
	Node *tail;		// 尾结点指针
	int size;		// 用于记录链表的长度
} LinkedList;

// 创建一个空的链表
LinkedList *create_linked_list();
// 销毁链表
void destroy_linked_list(LinkedList *list);
// 头插法
bool add_before_head(LinkedList *list, DataType new_data);
// 尾插法
bool add_behind_tail(LinkedList *list, DataType new_data);
// 根据索引插入一个新结点
bool add_by_idx(LinkedList *list, int idx, DataType new_data);
// 根据索引搜索一个结点
Node *search_by_idx(LinkedList *list, int idx);
// 根据数据搜索一个结点
Node *search_by_data(LinkedList *list, DataType data);
// 根据数据删除一个结点
bool delete_by_data(LinkedList *list, DataType data);
// 扩展: 根据索引删除一个结点
bool delete_by_idx(LinkedList *list, int idx);
// 打印单链表 格式为：1 -> 2 -> 3 ->...
void print_list(LinkedList *list);
                
#endif // !LINKED_LIST_H
