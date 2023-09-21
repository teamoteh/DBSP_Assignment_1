#include "bptree.h"
#include <stdlib.h>
#include <string.h>

BPTreeNode* root = NULL;  // Root of B+ tree

BPTreeNode* createNode() {
    BPTreeNode* newNode = (BPTreeNode*)malloc(sizeof(BPTreeNode));
    newNode->num_keys = 0;
    for (int i = 0; i < MAX_KEYS + 1; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

void insert(int key) {
    if (root == NULL) {
        root = createNode();
        root->keys[0] = key;
        root->num_keys = 1;
    } else {
        // More complex insertion with splitting etc. 
        BPTreeNode* child = insert_recursive(root, key);
        if (child != NULL) {
            // If child node is returned, it means the root was split
            BPTreeNode* newRoot = createNode();
            newRoot->keys[0] = root->keys[root->num_keys - 1];
            newRoot->children[0] = root;
            newRoot->children[1] = child;
            newRoot->num_keys = 1;
            root = newRoot;
        }
    }
}

BPTreeNode* insert_recursive(BPTreeNode* node, int key) {
    // If leaf node
    if (node->children[0] == NULL) {
        // Insert key at appropriate position
        int i;
        for (i = node->num_keys - 1; i >= 0 && key < node->keys[i]; i--) {
            node->keys[i + 1] = node->keys[i];
        }
        node->keys[i + 1] = key;
        node->num_keys++;
        
        // Check for node split
        if (node->num_keys == MAX_KEYS) {
            return split(node);
        }
        return NULL;
    } 
    // If internal node
    else {
        // Find child to proceed with
        int i;
        for (i = 0; i < node->num_keys && key > node->keys[i]; i++);

        // Recurse to child
        BPTreeNode* child = insert_recursive(node->children[i], key);
        
        // If child was split
        if (child != NULL) {
            // Insert the child in the current node
            for (int j = node->num_keys; j > i; j--) {
                node->keys[j] = node->keys[j - 1];
                node->children[j + 1] = node->children[j];
            }
            node->keys[i] = child->keys[child->num_keys - 1];
            node->children[i + 1] = child;
            node->num_keys++;
            
            if (node->num_keys == MAX_KEYS) {
                return split(node);
            }
        }
        return NULL;
    }
}

BPTreeNode* search(int key) {
    return search_recursive(root, key);
}

BPTreeNode* search_recursive(BPTreeNode* node, int key) {
    if (!node) return NULL;

    int i;
    for (i = 0; i < node->num_keys && key > node->keys[i]; i++);

    if (i < node->num_keys && key == node->keys[i]) {
        return node; // Key found in the current node
    } else if (node->children[i]) {
        return search_recursive(node->children[i], key); // Search in the corresponding subtree
    } else {
        return NULL; // Key not found and we're at a leaf node
    }
}