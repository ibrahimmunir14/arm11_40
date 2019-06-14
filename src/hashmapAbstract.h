#ifndef HASHMAPABSTRACT_HASHMAPABSTRACT_H
#define HASHMAPABSTRACT_HASHMAPABSTRACT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10 // What is this for?

typedef struct Pair {
    char *key;
    int value;
} pair_t;

typedef struct node {
    pair_t pair;
    struct node *next;
} node_t;

node_t **initHashmap();

int addHashmapEntry(node_t **hashmap, char *key, int value);

int getHashmapValue(node_t **hashmap, char *key);

bool isEmptyHashmap(node_t **hashmap);

int sizeHashmap(node_t **hashmap);

node_t **deleteHashmap(node_t **hashmap, char *key); // returns pointer to head of hashmap (so can delete head)

void display(node_t **hash_table);

#endif //HASHMAPABSTRACT_HASHMAPABSTRACT_H
