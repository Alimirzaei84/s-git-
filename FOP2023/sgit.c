#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include<unistd.h>
#include <windows.h>
#include<io.h>
#define MAX_COMMAND_lenghth 70

void Make_a_hidden_dir(const char *dirName, const char *message_success){
    DIR* dir;
    char currentDir[FILENAME_MAX];
    _getcwd(currentDir, sizeof(currentDir));
    while(1) {
        dir = opendir(dirName);
        if (dir) {
            printf("There is already exists!\n");
            closedir(dir);
            break;
        } else if (ENOENT == errno) {
            _chdir(currentDir); 
            CreateDirectory(dirName, NULL);
            SetFileAttributes(dirName, FILE_ATTRIBUTE_HIDDEN);
            puts(message_success);
            break;
        } else if (chdir("..") != 0) { 
            printf("An error occurred.\n");
            break;
        }
    }
}
void Set_Config(const char **argv){
    FILE *config = NULL;
    if(!strcmp(argv[2], "-global")){
        if(!strcmp(argv[3], "user.name")) config = fopen("C:\\username.txt", "w+");
        else if(!strcmp(argv[3], "user.email")) config = fopen("C:\\useremail.txt", "w+");
    }else{
        if(!strcmp(argv[2], "user.name")) config = fopen("username.txt", "w+");
        else if(!strcmp(argv[2], "user.email")) config = fopen("useremail.txt", "w+");
    }
    if(config == NULL) {
        printf("Try again!\n");
    } else {
        if(!strcmp(argv[2], "-global")) {
            fputs(argv[4], config);
        } else {
            fputs(argv[3], config);
        }
        fclose(config);
        printf("The config is now set\n");
    }
}


int main(int argc, char **argv){
    char main_command[MAX_COMMAND_lenghth]; strcpy(main_command, argv[1]);
     if(!strcmp(main_command, "init")){ 
        Make_a_hidden_dir(".sgit", "The local repo created :)) \n");

    }else if(!strcmp(main_command, "config")) Set_Config(argv);
    else printf("Invalid command\n");
    return 0;
}