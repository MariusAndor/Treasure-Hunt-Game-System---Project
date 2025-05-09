#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "treasure_manager.h"

#define BUFFER_SIZE 64
#define PIPE_BUFFER_SIZE 64
#define PATH_FILE_SIZE 256
#define ID_SIZE 32
#define COMMAND_SIZE 512
#define TwoPATHs_FILE_SIZE 527


pid_t monitor_pid = -1;
int pipe_fd[2] = {0,0};
int stdout_fd = -1;
const char COMMAND_FILE_PATH[] = "command_file.txt";

int readCommandFromFile(const char *path, char *command)
{
    int path_fd = open(path, O_RDONLY, 0777);
    if (path_fd == -1)
    {
        perror("The files could not be opened or could not be found  --readCommandFromFile()\n");
        exit(1);
    }

    if (read(path_fd, command, COMMAND_SIZE) <= 0)
    {
        perror("Error at reading data from file  --readCommandFromFile()\n");
        close(path_fd);
        return -1;
    }
    if (strlen(command) == COMMAND_SIZE)
    {
        perror("The command is to long, it shall be less than 512 bytes\n");
    }

    command[strlen(command)] = 0;

    close(path_fd);

    return 1;
}

int writeCommandInFile(const char *path, char *command)
{
    int path_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (path_fd == -1)
    {
        perror("The files could not be opened or could not be found  --writeCommandInFile()\n");
        exit(1);
    }

    if (write(path_fd, command, strlen(command)) != strlen(command))
    {
        perror("Error at writing data from file  --writeCommandInFile()\n");
        close(path_fd);
        return -1;
    }

    if (write(path_fd, "\0", 1) < 0)
    {
        perror("Error at writing data from file 2 --writeCommandInFile()\n");
        close(path_fd);
        return -1;
    }

    close(path_fd);
    return 1;
}

int getHuntId(char *flag)
{
    printf("  Enter a hunt_id: ");
    char hunt_id[PATH_FILE_SIZE];
    char command[COMMAND_SIZE];

    scanf("%s", hunt_id);
    sprintf(command, "./treasure_manager %s %s", flag, hunt_id);
    getc(stdin); // CONSUME THE \n

    command[strlen(command)] = '\0';

    if (writeCommandInFile(COMMAND_FILE_PATH, command) == -1)
    {
        return -1;
    }

    return 1;
}

int getTreasureId()
{
    printf("  Enter a treasure_id: ");
    char treasure_id[ID_SIZE];
    char old_command[COMMAND_SIZE];
    char command[COMMAND_SIZE + ID_SIZE];

    scanf("%s", treasure_id);
    getc(stdin); // CONSUME THE \n

    readCommandFromFile(COMMAND_FILE_PATH, old_command);
    sprintf(command, "%s %s", old_command, treasure_id);

    command[strlen(command)] = '\0';
    writeCommandInFile(COMMAND_FILE_PATH, command);

    return 1;
}

void separateArgvFromCommand(char argv_temp[][32],char* command){
    char* token = strtok(command," ");
    unsigned int index = 0;
    
    while(token != NULL){
        strcpy(argv_temp[index],token);
        index++;
        token = strtok(NULL," ");
    }
}

void list_treasures()
{
    char command[COMMAND_SIZE];
    if (readCommandFromFile(COMMAND_FILE_PATH, command) != 1)
    {
        perror("Error at reading data from file\n");
        return;
    }


    char argv_temp[3][32];
    separateArgvFromCommand(argv_temp,command);

    pid_t pid = fork();
    if(pid == 0){
        //CHILD
        dup2(pipe_fd[1],STDOUT_FILENO); //REDIRECTING THE STANDARD OUTPUT TO PIPE

        execl("./treasure_manager",argv_temp[0],argv_temp[1],argv_temp[2],NULL);
    }else if(pid > 0){
        //PARENT
        wait(NULL);
    }

}

void view_hunts()
{
    char command[COMMAND_SIZE];
    if (readCommandFromFile(COMMAND_FILE_PATH, command) != 1)
    {
        perror("Error at reading data from file\n");
        return;
    }

    dup2(pipe_fd[1],STDOUT_FILENO); //REDIRECTING THE STANDARD OUTPUT TO PIPE

    system(command);
}

int countTheTreasuresInHunt(char* path){
    int path_fd = open(path,O_RDONLY,0777);
    int count = 0;
    treasure_t t;

    while(read(path_fd,&t,sizeof(treasure_t)) > 0 ){
        count++;
    }

    return count;
}

void list_hunts()
{
    struct dirent *entry;
    DIR* dir = opendir(".");

    if(dir == NULL){
        perror("Error at opening the directory\n");
        return ;
    }
    dup2(pipe_fd[1],STDOUT_FILENO); //REDIRECTING THE STANDARD OUTPUT TO PIPE

    printf("  Hunt id's list:\n");
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0){
            continue;
        }

        char path[PATH_FILE_SIZE+2];
        sprintf(path,"./%s",entry->d_name);

        struct stat st;

        if(stat(path,&st) == 0 && S_ISDIR(st.st_mode) && path[2] != '.'){
            printf("     %s has ",entry->d_name);

            DIR* subdir = opendir(entry->d_name);
            if(subdir){
                struct dirent* subentry;
                
                char hunt_path[TwoPATHs_FILE_SIZE];

                if((subentry = readdir(subdir)) != NULL){
                    if(strcmp(subentry->d_name,".") != 0 && strcmp(subentry->d_name,"..") != 0){
                        sprintf(hunt_path,"%s/%s_treasures.dat",entry->d_name,entry->d_name);
                    
                        int numberOfTreasures = countTheTreasuresInHunt(hunt_path);
                        if(numberOfTreasures <= 1){
                            printf("%d treasure\n",numberOfTreasures);
                        }else{
                            printf("%d treasures\n",numberOfTreasures);
                        }
                    }
                }

                closedir(subdir);

            }else{
                perror("  Could not open subdirectory\n");
            }

        }
    }
}

int listHuntsOption(){
    char command[] = "./treasure_manager";

    if (writeCommandInFile(COMMAND_FILE_PATH, command) == -1)
    {
        return -1;
    }

    return 1;
}

void handler(int signum)
{
    if (signum == SIGUSR1)
    {
        char command[COMMAND_SIZE];
        readCommandFromFile(COMMAND_FILE_PATH,command);
        if(strcmp(command,"./treasure_manager") == 0){
            list_hunts();
        }else{
            list_treasures();
        }   
    }
    else if (signum == SIGUSR2)
    {
        view_hunts();
    }
}

void stop_monitor(){
    pid_t proces_pid = fork();
    if(proces_pid == 0){
        //CHILD
        char string[10];
        while(scanf("%10s",string) == 1){
            printf("Error,wait until it ends\n");
        }
    }else if(proces_pid > 0){
        //PARENT
        sleep(3);
        kill(proces_pid,SIGKILL);
        int stat;
        waitpid(proces_pid,&stat,0);

        if(kill(monitor_pid, SIGKILL) == 0){
            printf("Process %d terminated \n",monitor_pid);   
        }else{
            printf("Failed to kill the process\n");
        }

    }
   
}

int exit_monitor(){
    int status;
    pid_t result = waitpid(monitor_pid,&status,WNOHANG);

    if(result == 0){
        printf("Process is still running\n");
    }else if(result == monitor_pid){
        printf("Sucessfully exited the program\n");
        return 0;
    }

    return 1;
}

void createCommandFileIfNotExisting(const char* path){
    int path_fd = open(path, O_CREAT | O_TRUNC, 0777);
    if (path_fd == -1)
    {
        perror("The files could not be created  --createCommandFileIfNotExisting()\n");
        exit(1);
    }

    close(path_fd);
}

int start_monitor()
{
    
    if(pipe(pipe_fd) == -1){
        perror("Error at pipe\n");
        exit(1);
    }

    stdout_fd = dup(STDOUT_FILENO);

    monitor_pid = fork();
    if(monitor_pid < 0){
        printf("Error at creating a process\n");
        exit(1);
    }

    
    if (monitor_pid == 0)
    {
        // CHILD
        close(pipe_fd[0]); //READING CLOSED
        
        while (1)
        {
            pause();
        };
    }
    else if (monitor_pid > 0)
    {
        // PARENT
        char buffer[BUFFER_SIZE];
        createCommandFileIfNotExisting(COMMAND_FILE_PATH);

            // For Pipes
        char pipe_buffer[PIPE_BUFFER_SIZE];
        int buffer_size = -1;

        close(pipe_fd[1]); //WRITING CLOSED

        while (1)
        {
            usleep(100000);

            printf("Enter a command: ");

            if (fgets(buffer, sizeof(buffer), stdin) == 0)
            {
                perror("Error at reading the commands\n");
                exit(1);
            }
            buffer[strlen(buffer) - 1] = 0;

            if (strcmp(buffer, "start_monitor") == 0)
            {
                printf("=== A procces is already running ===\n");
                
            }
            else if (strcmp(buffer, "list_treasures") == 0)
            {
                if (getHuntId("--list") == -1)
                {
                    printf("An error occured when trying to get the hunt id\n");
                    continue;
                }
                
                kill(monitor_pid, SIGUSR1);

                //=== TESTING TO SEE IF THE OUTPUT IS PRINTED FROM THE PIPE
                //printf("\n-> Printing the output from PIPE\n\n");

                while((buffer_size = read(pipe_fd[0],pipe_buffer,sizeof(pipe_buffer)-1)) > 0){
                    pipe_buffer[buffer_size]='\0';
                    printf("%s",pipe_buffer);
                    if(buffer_size < PIPE_BUFFER_SIZE-1){
                        break;
                    }
                }

                
            }
            else if(strcmp(buffer, "view_treasure") == 0)
            {
                if (getHuntId("--view") == -1)
                {
                    printf("An error occured when trying to get the hunt id\n");
                    continue;
                }
                if (getTreasureId() == -1)
                {
                    printf("An error occured when trying to get the treasure id\n");
                    continue;
                }

                kill(monitor_pid, SIGUSR2);

                //=== TESTING TO SEE IF THE OUTPUT IS PRINTED FROM THE PIPE
                //printf("\n-> Printing the output from PIPE\n\n");

                while((buffer_size = read(pipe_fd[0],pipe_buffer,sizeof(pipe_buffer)-1)) > 0){
                    pipe_buffer[buffer_size]='\0';
                    printf("%s",pipe_buffer);
                    if(buffer_size < PIPE_BUFFER_SIZE-1){
                        break;
                    }
                }

            }
            else if(strcmp(buffer,"list_hunts") == 0){
                
                if(listHuntsOption() == -1)
                {
                    continue;
                }

                kill(monitor_pid, SIGUSR1);

                //=== TESTING TO SEE IF THE OUTPUT IS PRINTED FROM THE PIPE
                //printf("\n-> Printing the output from PIPE\n\n");

                while((buffer_size = read(pipe_fd[0],pipe_buffer,sizeof(pipe_buffer)-1)) > 0){
                    pipe_buffer[buffer_size]='\0';
                    
                    printf("%s",pipe_buffer);
                    if(buffer_size < PIPE_BUFFER_SIZE-1){
                        break;
                    }
                }

            }
            else if(strcmp(buffer,"stop_monitor") == 0){
                stop_monitor();
                break;
            }
            else if(strcmp(buffer,"exit") == 0){
                if(exit_monitor() == 0){
                    return 0;
                }
            }
            else
            {
                printf("Enter a valid command like: \n  list_hunts\n  list_treasures\n  view_treasure\n  stop_monitor\n");
            }
        }
    }

    close(pipe_fd[0]);

    return 1;
}

int main(){
    struct sigaction sa_struct;
    sa_struct.sa_handler = handler;
    sigemptyset(&sa_struct.sa_mask);
    sa_struct.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa_struct, NULL) == -1)
    {
        perror("Error at sigaction SIGUSR1 \n");
        exit(1);
    }

    if (sigaction(SIGUSR2, &sa_struct, NULL) == -1)
    {
        perror("Error at sigaction SIGUSR2 \n");
        exit(1);
    }

    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("Enter a command: ");
        if (fgets(buffer, sizeof(buffer), stdin) == 0)
        {
            perror("Error at reading the commands\n");
            exit(1);
        }

        buffer[strlen(buffer) - 1] = 0;

        if (strcmp(buffer, "start_monitor") == 0)
        {
            printf("=== A procces started ===\n\n");
            start_monitor();
        }else if(strcmp(buffer,"exit") == 0){
            if(exit_monitor() == 0){
                return 0;
            }
        }
        else
        {
            printf("You may start the monitor using the flag <start_monitor> or exit the monitor using <exit>\n");
        }
    }

    return 0;
}
