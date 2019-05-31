#ifndef HASHMAPABSTRACT_HASHMAPABSTRACT_H
#define HASHMAPABSTRACT_HASHMAPABSTRACT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10

typedef struct Pair {
    char *key;
    int value;
} pair_t;

typedef struct node {
    pair_t pair;
    struct node *next;
} node_t;

int addHashmapEntry(node_t **hashmap, char *key, int value);

int getHashmap(node_t **hashmap, char *key);

bool isEmptyHashmap(node_t **hashmap);

int sizeHashmap(node_t **hashmap);

node_t **deleteHashmap(node_t **hashmap, char *key); // returns pointer to head of hashmap (so can delete head)

int hash(char *key);

void pushList(node_t *head, pair_t pair);

int getList(node_t *head, const char *key);

void display(node_t **hash_table);

void displayList(node_t *head);

node_t *deleteList(node_t *head, const char *key); // returns pointer to head of list

#endif //HASHMAPABSTRACT_HASHMAPABSTRACT_H
