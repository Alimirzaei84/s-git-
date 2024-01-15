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