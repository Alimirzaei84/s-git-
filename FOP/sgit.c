#include "information.h"

void create_hidden_dir()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error getting current working directory");
        return;
    }
    int found = 0;
    char path[1024];
    strcpy(path, cwd);
    while (1)
    {
        strcat(path, "/.sgit");
        if (dir_exists(path))
        {
            printf("\nError : Already a repository exists in %s\n\n", path);
            found = 1;
            break;
        }
        path[strlen(path) - 6] = '\0';
        if (strcmp(path, "/") == 0)
        {
            break;
        }
        strcpy(path, dirname(path));
    }
    if (!found)
    {

        strcpy(path, cwd);
        strcat(path, "/.sgit");
        mkdir(path, 0777);

        char path_of_commits[1024];
        char path_of_branches[1024];

        strcpy(path_of_commits, cwd);
        strcat(path_of_commits, "/.sgit/.commits");
        mkdir(path_of_commits, 0777);

        strcpy(path_of_branches, cwd);
        strcat(path_of_branches, "/.sgit/.branches");
        mkdir(path_of_branches, 0777);

        FILE *fp = fopen(".sgit/.commits/hash_commit.txt", "w");
        if (fp == NULL)
        {
            exit(1);
        }
        fprintf(fp, "%d", 100);
        fclose(fp);
        fp = fopen(".sgit/.commits/log.txt", "w");
        if (fp == NULL)
        {
            exit(1);
        }
        fclose(fp);

        fp = fopen(".sgit/.branches/current_branch.txt", "w");
        if (fp == NULL)
        {
            exit(1);
        }
        fprintf(fp, "Master");
        fclose(fp);

        fp = fopen(".sgit/.branches/BranchesList.txt", "w");
        if (fp == NULL)
        {
            exit(1);
        }
        fprintf(fp, "0|Master\n");
        fclose(fp);

        fp = fopen(".sgit/valid.txt", "w");
        if (fp == NULL)
        {
            exit(1);
        }
        fprintf(fp, "config\nadd\nreset\nstatus\ncommit\nset\nreplace\nremove\nlog\nbranch\ncheckout\nrevert\ntag\nstash\ngrep\ndiff\nmerge\n");
        fclose(fp);

        mkdir(".sgit/.precommit", 0777);

        fp = fopen(".sgit/.precommit/hooks.txt", "w");
        if (fp == NULL)
        {
            perror("89\n");
            exit(1);
        }
        fprintf(fp, "0 todo-check\n0 character-limit\n0 static-error-check\n0 file-size-check\n");
        fclose(fp);

        fp = fopen(".sgit/.precommit/situation.txt", "w");
        if (fp == NULL)
        {
            exit(1);
        }
        fprintf(fp, "TRUE");
        fclose(fp);

        printf("Created a local repository in %s\n", path);
    }
    else
        printf("error!!\n");
}

void add(char *name)
{
    // char input[1024];
    // if (realpath(name, input) == NULL)
    // {
    //     printf("\nError resolving input path\n");
    //     return;
    // }
    // if (access(input, F_OK) != 0)
    // {
    //     perror("Error accessing input path");
    //     return;
    // }
    // Find_a_Repository char cwd[1024];
    // if (getcwd(cwd, sizeof(cwd)) == NULL)
    // {
    //     perror("Error getting current working directory");
    //     return;
    // }
    // char dest[100];
    // strcpy(dest, cwd);
    // strcat(dest, "/.sgit/.staging");
    // if (access(dest, F_OK) != 0)
    // {
    //     mkdir(dest, 0777);
    // }
    // puts(input);
    // puts(dest);
    // //copy(input, dest);
    // printf("Add with success\n");
    if (!file_exists(name) && !dir_exists(name))
    {
        printf("\nError resolving input path\n");
        return;
    }
    char cmd[256];
    if (!dir_exists(".sgit/.staging"))
        mkdir(".sgit/.staging", 0777);
    sprintf(cmd, "cp --parents  -uR %s .sgit/.staging/", name);
    if (!system(cmd))
    {
        printf("Add with success\n");
    }
}

void check_with_staging_area()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error getting current working directory");
        return;
    }
    char base[2048];
    Find_a_Repository
        sprintf(base, ".sgit/.staging");
    DIR *dir = opendir(cwd);
    if (dir == NULL)
    {
        perror("Error opening current directory");
        exit(1);
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        char temp[2048];
        sprintf(temp, "%s/%s", base, entry->d_name);
        if (!access(temp, F_OK))
        {
            printf("\n*** %s is in staging area ***\n", entry->d_name);
        }
        else
        {
            printf("\n%s is not staged\n", entry->d_name);
        }
    }
}

void reset(char *path)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error getting current working directory");
        return;
    }

    /* reset -undo */
    if (!strcmp(path, "-undo"))
    {
        // char *last_path = get_last_file_or_directory(".sgit/.staging");
        // if (last_path == NULL)
        // {
        //     perror("Try again\n");
        //     exit(1);
        // }

        // char cmd[256];
        // sprintf(cmd, "rm -r %s ", last_path);
        // if (!system(cmd))
        // {
        //     printf("Resetting the last file or directory was Successfull\n");
        // }

        // free(last_path);
        remove_last_files_or_directories(".sgit/.staging");
        return;
    }

    /*classis reset*/
    char full_path[1024];
    sprintf(full_path, "%s/.sgit/.staging/%s", cwd, path);
    char buffer[1024];
    sprintf(buffer, "rm -r %s ", full_path);
    if (system(buffer) == 0)
    {
        printf("\nreset is done\n");
    }
    else
    {
        perror("\ncaan not reset this\n");
    }
}

void config_alias(char **argv)
{
    if (!strcmp(argv[2], "-global"))
    {
        FILE *alias = fopen("/mnt/c/Users/hadij/alias.txt", "a");
        char *name_of_alias = strtok(argv[3], ".");
        memmove(name_of_alias, name_of_alias + 1, strlen(name_of_alias));
        char alias_command[100];
        strcpy(alias_command, argv[4]);
        if (!isvalid_command(alias_command))
        {
            perror("\nInvalid command \n");
            exit(1);
        }
        else
        {
            char buffer[1000];
            strcpy(buffer, name_of_alias);
            strcat(buffer, " ");
            strcat(buffer, alias_command);
            strcat(buffer, "\n");
            fputs(buffer, alias);
        }
        fclose(alias);
    }
    else
    {
        FILE *alias = fopen(".sgit/alias.txt", "a");
        char *name_of_alias = strstr(argv[2], ".");
        memmove(name_of_alias, name_of_alias + 1, strlen(name_of_alias));
        char alias_command[100];
        strcpy(alias_command, argv[3]);
        if (!isvalid_command(alias_command))
        {
            perror("\nInvalid command \n");
            exit(1);
        }
        else
        {
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

void Set_Config(char **argv)
{
    char cwd[1024];
    getcwd(cwd, 1000);
    puts(cwd);
    FILE *config = NULL;
    if (!strcmp(argv[2], "-global"))
    {
        if (!strcmp(argv[3], "user.name"))
            config = fopen("/mnt/c/Users/hadij/username.txt", "w");
        else if (!strcmp(argv[3], "user.email"))
            config = fopen("/mnt/c/Users/hadij/useremail.txt", "w");
        else
        {
            config_alias(argv);
            return;
        }
    }
    else if (!strcmp(argv[2], "user.name"))
        config = fopen(".sgit/username.txt", "w");
    else if (!strcmp(argv[2], "user.email"))
        config = fopen(".sgit/useremail.txt", "w");
    else
    {
        config_alias(argv);
        return;
    }
    if (config == NULL)
    {
        printf("Try again!\n");
        exit(1);
    }
    else
    {
        if (!strcmp(argv[2], "-global"))
        {
            fputs(argv[4], config);
        }
        else
        {
            fputs(argv[3], config);
        }
        fclose(config);
        printf("The config is now set\n");
    }
}

int load_alias(char *alias_name)
{
    char line[300];
    FILE *alias;

    if ((alias = fopen(".sgit/alias.txt", "r")) != NULL)
    {
        while (fgets(line, 300, alias) != NULL)
        {
            char *aliases = strtok(line, " ");
            char *command = strtok(NULL, "\n");
            if (!strcmp(aliases, alias_name))
            {
                system(command);
                fclose(alias);
                return 0;
            }
        }
        fclose(alias);
    }

    if ((alias = fopen("/mnt/c/Users/hadij/alias.txt", "r")) != NULL)
    {
        while (fgets(line, 300, alias) != NULL)
        {
            char *aliases = strtok(line, " ");
            char *command = strtok(NULL, "\n");
            if (!strcmp(aliases, alias_name))
            {
                system(command);
                fclose(alias);
                return 0;
            }
        }
        fclose(alias);
    }

    return -1;
}

// void status()
// {
//     FILE *fp;
//     char buffer[256];
//     char cwd[256];
//     getcwd(cwd, sizeof(cwd));
//     strcat(cwd, "/.sgit/.staging");
//     if (!dir_exists(cwd))
//     {
//         printf("Ther is not anything in staging area!!\nSo we compare with current commit\n\n");
//         int commit_ID;
//         FILE *current_commit = fopen(".sgit/.branches/CurrentCommitID.txt", "r");
//         if (current_commit == NULL)
//         {
//             printf("Try again!\n");
//             exit(1);
//         }
//         fscanf(current_commit, "%d", &commit_ID);
//         fclose(current_commit);
//         char cwd2[256];
//         getcwd(cwd2, sizeof(cwd2));
//         sprintf(cwd, "%s/.sgit/.commits/number%d", cwd2, commit_ID);
//     }
//     char command[512];
//     sprintf(command, "diff -srq  %s . ", cwd);
//     fp = popen(command, "r");
//     if (fp == NULL)
//     {
//         fprintf(stderr, "Error opening pipe\n");
//         exit(1);
//     }
//     while (fgets(buffer, sizeof(buffer), fp) != NULL)
//     {
//         char temp[1024];
//         sscanf(buffer, "%s ", temp);
//         // The FILE is in both directories but Deleted or OK ?
//         if (strcmp(temp, "Only"))
//         {
//             sscanf(buffer + 6, "%s ", temp);
//             char *name = strrchr(temp, '/');
//             if (*name == '.')
//                 continue;
//             char *situation = strrchr(buffer, ' ');
//             situation++;
//             // Modified
//             if (!strcmp(situation, "differ\n"))
//             {
//                 printf("%s : +M \n", name + 1);
//             }
//             // As the same
//             else if (!strcmp(situation, "identical\n"))
//             {
//                 printf("%s : +A \n", name + 1);
//             }
//         }
//         // Only in one directory
//         else
//         {
//             char name[256];
//             sscanf(buffer, "%*s %*s %*s %s ", name);
//             // ignore  . and .. and hidden directories
//             if (*name == '.')
//                 continue;
//             // A or D ?
//             if (buffer[8] == '.')
//             {
//                 printf("%s : -A \n", name);
//             }
//             else if (buffer[8] == '/')
//             {
//                 printf("%s : +D \n", name);
//             }
//         }
//     }
//     pclose(fp);
// }

void status()
{
    FILE *cmd = popen("find ./* ", "r");
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), cmd))
    {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove the '\n'

        memmove(buffer, buffer + 2, strlen(buffer) - 1);

        Find_a_Repository

            if (!search_file(".sgit", buffer))
        {
            printf("-A   %s\n", buffer);
            continue;
        }

        else if ((!dir_exists(".sgit/.commits") || !search_file(".sgit/.commits", buffer)) && search_file(".sgit/.staging", buffer))
        {
            printf("+A   %s\n", buffer);
            continue;
        }

        // find file in the last commit of that
        FILE *fp = fopen(".sgit/.commits/log.txt", "r");
        if (fp == NULL)
        {
            printf("try again\n");
            exit(1);
        }
        char path_to_commit[256];
        char line[256];
        int found = false;
        while (fgets(line, 200, fp))
        {
            if (!strncmp(line, "number", strlen("number")))
            {
                while (1)
                {
                    fgets(path_to_commit, 200, fp);
                    if (!strcmp(path_to_commit, "\n"))
                        break;
                    path_to_commit[strcspn(path_to_commit, "\n")] = '\0';
                    if (!strcmp(path_to_commit, buffer))
                    {
                        found = true;
                        break;
                        fclose(fp);
                    }
                }
            }
            if (found)
                break;
        }
        line[strcspn(line, "\n")] = '\0';
        char path[256] = ".sgit/.commits/";
        strcat(path, line);
        strcat(path, "/");
        strcat(path, buffer);
        if (!compare_files(path, buffer))
        {
            if (search_file(".sgit/.staging", buffer))
                printf("+");
            else
                printf("-");
            printf("M   %s\n", buffer);
        }
    }
    fclose(cmd);

    // check for D option
    cmd = fopen(".sgit/.commits/log.txt", "r");
    if (cmd == NULL)
    {
        printf("try again\n");
    }

    // ignore 6 line
    for (int i = 0; i < 6; ++i)
        fgets(buffer, 4000, cmd);

    while (fgets(buffer, 4000, cmd))
    {
        if (!strcmp(buffer, "\n"))
            break;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (!file_exists(buffer))
        {
            if (search_file(".sgit/.staging", buffer))
                printf("+D   ");
            else
                printf("-D    ");
            puts(buffer);
        }

        if (!strcmp(buffer, "\n"))
            break;
    }

    fclose(cmd);
}

int commit(char *mess, char *swith)
{

    Find_a_Repository

        char cwd[1024];
    char path_of_staging[1024];
    char path_of_commits[1024];
    char buffer[2048];

    if (!strcmp(swith, "-s"))
    {
        Find_shortcut_message(mess);
    }

    getcwd(cwd, sizeof(cwd));
    strcpy(path_of_staging, cwd);
    strcat(path_of_staging, "/.sgit/.staging");
    if (!dir_exists(path_of_staging))
    {
        perror("Please add something\n");
        return -1;
    }
    if (strlen(mess) > 72)
    {
        perror("The message is too long\n");
        return -1;
    }
    if (strlen(mess) < 3)
    {
        perror("The message is too short\n");
        return -1;
    }
    if (!precommit_situation())
    {
        perror("\n!!Warning!!\nYou have some failed hooks!\n\n");
        exit(1);
    }
    int commit_ID = 0;
    FILE *hash = fopen(".sgit/.commits/hash_commit.txt", "r");
    if (hash == NULL)
    {
        perror("Try again later\n");
        return -1;
    }
    fscanf(hash, "%d", &commit_ID);
    fclose(hash);
    hash = fopen(".sgit/.commits/hash_commit.txt", "w");
    commit_ID++;
    fprintf(hash, "%d", commit_ID);
    fclose(hash);
    FILE *current_commit = fopen(".sgit/.branches/CurrentCommitID.txt", "w");
    fprintf(current_commit, "%d", commit_ID);
    fclose(current_commit);

    sprintf(path_of_commits, ".sgit/.commits/number%d", commit_ID);

    FILE *original_FILE = fopen(".sgit/.commits/log.txt", "r");
    FILE *temp_FILE = fopen(".sgit/.commits/temp.txt", "w");
    char *name = find_name();
    char *email = find_email();
    char *branch = find_current_branch();
    fprintf(temp_FILE, "number%d\n%s\n%s\n%s\n%s\n", commit_ID, branch, mess, name, email);
    free(name);
    free(branch);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(temp_FILE, "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    char *dir = ".sgit/.staging";
    char *command = "find ";
    char *options = "-type f -printf \"%p\\n\"";
    char *full_command = malloc(strlen(command) + strlen(dir) + strlen(options) + 1);
    sprintf(full_command, "%s%s %s", command, dir, options);
    FILE *fp;
    char path[1035];
    fp = popen(full_command, "r");
    if (fp == NULL)
    {
        printf("Failed to run command\n");
        exit(1);
    }
    while (fgets(path, sizeof(path) - 1, fp))
    {
        char *tok = strchr(path, '/');
        tok++;
        tok = strchr(tok, '/');
        tok++;
        fputs(tok, temp_FILE);
    }
    fprintf(temp_FILE, "\n");

    char bufffer[1024];
    while (fgets(bufffer, sizeof(bufffer), original_FILE))
    {
        fprintf(temp_FILE, "%s", bufffer);
    }
    fclose(temp_FILE);

    remove(".sgit/.commits/log.txt");
    rename(".sgit/.commits/temp.txt", ".sgit/.commits/log.txt");

    pclose(fp);
    free(full_command);
    sprintf(buffer, "mv %s* %s ", path_of_staging, path_of_commits);
    system(buffer);
    return 0;
}

void LOG(int argc, char **argv)
{
    Find_a_Repository

        size_t count_of_commits = 1;
    FILE *log = fopen(".sgit/.commits/log.txt", "r");
    if (!log)
    {
        perror("Error opening log FILE.");
        exit(1);
    }
    char line[1024];
    char line_branch[1024];
    char line_message[1024];
    char line_name[1024];
    char line_email[1024];
    char line_time[1024];
    bool flag = false;
    while (fgets(line, sizeof(line), log))
    {
        if (!strncmp(line, "number", 6))
        {
            flag = false;
            fgets(line_branch, sizeof(line), log);
            fgets(line_message, sizeof(line), log);
            fgets(line_name, sizeof(line), log);
            fgets(line_email, sizeof(line), log);
            fgets(line_time, sizeof(line), log);

            int count_of_FILEs = 0;
            char temp_line[1024];
            do
            {
                fgets(temp_line, sizeof(line), log);
                count_of_FILEs++;
            } while (strcmp(temp_line, "\n"));
            count_of_FILEs--;

            static struct tm date1 = {0};
            static struct tm date2 = {0};
            char date_of_commit[128];
            strncpy(date_of_commit, line_time, 19);
            date_of_commit[19] = '\0';
            strncpy(date_of_commit, line_time, 10);
            strptime(date_of_commit, "%Y-%m-%d", &date1);
            strptime(argv[3], "%Y-%m-%d", &date2);
            double diff = difftime(mktime(&date2), mktime(&date1));

            if (argc == 2)
            {
                flag = true;
            }
            else if (!strcmp(argv[2], "-n") && (count_of_commits <= atoi(argv[3])))
            {
                flag = true;
                count_of_commits++;
            }
            else if (!strcmp(argv[2], "-branch") && strstr(line_branch, argv[3]))
            {
                flag = true;
            }
            else if (!strcmp(argv[2], "-author") && strstr(line_name, argv[3]))
            {
                flag = true;
            }
            else if (!strcmp(argv[2], "-search") && strstr(line_message, argv[3]))
            {
                flag = true;
            }
            else if (!strcmp(argv[2], "-before") && (diff >= 0))
            {
                flag = true;
            }
            else if (!strcmp(argv[2], "-since") && (diff <= 0))
            {
                flag = true;
            }

            if (flag)
            {
                printf("\n********************\n");
                printf("Commit with Id %s ", line);
                printf("Commit message : %s ", line_message);
                printf("Author name : %s ", line_name);
                printf("Author email : %s ", line_email);
                printf("DATE and Time of Commit : %s ", line_time);
                printf("In branch : %s ", line_branch);
                printf("number of FILEs in this commit : %d \n", count_of_FILEs);
                printf("\n********************\n");
                printf("\n\n");
            }
        }
    }

    fclose(log);
}

void Run_branch_command(int argc, char **argv)
{
    if (argc == 2)
    {
        show_branches_list();
    }
    else
    {
        Find_a_Repository

            bool flag = false;
        FILE *fp = fopen(".sgit/.branches/BranchesList.txt", "a+");
        if (fp == NULL)
        {
            exit(1);
        }

        rewind(fp);
        char line[1024];
        char name_of_branch[128];
        strcpy(name_of_branch, argv[2]);

        while (fgets(line, sizeof(line), fp))
        {
            char *name = strchr(line, '|');
            name++;
            if (!strncmp(name, name_of_branch, strlen(name_of_branch)))
            {
                flag = true;
            }
        }

        if (flag)
        {
            printf("Ther is already a branch with this name!\n");
            exit(0);
        }

        int number = count_of_master_commits_upto_now();
        fprintf(fp, "%d|%s\n", number, name_of_branch);

        fclose(fp);
    }
}

void grep(int argc, char **argv)
{
    bool flag_n = false;
    if ((argc == 7 || argc == 9) && (!strcmp(argv[argc - 1], "-n")))
    {
        flag_n = true;
    }

    char file_path[512];
    if ((argc > 6) && (!strcmp(argv[6], "-c")))
    {
        sprintf(file_path, ".sgit/.commits/number%d/%s", atoi(argv[7]), argv[3]);
    }
    else
    {
        strcpy(file_path, argv[3]);
    }

    size_t count = 0;
    char line[1024];
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        exit(1);
    }

    char *word = argv[5];
    while (fgets(line, sizeof(line), fp))
    {
        count++;
        if (strstr(line, word))
        {
            if (flag_n)
            {
                printf("In line : %lu", count);
            }
            printf("--> ");
            print_word_in_another_colour(word, line);
            printf("\n");
        }
    }
}

void check_out(char *dest)
{
    char cwd[512];
    getcwd(cwd, sizeof(cwd));

    Find_a_Repository

        if (dir_exists(".sgit/.staging"))
    {
        perror("Please commit your changes first!!\n");
        exit(1);
    }

    // If this is HEAD
    if (!strcmp(dest, "HEAD"))
    {
        check_out("Master");
    }

    // If this is a commit ID>>
    else if (isnumber(dest))
    {
        FILE *fp = fopen(".sgit/.commits/log.txt", "r");
        if (fp == NULL)
        {
            exit(1);
        }
        char number100[256] = "number";
        strcat(number100, dest);
        strcat(number100, "\n");
        char buffer[512];
        while (fgets(buffer, sizeof(buffer), fp))
        {
            if (!strcmp(buffer, number100))
            {
                fgets(buffer, sizeof(buffer), fp);
                fgets(buffer, sizeof(buffer), fp);
                fgets(buffer, sizeof(buffer), fp);
                fgets(buffer, sizeof(buffer), fp);
                fgets(buffer, sizeof(buffer), fp);

                while (1)
                {
                    fgets(buffer, sizeof(buffer), fp);
                    if (!strcmp(buffer, "\n"))
                    {
                        break;
                    }
                    buffer[strcspn(buffer, "\n")] = '\0';
                    number100[strcspn(number100, "\n")] = '\0';
                    char command[512];
                    sprintf(command, "rm %s ", buffer);
                    if (file_exists(buffer) || dir_exists(buffer))
                        system(command);
                    sprintf(command, "cp -ur .sgit/.commits/%s/. %s ", number100, cwd);
                    system(command);
                }
                break;
            }
        }
        fclose(fp);
        FILE *current_commit = fopen(".sgit/.branches/CurrentCommitID.txt", "w");
        fprintf(current_commit, "%d", atoi(dest));
        fclose(current_commit);
    }

    // If this is a Branch name>>
    else
    {
        int ID_of_head_branch = HEAD_of_branch(dest);
        char temp[256];
        sprintf(temp, "%d", ID_of_head_branch);
        check_out(temp);
        FILE *fp = fopen(".sgit/.branches/current_branch.txt", "w");
        if (fp == NULL)
        {
            exit(1);
        }
        fputs(dest, fp);
        fclose(fp);
    }
}

void stash_command(int argc, char **argv)
{
    if (!strcmp(argv[2], "push"))
    {
        push_stash(argc, argv);
    }
    else if (!strcmp(argv[2], "list"))
    {
        push_list();
    }
    else if (!strcmp(argv[2], "show"))
    {
        show_stash_diff();
    }
    else if (!strcmp(argv[2], "pop"))
    {
        if (revert_to_backup("/mnt/c/Users/hadij/b/.sgit/.stash", ".") == 0)
        {
            printf("Pop was successful\nAnd\n");
            clear_stash();
        }
    }
    else if (!strcmp(argv[2], "clear"))
    {
        clear_stash();
    }
}

void Run_tag(int argc, char **argv)
{
    if (argc == 2)
    {
        tag_list();
    }
    else if (!strcmp(argv[2], "show"))
    {
        show_tag(argv[3]);
    }
    else
    {
        bool owerwrite = false;
        if (!strcmp(argv[argc - 1], "-f"))
            owerwrite = true;
        int commit_ID = find_current_commitID();
        if ((argc > 4) && !strcmp(argv[4], "-c"))
            commit_ID = atoi(argv[5]);
        else if ((argc > 6) && !strcmp(argv[6], "-c"))
            commit_ID = atoi(argv[7]);
        if ((argc > 4) && !strcmp(argv[4], "-m"))
        {
            append_tag(argv[3], argv[5], commit_ID, owerwrite);
            return;
        }
        append_tag(argv[3], "", commit_ID, owerwrite);
    }
}

void hook(int argc, char **argv)
{
    if (argc == 4)
    {
        bool all_or_applied = false;
        if (!strcmp(argv[3], "list"))
        {
            all_or_applied = true;
        }
        show_hooks(all_or_applied);
    }
    else
    {
        int situation = 0;
        if (!strcmp(argv[2], "add"))
        {
            situation = 1;
        }
        change_list_of_hooks(situation, argv[4]);
    }
}

void revert(int argc, char **argv)
{

    FILE *fp = fopen(".sgit/.commits/log.txt", "r");
    if (fp == NULL)
        exit(1);
    bool need_to_new_commit = true;
    char commit_ID[128] = "";
    char message[128] = "";
    char buffer[512];
    if (!strcmp(argv[2], "-n"))
    {
        need_to_new_commit = false;
        if (argc == 4)
            strcpy(commit_ID, argv[3]);
        else
        {
            fgets(buffer, sizeof(buffer), fp); // read the last commit ID
            sscanf(buffer, "number%s\n", commit_ID);
        }
    }
    else
    {
        if (!strcmp(argv[2], "-m"))
            strcpy(message, argv[3]);

        if (!strncmp(argv[argc - 1], "HEAD", strlen("HEAD")))
        {
            int X = 0;
            sscanf(argv[argc - 1], "HEAD-%d", &X);
            // find the Xth commit ID from now
            rewind(fp);
            size_t count = 0;
            while (fgets(buffer, sizeof(buffer), fp))
            {
                if (!strncmp(buffer, "number", strlen("number")))
                {
                    count++;
                    if (count == X)
                    {
                        sscanf(buffer, "number%s\n", commit_ID);
                        break;
                    }
                }
            }
        }
        else
        {
            strcpy(commit_ID, argv[argc - 1]);
        }

        if (!strlen(message)) // if we don't have '-m', we should find commit message with commit ID to can commit
        {
            rewind(fp);
            while (fgets(buffer, sizeof(buffer), fp))
            {
                if (!strncmp(buffer, "number", strlen("number")))
                {
                    char temp[128];
                    sprintf(temp, "number%s\n", commit_ID);
                    if (!strcmp(temp, buffer))
                    {
                        fgets(buffer, sizeof(buffer), fp); // Ignore branch name
                        fgets(message, sizeof(message), fp);
                        message[strcspn(message, "\n")] = '\0';
                        break;
                    }
                }
            }
        }
    }

    fclose(fp);

    // check_out(commit_ID)
    char cwd[512];
    getcwd(cwd, sizeof(cwd));
    fp = fopen(".sgit/.commits/log.txt", "r");
    if (fp == NULL)
    {
        exit(1);
    }
    char number100[256] = "number";
    strcat(number100, commit_ID);
    strcat(number100, "\n");
    while (fgets(buffer, sizeof(buffer), fp))
    {
        if (!strcmp(buffer, number100))
        {
            fgets(buffer, sizeof(buffer), fp);
            fgets(buffer, sizeof(buffer), fp);
            fgets(buffer, sizeof(buffer), fp);
            fgets(buffer, sizeof(buffer), fp);
            fgets(buffer, sizeof(buffer), fp);

            while (1)
            {
                fgets(buffer, sizeof(buffer), fp);
                if (!strcmp(buffer, "\n"))
                {
                    break;
                }
                buffer[strcspn(buffer, "\n")] = '\0';
                number100[strcspn(number100, "\n")] = '\0';
                char command[512];
                if (file_exists(buffer) || dir_exists(buffer))
                    sprintf(command, "rm %s ", buffer);
                system(command);
                sprintf(command, "cp -ur .sgit/.commits/%s/. %s ", number100, cwd);
                system(command);
            }
            break;
        }
    }
    fclose(fp);

    /* If we do not have '-n' flag, should do a new commit of working directory to the repository. */
    puts(message);
    if (need_to_new_commit)
    {
        char cwd[1024];

        char path_of_commits[1024];
        char buffer[2048];

        getcwd(cwd, sizeof(cwd));

        if (strlen(message) > 72)
        {
            perror("The message is too long\n");
            exit(1);
        }
        if (strlen(message) < 3)
        {
            perror("The message is too short\n");
            exit(1);
            ;
        }
        // if(!precommit_situation())
        // {
        //     perror("\n!!Warning!!\nYou have some failed hooks!\n\n");
        //     exit(1);
        // }
        int commitID = 0;
        FILE *hash = fopen(".sgit/.commits/hash_commit.txt", "r");
        if (hash == NULL)
        {
            perror("Try again later\n");
            exit(1);
        }
        fscanf(hash, "%d", &commitID);
        fclose(hash);
        hash = fopen(".sgit/.commits/hash_commit.txt", "w");
        commitID++;
        fprintf(hash, "%d", commitID);
        fclose(hash);
        FILE *current_commit = fopen(".sgit/.branches/CurrentCommitID.txt", "w");
        fprintf(current_commit, "%d", commitID);
        fclose(current_commit);

        sprintf(path_of_commits, ".sgit/.commits/number%d", commitID);

        FILE *original_FILE = fopen(".sgit/.commits/log.txt", "r");
        FILE *temp_FILE = fopen(".sgit/.commits/temp.txt", "w");
        char *name = find_name();
        char *email = find_email();
        char *branch = find_current_branch();
        fprintf(temp_FILE, "number%d\n%s\n%s\n%s\n%s\n", commitID, branch, message, name, email);
        free(name);
        free(branch);
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(temp_FILE, "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        char *dir = ".";
        char *command = "find ";
        char *options = "-type f -printf \"%p\\n\"";
        char *full_command = malloc(strlen(command) + strlen(dir) + strlen(options) + 1);
        sprintf(full_command, "%s%s %s", command, dir, options);
        FILE *fp;
        char path[1035];
        fp = popen(full_command, "r");
        puts(full_command);
        if (fp == NULL)
        {
            printf("Failed to run command\n");
            exit(1);
        }
        while (fgets(path, sizeof(path) - 1, fp))
        {
            puts(path);
            char *tok = strchr(path, '/');
            tok++;
            if (tok[0] == '.')
                continue;
            fputs(tok, temp_FILE);
        }
        fprintf(temp_FILE, "\n");

        char bufffer[1024];
        while (fgets(bufffer, sizeof(bufffer), original_FILE))
        {
            fprintf(temp_FILE, "%s", bufffer);
        }
        fclose(temp_FILE);

        remove(".sgit/.commits/log.txt");
        rename(".sgit/.commits/temp.txt", ".sgit/.commits/log.txt");

        pclose(fp);
        free(full_command);
        sprintf(buffer, "rsync -a --exclude=\".*\" %s   %s/ ", cwd, path_of_commits); // commit(message, "-m");
        system(buffer);
    }
}

int main(int argc, char **argv)
{
    char main_command[100];
    strcpy(main_command, argv[1]);
    // Find_a_Repository
    //  init>>
    if (!strcmp(main_command, "init"))
    {
        create_hidden_dir();
        return 0;
    }
    // config>>
    if (!strcmp(main_command, "config"))
    {
        Set_Config(argv);
    }
    // add>>
    else if (!strcmp(main_command, "add"))
    {

        if (!strcmp(argv[2], "-n"))
        {
            check_with_staging_area();
            return 0;
        }

        for (int i = 2; i < argc; i++)
        {
            if (!strcmp(argv[i], "-f"))
                continue;
            add(argv[i]);
        }
    }
    // reset>>
    else if (!strcmp(main_command, "reset"))
    {
        if (!strcmp(argv[2], "-undo"))
            reset(argv[2]);
        else
        {
            for (int i = 2; i < argc; i++)
            {
                if (!strcmp(argv[i], "-f"))
                    continue;
                reset(argv[i]);
            }
        }
    }
    // status>>
    else if (!strcmp(main_command, "status"))
    {
        status();
    }
    // commit>>
    else if (!strcmp(main_command, "commit"))
    {
        if (argc < 4)
        {
            printf("Commit message please!\n");
            return 0;
        }
        if (commit(argv[3], argv[2]) == 0)
        {
            printf("Commit successfully\n");
            system("sgit log -n 1 ");
        }
    }
    // About shortcut messages>>
    // set_shortcut
    else if (!strcmp(main_command, "set"))
    {
        set_shortcut(argv);
    }
    // replace_shortcut
    else if (!strcmp(main_command, "replace"))
    {
        replace_shortcut(argv);
    }
    // remove_shortcut
    else if (!strcmp(main_command, "remove"))
    {
        remove_shortcut(argv);
    }
    // Log>>
    else if (!strcmp(main_command, "log"))
    {
        LOG(argc, argv);
    }
    // Branch command>>
    else if (!strcmp(main_command, "branch"))
    {
        Run_branch_command(argc, argv);
    }
    // Check out>>
    else if (!strcmp(main_command, "checkout"))
    {
        check_out(argv[2]);
    }
    // grep>>
    else if (!strcmp(main_command, "grep"))
    {
        grep(argc, argv);
    }
    // diff_command>>
    else if (!strcmp(main_command, "diff"))
    {
        if (!strcmp(argv[2], "-f"))
        {
            if (argc == 5)
                diff(argv[3], argv[4], "1-10000", "1-10000");
            else
                diff(argv[3], argv[4], argv[6], argv[8]);
        }
        else if (!strcmp(argv[2], "-c"))
        {
            char name_commit1[256], name_commit2[256];
            sprintf(name_commit1, ".sgit/.commits/number%d", atoi(argv[3]));
            sprintf(name_commit2, ".sgit/.commits/number%d", atoi(argv[4]));
            diff_directories(name_commit1, name_commit2);
        }
    }
    // stash>>
    else if (!strcmp(main_command, "stash"))
    {
        stash_command(argc, argv);
    }
    // tag>>
    else if (!strcmp(main_command, "tag"))
    {
        Run_tag(argc, argv);
    }
    // pre-commit>>
    else if (!strcmp(main_command, "pre-commit"))
    {
        if (argc == 2)
        {
            Check_pre_commit();
        }
        else
            hook(argc, argv);
    }
    // Revert>>
    else if (!strcmp(main_command, "revert"))
    {
        revert(argc, argv);
    }
    // maybe it is an alias>>
    else if (!load_alias(argv[1]))
    {
        printf("\n Yes this was an alias\n");
    }
    // ??? Invalid ???
    else
    {
        printf("\nInvalid\n");
        exit(1);
    }
    return 0;
}