#include "hashmapAbstract.h"


int addHashmap(node_t **hashmap, pair_t pair) {
    int hashedKey = hash(pair.key) % SIZE;
    if (hashmap[hashedKey] == NULL) {
        hashmap[hashedKey] = malloc(sizeof(node_t));
        hashmap[hashedKey]->pair = pair;
        hashmap[hashedKey]->next = NULL;
    } else {
        pushList(hashmap[hashedKey], pair);
    }
    return 0;
}

int getHashmap(node_t **hashmap, char *key) {
    int hashedKey = hash(key);
    return getList(hashmap[hashedKey], key);
}

bool isEmptyHashmap(node_t **hashmap) {
    for (int i = 0; i < SIZE; i++) {
        if (hashmap[i] != NULL) {
            return false;
        }
    }
    return true;
}

node_t **deleteHashmap(node_t **hashmap, char *key) {
    int hashedKey = hash(key) % SIZE;
    hashmap[hashedKey] = deleteList(hashmap[hashedKey], key);
    return hashmap;
}

int sizeHashmap(node_t **hashmap) {
    return SIZE;
}

void pushList(node_t *head, pair_t pair) {
    node_t *curr = head;
    node_t *newNode = NULL;
    newNode = malloc(sizeof(node_t));
    newNode->pair = pair;

    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = newNode;
    curr->next->next = NULL;
}

int getList(node_t *head, const char *key) {
    node_t *curr = head;
    while (curr->pair.key != key && curr->next != NULL) {
        curr = curr->next;
    }
    return curr->pair.value;
}

node_t *deleteList(node_t *head, const char *key) { //returns pointer to head of list
    node_t *curr = head;
    node_t *prev = head;

    if (head->pair.key == key) {
        return head->next;
    }
    while (curr->pair.key != key && curr != NULL) {
        prev = curr;
        curr = curr->next;
    }
    if (curr != NULL) {
        prev->next = curr->next;
    }
    return head;
}

void display(node_t **hash_table) {
    for (int i = 0; i < SIZE; i++) {
        printf("Hashmap[%d]\t\t", i);
        displayList(hash_table[i]);
        printf("\n");
    }
}

void displayList(node_t *head) {
    int count = 0;
    while (head != NULL) {
        printf("Node[%d]:\t(value) %d\t\t(key) %s\t\t", count, head->pair.value, head->pair.key);
        count++;
        head = head->next;
    }
    printf("\n");
}

int hash(char *key) {
    unsigned long hash = 5381;
    int c;
    while (c = *key++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}