#include "foo.h" 
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
  sprintf(buffer, "where %s >nul 2>nul", command);
  int result = system(buffer);
  return result == 0;
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

void foo(int id, char *name)
{
    fprintf(stderr, "foo(%d, \"%s\");\n", id, name);

}