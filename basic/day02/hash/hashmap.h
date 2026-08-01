#pragma once
#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASHMAP_CAPACITY 10 

typedef char *KeyType;
typedef char *ValueType;

typedef struct node_s {
    KeyType key;
    ValueType val;
    struct node_s *next;
} KeyValueNode;

typedef struct {
    KeyValueNode *buckets[HASHMAP_CAPACITY];    
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