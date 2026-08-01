#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "hashmap.h"

void test() {
	HashMap *map = hashmap_create();
	hashmap_put(map, "yi", "1");
	hashmap_put(map, "er", "2");
	hashmap_put(map, "sam", "3");
	hashmap_put(map, "si", "4");
	printf("%s\n", hashmap_put(map, "yi", "2"));
	printf("%s\n", hashmap_get(map, "si"));
	printf("%d\n", hashmap_remove(map, "yi"));
	printf("%s\n", hashmap_get(map, "yi"));
	hashmap_destroy(map);
}

int main() {
	test();
	return 0;
}