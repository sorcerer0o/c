#include "hash_map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    // 创建哈希表
    HashMap *map = hashmap_create();
    if (map == NULL) {
        printf("哈希表创建失败。\n");
        return 1;
    }

    // 插入键值对
    hashmap_put(map, "key1", "value1");  // 首次插入，应成功
    hashmap_put(map, "key2", "value2");  // 再插一个新键值对，应成功
    char *old_value = hashmap_put(map, "key1", "value3");  // 更新已存在的键值对，应返回旧值"value1"
    printf("更新键值对时返回的旧值应为value1，实际返回：%s\n", old_value);

    // 查询键值对
    char *value = hashmap_get(map, "key1");
    printf("期待得到value3，实际得到：%s\n", value);  // 预期输出value3
    value = hashmap_get(map, "key2");
    printf("期待得到value2，实际得到：%s\n", value);  // 预期输出value2
    value = hashmap_get(map, "key3");
    if (value == NULL) {
        printf("如预期，key3未找到\n");  // 预期找不到key3
    }

    // 删除键值对
    bool deleted = hashmap_remove(map, "key1");  // 删除key1, 预期删除成功
    if (deleted) {
        printf("key1成功删除。\n");
    }
    deleted = hashmap_remove(map, "key3");  // 尝试删除不存在的key3, 预期删除失败
    if (!deleted) {
        printf("key3未找到且未删除，符合预期。\n");
    }

    // 销毁哈希表
    hashmap_destroy(map);

    return 0;
}
