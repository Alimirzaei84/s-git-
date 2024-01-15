#ifndef FOO_DOT_H    /* This is an "include guard" */
#define FOO_DOT_H    /* prevents the file from being included twice. */
           #include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <windows.h>
#include<io.h>
#include <stdbool.h>
#define MAX 200          /* Including a header file twice causes all kinds */
                     /* of interesting problems.*/

/**
 * This is a function declaration.
 * It tells the compiler that the function exists somewhere.
 */
int is_valid_command(const char *command) {
 
  // execute the command using the system function
  int result = system(command);
  if(!result){
  
  return 1;
  }
  else{
     return 0;
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
void foo(int id, char *name);

#endif /* FOO_DOT_H */