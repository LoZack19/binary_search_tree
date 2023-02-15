#ifndef BST_NODE_H
#define BST_NODE_H

#include <stdio.h>

#define ERROR 1
#define SUCCESS 0

enum visit {
    PREORDER = 0,
    INORDER,
    POSTORDER
};

/**
 * Struttura di un nodo :
 *
 *              │
 *     ┌────────┴────────┐
 *     │      father     │
 *     ├─────────────────┤
 *     │      item       │
 *     ├────────┬────────┤
 *     │ left   │ right  │
 *     └───┬────┴────┬───┘
 *         │         │
 **/

struct bst_node {
    struct bst_node *father;
    void *item;
    int descendants;
    struct bst_node *left;
    struct bst_node *right;
};

struct bst_node *bst_node_create(
    struct bst_node *father,
    void *item, void *(*copy)(void *),
    struct bst_node *left, struct bst_node *right
    );
void bst_node_destroy(struct bst_node *n, void (*destroy)(void *));

int bst_node_insert(
    struct bst_node *n,
    void *item,
    int (*compare)(void *, void *),
    void *(*copy)(void *)
    );
int bst_node_delete(
    struct bst_node *n,
    void *key,
    void (*destroy)(void *),
    int (*compare)(void *, void *)
    );

void *bst_node_min(struct bst_node *n);
void *bst_node_max(struct bst_node *n);
void *bst_node_successor(
    struct bst_node *root,
    void *key,
    int compare(void *, void *)
    );
void *bst_node_predecessor(
    struct bst_node *root,
    void *key,
    int compare(void *, void *)
    );

void *bst_node_search(struct bst_node *n, void *key, int (*compare)(void *, void *));
void *bst_node_select(struct bst_node *n, int i);
int bst_node_partition(struct bst_node *n, void *key, int (*compare)(void *, void *));
struct bst_node *bst_node_median_partition(struct bst_node *n);
int bst_node_balance(struct bst_node *n);

void bst_node_print_node(
    struct bst_node *n,
    FILE *outfile,
    void (*print)(void *, FILE *)
    );
void bst_node_print_tree(
    struct bst_node *root,
    FILE *outfile,
    void (*print)(void *, FILE *),
    enum visit visit
    );
#endif