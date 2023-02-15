#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <include/bst.h>

#define INFCHAR 0xFF
#define INFSTRING "\xff"

void *copy(void *item)
{
    int *item_copy;

    item_copy = malloc(sizeof(*item_copy));
    if (!item_copy)
        return NULL;
    
    memcpy(item_copy, item, sizeof(*item_copy));
    return item_copy;
}

void print(void *item, FILE *outfile)
{
    fprintf(outfile, "%d", *(int *)item);
}

int compare(void *item1, void *item2)
{      
    return *(int *)item1 - *(int *)item2;
}

void destroy(void *item)
{
    free(item);
}

int *addr(int n)
{
    static int res;
    res = n;
    return &res;
}

int main(int argc, char *argv[])
{
    struct bst *bst;
    int n;

    if (argc != 2) {
        printf("Usage: %s <up to>\n", argv[0]);
        return ERROR;
    }

    n = atoi(argv[1]);
    if (n == 0) {
        puts("Invalid number");
        return ERROR;
    }

    bst = bst_create(addr(INT_MAX), copy, destroy, compare, print);
    if (!bst)
        return 1;
    
    for (int i = 1; i <= n; i++)
        bst_insert(bst, &i);
    
    bst_balance(bst);
    bst_print(bst, stdout, PREORDER);

    bst_destroy(bst);
    return 0;
}