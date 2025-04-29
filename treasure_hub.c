#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 64
#define PATH_FILE_SIZE 256
#define ID_SIZE 32
#define COMMAND_SIZE 512
#define PATH_FILE_SIZE 256

pid_t monitor_pid = 0;
const char COMMAND_FILE_PATH[] = "command_file.txt";


int readCommandFromFile(const char* path,char* command){    
    int path_fd = open(path,O_RDONLY,0777);
    if(path_fd == -1){
        perror("The files could not be opened or could not be found  --readCommandFromFile()\n");
        exit(1);
    }

    if(read(path_fd,command,COMMAND_SIZE) <= 0){
        perror("Error at reading data from file  --readCommandFromFile()\n");
        close(path_fd);
        return -1;
    }
    if(strlen(command) == COMMAND_SIZE){
        perror("The command is to long, it shall be less than 512 bytes\n");
    }

    command[strlen(command)] = 0;

    close(path_fd);

    return 1;
}

int writeCommandInFile(const char* path, char* command){
    int path_fd = open(path,O_WRONLY | O_CREAT | O_TRUNC,0777);
    if(path_fd == -1){
        perror("The files could not be opened or could not be found  --writeCommandInFile()\n");
        exit(1);
    }

    if(write(path_fd,command,strlen(command)) != strlen(command)){
        perror("Error at writing data from file  --writeCommandInFile()\n");
        close(path_fd);
        return -1;
    }

    if(write(path_fd,"\0",1) < 0){
        perror("Error at writing data from file 2 --writeCommandInFile()\n");
        close(path_fd);
        return -1;
    }

    close(path_fd);
    return 1;
}

int getHuntId(char* flag){
    printf("  Enter a hunt_id: ");
    char hunt_id[PATH_FILE_SIZE];
    char command[COMMAND_SIZE];

    scanf("%s",hunt_id);
    sprintf(command,"./treasure_manager %s %s",flag,hunt_id);
    getc(stdin);//CONSUME THE \n

    command[strlen(command)] = '\0';

    if(writeCommandInFile(COMMAND_FILE_PATH,command) == -1){
        return -1;
    }

    return 1;
}

int getTreasureId(){
    printf("  Enter a treasure_id: ");
    char treasure_id[ID_SIZE];
    char old_command[COMMAND_SIZE];
    char command[COMMAND_SIZE+ID_SIZE];

    scanf("%s",treasure_id);
    getc(stdin);//CONSUME THE \n

    readCommandFromFile(COMMAND_FILE_PATH,old_command);
    sprintf(command,"%s %s",old_command,treasure_id);
    
    command[strlen(command)] = '\0';
    writeCommandInFile(COMMAND_FILE_PATH,command);

    return 1;
}

void list_treasures(){
    char command[COMMAND_SIZE];
    if(readCommandFromFile(COMMAND_FILE_PATH,command) != 1){
        perror("Error at reading data from file\n");
        return ;
    }

    system(command);
}

void view_hunts(){
    char command[COMMAND_SIZE];
    if(readCommandFromFile(COMMAND_FILE_PATH,command) != 1){
        perror("Error at reading data from file\n");
        return ;
    }

    system(command);
}

void handler(int signum){
    if(signum == SIGUSR1){
        list_treasures();
    }else if(signum == SIGUSR2){
        view_hunts();
    }
}

void start_monitor(){
    if(monitor_pid < 0){
        printf("A procces is already running\n");
        return ;
    }

    monitor_pid = fork();
    if(monitor_pid == 0){
        //CHILD
        while(1){
            pause();
        };
    }else if(monitor_pid > 0){
        //PARENT
        char buffer[BUFFER_SIZE];
        while(1){
        printf("Enter a command: ");
        if(fgets(buffer,sizeof(buffer),stdin) == 0){
            perror("Error at reading the commands\n");
            exit(1);
        }
        buffer[strlen(buffer)-1] = 0;

        if(strcmp(buffer,"start_monitor") == 0){
            printf("--- A procces is already running ---\n");
        }else if(strcmp(buffer,"list_treasures") == 0){
            if(getHuntId("--list") == -1){
                printf("An error occured when trying to get the hunt id\n");
                continue;
            }
            
            kill(monitor_pid,SIGUSR1);
        }else if(strcmp(buffer,"view_treasure") == 0){
            if(getHuntId("--view") == -1){
                printf("An error occured when trying to get the hunt id\n");
                continue;
            }
            if(getTreasureId() == -1){
                printf("An error occured when trying to get the treasure id\n");
                continue;
            }

            sleep(1);
            
            kill(monitor_pid,SIGUSR2);
        }else{
            printf("Enter a valid command like: \n  list_hunts\n  list_treasures\n  view_treasure\n  stop_monitor\n");
        }

        sleep(1);
    }


    }
}


int main(){

    struct sigaction sa_struct;
    sa_struct.sa_handler = handler;
    sigemptyset(&sa_struct.sa_mask);
    sa_struct.sa_flags = 0;


    if(sigaction(SIGUSR1,&sa_struct,NULL) == -1){
        perror("Error at sigaction SIGUSR1 \n");
        exit(1);
    }

    if(sigaction(SIGUSR2,&sa_struct,NULL) == -1){
        perror("Error at sigaction SIGUSR2 \n");
        exit(1);
    }

    char buffer[BUFFER_SIZE];

    while(1){
        printf("Enter a command: ");
        if(fgets(buffer,sizeof(buffer),stdin) == 0){
            perror("Error at reading the commands\n");
            exit(1);
        }

        buffer[strlen(buffer)-1] = 0;

        if(strcmp(buffer,"start_monitor") == 0){
            printf("=== A procces started ===\n\n");
            start_monitor();
        }else{
            printf("You may start first the monitor using the flag <start_monitor>\n");
        }
    }

    return 0;
}
