#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h> // 包含它是为了使用别名uint32_t
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define HASHMAP_CAPACITY 10 // 哈希表中数组的长度固定是10

// 此时哈希表用于存储字符串类型的键值对
typedef char *KeyType;
typedef char *ValueType;

// 键值对结点
typedef struct node_s {
    KeyType key;
    ValueType val;
    struct node_s *next;
} KeyValueNode;

typedef struct {
    // 哈希桶
    KeyValueNode *buckets[HASHMAP_CAPACITY];    // 直接给定哈希桶的数量是10个
    // 哈希函数需要的种子值
    uint32_t hash_seed;
} HashMap;

// 创建一个固定容量的哈希表
HashMap *hashmap_create();
// 销毁一个哈希表
void hashmap_destroy(HashMap *map);
// 插入一个键值对
ValueType hashmap_put(HashMap *map, KeyType key, ValueType val);
// 查询一个键值对
ValueType hashmap_get(HashMap *map, KeyType key);
// 删除某个键值对
bool hashmap_remove(HashMap *map, KeyType key);

#endif // !HASH_MAP_H
