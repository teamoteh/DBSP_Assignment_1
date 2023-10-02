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

void delete_key(int key) {
    if (root == NULL) {
        printf("Tree is empty!\n");
        return;
    }
    delete_recursive(root, key);
    // If root node has 0 keys, make its first child the new root
    if (root->num_keys == 0) {
        BPTreeNode* tmp = root;
        if (root->children[0]) {
            root = root->children[0];
        } else {
            root = NULL;
        }
        free(tmp);
    }
}

void delete_recursive(BPTreeNode* node, int key) {
    int idx = 0;

    // Find the index of the key or the child we should descend to
    while (idx < node->num_keys && node->keys[idx] < key) {
        idx++;
    }

    // Key is present in this node
    if (idx < node->num_keys && node->keys[idx] == key) {
        if (node->children[idx] != NULL) {
            // Key is in an internal node
            int pred = get_predecessor(node, idx);
            node->keys[idx] = pred;
            delete_recursive(node->children[idx], pred);
        } else {
            // Key is in a leaf node
            for (int i = idx; i < node->num_keys - 1; i++) {
                node->keys[i] = node->keys[i+1];
            }
            node->num_keys--;
        }
    } else {
        // Key might be in some child
        if (node->children[idx] == NULL) {
            printf("The key %d is not present in the tree.\n", key);
            return;
        }
        delete_recursive(node->children[idx], key);
    }

    if (node->children[idx]->num_keys < (MAX_KEYS - 1) / 2) {
        if (idx != 0 && node->children[idx - 1]->num_keys > (MAX_KEYS - 1) / 2) {
            borrow_from_prev(node, idx);
        } else if (idx != node->num_keys && node->children[idx + 1]->num_keys > (MAX_KEYS - 1) / 2) {
            borrow_from_next(node, idx);
        } else {
            if (idx != node->num_keys) {
                merge_nodes(node, idx);
            } else {
                merge_nodes(node, idx - 1);
            }
        }
    }
}

int get_predecessor(BPTreeNode* node, int idx) {
    // Move to the rightmost key of the child preceding idx
    BPTreeNode* cur = node->children[idx];
    while (cur->children[0] != NULL) {
        cur = cur->children[cur->num_keys];
    }
    // Return the last key
    return cur->keys[cur->num_keys - 1];
}

void borrow_from_prev(BPTreeNode* node, int idx) {
    BPTreeNode* child = node->children[idx];
    BPTreeNode* sibling = node->children[idx - 1];

    for (int i = child->num_keys - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
    }

    if (!child->children[0]) {
        for (int i = child->num_keys; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
        child->children[0] = sibling->children[sibling->num_keys];
    }

    child->keys[0] = node->keys[idx - 1];
    node->keys[idx - 1] = sibling->keys[sibling->num_keys - 1];
    child->num_keys += 1;
    sibling->num_keys -= 1;
}

void borrow_from_next(BPTreeNode* node, int idx) {
    BPTreeNode* child = node->children[idx];
    BPTreeNode* sibling = node->children[idx + 1];

    child->keys[child->num_keys] = node->keys[idx];
    node->keys[idx] = sibling->keys[0];

    if (!child->children[0]) {
        child->children[child->num_keys + 1] = sibling->children[0];
    }

    for (int i = 1; i < sibling->num_keys; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }

    if (!sibling->children[0]) {
        for (int i = 1; i <= sibling->num_keys; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    child->num_keys += 1;
    sibling->num_keys -= 1;
}

void merge_nodes(BPTreeNode* node, int idx) {
    BPTreeNode* child = node->children[idx];
    BPTreeNode* sibling = node->children[idx + 1];

    child->keys[child->num_keys] = node->keys[idx];

    for (int i = 0; i < sibling->num_keys; ++i) {
        child->keys[i + child->num_keys + 1] = sibling->keys[i];
    }

    if (!child->children[0]) {
        for (int i = 0; i <= sibling->num_keys; ++i) {
            child->children[i + child->num_keys + 1] = sibling->children[i];
        }
    }

    for (int i = idx + 1; i < node->num_keys; ++i) {
        node->keys[i - 1] = node->keys[i];
        node->children[i] = node->children[i + 1];
    }

    child->num_keys += sibling->num_keys + 1;
    node->num_keys--;

    free(sibling);
}

int count_nodes(BPTreeNode* node) {
    if (!node) return 0;
    int count = 1; // Counting the current node
    if (node->children[0]) {
        for (int i = 0; i <= node->num_keys; i++) {
            count += count_nodes(node->children[i]);
        }
    }
    return count;
}

// This function calculates the depth/levels of a B+ tree
int count_levels(BPTreeNode* node) {
    if (!node) return 0;
    // If it's a leaf node, just return 1 (its own level)
    if (!node->children[0]) return 1;
    // Otherwise, return the depth of the first child + 1 (for the current level)
    return 1 + count_levels(node->children[0]);
}