#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#include <sys/stat.h>
#include <sys/types.h>


#define USERNAME_SIZE 50
#define CLUETEXT_SIZE 250
#define PATH_FILE_SIZE 256


typedef struct gps{
    float x;
    float y;
}gps_t;

typedef struct treasure{
    unsigned int id;
    char username[USERNAME_SIZE];
    char clueText[CLUETEXT_SIZE];
    gps_t coordinates;
    int value;
}treasure_t;


void ListTreasure(treasure_t* t){

    printf("Treasure ID: %d\n",t->id);
    printf("Username: %s\n",t->username);
    printf("GPS Coordinates\n -x: %f \n -y: %f\n",t->coordinates.x,t->coordinates.y);
    printf("Clue Text: %s\n",t->clueText);
    printf("Value: %d\n\n",t->value);
    
}

void printFromFile(){
    FILE* file = NULL;

    if((file = fopen("game1/game1_treasures.dat","ab+")) == NULL)
    {
        perror("Error at opening the treasures file");
        exit(1);
    }

    treasure_t* t =malloc(sizeof(treasure_t));
    fread(t,sizeof(treasure_t),1,file);

    ListTreasure(t);

    fclose(file);
}

treasure_t* AddTreasure(treasure_t* t,char* directoryName){
    
    // === READ DATA ===
    printf("Treasure ID: ");
    scanf("%u",&t->id);

    printf("Username: ");
    scanf("%50s",t->username);

    printf("GPS Coordinates\n -x: ");
    scanf("%f",&t->coordinates.x);
    printf(" -y: ");
    scanf("%f",&t->coordinates.y);

    printf("Value: ");
    scanf("%d",&t->value);

    printf("Clue Text: \n");
    scanf("%250s",t->clueText);

    
    if(strlen(directoryName) > PATH_FILE_SIZE-1)
    {
        perror("The game name length should be less than 256 characters\n");
        exit(1);
    }

    // === CREAT DIRECTORY ===
    if(mkdir(directoryName) != 0)
    {
        fprintf(stderr, "The directory %s either exists or could not be created \n",directoryName);
        exit(1);
    }

    // === CREATING FILES ===
    char treasures_path[PATH_FILE_SIZE];
    char logged_hunt_path[PATH_FILE_SIZE];
    char main_logged_hunt_path[PATH_FILE_SIZE];

    snprintf(treasures_path, sizeof(treasures_path), "%s/%s_treasures.dat", directoryName, directoryName);
    snprintf(logged_hunt_path, sizeof(logged_hunt_path), "%s/logged_hunt.dat", directoryName);
    snprintf(main_logged_hunt_path, sizeof(main_logged_hunt_path),"logged_hunt_%s.dat", directoryName);

    FILE* treasures_file = NULL;
    FILE* logged_hunt_file = NULL;
    FILE* main_logged_hunt_file = NULL;

    if((treasures_file = fopen(treasures_path,"ab+")) == NULL)
    {
        perror("Error at opening the treasures file");
        exit(1);
    }
    if((logged_hunt_file = fopen(logged_hunt_path,"ab+")) == NULL)
    {
        perror("Error at opening the logged file");
        exit(1);
    }
    if((main_logged_hunt_file = fopen(main_logged_hunt_path,"ab+")) == NULL)
    {
        perror("Error at opening the main logged file");
        exit(1);
    }


    // === WRITING VALUES IN FILES ===

    fwrite(t,sizeof(treasure_t),1,treasures_file);

    // === SYMBOLIC LINK ===
    // TO DO - SymLink not created
    if(CreateSymbolicLink(logged_hunt_path,main_logged_hunt_path,0) == 0)
    {
        perror("Symbolic link could not be created\n");
        exit(1);
    }



    fclose(treasures_file);
    fclose(logged_hunt_file);
    fclose(main_logged_hunt_file);

    return t;
}


int main(int argc, char** argv ){
    
    if(argc < 2)
    {
        perror("Too few arguments\n\nHere is a list of flags:\n-------------\n\t--add <game name>\n\t--list\n\t--view\n\t--remove_treasure\n\t--remove_hunt\n");
        exit(1);
    }

            //Q: Vrea array sau o variabila?
    treasure_t* treasure = NULL;

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



        treasure = AddTreasure(treasure,argv[2]);

    }else if(strcmp(argv[1],"--list")==0){

    }else{

    }

    return 0;
}