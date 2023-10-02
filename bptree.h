#ifndef BPTREE_H
#define BPTREE_H
#define BLOCK_SIZE 400  
#define MAX_KEYS (BLOCK_SIZE / sizeof(int) - 1)  // This assumes the inclusion of BLOCK_SIZE

typedef struct BPTreeNode {
    int num_keys;
    int keys[MAX_KEYS];
    struct BPTreeNode* children[MAX_KEYS + 1];
} BPTreeNode;

BPTreeNode* createNode();
void insert(int key);
BPTreeNode* search(int key);
void delete_key(int key);
int count_nodes(BPTreeNode* node);
int count_levels(BPTreeNode* node);

// Additional function declarations if required, e.g., delete.

#endif // BPTREE_H
