#include "linkedlist.h"

int main(void) {
    LinkedList *list = create_linked_list();

    // 测试头部插入
    add_before_head(list, 10);
    add_before_head(list, 20);
    add_before_head(list, 30);

    printf("在头部插入30, 20, 10后:\n");
    print_list(list); // 预期输出: 30 -> 20 -> 10

    // 测试尾部插入
    add_behind_tail(list, 40);
    add_behind_tail(list, 50);

    printf("在尾部插入40, 50后:\n");
    print_list(list); // 预期输出: 30 -> 20 -> 10 -> 40 -> 50

    // 测试按索引插入
    add_by_idx(list, 2, 25); // 在索引2的位置插入25

    printf("在索引2插入25后:\n");
    print_list(list); // 预期输出: 30 -> 20 -> 25 -> 10 -> 40 -> 50

    // 测试按索引搜索
    Node *node = search_by_idx(list, 2);
    printf("索引2处的元素: %d\n", node->data); // 预期输出: 25

    // 测试按数据搜索
    node = search_by_data(list, 40);
    printf("找到数据40的结点: %d\n", node->data); // 预期输出: 40

    // 销毁链表
    destry_linked_list(list);
    return 0;
}
