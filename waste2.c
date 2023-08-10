#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include "waste.h"
#define RESET_COLOR "\e[m"
#define MAKE_GREEN "\e[32m"
#define MAKE_BLUE "\e[36m"

char bg[1000000][100] = {'\0'};
pid_t Pid[100] = {0}, pi;
int k = 0, back = 0, p = 0;

void pip_ing(char *ans, int pip)
{
    char *commandParts[1000] = {'\0'};
    int standard_input = dup(0), standard_output = dup(1);
    int pipeObject[2];
    if (pipe(pipeObject) < 0)
    {
        perror(" Couldn't Create pipe. :( ");
        return;
    }
    if (dup2(pipeObject[1], STDOUT_FILENO) != 1)
        perror("Error");
    close(pipeObject[1]);
    char d[2] = "|";
    char *token = strtok(ans, d);
    int z = 0;
    char *a[100] = {"\0"};
    a[z++] = token;
    while (token != NULL)
    {
        token = strtok(NULL, d);
        if (token != NULL)
            a[z++] = token;
    }
    printf("F\n");
    command(a[0]);
    dup2(pipeObject[0], 0);
    close(pipeObject[0]);
    for (int i = 1; i < pip; i++)
    {
        if (pipe(pipeObject) < 0)
        {
            perror(" Couldn't Create pipe. :( ");
            return;
        }
        dup2(pipeObject[1], 1);
        close(pipeObject[1]);
        command(a[i]);
        dup2(pipeObject[0], 0);
        close(pipeObject[0]);
    }
    dup2(standard_output, 1);
    command(a[pip]);
    dup2(standard_input, 0);
    return;
}

void sig(int a, int b)
{
    if (Pid[a - 1] == 0)
        perror("Error Occoured!!");
    else
        kill(Pid[a - 1], b);
    printf("\n");
}

void jobs()
{
    int su[100];
    for (int i = 0; Pid[i] > 0; i++)
    {
        char *processStatFile = (char *)malloc(sizeof(char) * 1000);
        char *garbageString = (char *)malloc(sizeof(char) * 1000);
        char *stat = (char *)malloc(10);
        char *runOrStop = (char *)malloc(1000);
        sprintf(processStatFile, "/proc/%d/stat", Pid[i]);
        FILE *fileObject = fopen(processStatFile, "r");
        if (!fileObject)
            continue;
        fscanf(fileObject, "%s", garbageString);
        fscanf(fileObject, "%s", garbageString);

        fscanf(fileObject, "%s", stat);

        strcpy(runOrStop, strcmp(stat, "T") == 0 ? "Stopped" : "Running");
        dprintf(1, "[%d] %s %s [%d]\n", i, runOrStop, bg[Pid[i]], Pid[i]);
        fclose(fileObject);
    }
}

void fg(int sub)
{
    if (Pid[sub - 1] == 0)
        return;
    int status;
    long int x = Pid[sub - 1];
    // setpgid(x, getpgid(0));
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    // tcsetpgrp(0, x);

    kill(x, SIGCONT);
    waitpid(x, &status, WUNTRACED);
    tcsetpgrp(0, getpgid(0));
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    // pid[sub - 1] = 0;
}

void bag(int sub)
{
    kill(Pid[sub - 1], 18);
}

void die()
{
    int i = 0;
    pid_t pid;
    int stat;
    int flag;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        flag = 0;
        if (WIFEXITED(stat))
            flag = 1;
        if (flag)
            printf("\n%s with pid = %d exited normally\n", bg[pid], pid);
        else
            printf("\n%s with pid = %d exited abnormally\n", bg[pid], pid);
    }
    return;
}

void sigtstp_handler(int signum)
{
    if (back == 0)
    {
        int frt = fork();
        if (frt == 0)
        {

            kill(pi, SIGSTOP);
            exit(0);
        }
        else
        {
            printf("[%d] %d\n", k, pi);
            Pid[k++] = pi;
            back = 1;
        }
    }
}

void sigint_handler(int signum)
{
    if (back == 0)
    {
        kill(pi, 9);
        printf("\n");
    }
}

int isFile(char *name)
{
    DIR *directory = opendir(name);

    if (directory != NULL)
    {
        closedir(directory);
        return 0;
    }
    return 1;
}

void dfs(char *c, int d, int til, int len, char *file)
{
    char *files[100] = {NULL};
    struct dirent *de;
    DIR *dr;
    dr = opendir(c);
    if (dr == NULL)
        return;
    int i = 0;
    while ((de = readdir(dr)) != NULL)
    {
        if (de->d_name[0] != '.')
        {
            char arr[100] = {'\0'};
            strcpy(arr, c);
            strcat(arr, "/");
            strcat(arr, de->d_name);
            if ((d == 1 && isFile(arr) == 0) || (d == -1 && isFile(arr) == 1) || d == 0)
            {
                if (file[0] != '\0')
                {
                    // printf("F\n");
                    if (strcmp(file, de->d_name) == 0)
                    {
                        if (til != 1)
                            printf("%s\n", arr);
                        else
                            printf("~%s\n", &arr[len]);
                    }
                }
                else
                {
                    if (til != 1)
                        printf("%s\n", arr);
                    else
                        printf("~%s\n", &arr[len]);
                }
            }
            dfs(arr, d, til, len, file);
        }
    }
}

void command(char *ans)
{
    back = 0;
    char de[3] = "&\n", del[4] = " \t\n";
    int size = strlen(ans);
    if (ans[size - 1] == '&')
        back = 1;
    char *tok, *b[100] = {NULL};
    tok = strtok(ans, de);
    b[0] = tok;
    int i = 1;
    while (tok != NULL)
    {
        tok = strtok(NULL, de);
        if (tok != NULL)
            b[i++] = tok;
    }
    int j = 0;
    pid_t pid;
    while (b[j] != NULL)
    {
        char *token;
        token = strtok(b[j], del);
        int z = 0;
        char *a[100] = {"\0"};

        a[z++] = token;
        int fdo = 0, oo = 0, fdi = 0, oi = 0, flag = 0;
        while (token != NULL)
        {
            token = strtok(NULL, del);
            if (a[z - 1] == NULL)
            {
                z++;
                continue;
            }
            if (strcmp(a[z - 1], "<") == 0)
            {
                fdi = open(token, O_RDONLY, 0644);
                oi = dup(STDIN_FILENO);
                dup2(fdi, STDIN_FILENO);
                flag = 1;
            }
            if (strcmp(a[z - 1], ">") == 0)
            {
                fdo = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                oo = dup(STDOUT_FILENO);
                dup2(fdo, STDOUT_FILENO);
                flag = 1;
            }
            if (strcmp(a[z - 1], ">>") == 0)
            {
                fdo = open(token, O_WRONLY | O_CREAT | O_APPEND, 0644);
                oo = dup(STDOUT_FILENO);
                dup2(fdo, STDOUT_FILENO);
                flag = 1;
            }
            a[z] = token;
            z++;
        }
        pid = fork();
        if (pid == 0)
        {
            setpgid(0, 0);
            if (execvp(a[0], a) < 0)
            {
                perror("Command doesn't exist\n");
                exit(1);
            }
        }
        else
        {
            printf("%s\n", a[0]);
            strcpy(bg[pid], a[0]);
            j++;
            i--;
            if (i == 0 && back == 0)
                pi = pid;
            else
            {
                Pid[k++] = pid;
                printf("[%d] %d\n", j, pid);
            }
        }
        if (oo != 0)
            dup2(oo, STDOUT_FILENO);
        if (oi != 0)
            dup2(oi, STDIN_FILENO);
    }
    if (back == 0)
        waitpid(pi, NULL, WUNTRACED);
}
void print_echo(char **sting)
{
    int i = 1;
    while (sting[i] != NULL)
    {
        if (strcmp(sting[i], ">") == 0 || strcmp(sting[i], "<") == 0 || strcmp(sting[i], ">>") == 0)
            return;
        printf("%s ", sting[i++]);
    }
}

void print_pwd()
{
    char path[100];
    getcwd(path, 100);
    printf("%s", path);
}

void print_cd(char **sting, char *prev_dir, char *init_dir, char *dir)
{
    int j = 1;
    if (sting[j] == NULL)
    {
        chdir(init_dir);
        return;
    }
    int i = chdir(sting[j]);
    if (i != 0 && strcmp(sting[j], "-") != 0 && strcmp(sting[j], "~") != 0)
        printf("No such file or directory");
    else if (strcmp(sting[j], "-") == 0)
    {
        chdir(prev_dir);
        printf("%s\n", prev_dir);
    }
    else if (strcmp(sting[j], "~") == 0)
        chdir(init_dir);
    strcpy(prev_dir, dir);
}

void print_ls(char **sting, char *username, char *hostname)
{
    int e = 1, l = 0, a = 0, i = 0;
    char *arr[100] = {NULL};
    while (sting[e] != NULL)
    {
        if (strcmp(sting[e], ">") == 0 || strcmp(sting[e], "<") == 0 || strcmp(sting[e], ">>") == 0)
            break;
        if (strcmp(sting[e], "-l") == 0)
            l = 1;
        else if (strcmp(sting[e], "-a") == 0)
            a = 1;
        else if (strcmp(sting[e], "-la") == 0 || strcmp(sting[e], "-al") == 0)
        {
            l = 1;
            a = 1;
        }
        else
            arr[i++] = sting[e];
        e++;
    }
    if (i == 0)
        arr[0] = ".";
    i = 0;
    while (arr[i] != NULL)
    {
        printf("%s\n", arr[i]);
        int file = 0;
        struct dirent *de;
        DIR *dr;
        dr = opendir(arr[i]);
        if (dr == NULL)
        {
            if (arr[i][0] != '"')
            {
                printf("Could not open current file/directory");
                continue;
            }
            else
            {
                printf("F\n");
                file = 1;
                dr = opendir(".");
            }
        }
        int z = 0;
        char files[100][100] = {{'\0'}}, *fi[100] = {NULL};
        while ((de = readdir(dr)) != NULL)
        {
            if (file == 0)
            {
                char f[100] = {'\0'};
                strcpy(f, arr[i]);
                strcat(f, "/");
                strcat(f, de->d_name);
                strcpy(files[z], f);
                fi[z] = de->d_name;
                z++;
            }
            else
            {
                char f[100] = {'\0'};
                strcpy(f, ".");
                strcat(f, "/");
                printf("%s\n", de->d_name);
                if (de->d_name == arr[i])
                {
                    printf("F\n");
                    strcat(f, de->d_name);
                    strcpy(files[z], f);
                    fi[z] = de->d_name;
                }
            }
        }
        if (file == 1)
        {
            strcpy(files[0], arr[i]);
            fi[0] = arr[i];
        }
        closedir(dr);
        char *swap;
        for (int k = z; k > 0; k--)
            for (int j = 0; j < k - 1; j++)
            {
                if (strcmp(fi[j], fi[j + 1]) > 0)
                {
                    swap = fi[j];
                    fi[j] = fi[j + 1];
                    fi[j + 1] = swap;
                }
            }
        for (int j = 0; j < z; j++)
        {
            if (l == 0 && ((a == 0 && fi[j][0] != '.') || a == 1))
            {
                struct stat buffer;
                char tempo[1000];
                char *path_type = getcwd(tempo, 1000);
                strcat(path_type, "/");
                strcat(path_type, fi[j]);
                if (stat(path_type, &buffer) != -1)
                {
                    if (S_ISDIR(buffer.st_mode))
                        printf(MAKE_BLUE "%s " RESET_COLOR, fi[j]);
                    else if (buffer.st_mode & S_IXUSR)
                        printf(MAKE_GREEN "%s " RESET_COLOR, fi[j]);
                    else
                        printf("%s ", fi[j]);
                }
            }
            else if (l == 1 && ((a == 0 && fi[j][0] != '.') || a == 1))
            {
                // printf("%s ", files[j]);
                if (isFile(files[j]) == 0)
                    printf("d");
                struct stat file;
                if (stat(files[j], &file) == -1)
                    perror("Error Occurred\n");
                if (file.st_mode & S_IRUSR)
                    printf("r");
                else
                    printf("-");
                if (file.st_mode & S_IWUSR)
                    printf("w");
                else
                    printf("-");
                if (file.st_mode & S_IXUSR)
                    printf("x");
                else
                    printf("-");
                if (file.st_mode & S_IRGRP)
                    printf("r");
                else
                    printf("-");
                if (file.st_mode & S_IWGRP)
                    printf("w");
                else
                    printf("-");
                if (file.st_mode & S_IXGRP)
                    printf("x");
                else
                    printf("-");
                if (file.st_mode & S_IROTH)
                    printf("r");
                else
                    printf("-");
                if (file.st_mode & S_IWOTH)
                    printf("w");
                else
                    printf("-");
                if (file.st_mode & S_IXOTH)
                    printf("x");
                else
                    printf("-");
                printf(" ");
                printf("%li ", file.st_nlink);
                printf("%s ", username);
                printf("%s ", hostname);
                printf("%ld ", file.st_size);
                struct tm *time;
                char output[500];
                time = localtime(&(file.st_mtime));
                if (time == NULL)
                {
                    perror("localtime");
                    exit(0);
                }
                strftime(output, sizeof(output), "%b %d %R", time);
                printf("%s ", output);
                // printf("%ld ", file.st_mtime);
                struct stat buffer;
                char tempo[1000];
                char *path_type = getcwd(tempo, 1000);
                strcat(path_type, "/");
                strcat(path_type, fi[j]);
                if (stat(path_type, &buffer) != -1)
                {
                    if (S_ISDIR(buffer.st_mode))
                        printf(MAKE_BLUE "%s\n" RESET_COLOR, fi[j]);
                    else if (buffer.st_mode & S_IXUSR)
                        printf(MAKE_GREEN "%s\n" RESET_COLOR, fi[j]);
                    else
                        printf("%s\n", fi[j]);
                }
            }
        }
        i++;
        printf("\n");
    }
}

void print_discover(char **sting, char *init_dir, int size)
{
    char *dir, file[100] = {'\0'};
    int d = 0, f = 0, an = 0, i = 1;
    if (sting[i] == NULL || strcmp(sting[i], ">") == 0 || strcmp(sting[i], "<") == 0 || strcmp(sting[i], ">>") == 0)
        dir = ".";
    else if (strcmp(sting[i], "-d") == 0)
    {
        dir = ".";
        d = 1;
    }
    else if (strcmp(sting[i], "-f") == 0)
    {
        dir = ".";
        f = 1;
        printf("F\n");
    }
    else if (isFile(sting[i]) == 0)
        dir = sting[i];
    else
    {
        dir = ".";
        strcpy(file, &sting[i][1]);
    }
    while (sting[i] != NULL && strcmp(sting[i], ">") != 0 && strcmp(sting[i], "<") != 0 && strcmp(sting[i], ">>") != 0)
    {
        i++;
        if (strcmp(sting[i], ">") == 0 || strcmp(sting[i], "<") == 0 || strcmp(sting[i], ">>") == 0)
            break;
        if (sting[i] != NULL)
        {
            if (strcmp(sting[i], "-d") == 0)
                d = 1;
            else if (strcmp(sting[i], "-f") == 0)
                f = 1;
            else
                strcpy(file, &sting[i][1]);
        }
    }
    if (d == 1 && f == 0)
        an = 1;
    else if (d == 0 && f == 1)
        an = -1;
    if (file[0] != '\0')
    {
        int j = strlen(file);
        file[j - 1] = '\0';
    }
    if (strcmp(dir, "~") == 0)
        dfs(init_dir, an, 1, size, file);
    else
        dfs(dir, an, 0, size, file);
}

void print_history(char *sting)
{
    char buffer[100] = {'\0'};
    FILE *h_file = fopen("History.txt", "r");
    int i = 0;
    while (fscanf(h_file, "%[^\n]s", buffer) != EOF)
    {
        printf("%s\n", buffer);
        fseek(h_file, 1, 1);
    }
    fclose(h_file);
}

void print_pinfo(char **sting)
{
    long long int memory, f1, f2;
    int pid;
    char c;
    FILE *file;
    if (sting[1] == NULL)
        file = fopen("/proc/self/stat", "r");
    else if (strcmp(sting[1], ">") == 0 && strcmp(sting[1], "<") == 0 && strcmp(sting[1], ">>") == 0)
        file = fopen("/proc/self/stat", "r");
    else
    {
        char s[100] = {'\0'};
        strcpy(s, "/proc/");
        strcat(s, sting[1]);
        strcat(s, "/stat");
        file = fopen(s, "r");
    }
    fscanf(file, "%d %*s %c %*s %lld %*s %*s %lld %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lld",
           &pid, &c, &f1, &f2, &memory);
    printf("pid : %d\n", pid);
    if (f1 == f2)
        printf("process status : %c+\n", c);
    else
        printf("process status : %c\n", c);
    printf("memory : %lld\n", memory);
    char dir[1000] = {'\0'};
    if (sting[1] == NULL)
    {
        int len = readlink("/proc/self/exe", dir, 999);
    }
    else
    {
        char s[100] = {'\0'};
        strcpy(s, "/proc/");
        strcat(s, sting[1]);
        strcat(s, "/stat");
        int len = readlink(s, dir, 999);
    }
    printf("Executable path: %s\n", dir);
}
