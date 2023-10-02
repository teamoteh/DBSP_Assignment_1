#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK_SIZE_MB 100   // Assuming 100MB for simplicity. Change as needed.
#define BLOCK_SIZE 400  
#define NUM_BLOCKS (DISK_SIZE_MB * 1024 * 1024) / BLOCK_SIZE
#define MAX_KEYS (BLOCK_SIZE / sizeof(int) - 1)  // Just a basic calculation. Might need adjustments based on your exact B+ tree node structure

typedef struct Block {
    char data[BLOCK_SIZE];
} Block;

typedef struct {
    int num_keys;
    int keys[MAX_KEYS];
    struct BPTreeNode* children[MAX_KEYS + 1];
} BPTreeNode;

typedef struct {
    char date[11];          // Date in format: YYYY-MM-DD
    int teamID_home;        // Home team ID
    int PTS_home;           // Points by home team
    float FG_PCT_home;      // Field goal percentage by home team
    float FT_PCT_home;      // Free throw percentage by home team
    float FG3_PCT_home;     // 3-point field goal percentage by home team
    int AST_home;           // Assists by home team
    int REB_home;           // Rebounds by home team
    int HOME_TEAM_WINS;     // 1 if home team wins, 0 otherwise
} NBA_Game;

Block disk_storage[NUM_BLOCKS];
BPTreeNode* root = NULL;  // Root of B+ tree

int load_games_from_file(const char* filename, NBA_Game* games) {
    printf("Trying to open file: %s\n", filename);
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open the file");
        return 0;
    }

    char buffer[1024];
    int count = 0;

    // Skipping the header
    printf(fgets(buffer, sizeof(buffer), file));

    while(fgets(buffer, sizeof(buffer), file)){
        sscanf(buffer, "%s %d %d %f %f %f %d %d %d", 
               games[count].date, 
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
    NBA_Game* games = (NBA_Game*) malloc(100000000 * sizeof(NBA_Game)); // Assuming a maximum of 10,000 games, you can adjust this
    if(!games) {
            perror("Memory allocation failed");
            return 1;
        }

    int total_records = load_games_from_file("games.txt", games);

    int record_size = sizeof(NBA_Game);
    int records_per_block = BLOCK_SIZE / record_size;
    int total_blocks = (total_records + records_per_block - 1) / records_per_block;

    printf("Number of records: %d\n", total_records);
    printf("Size of a record: %d bytes\n", record_size);
    printf("Number of records stored in a block: %d\n", records_per_block);
    printf("Number of blocks for storing the data: %d\n", total_blocks);

    free(games);
    return 0;
}
