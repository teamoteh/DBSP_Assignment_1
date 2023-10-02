#ifndef BPTREE2_H
#define BPTREE2_H
#define BLOCK_SIZE 400 
#include <stdbool.h>
#include <stdio.h>

typedef struct node {
  void **pointers;
  int *keys;
  struct node *parent;
  bool is_leaf;
  int num_keys;
  struct node *next;
} node;

typedef struct record{
    char day;
    char month;
    short int year;
    int teamID_home;        // Home team ID
    short int PTS_home;           // Points by home team
    float FG_PCT_home;      // Field goal percentage by home team
    float FT_PCT_home;      // Free throw percentage by home team
    float FG3_PCT_home;     // 3-point field goal percentage by home team
    char AST_home;           // Assists by home team
    char REB_home;           // Rebounds by home team
    char HOME_TEAM_WINS;     // 1 if home team wins, 0 otherwise
} Record;

void enqueue(node *new_node);
node *dequeue(void);
int height(node *const root);
int pathToLeaves(node *const root, node *child);
void printLeaves(node *const root);
void printTree(node *const root);
void findAndPrint(node *const root, int key);
void findAndPrintRange(node *const root, int range1, int range2);
int findRange(node *const root, int key_start, int key_end,
int returned_keys[], void *returned_pointers[]);
node *findLeaf(node *const root, int key);
Record *find(node *root, int key, node **leaf_out);
int cut(int length);


node *makeNode(void);
node *makeLeaf(void);
int getLeftIndex(node *parent, node *left);
node *insertIntoLeaf(node *leaf, int key, Record *pointer);
node *insertIntoLeafAfterSplitting(node *root, node *leaf, int key,
                   Record *pointer);
node *insertIntoNode(node *root, node *parent,
           int left_index, int key, node *right);
node *insertIntoNodeAfterSplitting(node *root, node *parent,
                   int left_index,
                   int key, node *right);
node *insertIntoParent(node *root, node *left, int key, node *right);
node *insertIntoNewRoot(node *left, int key, node *right);
node *startNewTree(int key, Record *pointer);
node *insert(node *root, int key);

#endif