/* Include libraries */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libgen.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <glob.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define DEBUG printf("%d\n", __LINE__);

#define Find_a_Repository      \
  while (!dir_exists(".sgit")) \
    chdir("..");

// tree --sort ctime -I "*time*|-a"

/* Function Declaration */

int dir_exists(char *path)
{
  struct stat st;
  if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int file_exists(const char *path)
{
  return access(path, F_OK) != -1;
}

bool precommit_situation()
{
  FILE *fp = fopen(".sgit/.precommit/situation.txt", "r");
  if (fp == NULL)
  {
    perror("Try again\n");
    exit(1);
  }

  char sit[128];
  fscanf(fp, "%s", sit);
  fclose(fp);
  if (!strcmp(sit, "TRUE"))
    return true;
  else
    return false;
}

int compare_files(const char *file1, const char *file2)
{
  FILE *f1 = fopen(file1, "rb");
  FILE *f2 = fopen(file2, "rb");

  if (f1 == NULL || f2 == NULL)
  {
    puts(file1);
    puts(file2);
    perror("Error opening files");
    return -1;
  }

  int ch1, ch2;
  int is_same = 1;

  while (((ch1 = fgetc(f1)) != EOF) && ((ch2 = fgetc(f2)) != EOF))
  {
    if (ch1 != ch2)
    {
      is_same = 0;
      break;
    }
  }

  if (is_same && feof(f1) && feof(f2))
  {
    // printf("Files %s and %s have the same content\n", file1, file2);
  }
  else
  {
    // printf("Files %s and %s have different content\n", file1, file2);
  }

  fclose(f1);
  fclose(f2);

  return is_same;
}

int isFileSizeOver5MB(const char *filename)
{
  struct stat st;
  if (stat(filename, &st) == 0)
  {
    // File size in bytes
    long long fileSize = st.st_size;

    // Convert to megabytes (1 MB = 1024 * 1024 bytes)
    double fileSizeMB = (double)fileSize / (1024 * 1024);

    if (fileSizeMB > 5.0)
    {
      return 1; // File size is over 5 MB
    }
    else
    {
      return 0; // File size is not over 5 MB
    }
  }
  else
  {
    printf("Error accessing file: %s\n", filename);
    return -1; // Error occurred
  }
}

long count_characters_in_file(const char *filepath)
{
  FILE *file = fopen(filepath, "r");
  if (file == NULL)
  {
    printf("Error opening file %s\n", filepath);
    return -1;
  }

  long character_count = 0;
  char ch;
  while ((ch = fgetc(file)) != EOF)
  {
    character_count++;
  }

  fclose(file);
  return character_count;
}

int search_file(const char *dir_path, const char *file_name)
{
  DIR *d = opendir(dir_path);
  struct dirent *dir;
  int found = 0;

  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
      {
        char subdir_path[1024];
        snprintf(subdir_path, sizeof(subdir_path), "%s/%s", dir_path, dir->d_name);
        found = search_file(subdir_path, file_name);
        if (found)
        {
          closedir(d);
          return 1;
        }
      }
      else if (dir->d_type == DT_REG && strcmp(dir->d_name, file_name) == 0)
      {
        closedir(d);
        return 1;
      }
    }
    closedir(d);
  }

  return 0;
}

void print_word_in_another_colour(char *word, char *str)
{
  char *pos = strstr(str, word);
  int index = pos - str;

  printf("%.*s\033[1;31m%s\033[0m%s\n", index, str, word, pos + strlen(word));
}

int min(int a, int b)
{
  return (a < b) ? a : b;
}

int is_null_space(char *str)
{
  while (*str)
  {
    if (!isspace(*str))
    {
      return 0;
    }
    str++;
  }
  return 1;
}

void remove_last_files_or_directories(char *path)
{
  Find_a_Repository

      DIR *dir;
  struct dirent *ent;
  struct stat st;
  char **last_files = NULL;
  time_t last_time = 0;
  int count = 0;

  if ((dir = opendir(path)) != NULL)
  {
    while ((ent = readdir(dir)) != NULL)
    {
      char *full_path = (char *)malloc(strlen(path) + strlen(ent->d_name) + 2);
      sprintf(full_path, "%s/%s", path, ent->d_name); /* printf("%d\n", st.st_mtime);puts(full_path); */
      if (stat(full_path, &st) == 0 && (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode)) && ((st.st_mtime - last_time) > 0) && full_path[strlen(full_path) - 1] != '.')
      {
        if (st.st_mtime > last_time)
        {
          for (int i = 0; i < count; i++)
          {
            free(last_files[i]); // Free each file or directory path
          }
          // free(last_files); // Free the array of files or directories
          count = 0;
          last_time = min(st.st_mtime, last_time);
        }
        last_files = realloc(last_files, (count + 1) * sizeof(char *));
        last_files[count] = strdup(full_path);
        count++;
      }
      // free(full_path); // Free the memory allocated for full_path
    }
    closedir(dir);

    for (int i = 0; i < count; i++)
    {
      remove(last_files[i]); // Remove each file or directory
      free(last_files[i]);   // Free the memory allocated for each file or directory
    }
    // free(last_files); // Free the array of files or directories
  }
}

char *get_last_file_or_directory(char *path)
{
  DIR *dir;
  struct dirent *ent;
  struct stat st;
  char *last_file = NULL;
  time_t last_time = 0;

  if ((dir = opendir(path)) != NULL)
  {
    while ((ent = readdir(dir)) != NULL)
    {
      char *full_path = (char *)malloc(strlen(path) + strlen(ent->d_name) + 2);
      sprintf(full_path, "%s/%s", path, ent->d_name);
      if (stat(full_path, &st) == 0 && (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode)) && st.st_mtime > last_time && full_path[strlen(full_path) - 1] != '.')
      {
        last_time = st.st_mtime;
        free(last_file);
        last_file = strdup(full_path);
      }
      free(full_path);
    }
    closedir(dir);
  }

  if (last_file != NULL)
  {
    return last_file;
  }
  else
  {
    return NULL;
  }
}

int strcmp_ignore_spaces(char *s1, char *s2)
{
  s1[strcspn(s1, "\n")] = '\0';
  s2[strcspn(s2, "\n")] = '\0';
  while (*s1 && *s2)
  {
    if (isspace(*s1))
    {
      s1++;
      continue;
    }
    if (isspace(*s2))
    {
      s2++;
      continue;
    }
    if (*s1 != *s2)
    {
      return *s1 - *s2;
    }
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int isvalid_command(const char *command)
{
  char sec1[128], sec2[128], buffer[128];
  sscanf(command, "%s %s ", sec1, sec2);
  if (strcmp(sec1, "sgit"))
    return 0;

  Find_a_Repository

      FILE *fp = fopen(".sgit/valid.txt", "r");
  if (fp == NULL)
  {
    perror("Please Install sgit\n");
    exit(1);
  }
  while (fgets(buffer, sizeof(buffer), fp))
  {
    buffer[strcspn(buffer, "\n")] = '\0';
    if (!strcmp(buffer, sec2))
    {
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  return 0;
}

void copy(char *path, char *dest)
{
  char command[1024];
  sprintf(command, "cp -ur  %s %s", path, dest);
  system(command);
}

char *find_email()
{
  FILE *fp = NULL;
  char *email = malloc(128 * sizeof(char));
  int file1Exists = access("/mnt/c/Users/hadij/useremail.txt", F_OK) != -1;
  int file2Exists = access(".sgit/useremail.txt", F_OK) != -1;

  if (!file1Exists && !file2Exists)
  {
    perror("please set your email\n");
    exit(1);
  }

  if (file1Exists && file2Exists)
  {
    struct stat file1Stat, file2Stat;
    int err = stat("/mnt/c/Users/hadij/useremail.txt", &file1Stat);
    if (err != 0)
    {
      perror("Error: ");
      exit(1);
    }
    err = stat(".sgit/useremail.txt", &file2Stat);
    if (err != 0)
    {
      perror("Error: ");
      exit(1);
    }
    if (file1Stat.st_mtime > file2Stat.st_mtime)
    {
      fp = fopen("/mnt/c/Users/hadij/useremail.txt", "r");
      if (fp == NULL)
      {
        perror("Error: ");
        exit(1);
      }
    }
    else
    {
      fp = fopen(".sgit/useremail.txt", "r");
      if (fp == NULL)
      {
        perror("Error: ");
        exit(1);
      }
    }
  }

  else if (file1Exists)
  {
    fp = fopen("/mnt/c/Users/hadij/useremail.txt", "r");
    if (fp == NULL)
    {
      perror("Error\n");
      exit(1);
    }
  }

  else
  {
    fp = fopen(".sgit/useremail.txt", "r");
    if (fp == NULL)
    {
      perror("Error\n");
      exit(1);
    }
  }
  fscanf(fp, "%[^\\n]s", email);
  fclose(fp);

  return email;
}

char *find_name()
{
  FILE *fp = NULL;
  char *name = malloc(128 * sizeof(char));
  int file1Exists = access("/mnt/c/Users/hadij/username.txt", F_OK) != -1;
  int file2Exists = access(".sgit/username.txt", F_OK) != -1;

  if (!file1Exists && !file2Exists)
  {
    perror("please set your name\n");
    exit(1);
  }

  if (file1Exists && file2Exists)
  {
    struct stat file1Stat, file2Stat;
    int err = stat("/mnt/c/Users/hadij/username.txt", &file1Stat);
    if (err != 0)
    {
      perror("Error: ");
      exit(1);
    }
    err = stat(".sgit/username.txt", &file2Stat);
    if (err != 0)
    {
      perror("Error: ");
      exit(1);
    }
    if (file1Stat.st_mtime > file2Stat.st_mtime)
    {
      fp = fopen("/mnt/c/Users/hadij/username.txt", "r");
      if (fp == NULL)
      {
        perror("Error: ");
        exit(1);
      }
    }
    else
    {
      fp = fopen(".sgit/username.txt", "r");
      if (fp == NULL)
      {
        perror("Error: ");
        exit(1);
      }
    }
  }

  else if (file1Exists)
  {
    fp = fopen("/mnt/c/Users/hadij/username.txt", "r");
    if (fp == NULL)
    {
      perror("Error\n");
      exit(1);
    }
  }

  else
  {
    fp = fopen(".sgit/username.txt", "r");
    if (fp == NULL)
    {
      perror("Error\n");
      exit(1);
    }
  }
  fscanf(fp, "%[^\\n]s", name);
  fclose(fp);

  return name;
}

void set_shortcut(char **argv)
{
  FILE *fp = fopen(".sgit/shortcut.txt", "a");
  fputs(argv[3], fp);
  fprintf(fp, "|");
  fputs(argv[5], fp);
  fprintf(fp, "\n");
  fclose(fp);
}

void remove_shortcut(char **argv)
{
  bool flag = false;
  FILE *fp = fopen(".sgit/shortcut.txt", "r");
  FILE *temp_file = fopen(".sgit/temp.txt", "w");
  char buffer[1024];

  while (fgets(buffer, sizeof(buffer), fp))
  {
    char *temp = strchr(buffer, '|');
    temp++;
    temp[strcspn(temp, "\n")] = '\0';
    if (!strcmp(temp, argv[3]))
    {
      flag = true;
      continue;
    }
    fputs(buffer, temp_file);
  }

  fclose(temp_file);
  fclose(fp);

  remove(".sgit/shortcut.txt");
  rename(".sgit/temp.txt", ".sgit/shortcut.txt");

  if (flag)
  {
    printf("The shortcut removed\n");
  }
  else
  {
    printf("Ther is not any shortcut with name %s\n", argv[3]);
  }
}

void replace_shortcut(char **argv)
{
  bool flag = false;
  FILE *fp = fopen(".sgit/shortcut.txt", "r");
  FILE *temp_file = fopen(".sgit/temp.txt", "w");
  char buffer[1024];

  while (fgets(buffer, sizeof(buffer), fp))
  {
    char *temp = strchr(buffer, '|');
    temp++;
    temp[strcspn(temp, "\n")] = '\0';
    if (!strcmp(temp, argv[5]))
    {
      fprintf(temp_file, "%s|%s\n", argv[3], argv[5]);
      flag = true;
    }
    else
    {
      fputs(buffer, temp_file);
    }
  }

  fclose(temp_file);
  fclose(fp);

  remove(".sgit/shortcut.txt");
  rename(".sgit/temp.txt", ".sgit/shortcut.txt");

  if (flag)
  {
    printf("The shortcut replaced\n");
  }
  else
  {
    printf("Ther is not any shortcut with name %s\n", argv[5]);
  }
}

void Find_shortcut_message(char *name)
{
  char copy[512];
  strcpy(copy, name);
  bool flag = false;
  FILE *fp = fopen(".sgit/shortcut.txt", "r");
  if (fp == NULL)
  {
    printf("Try again\n");
    exit(1);
  }
  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), fp))
  {
    char *temp = strchr(buffer, '|');
    temp++;
    temp[strcspn(temp, "\n")] = '\0';
    if (!strcmp(temp, copy))
    {
      int len = temp - buffer;
      char substr[len];
      strncpy(substr, buffer, len);
      substr[len - 1] = '\0';
      flag = true;
      strcpy(name, substr);
      break;
    }
  }

  fclose(fp);

  if (!flag)
  {
    printf("Ther is not any shortcut with name %s\n", copy);
    exit(1);
  }
}

char *find_current_branch()
{
  char *branch = (char *)calloc(128, sizeof(char));
  FILE *fp = fopen(".sgit/.branches/current_branch.txt", "r");
  if (fp == NULL)
  {
    exit(1);
  }
  fgets(branch, sizeof(branch), fp);
  fclose(fp);
  return branch;
}

int find_current_commitID()
{
  FILE *fp = fopen(".sgit/.branches/CurrentCommitID.txt", "r");
  if (fp == NULL)
  {
    exit(1);
  }
  unsigned int ID = 100;
  fscanf(fp, "%u", &ID);
  fclose(fp);
  return ID;
}

int count_of_master_commits_upto_now()
{
  FILE *fp = fopen(".sgit/.commits/log.txt", "r");
  if (!fp)
  {
    exit(1);
  }
  int count = 0;
  char buffer[512];
  while (fgets(buffer, sizeof(buffer), fp))
  {
    if (!strncmp(buffer, "number", 6))
    {
      fgets(buffer, sizeof(buffer), fp);
      if (!strncmp(buffer, "Master", strlen("Master")))
        count++;
    }
  }
  fclose(fp);
  return count;
}

void show_branches_list()
{
  Find_a_Repository

      FILE *fp = fopen(".sgit/.branches/BranchesList.txt", "r");
  if (!fp)
  {
    exit(1);
  }
  char buffer[512];
  while (fgets(buffer, sizeof(buffer), fp))
  {
    char *name = strchr(buffer, '|');
    puts(name);
  }

  fclose(fp);
}

int isnumber(char *string)
{
  uid_t i = 0;
  while (string[i])
  {
    if (!isdigit(string[i]))
    {
      return 0;
    }
    i++;
  }
  return 1;
}

int HEAD_of_branch(char *branch_name)
{
  Find_a_Repository

      FILE *fp = fopen(".sgit/.commits/log.txt", "r");
  if (fp == NULL)
  {
    exit(1);
  }
  int ID;
  char buffer1[1024];
  char buffer2[1024];

  while (fgets(buffer1, sizeof(buffer1), fp))
  {
    if (!strncmp(buffer1, "number", 6))
    {
      fgets(buffer2, sizeof(buffer2), fp);
      if (!strncmp(buffer2, branch_name, strlen(branch_name)))
      {
        sscanf(buffer1, "number%d\n", &ID);
      }
    }
  }

  fclose(fp);
  return ID;
}

void push_stash(int argc, char **argv)
{
  char command[1024] = "rsync -a --exclude=\".*\" . .sgit/.stash/ ";
  system(command);

  FILE *fp = fopen(".sgit/stash.txt", "w");
  if (fp == NULL)
  {
    exit(1);
  }
  fprintf(fp, "Entry0|");
  char *branch = find_current_branch();
  fprintf(fp, " In branch : %s ", branch);
  free(branch);
  if (argc == 5)
  {
    fprintf(fp, " with message : %s ", argv[4]);
  }
  fprintf(fp, "\n");
  int commit_ID;
  FILE *current_commit = fopen(".sgit/.branches/CurrentCommitID.txt", "r");
  fscanf(current_commit, "%d", &commit_ID);
  fclose(current_commit);
  fprintf(fp, "On Commit:%d", commit_ID);
  fclose(fp);

  if (!system("rm -r .sgit/.staging"))
    printf("Push seccessfully\n");
  else if (!system("rm -r .sgit/.staging"))
    printf("Push seccessfully\n");
  else
    perror("ERROR!!!!\n");
  exit(1);
}

void push_list()
{
  FILE *fp = fopen(".sgit/stash.txt", "r");
  if (fp == NULL)
  {
    exit(1);
  }

  char buffer[256];
  fgets(buffer, sizeof(buffer) - 1, fp);
  char *temp = strchr(buffer, '|');
  temp++;
  printf("Entry 0 :\n");
  puts(temp);

  fclose(fp);
}

void clear_stash()
{
  system("rm -r  .sgit/.stash");
  system("rm .sgit/stash.txt");
  printf("Stashed changes are cleared.\n");
}

void diff(char *file1, char *file2, char *lines1, char *lines2)
{
  bool diffrent = false;
  char path1[512], path2[512];
  realpath(file1, path1);
  realpath(file2, path2);
  uid_t start_line1, start_line2;
  uid_t end_line1, end_line2;
  sscanf(lines1, "%d-%d", &start_line1, &end_line1);
  sscanf(lines2, "%d-%d", &start_line2, &end_line2);
  int len1 = end_line1 - start_line1 + 1;
  int len2 = end_line2 - start_line2 + 1;
  FILE *f1 = fopen(path1, "r"), *f2 = fopen(path2, "r");
  if (f1 == NULL || f2 == NULL)
  {
    perror("The files do not exist\n");
    exit(1);
  }
  char buffer1[512], buffer2[512];
  for (int i = 1; i < start_line1; i++)
  {
    fgets(buffer1, sizeof(buffer1), f1);
  }
  for (int j = 1; j < start_line2; j++)
  {
    fgets(buffer2, sizeof(buffer2), f2);
  }
  int min_len = min(len1, len2);
  for (int i = 0, j = 0; i < min_len && j < min_len; i++, j++)
  {
    while (i < min_len && fgets(buffer1, sizeof(buffer1), f1) && is_null_space(buffer1))
    {
      i++;
    }
    while (j < min_len && fgets(buffer2, sizeof(buffer2), f2) && is_null_space(buffer2))
    {
      j++;
    }

    if (feof(f1) || feof(f2))
    {
      break;
    }
    if (strcmp_ignore_spaces(buffer1, buffer2))
    {
      diffrent = true;
      printf("\n");
      printf("<< << << << << << \n");
      printf("File : %s-> Line : %d\n", file1, start_line1 + i);
      printf("%s%s%s", ANSI_COLOR_RED, buffer1, ANSI_COLOR_RESET);
      printf("\n");

      printf("File : %s-> Line : %d\n", file2, start_line2 + j);
      printf("%s%s%s", ANSI_COLOR_BLUE, buffer2, ANSI_COLOR_RESET);
      printf("\n");
      printf(">> >> >> >> >> >> \n");
      printf("\n");
    }
  }
  if (diffrent)
  {
    if (len1 == len2)
    {
      printf("The end of files\n");
    }
    else if (len1 > len2)
    {
      printf("The end of file 2 \n");
    }
    else if (len2 > len1)
    {
      printf("The end of file1 \n");
    }
    printf("**********\n");
  }
}

void diff_directories(char *dir1, char *dir2)
{
  char cmd[128], buffer[512];
  sprintf(cmd, "diff -qr %s %s ", dir1, dir2);
  printf("%s\n", dir1);
  printf("%s\n", dir2);
  printf("%s\n", cmd);
  FILE *fp = popen(cmd, "r");
  if (fp == NULL)
  {
    printf("try again\n");
    exit(1);
  }
  while (fgets(buffer, sizeof(buffer), fp))
  {
    if (!strncmp(buffer, "Only", strlen("Only")))
    {
      char *temp = strrchr(buffer, ':');
      temp += 2;
      printf("\nThe file : %s is only in one of them\n\n", temp);
    }
    else
    {
      char *file1 = strtok(buffer, " ");
      file1 = strtok(NULL, " ");
      char *file2 = strtok(NULL, " ");
      file2 = strtok(NULL, " ");
      diff(file1, file2, "1-10000", "1-10000");
    }
  }
  pclose(fp);
}

void show_stash_diff()
{
  char cwd[128];
  getcwd(cwd, 128);
  FILE *fp = fopen(".sgit/stash.txt", "r");
  if (fp == NULL)
  {
    printf("Try again!\n");
    exit(1);
  }
  char line[128];

  // Ignore first line
  fgets(line, sizeof(line), fp);

  fgets(line, sizeof(line), fp);
  char *ID = strchr(line, ':');
  ID++;
  ID[strcspn(ID, "\n")] = '\0';
  char path_to_commit[128];
  char path_to_stash[128];
  sprintf(path_to_commit, "%s/.sgit/.commits/number%s", cwd, ID);
  sprintf(path_to_stash, "%s/.sgit/.stash", cwd);
  diff_directories(path_to_commit, path_to_stash);
}

void set_precommit_situation(int sit)
{
  FILE *fp = fopen(".sgit/.precommit/situation.txt", "w");
  if (fp == NULL)
  {
    perror("Try again\n");
    exit(1);
  }
  if (sit)
  {
    fprintf(fp, "TRUE");
  }
  else
  {
    fprintf(fp, "FALSE");
  }
  fclose(fp);
}

int revert_to_backup(const char *backup_path, const char *current_path)
{
  DIR *dir;
  struct dirent *entry;

  dir = opendir(backup_path);
  if (dir == NULL)
  {
    perror("Unable to open stash\n");
    return 1;
  }

  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_name[0] == '.')
    {
      continue;
    }

    char source_path[1024];
    snprintf(source_path, sizeof(source_path), "%s/%s", backup_path, entry->d_name);

    char dest_path[1024];
    snprintf(dest_path, sizeof(dest_path), "%s/%s", current_path, entry->d_name);

    if (entry->d_type == DT_DIR)
    {
      mkdir(dest_path, 0700);
      revert_to_backup(source_path, dest_path);
    }
    else
    {
      if (rename(source_path, dest_path))
      {
        perror("Error moving file");
        closedir(dir);
        return 1;
      }
    }
  }

  closedir(dir);
  return 0;
}

void append_tag(char *tag_name, char *message, int commit_ID, bool can_owerwrite)
{
  FILE *fp = fopen(".sgit/tags.txt", "r+");
  if (fp == NULL)
  {
    fp = fopen(".sgit/tags.txt", "w");
    fclose(fp);
  }

  char *name = find_name();
  char *email = find_email();
  char line[256];
  while (fgets(line, sizeof(line), fp))
  {
    char *temp = strchr(line, ':');
    if (temp == NULL)
      continue;
    temp++;
    temp[strcspn(temp, "\n")] = '\0';
    if (!strcmp(temp, tag_name))
    {
      if (!can_owerwrite)
      {
        printf("The tag is already exists\n");
        exit(0);
      }

      fprintf(fp, "%s\n%d\n%s\n%s\n", message, commit_ID, name, email);
      time_t t = time(NULL);
      struct tm tm = *localtime(&t);
      fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
      fprintf(fp, "\n");
      printf("***The tag OwerWrited successfully***\n");
      fclose(fp);
      return;
    }
  }

  fprintf(fp, "tag:%s\n%s\n%d\n%s\n%s\n", tag_name, message, commit_ID, name, email);

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

  fprintf(fp, "\n");
  fclose(fp);
  printf("***The tag Appended successfuly***\n");
}

int pstrcmp(const void *a, const void *b)
{
  return strcmp(*(const char **)a, *(const char **)b);
}

void tag_list()
{
  FILE *fp = fopen(".sgit/tags.txt", "r");
  if (fp == NULL)
  {
    printf("Error: could not open file\n");
    exit(1);
  }

  char **lines = malloc(128 * sizeof(char *));
  for (int i = 0; i < 128; i++)
  {
    lines[i] = malloc(1024 * sizeof(char));
  }
  size_t n = 0;
  char temp[1024];
  while (fgets(temp, 1000, fp))
  {
    if (!strncmp(temp, "tag:", 4))
    {
      strcpy(lines[n], temp);
      n++;
    }
  }

  qsort(lines, n, sizeof(char *), pstrcmp);

  printf("\n<< Sorting Alphabetically >>\n\n");
  for (int i = 0; i < n; i++)
  {
    printf("number : %d -->  ", (i + 1));
    printf("%s\n", lines[i]);
  }

  for (int i = 0; i < 128; i++)
  {
    free(lines[i]);
  }
  free(lines);
  fclose(fp);
}

void show_tag(const char *tag_name)
{
  FILE *fp = fopen(".sgit/tags.txt", "r");
  if (fp == NULL)
  {
    printf("Error: could not open file\n");
    exit(1);
  }
  char buffer[512];
  char **lines = malloc(128 * sizeof(char *));
  while (fgets(buffer, 500, fp))
  {
    char *temp = strchr(buffer, ':');
    if (temp == NULL)
      continue;
    temp++;
    temp[strcspn(temp, "\n")] = '\0';
    if (!strcmp(temp, tag_name))
    {
      printf("*****\n");
      printf("tag with Name : %s\n", tag_name);
      fgets(buffer, 500, fp);
      printf("wit message : %s", buffer);
      fgets(buffer, 500, fp);
      printf("with commit ID : %s", buffer);
      fgets(buffer, 500, fp);
      printf("Author name : %s", buffer);
      fgets(buffer, 500, fp);
      printf("Author email : %s", buffer);
      fgets(buffer, 500, fp);
      printf("Date and Time of tag : %s", buffer);
      printf("*****\n");

      fclose(fp);
      return;
    }
  }

  fclose(fp);
  printf("The tag with name \"%s\" not found!!\n", tag_name);
}

show_hooks(bool all_or_applied)
{
  FILE *fp = fopen(".sgit/.precommit/hooks.txt", "r");
  if (fp == NULL)
  {
    perror("Try again!\n");
    exit(1);
  }

  bool print = true;
  char line[128];
  while (fgets(line, sizeof(line), fp))
  {
    int sit;
    char hook_name[128];
    sscanf(line, "%d %s\n", &sit, hook_name);
    /* If user wants to see only applied or all of hooks */
    print = true;
    if (!all_or_applied && !sit)
      print = false;
    if (print)
      puts(hook_name);
  }

  fclose(fp);
}

void change_list_of_hooks(int situation, char *hook_name)
{
  FILE *fp = fopen(".sgit/.precommit/hooks.txt", "r+");
  if (fp == NULL)
  {
    perror("Error opening file");
    exit(1);
  }

  char line[128];
  while (fgets(line, sizeof(line), fp))
  {
    int hook_id;
    char name[128];

    if (sscanf(line, "%d %s", &hook_id, name) == 2)
    {
      if (strcmp(hook_name, name) == 0)
      {
        fseek(fp, -strlen(line), SEEK_CUR);
        fprintf(fp, "%d %s\n", situation, hook_name);
      }
    }
  }

  fclose(fp);
}

void Check_pre_commit()
{
  bool PASSED = true;
  FILE *fp = popen("find .sgit/.staging/* -type f ", "r");
  if (fp == NULL)
  {
    perror("Try agin!\n");
    exit(1);
  }

  FILE *hooks = fopen(".sgit/.precommit/hooks.txt", "r");
  if (hooks == NULL)
  {
    perror("Try again\n");
    exit(1);
  }

  char buffer[512];
  while (fgets(buffer, sizeof(buffer), fp))
  {
    char file_path[512];
    strcpy(file_path, buffer);
    // remove the end \n
    file_path[strlen(file_path) - 1] = '\0';
    memmove(buffer, buffer + strlen(".sgit/.staging/"), strlen(".sgit/.staging/") + 1);
    printf("-> %s%s%s", ANSI_COLOR_BLUE, buffer, ANSI_COLOR_RESET);
    char *type = strrchr(buffer, '.');
    type[strcspn(type, "\n")] = '\0';

    /* static-error-check */
    char hook_name[512];
    int need_to_check = 0;
    rewind(hooks);
    char line[512];
    while (fgets(line, sizeof(line), hooks))
    {
      sscanf(line, "%d %s\n", &need_to_check, hook_name);
      if (!strcmp(hook_name, "static-error-check"))
      {
        break;
      }
    }
    if (need_to_check)
    {
      printf("static-error-check....................");
      if (!strcmp(type, ".c") || !strcmp(type, ".cpp"))
      {
        char cmd[256];
        sprintf(cmd, "gcc  %s ", file_path);
        if (system(cmd))
        {
          set_precommit_situation(0);
          PASSED = false;
          printf("Failed\n");
        }
        else
        {
          printf("Passed\n");
        }
      }
      else
      {
        printf("Skipped\n");
      }
    }

    /* check TODO */
    need_to_check = 0;
    rewind(hooks);
    while (fgets(line, sizeof(line), hooks))
    {
      sscanf(line, "%d %s\n", &need_to_check, hook_name);
      if (!strcmp(hook_name, "todo-check"))
      {
        break;
      }
    }
    if (need_to_check)
    {
      printf("todo-check....................");

      if (!strcmp(type, ".txt"))
      {
        bool flag = true;
        char cwd[1024];
        FILE *txt = fopen(file_path, "r");
        if (txt == NULL)
        {
          perror("Try again\n");
          exit(1);
        }

        char buffer[512];
        while (fgets(buffer, sizeof(buffer), txt))
        {
          if (strstr(buffer, "TODO"))
          {
            set_precommit_situation(0);
            PASSED = false;
            printf("Failed\n");
            flag = false;
            break;
          }
        }
        if (flag)
        {
          printf("Passed\n");
        }

        fclose(txt);
      }

      else if (!strcmp(type, ".c") || !strcmp(type, ".cpp"))
      {
        bool flag = true;
        FILE *cpp = fopen(file_path, "r");
        if (cpp == NULL)
        {
          perror("Try again\n");
          exit(1);
        }
        char buffer[512];
        while (fgets(buffer, sizeof(buffer), cpp))
        {

          if (strstr(buffer, "//"))
          {
            char *start_comment = strstr(buffer, "//");
            if (strstr(start_comment, "TODO"))
            {
              set_precommit_situation(0);
              PASSED = false;
              printf("Failed\n");
              flag = false;
              break;
            }
          }
        }

        if (flag)
        {
          printf("Passed\n");
        }

        fclose(cpp);
      }
      else
      {
        printf("Skipped\n");
      }
    }

    /* file-size-check */
    need_to_check = 0;
    rewind(hooks);
    while (fgets(line, sizeof(line), hooks))
    {
      sscanf(line, "%d %s\n", &need_to_check, hook_name);
      if (!strcmp(hook_name, "file-size-check"))
      {
        break;
      }
    }
    if (need_to_check)
    {
      if (isFileSizeOver5MB(file_path))
      {
        set_precommit_situation(0);
        PASSED = false;
        printf("file-size-check....................Failed\n");
      }
      else
      {
        printf("file-size-check....................Passed\n");
      }
    }

    /* character-limit */
    need_to_check = 0;
    rewind(hooks);
    while (fgets(line, sizeof(line), hooks))
    {
      sscanf(line, "%d %s\n", &need_to_check, hook_name);
      if (!strcmp(hook_name, "character-limit"))
      {
        break;
      }
    }
    if (need_to_check)
    {
      if (!strcmp(type, ".c") || !strcmp(type, ".cpp") || !strcmp(type, ".txt"))
      {
        if (count_characters_in_file(file_path) > 20000)
        {
          printf("character-limit..............Failed\n");
          set_precommit_situation(0);
          PASSED = false;
        }
        else
        {
          printf("character-limit..............Passed\n");
        }
      }
      else
        printf("character-limit..............Skipped\n");
    }
    putc('\n', stdout);
  }
  fclose(fp);
  fclose(hooks);

  if (PASSED)
    set_precommit_situation(1);
}
