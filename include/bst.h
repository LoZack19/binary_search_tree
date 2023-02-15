#ifndef BST_H
#define BST_H

#include <include/bst_node.h>

struct bst {
    struct bst_node *fakeroot; // holds +inf item

    void *(*copy)(void *);
    void (*destroy)(void *);
    int (*compare)(void *, void *);
    void (*print)(void *, FILE *);
};

struct bst *bst_create(
    void *inf,
    void *(*copy)(void *),
    void (*destroy)(void *),
    int (*compare)(void *, void *),
    void (*print)(void *, FILE *)
    );
void bst_destroy(struct bst *b);

int bst_insert(struct bst *b, void *item);
int bst_delete(struct bst *b, void *key);

void *bst_min(struct bst *b);
void *bst_max(struct bst *b);
void *bst_successor(struct bst *b, void *key);
void *bst_predecessor(struct bst *b, void *key);

void *bst_search(struct bst *b, void *key);
void *bst_select(struct bst *b, int i);
int bst_partition(struct bst *b, void *key);
int bst_balance(struct bst *b);

void bst_print(struct bst *b, FILE *outfile, enum visit visit);

#endif