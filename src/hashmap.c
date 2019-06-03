#include "hashmapAbstract.h"

//hashes key to get hash code
static int hash(const char *key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

/* Internal Linked List Operations */
//push pair to back of linked list
static void pushList(node_t *head, pair_t pair) {
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

//get value for given key from linked list
static int getList(node_t *head, const char *key) {
    node_t *curr = head;
    while (curr->pair.key != key && curr->next != NULL) {
        curr = curr->next;
    }
    return curr->pair.value;
}

//delete item with given key from linked list
static node_t *deleteList(node_t *head, const char *key) { //returns pointer to head of list
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

//display linked list
static void displayList(node_t *head) {
    int count = 0;
    while (head != NULL) {
        printf("Node[%d]:    (value) %d\t(key) %s\t", count, head->pair.value, head->pair.key);
        count++;
        head = head->next;
    }
}


//returns a pointer to empty hashmap of size SIZE
node_t **initHashmap() {
    int hashmapSize = SIZE * sizeof(node_t *);
    node_t **hash_table = malloc(hashmapSize);
    for (int i = 0; i < SIZE; i++) {
        hash_table[i] = NULL;
    }
    return hash_table;
}

//add key, value pair to hashmap
int addHashmapEntry(node_t **hashmap, char *key, int value) {
    pair_t pair = {key, value};
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

//get value from hashmap for given key
int getHashmapValue(node_t **hashmap, char *key) {
    int hashedKey = hash(key) % SIZE;
    return getList(hashmap[hashedKey], key);
}

//check if hashmap is empty
bool isEmptyHashmap(node_t **hashmap) {
    for (int i = 0; i < SIZE; i++) {
        if (hashmap[i] != NULL) {
            return false;
        }
    }
    return true;
}

//delete element with given key from hashmap
node_t **deleteHashmap(node_t **hashmap, char *key) {
    int hashedKey = hash(key) % SIZE;
    hashmap[hashedKey] = deleteList(hashmap[hashedKey], key);
    return hashmap;
}

//get size of hashmap
int sizeHashmap(node_t **hashmap) {
    return SIZE;
}

//display hashmap
void display(node_t **hash_table) {
    for (int i = 0; i < SIZE; i++) {
        printf("Hashmap[%d]\t\t", i);
        displayList(hash_table[i]);
        printf("\n");
    }
}