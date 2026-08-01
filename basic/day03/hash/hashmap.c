#include "hash_map.h"

/* murmur_hash2 */
static uint32_t hash(const void *key, int len, uint32_t seed) {
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    uint32_t h = seed ^ len;
    const unsigned char *data = (const unsigned char *)key;

    while (len >= 4) {
        uint32_t k = *(uint32_t *)data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        len -= 4;
    }

    switch (len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
            h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

// 创建一个固定容量的哈希表，并且此时哈希表是空的
HashMap *hashmap_create() {
    HashMap *map = calloc(1, sizeof(HashMap));
    if (map == NULL) {
        printf("error: calloc failed in hashmap_create.\n");
        return NULL;
    }
    map->hash_seed = time(NULL);
    return map;
}

// 销毁一个哈希表
void hashmap_destroy(HashMap *map) {
    // 遍历每一个哈希桶(也就是遍历数组的每一个元素)，逐一销毁链表结点，最后结构体自身
    for (int i = 0; i < HASHMAP_CAPACITY; i++) {
        KeyValueNode *curr = map->buckets[i];
        while (curr != NULL) {
            KeyValueNode *tmp = curr->next;
            free(curr);
            curr = tmp;
        }
    }
    free(map);
}

/*
    插入一个键值对
    由于键值对的key是唯一的
    所以插入操作具有两种可能性：
    1.插入的key已经重复了，那么插入行为表现为更新value，并且返回旧的value值
    2.插入的key没有重复，那就将这个键值对插入哈希表当中，返回NULL
*/
ValueType hashmap_put(HashMap *map, KeyType key, ValueType val) {
    // 1.计算key的哈希值并且确定哈希桶的位置
    int idx = hash(key, strlen(key), map->hash_seed) % HASHMAP_CAPACITY;

    // 2.遍历这个哈希桶,检查key是否重复
    KeyValueNode *curr = map->buckets[idx];
    while (curr != NULL) {
        if (strcmp(key, curr->key) == 0) {
            // 当前key已经存在于哈希表中了,于是更新value,并返回旧value
            ValueType old_val = curr->val;
            curr->val = val;
            return old_val;
        }
        curr = curr->next;
    }   // while循环结束时,curr指向NULL,这说明哈希表当中key是不存在的,于是开始插入此键值对

    // 3.新分配一个键值对结点,并且初始化它
    KeyValueNode *new_node = malloc(sizeof(KeyValueNode));
    if (new_node == NULL) {
        printf("error: malloc failed in hashmap_put.\n");
        exit(-1);
    }
    new_node->key = key;    // 由于key和val都是字符串,都是char*指针类型,所以用=是可以的
    new_node->val = val;

    // 4.执行链表的头插法,将新结点链接到链表中
    new_node->next = map->buckets[idx];
    map->buckets[idx] = new_node;

    return NULL;
}

// 查询一个键值对
ValueType hashmap_get(HashMap *map, KeyType key) {
    // 1.计算key的哈希值并且确定哈希桶的位置
    int idx = hash(key, strlen(key), map->hash_seed) % HASHMAP_CAPACITY;

    // 2.遍历这个哈希桶,检查key是否存在,如果存在就说明找到了目标键值对
    KeyValueNode *curr = map->buckets[idx];
    while (curr != NULL) {
        if (strcmp(key, curr->key) == 0) {
            // 找到了目标键值对结点
            return curr->val;
        }
        curr = curr->next;
    }   // while循环结束时,curr指向NULL,这说明哈希表当中key是不存在的,查找失败
    return NULL;
}

// 删除某个键值对
bool hashmap_remove(HashMap *map, KeyType key) {
    // 1.计算key的哈希值并且确定哈希桶的位置
    int idx = hash(key, strlen(key), map->hash_seed) % HASHMAP_CAPACITY;

    // 2.初始化两个指针，用来遍历哈希桶
    KeyValueNode *curr = map->buckets[idx];
    KeyValueNode *prev = NULL;
    while (curr != NULL) {
        if (strcmp(key, curr->key) == 0) {
            // 找到了目标键值对结点，开始执行它的删除操作
            if (prev == NULL){
                // 删除的是第一个结点，更新头指针
                map->buckets[idx] = curr->next;
            }
            else{
                // 删除的不是第一个结点，此时不需要更新头指针
                prev->next = curr->next;
            }
            // 以上操作使得待删除结点从链表结构中被移除了，于是free这个结点
            free(curr);
            return true;    // 删除成功
        }
        prev = curr;    // prev要紧跟curr的步伐
        curr = curr->next;
    }   // while循环结束时,curr指向NULL,这说明哈希表当中key是不存在的,没有目标结点，删除失败

    return false;
}
