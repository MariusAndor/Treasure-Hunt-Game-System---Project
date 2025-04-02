#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

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
    gps_t coordinates;
    int value;
    char clueText[CLUETEXT_SIZE];
}treasure_t;


void printTreasure(treasure_t* t){

    printf("Treasure ID: %d\n",t->id);
    printf("Username: %s\n",t->username);
    printf("GPS Coordinates\n -x: %.2f \n -y: %.2f\n",t->coordinates.x,t->coordinates.y);
    printf("Value: %d\n",t->value);
    printf("Clue Text: %s\n",t->clueText);
    
}

void List(char* huntId){
    FILE* file = NULL;

    char path[PATH_FILE_SIZE];
    snprintf(path, sizeof(path), "%s/%s_treasures.dat", huntId,huntId);
    
    if((file = fopen(path,"rb") ) == NULL){
        perror("When trying to list the hunts, the files could not be opened or could not be found\n");
        exit(1);
    }

    treasure_t* t = malloc(sizeof(treasure_t));
    if(t == NULL){
        perror("Not enough space to allocate for treasure_t -- List()\n");
    }


    printf("\nThe Hunt Name: %s",huntId);

    struct stat fileStat;

    if(stat(path,&fileStat) == -1){
        perror("Could not obtain any informations about the files");
        exit(1);
    }

    printf("\nFile Size: %lld bytes\n", (long long) fileStat.st_size);

    // Afișează ultima modificare a fișierului
    printf("Last modification time: %s\n", ctime(&fileStat.st_mtime));


    while(fread(t,sizeof(treasure_t),1,file)){
        printTreasure(t);
    }


}

/* === IN CASE I NEED A HASH MAP FOR ID ===
unsigned int hashId(const char *str ){
    unsigned int hash = 0;
    while(*str){
        hash = (hash * 31) + *str;
        str++;
    }

    return hash % 1000;
}
*/

void appendData(treasure_t* t, const char* directoryName){

    char treasures_path[PATH_FILE_SIZE];
    snprintf(treasures_path, sizeof(treasures_path), "%s/%s_treasures.dat", directoryName, directoryName);

    FILE* treasures_file = NULL;
    if((treasures_file = fopen(treasures_path,"ab+")) == NULL)
    {
        perror("Error at opening the treasures file");
        exit(1);
    }

    fseek(treasures_file, 0, SEEK_END);

    fwrite(t,sizeof(treasure_t),1,treasures_file);

    fclose(treasures_file);

}

int AddTreasure(treasure_t* t,char* directoryName){
    
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
    getchar();

    printf("Clue Text: \n");
    fgets(t->clueText,sizeof(t->clueText),stdin);

    
    if(strlen(directoryName) > PATH_FILE_SIZE-1)
    {
        perror("The game name length should be less than 256 characters\n");
        exit(1);
    }

    // === CREAT DIRECTORY OR APPEND THE TREASURE TO EXISTING TREASURES ===

    if(mkdir(directoryName) != 0){
        // The Data needs to be appended to the existing files
        appendData(t,directoryName);
        return 1;
    }


    // === CREATING FILES ===
    char treasures_path[PATH_FILE_SIZE];
    char logged_hunt_path[PATH_FILE_SIZE];
    char main_logged_hunt_path[PATH_FILE_SIZE];

    snprintf(treasures_path, sizeof(treasures_path), "%s/%s_treasures.dat", directoryName, directoryName);
    snprintf(logged_hunt_path, sizeof(logged_hunt_path), "%s/logged_hunt.dat", directoryName);
    snprintf(main_logged_hunt_path, sizeof(main_logged_hunt_path),"logged_hunt-%u.dat", t->id);

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

    if(CreateSymbolicLink(main_logged_hunt_path,logged_hunt_path,0) != 0)
    {
        perror("Symbolic link could not be created\n");
        exit(1);
    }

    fclose(treasures_file);
    fclose(logged_hunt_file);
    fclose(main_logged_hunt_file);

    return 1;
}


int main(int argc, char** argv ){
    
    if(argc < 2)
    {
        perror("Too few arguments\n\nHere is a list of flags:\n-------------\n\t--add <game name>\n\t--list\n\t--view\n\t--remove_treasure\n\t--remove_hunt\n");
        exit(1);
    }

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



        AddTreasure(treasure,argv[2]);

    }else if(strcmp(argv[1],"--list")==0){
        if(argc != 3){
            perror("Enter a game name\n");
            exit(1);
        }

        List(argv[2]);

    }else{

    }

    return 0;
}