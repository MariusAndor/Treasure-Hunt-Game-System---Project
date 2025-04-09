#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>      

#include <unistd.h>
#include <dirent.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#define USERNAME_SIZE 50
#define CLUETEXT_SIZE 250
#define PATH_FILE_SIZE 256
#define TwoPATHs_FILE_SIZE 527
#define LOG_SIZE 256
#define BUFF_SIZE 1024

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

typedef enum operations{
    ADD,
    LIST,
    VIEW,
    REMOVE_TREASURE
}operation_t;


void printTreasure(treasure_t* t){
    
    printf("Treasure ID: %d\n",t->id);
    printf("Username: %s\n",t->username);
    printf("GPS Coordinates\n -x: %.2f \n -y: %.2f\n",t->coordinates.x,t->coordinates.y);
    printf("Value: %d\n",t->value);
    printf("Clue Text: %s\n",t->clueText);
    
}

int appendData(treasure_t* t, const char* directoryName){

    char treasures_path[PATH_FILE_SIZE];
    if(snprintf(treasures_path, sizeof(treasures_path), "%s/%s_treasures.dat", directoryName, directoryName) < 0){
        perror("Error at creating the path  --appendData()\n");
        exit(1);
    }

    int treasure_fd = 0;

    treasure_fd = open(treasures_path,O_WRONLY | O_APPEND,0777);

    if(treasure_fd == -1){
        perror("Error at opening the treasures file");
        exit(1);
    }

    lseek(treasure_fd, 0, SEEK_END);

    if(write(treasure_fd,t,sizeof(treasure_t)) == -1){
        perror("Error at writing in treasure file");
        close(treasure_fd);
        exit(1);
    }

    close(treasure_fd);

    return 1;
}

int AddTreasure(treasure_t* t,char* directoryName){
    
    // === READ DATA ===
    printf("Treasure ID: ");
    if(scanf("%u",&t->id) != 1){
        perror("Enter a valid Id(unsigned int)\n");
        exit(1);
    }

    getchar();
    printf("Username: ");
    if(fgets(t->username,sizeof(t->username),stdin) == NULL){
        perror("Enter a valid Username(MAX 50 characters)\n");
        exit(1);
    }
    t->username[strlen(t->username) -1] = '\0';     //Removing the \n character

    printf("GPS Coordinates\n -x: ");
    if(scanf("%f",&t->coordinates.x) != 1){
        perror("Enter a valid x coordinate(float)\n");
        exit(1);
    }
    printf(" -y: ");
    if(scanf("%f",&t->coordinates.y) != 1){
        perror("Enter a valid y coordinate(float)\n");
        exit(1);
    }

    printf("Value: ");
    if(scanf("%d",&t->value) != 1){
        perror("Enter a valid value (int)\n");
        exit(1);
    }
    getchar();

    printf("Clue Text: \n");
    if(fgets(t->clueText,sizeof(t->clueText),stdin) == NULL){
        perror("Enter a valid clue text (MAX 256 characters)\n");
        exit(1);
    }

    
    if(strlen(directoryName) > PATH_FILE_SIZE-1)
    {
        perror("The game name length should be less than 256 characters\n");
        exit(1);
    }

    // === CREAT DIRECTORY OR APPEND THE TREASURE TO EXISTING TREASURES ===

    if(mkdir(directoryName,S_IRWXU | S_IRWXG | S_IRWXO | S_IXOTH) != 0){
        // The Data needs to be appended to the existing files
        if(appendData(t,directoryName) != 1){
            return -1;
        }
        return 1;
    }


    // === CREATING FILES ===
    
    char treasures_path[TwoPATHs_FILE_SIZE];
    char logged_hunt_path[PATH_FILE_SIZE];
    char main_logged_hunt_path[PATH_FILE_SIZE];

    if(snprintf(treasures_path, sizeof(treasures_path), "%s/%s_treasures.dat", directoryName, directoryName) < 0){
        perror("Error at creating the treasure_path  --AddTreasure()\n");
        exit(1);
    }
    if(snprintf(logged_hunt_path, sizeof(logged_hunt_path), "%s/logged_hunt.dat", directoryName) < 0){
        perror("Error at creating the logged_path  --AddTreasure()\n");
        exit(1);
    }

    if(snprintf(main_logged_hunt_path, sizeof(main_logged_hunt_path),"logged_hunt-%s.dat", directoryName) < 0){
        perror("Error at creating the main_logged_path  --AddTreasure()\n");
        exit(1);
    }


    int treasure_fd = 0,logged_hunt_fd = 0,main_logged_fd = 0;

    treasure_fd = open(treasures_path,O_RDWR | O_CREAT | O_APPEND,0777);
    logged_hunt_fd = open(logged_hunt_path,O_CREAT,0777);
    main_logged_fd = open(main_logged_hunt_path,O_RDWR | O_CREAT,0777);

    if(treasure_fd == -1){
        perror("Error at opening the treasures file");
        exit(1);
    }
    if(logged_hunt_fd == -1){
        perror("Error at opening the logged_hunt file");
        exit(1);
    }
    if(main_logged_fd == -1){
        perror("Error at opening the main_logged file");
        exit(1);
    }

    // === SYMBOLIC LINK ===

    if(symlink(main_logged_hunt_path,logged_hunt_path) == -1){
        perror("Symbolic link could not be created\n");
        exit(1);
    }
    

    // === WRITING VALUES IN FILES ===

    if(write(treasure_fd,t,sizeof(treasure_t)) == -1){
        perror("Error at writing in treasure file");
        close(treasure_fd);
        exit(1);
    }

    close(treasure_fd);
    close(main_logged_fd);

    return 1;
}

int RemoveTreasure(char* hunt_id,char* treasure_id){

    char treasures_path[PATH_FILE_SIZE];
    char temp_path[PATH_FILE_SIZE];
    if(snprintf(treasures_path, sizeof(treasures_path), "%s/%s_treasures.dat", hunt_id, hunt_id) < 0){
        perror("Error at creating the treasurepath  --RemoveTreasure()\n");
        exit(1);
    }

    if(snprintf(temp_path,sizeof(temp_path),"%s/temp_file.dat",hunt_id) < 0){
        perror("Error at creating the temp_path  --RemoveTreasure()\n");
        exit(1);
    }


    int treasure_fd = open(treasures_path,O_RDONLY,0644);
    int temp_fd = open(temp_path,O_WRONLY | O_CREAT | O_APPEND, 0644);
    
    if(treasure_fd == -1){
        perror("Error at opening the treasures file");
        exit(1);
    }

    if(temp_fd == -1){
        perror("Error at opening the temp file");
        exit(1);
    }

    treasure_t t;
    int foundTheTreasure = 0;
    unsigned int id = atoi(treasure_id);

    while(read(treasure_fd,&t,sizeof(treasure_t))){
        if(t.id != id){
            if(write(temp_fd,&t,sizeof(treasure_t)) < 0){
                perror("Error at writing in temp file\n");
                exit(1);
            }
        }else{
            foundTheTreasure = 1;

        }
    }

    close(temp_fd);
    close(treasure_fd);

    if(foundTheTreasure == 1){
        unlink(treasures_path);
        rename(temp_path,treasures_path);
    }else{
        unlink(temp_path);
        printf("The treasure with the id: %d from %s, could not be found\n",id,hunt_id);
        return 0;
    }

    printf("The treasure with the id: %d from %s, was removed succefully\n",id,hunt_id);

    return 1;
}

int RemoveHunt(char* hunt_id){
    
    DIR* dir = NULL;
    char dirPath[PATH_FILE_SIZE];
    char filePath[TwoPATHs_FILE_SIZE];
    char logged_huntPath[PATH_FILE_SIZE];

    if(snprintf(dirPath,sizeof(dirPath),"%s",hunt_id) < 0){
        perror("Error at creating the dirpath  --RemoveHunt()\n");
        exit(1);
    }
    if(snprintf(logged_huntPath,sizeof(logged_huntPath),"logged_hunt-%s.dat",hunt_id) < 0){
        perror("Error at creating the loggedpath  --RemoveHunt()\n");
        exit(1);
    }

    if((dir = opendir(dirPath)) == 0){
        perror("The directory does not exist or could not be opened --RemoveHunt()\n");
        exit(1);
    }

    if(remove(logged_huntPath) != 0){
        perror("Error at removing/finding the logged_hunt Path --RemoveHunt()\n");
        exit(1);
    }

    

    struct dirent* entry = NULL;    // Every entry of the directory

    while((entry = readdir(dir)) != NULL){
        
        // We doesn't take in consideration the current and the parent dir
        if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0){
            continue;
        }

        snprintf(filePath,sizeof(filePath),"%s/%s",dirPath,entry->d_name);

        remove(filePath);
    }

    if(closedir(dir) == -1){
        perror("Error at closing the dir --RemoveHunt()\n");
        exit(1);
    }

    if(rmdir(dirPath) == -1){
        perror("Error at removing the directory --RemoveHunt()\n");
        exit(1);
    }

    printf("The treasure hunt %s was removed succesfully\n",hunt_id);

    return 1;
}   

int ViewHunt(char* hunt_id,char* treasure_id){
    char treasures_path[PATH_FILE_SIZE];
    if(snprintf(treasures_path, sizeof(treasures_path), "%s/%s_treasures.dat", hunt_id, hunt_id) < 0){
        perror("Error at creating the path  --ViewHunt()\n");
        exit(1);
    }

    int treasure_fd = open(treasures_path,O_RDONLY,0644);
    treasure_t t;

    unsigned int id = atoi(treasure_id);
    int foundTheTreasure = -1;

    int bytes = 0;
    while((bytes = read(treasure_fd,&t,sizeof(treasure_t))) > 0){
        if(t.id == id){
            foundTheTreasure = 1;
            break;
        }
    }

    if(bytes < 0){
        perror("Error at reading --ViewHunt()\n");
        exit(1);
    }

    if(foundTheTreasure == 1){
        printTreasure(&t);
    }else{
        printf("Treasure with id: %u not found in %s\n",id,hunt_id);
    }

    return 1;
}

void List(char* hunt_id){
    char path[PATH_FILE_SIZE];
    if(snprintf(path, sizeof(path), "%s/%s_treasures.dat", hunt_id,hunt_id) < 0){
        perror("Error at creating the path  --List()\n");
        exit(1);
    }
    
    int path_fd = open(path,O_RDONLY,0644);
    if(path_fd == -1){
        perror("When trying to list the hunts, the files could not be opened or could not be found\n");
        exit(1);
    }

    treasure_t t;
    struct stat fileStat; // The stats of the file

    if(stat(path,&fileStat) == -1){
        perror("Could not obtain any informations about the files");
        exit(1);
    }

    printf("\nThe Hunt Name: %s",hunt_id);

    printf("\nFile Size: %lld bytes\n", (long long) fileStat.st_size);

    printf("Last modification time: %s\n", ctime(&fileStat.st_mtime));

    int bytes = 0;
    while((bytes = read(path_fd,&t,sizeof(treasure_t))) > 0){
        printTreasure(&t);
    }

    close(path_fd);

    if(bytes < 0){
        perror("Error at reading data --List()\n");
        exit(1);
    }    
}

void listLogs(char* hunt_id){

    char link_path[PATH_FILE_SIZE];
    char target_path[BUFF_SIZE];
    if(snprintf(link_path, sizeof(link_path), "%s/logged_hunt.dat", hunt_id) < 0){
        perror("Error at creating the path  --listLogs()\n");
        exit(1);
    }

    int link_path_fd = open(link_path,O_RDONLY,0644);
    if(link_path_fd == -1){
        perror("Error at opening the path --listLogs()\n");
        exit(1);
    }

    struct stat stat_buf;
    if(lstat(link_path,&stat_buf) == -1){
        perror("Error at lstat  --listLogs()\n");
        exit(1);
    }

    if(!S_ISLNK(stat_buf.st_mode)){
        printf("%s is not a symboloc link \n",link_path);
        return ;
    }


    ssize_t len = 0;  
    while((len = readlink(link_path, target_path, sizeof(target_path) - 1)) > 0){
        target_path[len] = '\0';
        printf("%s",target_path);
    }

    if (len == -1) {
        perror("Could not read the link  --listLogs()");
        exit(1);
    }
}

void addLogs(operation_t operation,char* hunt_id,char* treasure_id){
    char logged_hunt_path[PATH_FILE_SIZE];
    int logged_hunt_fd = 0;

    if(snprintf(logged_hunt_path,sizeof(logged_hunt_path),"%s/logged_hunt",hunt_id) < 0){
        perror("Error at creating the path  --addLogs()\n");
        exit(1);
    }

    if((logged_hunt_fd = open(logged_hunt_path,O_WRONLY | O_APPEND, 0777)) == -1){
        fprintf(stderr, "Error opening the logged file: %s\n", strerror(errno));
        exit(1);
    }

    char log[LOG_SIZE];

    switch (operation){
    case ADD:

        if(snprintf(log,sizeof(log),"--add <%s> function called\n",hunt_id) < 0){
            perror("Error at creating the log1  --addLogs()");
            exit(1);
        }
    
        write(logged_hunt_fd,log,sizeof(log));

        break;
    case LIST:
        
        if(snprintf(log,sizeof(log),"--list <%s> function called\n",hunt_id) < 0){
            perror("Error at creating the log2  --addLogs()");
            exit(1);
        }
        
        write(logged_hunt_fd,log,sizeof(log));

        break;
    case VIEW:

        if(snprintf(log,sizeof(log),"--view <%s> <%s> function called\n",hunt_id,treasure_id) < 0){
            perror("Error at creating the log3  --addLogs()");
            exit(1);
        }

        write(logged_hunt_fd,log,sizeof(log));

        break;
    case REMOVE_TREASURE:
        if(snprintf(log,sizeof(log),"--remove_treasure <%s> <%s> function called\n",hunt_id,treasure_id) < 0){
            perror("Error at creating the log4  --addLogs()");
            exit(1);
        }

        write(logged_hunt_fd,log,sizeof(log));

        break;
    default:
        break;
    }

    if(close(logged_hunt_fd) != 0){
        perror("Error at closing the logged_hunt file  --addLogs()\n");
        exit(1);
    }
}

int main(int argc, char** argv ){
    
    if(argc < 2)
    {
        perror("Too few arguments\n\nHere is a list of flags:\n-------------\n\t--add <game name>\n\t--list\n\t--view\n\t--remove_treasure\n\t--remove_hunt\n");
        exit(1);
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

        AddTreasure(treasure,argv[2]);
        addLogs((operation = ADD),argv[2],NULL);

    }else if(strcmp(argv[1],"--list")==0){
        if(argc != 3){
            perror("Enter a game name\n");
            exit(1);
        }

        List(argv[2]);
        addLogs((operation = LIST),argv[2],NULL);

    }else if(strcmp(argv[1],"--remove_treasure") == 0){
        if(argc != 4){
            perror("Enter a game name and a specific trasure(id)\n");
            exit(1);
        }

        RemoveTreasure(argv[2],argv[3]);
        addLogs((operation = REMOVE_TREASURE),argv[2],argv[3]);

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

        ViewHunt(argv[2],argv[3]);
        addLogs((operation = VIEW),argv[2],argv[3]);
    }else{
        printf("You've entered an unknown command\n");
    }

    //listLogs(argv[2]);

    return 0;
}