#include "bst.h"
#include <stdio.h>
#include <stdlib.h>

// Insert a free block into BST (sorted by size)
BSTNode* bst_insert(BSTNode *root, Block *blk) {
    if (!root) {
        BSTNode *node = malloc(sizeof(BSTNode));
        node->blk = blk;
        node->left = node->right = NULL;
        return node;
    }
    if (blk->size < root->blk->size)
        root->left = bst_insert(root->left, blk);
    else
        root->right = bst_insert(root->right, blk);
    return root;
}

// Find best fit (smallest free block >= requested size)
Block* bst_find_best_fit(BSTNode *root, size_t size) {
    if (!root) return NULL;
    if (root->blk->size < size)
        return bst_find_best_fit(root->right, size);
    Block *left_fit = bst_find_best_fit(root->left, size);
    return (left_fit && left_fit->size >= size) ? left_fit : root->blk;
}

// Delete a block (by size)
BSTNode* bst_delete(BSTNode *root, size_t size) {
    if (!root) return NULL;
    if (size < root->blk->size)
        root->left = bst_delete(root->left, size);
    else if (size > root->blk->size)
        root->right = bst_delete(root->right, size);
    else {
        if (!root->left) {
            BSTNode *temp = root->right;
            free(root);
            return temp;
        } else if (!root->right) {
            BSTNode *temp = root->left;
            free(root);
            return temp;
        } else {
            BSTNode *temp = root->right;
            while (temp->left)
                temp = temp->left;
            root->blk = temp->blk;
            root->right = bst_delete(root->right, temp->blk->size);
        }
    }
    return root;
}

// Traversal for debugging
void bst_inorder(BSTNode *root) {
    if (!root) return;
    bst_inorder(root->left);
    printf("Free Block: Start=%zu Size=%zu\n", root->blk->start, root->blk->size);
    bst_inorder(root->right);
}

void bst_clear(BSTNode *root) {
    if (!root) return;
    bst_clear(root->left);
    bst_clear(root->right);
    free(root);
}
