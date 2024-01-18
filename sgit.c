#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <windows.h>
#include <io.h>
#include <stdbool.h>
#define MAX 200
#define NEAT system("cls");

int is_valid_command(const char *command) {
  char buffer[256];
  sprintf(buffer, "where /r c:\\windows\\system32 %s >nul 2>nul", command);
  int result = system(buffer);
  return result == 0;
}

void Make_a_hidden_dir(const char *dirName, char *message_success, char *failed_message) {
    char currentDir[FILENAME_MAX];
    _getcwd(currentDir, sizeof(currentDir));
    char currentDir_copy[100];
    strcpy(currentDir_copy, currentDir);
    if (_access(dirName, 0) == 0) {
        printf("%s\n", failed_message);
        return;
    }
    while (1) {
        _chdir("..");
        if (_getcwd(currentDir, sizeof(currentDir)) == NULL) {
            printf("An error occurred while getting current directory.\n");
            return;
        }
        if (_access(dirName, 0) == 0) {
            _chdir(currentDir);
            printf("%s\n", failed_message);
            return;
        }
        if(strlen(currentDir) < 4) break;
    }
    if (_mkdir(dirName) == 0) {
           _chdir(currentDir_copy); 
            CreateDirectory(dirName, NULL);
            SetFileAttributes(dirName, FILE_ATTRIBUTE_HIDDEN);
            puts(message_success);
    } else {
        printf("Failed to create directory.\n");
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
NEAT printf("\n The alias is set\n");
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

int is_file_or_dir(const char* str) {
    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile(str, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
            return -1;
        } else {
            return -2;
        }
    }
    FindClose(h);
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        return 1;
    } else {
        return 0;
    }
}

int copy_file(char *source, char *dest){
  char buffer[100];
  if (access(dest, W_OK) == 0) {
    sprintf(buffer, "copy %s %s \n ", source, dest);
    return system(buffer);
  } else {
    printf("Error: Cannot copy file to %s\n", dest);
    return 1;
  }
}

int copy_dir(char *source, char *dest){
  char buffer[100];
  if (access(dest, F_OK) == 0) {
    sprintf(buffer, "xcopy %s %s  /s /e /h \n  ",source, dest);
    return system(buffer);
  } else {
    printf("Error: Cannot copy directory to %s\n", dest);
    return 1;
  }
}

void add(char *s){
   if(is_file_or_dir(s)){
     if(copy_dir(s, ".sgit\\.staging") == 0){NEAT
           FILE *status =  fopen(".sgit\\status.txt", "a");
          fprintf(status, "%s \n", s);
          fclose(status);
          printf("\n ********* Adding Successfully      *********\n********* Do you enjoy using Sgit? *********\n");
        return;
     }
   }
   else if(copy_file(s, ".sgit\\.staging") == 0){NEAT
      FILE *status =  fopen(".sgit\\.status.txt", "a");
      fprintf(status, "%s ", s);
      printf("\n ********* Adding Successfully      *********\n ********* Do you enjoy using Sgit? *********\n");
        return;
   }
   else printf("\nError\n");
}

void Print_Files_status() {
  DIR *dir;
  struct dirent *entry;
  char *path = ".";
  dir = opendir(path);
  if (dir == NULL) {
    perror("opendir");
    return;
  }
  FILE *status = fopen(".sgit\\status.txt", "r");
  if (status == NULL) {
    printf("Try again\n");
    return;
  }
  char temp[FILENAME_MAX];
  while (fgets(temp, FILENAME_MAX, status) != NULL) {
    char *token = strtok(temp, " ");
    if (token != NULL) {
      printf("%s --> This file or directory is added\n", token);
    }
  }
  rewind(status);
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] == '.') continue;
    bool is_add = false;
    while (fgets(temp, FILENAME_MAX, status) != NULL) {
      char *token = strtok(temp, " ");
      if (token != NULL && !strcmp(token, entry->d_name)) {
        is_add = true;
        break;
      }
    }
    printf("%s --> %s\n", entry->d_name, is_add ? "This file or directory is added " : "Is not added!!");
    rewind(status);
  }
  fclose(status);
  closedir(dir);
} 

int main(int argc, char **argv){
    char main_command[MAX]; strcpy(main_command, argv[1]);
    //init>>
     if(!strcmp(main_command, "init")) {
        Make_a_hidden_dir(".sgit", "\n The local repo created :)) \n\n", "\nThere is already exists!");
        //_chdir(".sgit");
        Make_a_hidden_dir(".sgit\\.staging", "\n And intitialized \n\n", "\n");
        // Make_a_hidden_dir(".commits", "\n And intitialized \n\n", "\n");
      return 0;
     }
     //config>>
     if(!strcmp(main_command, "config")){
         _chdir(".sgit");
       Set_Config(argv);
     }
     //add>>
     else if(!strcmp(main_command, "add")){
         if(!strcmp(argv[2],"-f")) {
            for (int i = 3 ;i < argc;i++)
            add(argv[i]);
         }
         else if(!strcmp(argv[2], "-n")){
            Print_Files_status();
         }
         else add(argv[2]);
     }
     //maybe it is an alias>>
     else Load_alias(argv);
    return 0;
}