#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include<unistd.h>
#include <windows.h>
#include<io.h>
#define FILENAME_MAX 20
#define MAX_MAIN_COMMAND_lenghth 70

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

int main(int argc, char **argv){
    char main_command[MAX_MAIN_COMMAND_lenghth]; strcpy(main_command, argv[1]);
    if(!strcmp(main_command, "init")) Make_a_hidden_dir("sgit", "The local repo created :)) \n");
    return 0;
}