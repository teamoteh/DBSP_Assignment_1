#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bptree2.h"
#include "bptree2.c"

#define DISK_SIZE 100   // Assuming 100MB for simplicity. Change as needed.
#define BLOCK_SIZE 400  
#define NUM_BLOCKS (DISK_SIZE * 1024 * 1024) / BLOCK_SIZE

typedef struct Block {
    char data[BLOCK_SIZE];
} Block;

// typedef struct record{
//     char day;
//     char month;
//     short int year;
//     int teamID_home;        // Home team ID
//     short int PTS_home;           // Points by home team
//     float FG_PCT_home;      // Field goal percentage by home team
//     float FT_PCT_home;      // Free throw percentage by home team
//     float FG3_PCT_home;     // 3-point field goal percentage by home team
//     char AST_home;           // Assists by home team
//     char REB_home;           // Rebounds by home team
//     char HOME_TEAM_WINS;     // 1 if home team wins, 0 otherwise
// } Record;

Block disk_storage[NUM_BLOCKS];

int load_games_from_file(const char* filename, Record* games) {
    printf("Trying to open file: %s\n", filename);
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open the file");
        return 0;
    }

    char buffer[1024];
    int count = 0;

    // remove the header input
    fgets(buffer, sizeof(buffer), file);

    while(fgets(buffer, sizeof(buffer), file)){
        sscanf(buffer, "%d/%d/%d %d %hd %f %f %f %hd %hd %hd", 
               &games[count].day,
                &games[count].month,
                &games[count].year,
               &games[count].teamID_home, 
               &games[count].PTS_home, 
               &games[count].FG_PCT_home,
               &games[count].FT_PCT_home,
               &games[count].FG3_PCT_home,
               &games[count].AST_home,
               &games[count].REB_home,
               &games[count].HOME_TEAM_WINS);
        count++;
    }
    fclose(file);
    return count;
}


int main() {
    printf("Loading Memory\n");
    printf("Disk Size : \t\t %d MB\n", DISK_SIZE);
    printf("Block Size : \t\t %d BYTES\n", BLOCK_SIZE);
    printf("Blocks per Disk : \t %d\n", NUM_BLOCKS);

    Record* games = (Record*) malloc(26651 * sizeof(Record));
    if(!games) {
            perror("Memory allocation failed");
            return 1;
        }
    int total_records = load_games_from_file("games.txt", games);
    
    printf("===================================================\n\n");
    
    //Begin Experiment 1
    printf("Experiment 1: \n");
    int record_size = sizeof(Record);
    int records_per_block = BLOCK_SIZE / record_size;
    int total_blocks = (total_records + records_per_block - 1) / records_per_block;

    printf("Number of records: %d\n", total_records);
    printf("Size of a record: %d bytes\n", record_size);
    printf("Number of records stored in a block: %d\n", records_per_block);
    printf("Number of blocks for storing the data: %d\n\n", total_blocks);
    printf("===================================================\n\n");

    //Begin Experiment 2
    printf("Experiment 2: \n");
    printf("Size of FG_PCT_home field: %d bytes\n", sizeof(games->FG_PCT_home));
    char *ptr = &games->FG_PCT_home;
    printf("Size of FG_PCT_home field ptr: %d bytes\n", sizeof(ptr));
    int param_n = 0;

    while ((param_n * sizeof(games->FG_PCT_home) + (param_n + 1) * sizeof(ptr)) <= BLOCK_SIZE) {
        param_n++;
    }
    param_n--;
     printf("The parameter n of the B+ tree: %d \n", param_n);
    int max_keys = param_n + 1; 

    node *root;
    root = NULL;
    char instruction;

    // for (int i = 0; i < total_records; i++){
    //     root = insert(root, games->FG_PCT_home[i], games[i]);
    // }
    root = insert(root, 5);
    root = insert(root, 45);
    root = insert(root, 25);
    root = insert(root, 35);
    root = insert(root, 15);

    printTree(root);
    findAndPrint(root, 15);
    // printf("Number of records: %d\n", MAX_KEYS);
    // for (int i = 0; i < total_records; i++) {
    //         insert(games[i].FG_PCT_home);
    //     }
    free(games);
    return 0;
}
