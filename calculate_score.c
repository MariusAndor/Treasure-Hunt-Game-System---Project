#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <string.h>

#include <signal.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "treasure_manager.h"

#define CHUNK 10

typedef struct username_value_list{
    char** usernameList;
    int* valueList;
    unsigned int size;
    unsigned int index;
}username_value_list_t;

username_value_list_t* init_uVList(username_value_list_t* uVList){
    if((uVList = malloc(sizeof(username_value_list_t))) == NULL){
        perror("Error at allocating the list\n");
        exit(1);
    }

    uVList->size = 0;
    uVList->index = 0;
    uVList->usernameList = NULL;
    uVList->valueList = NULL;

    return uVList;
}

void createOrExpandList(username_value_list_t* uVList,unsigned int counter){
    
    if(counter == uVList->size){
        if(uVList->usernameList == NULL){
            uVList->usernameList = malloc(sizeof(char* )*CHUNK);
            uVList->valueList = malloc(sizeof(int)*CHUNK);
            if(uVList->usernameList == NULL || uVList->valueList == NULL){
                perror("Error at alocating memory for the lists  --calculate()\n");
                exit(1);
            }
            uVList->size = CHUNK;
            for(int i=0; i<uVList->size; i++){
                uVList->usernameList[i] = malloc(sizeof(char)*USERNAME_SIZE);
                if(uVList->usernameList[i] == NULL){
                    perror("Error at allocating a bloc in usernameList \n");
                    exit(1);
                }
            }

            
        }else{
            uVList->usernameList = realloc(uVList->usernameList,sizeof(char*)*(uVList->size*CHUNK));
            uVList->valueList = realloc(uVList->valueList,sizeof(int)*(uVList->size+CHUNK));
            if(uVList->usernameList == NULL || uVList->valueList == NULL){
                perror("Error at reallocating memory for the lists  --calculate()\n");
                exit(1);
            }
            
            for(unsigned int i = uVList->size; i<uVList->size+CHUNK;i++){
                uVList->usernameList[i] = malloc(sizeof(char)*USERNAME_SIZE);
                if(uVList->usernameList[i] == NULL){
                    perror("Error at allocating a bloc in usernameList \n");
                    exit(1);
                }
            }

            uVList->size += CHUNK;
        }
    }

}

void printUsernameValueList(username_value_list_t* list){
    if(list->index == 0){
        printf("   There are no treasures in this hunt\n\n");
        return ;
    }
    
    for(int i=0; i<list->index; i++){
        printf("   Username: %s\n",list->usernameList[i]);
        printf("   Score: %d\n",list->valueList[i]);
    }
    printf("\n");
}

int checkIfIsInList(char* name,username_value_list_t* list){

    for(int i = 0; i<list->index; i++){
        if(strcmp(name,list->usernameList[i]) == 0){
            return i;
        }
    }

    return -1;
}

username_value_list_t* calculate(char* hunt_id){
    char hunt_path[TwoPATHs_FILE_SIZE];
    sprintf(hunt_path,"%s/%s_treasures.dat",hunt_id,hunt_id);

    int hunt_fd = open(hunt_path,O_RDONLY, 0777);
    if(hunt_fd == -1){
        perror("The file could not be opened  --calculate()\n");
        exit(1);
    }


    treasure_t t;
    username_value_list_t* uVList = NULL;
    uVList = init_uVList(uVList);

    unsigned int counter = 0;
    int find = 0;

    while((read(hunt_fd,&t,sizeof(t))) > 0){
        
        // Memory Adjustments
        createOrExpandList(uVList,counter);

        // Add or Sum the values of an username
        if((find = checkIfIsInList(t.username,uVList)) >= 0){
            // FOUND
            uVList->valueList[find] += t.value;
        }else{
            // NOT FOUND
            strcpy(uVList->usernameList[uVList->index],t.username);
            uVList->valueList[uVList->index] = t.value;
            uVList->index++;
        }

        counter++;
    }

    return uVList;    
}

int main(int argc, char** argv){
    if(argc < 2){
        printf("Few arguments:\n   Enter a <hunt_id>\n");
        return 0;
    }

    username_value_list_t* list = NULL;
    //calculate
    list = calculate(argv[1]);
    //print
    if(list == NULL){
        return -1;
    }
    printUsernameValueList(list);

    return 0;
}