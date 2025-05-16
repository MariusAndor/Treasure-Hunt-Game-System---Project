#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "treasure_manager.h"

int main(int argc, char** argv ){

    if(argc < 2)
    {
        printf("Too few arguments\n\nHere is a list of flags:\n-------------\n\t--add <hunt_id>\n\t--list <hunt_id>\n\t--view <hunt_id> <treasure_id>\n\t--remove_treasure <hunt_id> <treasure_id>\n\t--remove_hunt <hunt_id>\n");
        return 0;
    }  

    treasure_t* treasure = NULL;
    operation_t operation;

    if(strcmp(argv[1],"--add")==0){
        
        if(argc != 3){
            perror("Enter a game name\n");
            exit(1);
        }
        if(treasure == NULL){
            if((treasure = (treasure_t*)malloc(sizeof(treasure_t))) == NULL)
            {
                perror("The treasure could not be allocated\n");
                exit(1);
            }
        }

        if(AddTreasure(treasure,argv[2]) == 1){
            addLogs((operation = ADD),argv[2],NULL);
        }
        

    }else if(strcmp(argv[1],"--list")==0){
        if(argc != 3){
            perror("Enter a game name\n");
            exit(1);
        }

        if(List(argv[2]) != -1){
            addLogs((operation = LIST),argv[2],NULL);
        }
        

    }else if(strcmp(argv[1],"--remove_treasure") == 0){
        if(argc != 4){
            perror("Enter a game name and a specific trasure(id)\n");
            exit(1);
        }

        if(RemoveTreasure(argv[2],argv[3]) != -1){
            addLogs((operation = REMOVE_TREASURE),argv[2],argv[3]);
        }
        

    }else if(strcmp(argv[1],"--remove_hunt") == 0){
        if(argc != 3){
            perror("Enter a game name\n");
            exit(1);
        }

        RemoveHunt(argv[2]);

    }else if(strcmp(argv[1],"--view") == 0){
        if(argc != 4){
            perror("Enter a game name and a specific trasure(id)\n");
            exit(1);
        }

        if(ViewHunt(argv[2],argv[3]) == -1){
            return 0;
        }
        addLogs((operation = VIEW),argv[2],argv[3]);
    }else{
        printf("You've entered an unknown command\n");
        printf("Too few arguments\n\nHere is a list of flags:\n-------------\n\t--add <hunt_id>\n\t--list <hunt_id>\n\t--view <hunt_id> <treasure_id>\n\t--remove_treasure <hunt_id> <treasure_id>\n\t--remove_hunt <hunt_id>\n");
    }

    return 0;
}
