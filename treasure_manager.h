#ifndef TREASURE_MANAGER_H
#define TREASURE_MANAGER_H

#define ID_SIZE 32
#define USERNAME_SIZE 64
#define CLUETEXT_SIZE 256
#define PATH_FILE_SIZE 256
#define TwoPATHs_FILE_SIZE 527
#define LOG_SIZE 256
#define BUFF_SIZE 1024

typedef struct gps{
    float x;
    float y;
}gps_t;

typedef struct treasure{
    char id[ID_SIZE];
    char username[USERNAME_SIZE];
    gps_t coordinates;
    int value;
    char clueText[CLUETEXT_SIZE];
}treasure_t;

typedef enum operations{
    ADD,
    LIST,
    VIEW,
    REMOVE_TREASURE
}operation_t;

void printTreasure(treasure_t* );

int appendData(treasure_t* t, const char* directoryName);

int checkIfIDAlreadyExists(char* hunt_id,char* id);

int AddTreasure(treasure_t* t,char* directoryName);

int RemoveTreasure(char* hunt_id,char* treasure_id);

int RemoveHunt(char* hunt_id);

int ViewHunt(char* hunt_id,char* treasure_id);

void List(char* hunt_id);

void addLogs(operation_t operation,char* hunt_id,char* treasure_id);

#endif