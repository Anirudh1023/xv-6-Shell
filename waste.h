#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

void die();
//void command(char **sting, int back, char *fg);
void command(char *ans);
int isFile(char *name);
void dfs(char *c, int d, int til, int len, char *file);
void print_echo(char **sting);
void print_pwd();
void print_pinfo(char **sting);
void print_ls(char **sting, char *username, char *hostname);
void print_discover(char **sting, char *init_dir, int size);
void print_history(char *sting);
void print_cd(char **sting, char *prev_dir, char *init_dir, char *dir);
void jobs();
void fg(int sub);
void bag(int sub);
void sig(int a, int b);
void sigtstp_handler(int signum);
void sigint_handler(int signum);
void pip_ing(char *ans, int pip);