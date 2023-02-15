#include <include/bst_node.h>
#include <stdlib.h>
#include <string.h>

/* protects against dereference of NULL pointer */
#define DESCENDANTS(NODE)   ((NODE) ? (NODE)->descendants : 0)

/* struct bst_node *n non è mai nullo e ha sempre un padre */

void bst_node_update_descendants_(struct bst_node *n)
{
    while (n) {
        n->descendants = DESCENDANTS(n->left) + DESCENDANTS(n->right) + 1;
        n = n->father;
    }
}

struct bst_node *bst_node_create(
    struct bst_node *father,
    void *item, void *(*copy)(void *),
    struct bst_node *left, struct bst_node *right
    )
{
    struct bst_node *n;

    n = malloc(sizeof(*n));
    if (!n)
        return NULL;

    if (copy) {
        n->item = copy(item);
        if (!n->item) {
            free(n);
            return NULL;
        }
    } else { /* WARNING: Ownership transfer */
        n->item = item;
    }

    n->father = father;
    n->left = left;
    n->right = right;

    n->descendants = 1;
    if (left)
        n->descendants += left->descendants;
    if (right)
        n->descendants += right->descendants;

    return n;
}

/* destroys a single node */
void bst_node_destroy_(struct bst_node *n, void (*destroy)(void *))
{
    destroy(n->item);
    free(n);
}

/* destroys a node with its subtree */
void bst_node_destroy(struct bst_node *n, void (*destroy)(void *))
{
    if (n->left)
        bst_node_destroy(n->left, destroy);
    
    if (n->right)
        bst_node_destroy(n->right, destroy);
    
    bst_node_destroy_(n, destroy);
}

int bst_node_insert(
    struct bst_node *n,
    void *item,
    int (*compare)(void *, void *),
    void *(*copy)(void *)
    )
{
    struct bst_node *father = n->father;
    struct bst_node *new;

    if (n == NULL)
        return ERROR;

    while (n != NULL) {
        father = n;
        if (compare(item, n->item) < 0)
            n = n->left;
        else
            n = n->right;
    }

    new = bst_node_create(father, item, copy, NULL, NULL);
    if (!new)
        return ERROR;

    if (compare(item, father->item) < 0)
        father->left = new;
    else
        father->right = new;
    
    bst_node_update_descendants_(father);

    return SUCCESS;
}

static
struct bst_node *bst_node_search_(
    struct bst_node *n,
    void *key,
    int (*compare)(void *, void *)
    )
{
    while (n) {
        if (compare(key, n->item) == 0)
            return n;
        
        if (compare(key, n->item) < 0)
            n = n->left;
        else
            n = n->right;
    }

    return NULL;
}

void *bst_node_search(
    struct bst_node *n,
    void *key,
    int (*compare)(void *, void *)
    )
{
    struct bst_node *found;

    found = bst_node_search_(n, key, compare);
    if (!found)
        return NULL;
    
    return found->item;
}

/* trapianta successor in n */
static
void bst_node_transplant_(struct bst_node *old, struct bst_node *new)
{
    if (old == old->father->left)
        old->father->left = new;
    else
        old->father->right = new;

    if (new != NULL) { /* da verificare */
        new->father = old->father;
        bst_node_update_descendants_(new);
    }
}

static
struct bst_node *bst_node_min_(struct bst_node *n)
{
    struct bst_node *father = NULL;

    while (n) {
        father = n;
        n = n->left;
    }

    return father;
}

void *bst_node_min(struct bst_node *n)
{
    struct bst_node *min;

    min = bst_node_min_(n);
    if (!min)
        return NULL;
    
    return min->item;
}

static
struct bst_node *bst_node_max_(struct bst_node *n)
{
    struct bst_node *father = NULL;

    while (n) {
        father = n;
        n = n->right;
    }

    return father;
}

void *bst_node_max(struct bst_node *n)
{
    struct bst_node *max;

    max = bst_node_max_(n);
    if (!max)
        return NULL;
    
    return max->item;
}

void bst_node_delete_(
    struct bst_node *del,
    void (*destroy)(void *)
    )
{
    if (del->left == NULL) {
        bst_node_transplant_(del, del->right);
    } else if (del->right == NULL) {
        bst_node_transplant_(del, del->left);
    } else {
        struct bst_node *successor;

        successor = bst_node_min_(del->right);
        if (successor != del->right) {
            bst_node_transplant_(successor, successor->right);
            /* successor eredita n->right */
            successor->right = del->right;
            successor->right->father = successor;
        }
        bst_node_transplant_(del, successor);
        /* successor eredita n->left */
        successor->left = del->left;
        successor->left->father = successor;
    }

    bst_node_destroy_(del, destroy);
}

int bst_node_delete(
    struct bst_node *n,
    void *key,
    void (*destroy)(void *),
    int (*compare)(void *, void *)
    )
{
    struct bst_node *del;

    del = bst_node_search_(n, key, compare);
    if (!del)
        return ERROR;
    
    bst_node_delete_(del, destroy);
    return SUCCESS;
}

/**
 * IDEA:
 * - se bruciando la sinistra bruci oltre il limite allora
 *   non la brucio e ci vado (e il mio risultato deve essere là)
 *   altrimenti brucio e basta
 * OSS: se la sinistra non esiste è come bruciare zero
 * - poi brucio il nodo corrente
 *   se arrivo a conta zero è lui il mio nodo
 * - altrimenti a questo punto provo a cercare a destra e
 *   ci deve essere per forza
*/
static
struct bst_node *bst_node_select_r_(struct bst_node *n, int *i)
{
    if (*i > n->descendants)
        return NULL;

    if (n->left) {
        if (*i - n->left->descendants > 0) {
            *i -= n->left->descendants;
        } else {
            return bst_node_select_r_(n->left, i);
        }
    }

    if (*i - 1 > 0)
        *i -= 1;
    else
        return n;
    
    return bst_node_select_r_(n->right, i);
}

static
struct bst_node *bst_node_select_(struct bst_node *n, int i)
{
    i++;
    return bst_node_select_r_(n, &i);
}

void *bst_node_select(struct bst_node *n, int i)
{
    struct bst_node *selected;

    selected = bst_node_select_(n, i);
    if (!selected)
        return NULL;
    
    return selected->item;
}

struct bst_node *bst_node_successor_(struct bst_node *n)
{
    struct bst_node *father;

    if (n->right)
        return bst_node_min_(n->right);
    
    father = n->father;
    while (father && n == father->right) {
        n = father;
        father = father->father;
    }

    return father;
}

void *bst_node_successor(
    struct bst_node *root,
    void *key,
    int compare(void *, void *)
    )
{
    struct bst_node *key_node, *successor;

    key_node = bst_node_search_(root, key, compare);
    if (!key_node)
        return NULL;
    
    successor = bst_node_successor_(key_node);
    if (!successor)
        return NULL;
    
    return successor->item;
}


struct bst_node *bst_node_predecessor_(struct bst_node *n)
{
    struct bst_node *father;

    if (n->left)
        return bst_node_max(n->left);
    
    father = n->father;
    while (father && n == father->left) {
        n = father;
        father = father->father;
    }

    return father;
}

void *bst_node_predecessor(
    struct bst_node *root,
    void *key,
    int compare(void *, void *)
    )
{
    struct bst_node *key_node, *predecessor;

    key_node = bst_node_search_(root, key, compare);
    if (!key_node)
        return NULL;
    
    predecessor = bst_node_predecessor_(key_node);
    if (!predecessor)
        return NULL;
    
    return predecessor->item;
}

/*
 * Right rotation:
 * 
 *  ┌───►
 *  │      ┌─┐               ┌─┐
 *  │  ┌───┤B├────┐        ┌─┤A├────┐
 *     │   └─┘    │        │ └─┘    │
 *    ┌┴┐        ┌┴┐      ┌┴┐      ┌┴┐
 *  ┌─┤A├─┐      │c│      │a│    ┌─┤B├──┐
 *  │ └─┘ │      └─┘      └─┘    │ └─┘  │
 * ┌┴┐   ┌┴┐                    ┌┴┐    ┌┴┐
 * │a│   │b│                    │b│    │c│
 * └─┘   └─┘                    └─┘    └─┘
 * 
 * Metaphor:
 * 
 *
 *   ▲                         │
 *   │                         ▼
 *             pivot
 *   ────────────┬──────────────
 *   A           │             B
 *               │
 * light         │           heavy
 *               │
 *               b
 * 
 * I trust the exsistence of A and B. I don't trust
 * the exsistence of a, b, c
 */

static
void bst_node_father_update_(struct bst_node *newson, struct bst_node *oldson)
{
    struct bst_node *father;
    
    father = oldson->father;
    if (father->left == oldson)
        father->left = newson;
    else
        father->right = newson;
    
    newson->father = father;
}

void bst_node_right_rotate(struct bst_node *heavy)
{
    struct bst_node *light, *pivot;

    light = heavy->left;
    pivot = light->right;

    heavy->left = pivot;
    if (heavy->left)
        heavy->left->father = heavy;

    bst_node_father_update_(light, heavy);

    light->right = heavy;
    light->right->father = light;

    bst_node_update_descendants_(heavy);
}

void bst_node_left_rotate(struct bst_node *heavy)
{
    struct bst_node *light, *pivot;

    light = heavy->right;
    pivot = light->left;

    heavy->right = pivot;
    if (heavy->right)
        heavy->right->father = heavy;


    bst_node_father_update_(light, heavy);

    light->left = heavy;
    light->left->father = light;

    bst_node_update_descendants_(heavy);
}

/* do not perform this operation on the root node */
void bst_node_rise(struct bst_node *light)
{
    if (light == light->father->left)
        bst_node_right_rotate(light->father);
    else
        bst_node_left_rotate(light->father);
}

static
void bst_node_partition_(struct bst_node *n, struct bst_node *key)
{
    if (n == key)
        return;
    
    while (n->father != key) {
        bst_node_rise(key);
    }
}

int bst_node_partition(
    struct bst_node *n,
    void *key,
    int (*compare)(void *, void *)
    )
{
    struct bst_node *key_node;

    key_node = bst_node_search_(n, key, compare);
    if (!key_node)
        return ERROR;
    
    bst_node_partition_(n, key);
    return SUCCESS;
}

struct bst_node *bst_node_median_partition(struct bst_node *n)
{
    struct bst_node *median_node;

    median_node = bst_node_select_(n, n->descendants / 2);
    if (!median_node)
        return NULL;
    
    bst_node_partition_(n, median_node);
    return median_node;
}

int bst_node_balance(struct bst_node *n)
{
    int error;

    n = bst_node_median_partition(n);
    if (!n)
        return ERROR;
    
    if (n->left) {
        error = bst_node_balance(n->left);
        if (error)
            return ERROR;
    }
    
    if (n->right) {
        error = bst_node_balance(n->right);
        if (error)
            return ERROR;
    }

    return SUCCESS;
}

void bst_node_print_node(
    struct bst_node *n,
    FILE *outfile,
    void (*print)(void *, FILE *)
    )
{
    fputc(n->father->left == n ? 'L' : 'R', outfile);
    fputs(": {", outfile);
    print(n->item, outfile);
    fputs("}\n", outfile);
}

static
void print_tabs_(int howmany, int tabsize, FILE *outfile)
{
    for (int i = 0; i < howmany; i++)
        for (int j = 0; j < tabsize; j++)
            fputc(' ', outfile);
}

void bst_node_print_tree_(
    struct bst_node *n,
    FILE *outfile,
    void (*print)(void *, FILE *),
    enum visit visit,
    int depth
    )
{
    static const int TABSIZE = 4;

    if (n == NULL)
        return;
    
    if (visit == PREORDER) {
        print_tabs_(depth, TABSIZE, outfile);
        bst_node_print_node(n, outfile, print);
    }

    bst_node_print_tree_(n->left, outfile, print, visit, depth+1);

    if (visit == INORDER) {
        print_tabs_(depth, TABSIZE, outfile);
        bst_node_print_node(n, outfile, print);
    }

    bst_node_print_tree_(n->right, outfile, print, visit, depth+1);

    if (visit == POSTORDER) {
        print_tabs_(depth, TABSIZE, outfile);
        bst_node_print_node(n, outfile, print);
    }

}

void bst_node_print_tree(
    struct bst_node *root,
    FILE *outfile,
    void (*print)(void *, FILE *),
    enum visit visit
    )
{
    bst_node_print_tree_(root, outfile, print, visit, 0);
}
