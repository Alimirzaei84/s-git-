//#include"foo.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <windows.h>
#include<io.h>
#include <stdbool.h>
#define MAX 200

int is_valid_command(const char *command) {
  char buffer[256];
  sprintf(buffer, "where /r c:\\windows\\system32 %s >nul 2>nul", command);
  int result = system(buffer);
  return result == 0;
}

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

void config_alias(char **argv){
    char commmand[100] = "sgit ";
            strcat(commmand,argv[3]);
    if(!is_valid_command(commmand)){
        printf("\n Invalid command\n\n");
        return;
    }
char *name_of_alias = strstr(argv[2], ".");
memmove(name_of_alias, name_of_alias+1,strlen(name_of_alias));
name_of_alias[strlen(name_of_alias)] = '\0';
FILE *file_alias = fopen("alias.txt", "a");
fprintf(file_alias, "%s %s\n", name_of_alias,commmand);
printf("\n The alias is set\n");
}

void Set_Config(char **argv){
    FILE *config = NULL;
    if(!strcmp(argv[2], "-global")){
        if(!strcmp(argv[3], "user.name")) config = fopen("C:\\username.txt", "w+");
        else if(!strcmp(argv[3], "user.email")) config = fopen("C:\\useremail.txt", "w+");
    }
        else if(!strcmp(argv[2], "user.name")) config = fopen("username.txt", "w+");
        else if(!strcmp(argv[2], "user.email")) config = fopen("useremail.txt", "w+");
        else {
            config_alias(argv);
            return;
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
void Load_alias(char **argv){
    FILE *f = fopen("alias.txt", "r");
    if(f == NULL){
        printf("\n try again\n");
        return;
    }
    char line[200];
    while (fgets(line, 200, f) != NULL) {
        char *alias = strtok(line, " ");
        char *command = strtok(NULL, "\n");
        if(!strcmp(alias, argv[1])) {
            system(command);
            break;
        }
    }
    fclose(f); 
}

int main(int argc, char **argv){
    char main_command[MAX]; strcpy(main_command, argv[1]);
     if(!strcmp(main_command, "init")) {Make_a_hidden_dir(".sgit", "\n The local repo created :)) \n\n");
      return 0;
     }
     _chdir(".sgit");
     if(!strcmp(main_command, "config")) Set_Config(argv);
     else Load_alias(argv);
    return 0;
}