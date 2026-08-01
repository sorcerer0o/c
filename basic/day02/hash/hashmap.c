#include "hashmap.h"

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

// 创建一个固定容量的哈希表
HashMap *hashmap_create() {
	HashMap *map = calloc(1, sizeof(HashMap));
	if (map == NULL) {
		printf("calloc fail in hashmap create\n");
		return NULL;
	}
	map->hash_seed = 1;
	return map;
}
// 销毁一个哈希表
void hashmap_destroy(HashMap *map) {
	for (size_t i = 0; i < HASHMAP_CAPACITY; i++) {
		KeyValueNode *temp = map->buckets[i];
		while (temp != NULL)
		{
			KeyValueNode *prev = temp->next;
			free(temp);
			temp = prev;
		}
	}
	free(map);
}
// 插入一个键值对
ValueType hashmap_put(HashMap *map, KeyType key, ValueType val) {
	int idx = hash(key, strlen(key), map->hash_seed) % HASHMAP_CAPACITY;
	KeyValueNode *curr = map->buckets[idx];
	while (curr != NULL)
	{
		if (strcmp(curr->key, key) == 0) {
			ValueType old_val = curr->val;
			curr->val = val;
			return old_val;
		}
		curr = curr->next;
	}
	KeyValueNode *new_node = calloc(1, sizeof(KeyValueNode));
	if (new_node == NULL) {
		printf("calloc fail in hashmap_put\n");
		return NULL;
	}
	new_node->val = val;
	new_node->key = key;
	new_node->next = map->buckets[idx];
	map->buckets[idx] = new_node;

	return NULL;
}
// 查询一个键值对
ValueType hashmap_get(HashMap *map, KeyType key) {
	int idx = hash(key, strlen(key), map->hash_seed) % HASHMAP_CAPACITY;
	KeyValueNode *curr = map->buckets[idx];
	while (curr !=  NULL)
	{
		if (strcmp(curr->key, key) == 0) {
			return curr->val;
		}
		curr = curr->next;
	}

	return NULL;
}
// 删除某个键值对
bool hashmap_remove(HashMap *map, KeyType key) {
	int idx = hash(key, strlen(key), map->hash_seed) % HASHMAP_CAPACITY;
	KeyValueNode *curr = map->buckets[idx];
	KeyValueNode *prev = NULL;
	while (curr != NULL)
	{
		if (strcmp(curr->key, key) == 0) {
			if (prev == NULL) {
				map->buckets[idx] = curr->next;
			}
			else {
				prev->next = curr->next;
			}
			free(curr);
			return true;
		}
		prev = curr;
		curr = curr->next;
	}
	return false;
}