#include <include/bst_node.h>
#include <include/bst.h>
#include <stdlib.h>
#include <string.h>

#define ROOT(BST) ((BST)->fakeroot->left)

/**
 * b->fakeroot must be considered infinite in order
 * for the bst to work
 * 
 * compare(inf, inf) = 0
 * compare(inf, whatever) > 0
 * compare(whatever, inf) < 0
 * compare(what, ever) <==> what - ever
 **/

struct bst *bst_create(
    void *inf,
    void *(*copy)(void *),
    void (*destroy)(void *),
    int (*compare)(void *, void *),
    void (*print)(void *, FILE *)
    )
{
    struct bst *b;

    b = malloc(sizeof(*b));
    if (!b)
        return NULL;
    
    b->fakeroot = bst_node_create(NULL, inf, copy, NULL, NULL);
    if (!b->fakeroot) {
        free(b);
        return NULL;
    }

    b->copy = copy;
    b->destroy = destroy;
    b->compare = compare;
    b->print = print;

    return b;
}

void bst_destroy(struct bst *b)
{
    bst_node_destroy(b->fakeroot, b->destroy);
    free(b);
}

int bst_insert(struct bst *b, void *item)
{
    return bst_node_insert(b->fakeroot, item, b->compare, b->copy);
}

int bst_delete(struct bst *b, void *key)
{
    // Don't allow deletion of the fakeroot
    if (b->compare(key, b->fakeroot->item) == 0)
        return ERROR;
    
    return bst_node_delete(b->fakeroot, key, b->destroy, b->compare);
}

void *bst_min(struct bst *b)
{
    if (!ROOT(b))
        return NULL;
    
    return bst_node_min(ROOT(b));
}

void *bst_max(struct bst *b)
{
    if (!ROOT(b))
        return NULL;
    
    return bst_node_max(ROOT(b));
}

void *bst_successor(struct bst *b, void *key)
{
    void *item;

    if (!ROOT(b))
        return NULL;
    
    item = bst_node_successor(ROOT(b), key, b->compare);
    if (b->compare(item, b->fakeroot->item) == 0)
        return NULL;
    
    return item;
}

void *bst_predecessor(struct bst *b, void *key)
{
    if (!ROOT(b))
        return NULL;
    
    return bst_node_predecessor(ROOT(b), key, b->compare);
}

void *bst_search(struct bst *b, void *key)
{
    if (!ROOT(b))
        return NULL;
    
    return bst_node_search(ROOT(b), key, b->compare);
}

void *bst_select(struct bst *b, int i)
{
    if (!ROOT(b))
        return NULL;
    
    return bst_node_select(ROOT(b), i);
}

int bst_partition(struct bst *b, void *key)
{
    if (!ROOT(b))
        return ERROR;
    
    return bst_node_partition(ROOT(b), key, b->compare);
}

int bst_balance(struct bst *b)
{
    if (!ROOT(b))
        return ERROR;
    
    return bst_node_balance(ROOT(b));
}

void bst_print(struct bst *b, FILE *outfile, enum visit visit)
{
    if (!ROOT(b))
        return;
    
    return bst_node_print_tree(ROOT(b), outfile, b->print, visit);
}
