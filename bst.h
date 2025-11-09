#ifndef BST_H
#define BST_H

#include "heap.h"

typedef struct BSTNode {
    Block *blk;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

BSTNode* bst_insert(BSTNode *root, Block *blk);
BSTNode* bst_delete(BSTNode *root, size_t size);
Block* bst_find_best_fit(BSTNode *root, size_t size);
void bst_inorder(BSTNode *root);
void bst_clear(BSTNode *root);

#endif
