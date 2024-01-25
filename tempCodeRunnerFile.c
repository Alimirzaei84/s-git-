#include<stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include<libgen.h>
#include <sys/types.h>

int dir_exists(char *path) {
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        return 1;
    } else {
        return 0;
    }
}

int isvalid_command(const char *command) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "type %s >/dev/null 2>&1", command);
    return system(buffer) == 0;
}

void create_hidden_dir() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Error getting current working directory");
        return;
    }
    int found = 0; 
    char path[1024];
    strcpy(path, cwd);
    while (1) {
        strcat(path, "/.sgit");
        if (dir_exists(path)) { 
            printf("\nError : Already a repository exists in %s\n\n", path);
            found = 1;
            break;
        }
        path[strlen(path) - 6] = '\0';
        if (strcmp(path, "/") == 0) {
            break;
        }
        strcpy(path, dirname(path));
    }
    if (!found) {
        strcpy(path, cwd);
        strcat(path, "/.sgit");
        mkdir(path, 0777);
        printf("Created a local repository in %s\n", path);
    }
    else printf("error!!");
}

void copy(char *path, char *dest) {
    char command[1024];
    sprintf(command, "cp -ur %s %s", path, dest);
    system(command);
}

void add(char *name){
     char input[1024];
    if (realpath(name, input) == NULL) {
        printf("\nError resolving input path\n");
        return;
    }
      if (access(input, F_OK) != 0) {
        perror("Error accessing input path");
        return;
    }
     char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Error getting current working directory");
        return;
    }
    char dest[100];
    strcpy(dest, cwd);
    strcat(dest, "/.sgit/.staging");
     if (access(dest, F_OK) != 0) {
        mkdir(dest, 0777);
    }
    copy(input, dest);
    printf("\nAdd with success\n");
}

void check_with_staging_area(){
  char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Error getting current working directory");
        return;
    }
    char base[2048];
    sprintf(base, "%s/.sgit/.staging", cwd);
        DIR *dir = opendir(cwd);
    if (dir == NULL) {
        perror("Error opening current directory");
        exit(1);
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char temp[2048];
        sprintf(temp,"%s/%s",base, entry->d_name);
        if(!access(temp,F_OK)){
            printf("\n*** %s is in staging area ***\n", entry->d_name);
        }
        else{
            printf("\n%s is not staged\n",entry->d_name);
        }
    } 
}

void reset(char *path){
      char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Error getting current working directory");
        return;
    }
    char full_path[1024];
    sprintf(full_path,"%s/.sgit/.staging/%s",cwd,path);
    char buffer[1024];
    sprintf(buffer, "rm -r %s ", full_path);
    if(system(buffer) == 0){
        printf("\nreset is done\n");
    }
    else {
        perror("\ncaan not reset this\n");
    }
}

void config_alias(char **argv){
    if (!strcmp(argv[2], "-global")){
        FILE *alias = fopen("/alias.txt", "a");
        char *name_of_alias = strtok(argv[3], ".");
        memmove(name_of_alias, name_of_alias + 1 , strlen(name_of_alias));
        char alias_command[100];
        strcpy(alias_command, argv[4]);
        if(!isvalid_command(alias_command)){
            perror("\nInvalid command \n");
            exit(1);
        }
        else{
            char buffer[1000];
            strcpy(buffer, name_of_alias);
            strcat(buffer, " ");
            strcat(buffer, alias_command);
            strcat(buffer, "\n");
            fputs(buffer, alias);
        }
        fclose(alias);
    }
    else {
        FILE *alias = fopen("alias.txt", "a");
        char *name_of_alias = strstr(argv[2], ".");
        memmove(name_of_alias, name_of_alias + 1 , strlen(name_of_alias));
        char alias_command[100];
        strcpy(alias_command, argv[3]);
        if(!isvalid_command(alias_command)){
            perror("\nInvalid command \n");
            exit(1);
        }
        else{
            char buffer[1000];
            strcpy(buffer, name_of_alias);
            strcat(buffer, " ");
            strcat(buffer, alias_command);
            strcat(buffer, "\n");
            fputs(buffer, alias);
        }
             fclose(alias);
     }
 printf("\n The alias is set\n");
}

void Set_Config(char **argv){
    FILE *config = NULL;
    if(!strcmp(argv[2], "-global")){
        if(!strcmp(argv[3], "user.name")) config = fopen("/username.txt", "w");
        else if(!strcmp(argv[3], "user.email")) config = fopen("/useremail.txt", "w");
         else {
            config_alias(argv);
            return;
        }
    }
        else if(!strcmp(argv[2], "user.name")) config = fopen("username.txt", "w");
        else if(!strcmp(argv[2], "user.email")) config = fopen("useremail.txt", "w");
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

int load_alias(char *alias_name) {
    char line[300];
    FILE *alias;

    if ((alias = fopen("alias.txt", "r")) != NULL) {
        while (fgets(line, 300, alias) != NULL) {
            char *aliases = strtok(line, " ");
            char *command = strtok(NULL, "\n");
            if (!strcmp(aliases, alias_name)) {
                system(command);
                fclose(alias);
                return 0;
            }
        }
        fclose(alias);
    }

    if ((alias = fopen("/alias.txt", "r")) != NULL) {
        while (fgets(line, 300, alias) != NULL) {
            char *aliases = strtok(line, " ");
            char *command = strtok(NULL, "\n");
            if (!strcmp(aliases, alias_name)) {
                system(command);
                fclose(alias);
                return 0;
            }
        }
        fclose(alias);
    }

    return -1;
}


void status(){
      FILE *fp;
  char buffer[256];
  char cwd[256];
  getcwd(cwd, sizeof(cwd));
  strcat(cwd, "/.sgit/.staging");
  char command[512];
  sprintf(command, "diff -srq  %s . ", cwd);
  int status;
  fp = popen(command, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening pipe\n");
    return 1;
  }
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    char temp[1024];
    sscanf(buffer, "%s ", temp);
    //The file is in both directories but Deleted or OK ?
    if(strcmp(temp, "Only")){
      sscanf(buffer + 6, "%s ", temp);
      char *name = strrchr(temp, '/');
      if(*name == '.') continue;
    char *situation = strrchr(buffer , ' ');
    situation++;
    //Modified
     if(!strcmp(situation, "differ\n")){
        printf("%s : +M \n", name+1);
     }
     //As the same
     else if(!strcmp(situation, "identical\n")){
        printf("%s : ++ \n", name+1);
     }
    }
    //Only in one directory
    else{
        char name[256];
        sscanf(buffer, "%*s %*s %*s %s ", name);
        //ignore  . and .. and hidden directories
        if(*name == '.') continue;
        //A or D ?
        if (buffer[8] == '.') {
          printf("%s : -A \n", name);
        }
        else if(buffer[8] == '/') {
          printf("%s : +D \n", name);
        }
    }
  }
  pclose(fp);  
}


int main(int argc, char **argv){
     char main_command[100]; strcpy(main_command, argv[1]);
    //init>>
    if(!strcmp(main_command, "init")) {
        create_hidden_dir();
     return 0;
     }
     //config>>
     if(!strcmp(main_command, "config")){
       Set_Config(argv);
     }
      //add>>
      else if(!strcmp(main_command, "add")){
          if(!strcmp(argv[2],"-f")) {
            for (int i = 3 ;i < argc;i++)
            add(argv[i]);
         }
         else if(!strcmp(argv[2], "-n")){
           check_with_staging_area();
         }
         else add(argv[2]);
      }
     //reset>>
     else if(!strcmp(main_command, "reset")) {
        reset(argv[2]);
     }
     //status>>
     else if(!strcmp(main_command, "status")){
        status();
     }
     //commit>>
     
     
     //maybe it is an alias>>
     else if(!load_alias(argv[1])) {
        printf("\n Yes this was an alias\n");
     }
    // ??? Invalid ???
     else{
        printf("\nInvalid\n");
         exit(1);
     }
   return 0;
 }