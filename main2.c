#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>
#include "waste.h"

void died(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        died("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        died("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        died("tcsetattr");
}

int main()
{
    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];
    int result;
    result = gethostname(hostname, HOST_NAME_MAX);
    if (result)
    {
        perror("gethostname");
        return EXIT_FAILURE;
    }
    result = getlogin_r(username, LOGIN_NAME_MAX);
    if (result)
    {
        perror("getlogin_r");
        return EXIT_FAILURE;
    }
    char init_path[100], prev_tok[100];
    getcwd(init_path, 100);
    int size = strlen(init_path);
    char prev_dir[100] = {'\0'};
    int elements = 0;
    FILE *h_file = fopen("History.txt", "a+");
    char buffer[100] = {'\0'};
    char l_ele[100] = {'\0'};

    int i = 0;
    if (h_file == NULL)
        return i;

    while (fscanf(h_file, "%[^\n]s", buffer) != EOF)
    {
        fseek(h_file, 1, 1);
        elements++;
    }
    fclose(h_file);
    while (1)
    {
        signal(SIGCHLD, die);
        signal(SIGINT, sigint_handler);
        signal(SIGTSTP, sigtstp_handler);
        char inp[100] = {'\0'}, del[4] = " \t\n", d[2] = ";";
        char sting[100], *com[100] = {NULL};
        char c;
        setbuf(stdout, NULL);
        int pt = 0;
        enableRawMode();
        getcwd(sting, 100);
        there:
        if (strlen(sting) >= size)
            result = printf("<%s@%s:~%s>", username, hostname, &sting[size]);
        else
            result = printf("<%s@%s:~%s>", username, hostname, sting);
        printf("%s", inp);
        while (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (iscntrl(c))
            {
                if (c == 10)
                    break;
                else if (c == 127)
                {
                    if (pt > 0)
                    {
                        if (inp[pt - 1] == 9)
                        {
                            for (int i = 0; i < 7; i++)
                            {
                                printf("\b");
                            }
                        }
                        inp[--pt] = '\0';
                        printf("\b \b");
                    }
                }
                else if (c == 9)
                {
                    char inp2[100] = {'\0'};
                    strcpy(inp2, inp);

                    char d[2] = " ", com[100] = {'\0'};
                    char *t = strtok(inp2, d);
                    while (t != NULL)
                    {
                        t = strtok(NULL, d);
                        if (t != NULL)
                            strcpy(com, t);
                    }
                    DIR *dr = opendir(".");
                    struct dirent *de;
                    int z = 0;
                    char files[100][100] = {{'\0'}};
                    int min = 9999;
                    while ((de = readdir(dr)) != NULL)
                    {
                        char f[100] = {'\0'};
                        strcat(f, "/");
                        strcat(f, de->d_name);
                        if (!strncmp(com, de->d_name, strlen(com)))
                        {
                            strcpy(files[z], de->d_name);
                            if (min > strlen(files[z]))
                                min = strlen(files[z]);
                            z++;
                        }
                    }
                    if (z == 1)
                    {
                        for (int i = strlen(com); i < strlen(files[0]); i++)
                        {
                            printf("%c", files[0][i]);
                        }
                        strncat(inp, &files[0][strlen(com)], strlen(files[0]) - strlen(com));
                        pt = strlen(inp);
                    }
                    else
                    {
                        for (int i = strlen(com); i < min; i++)
                        {
                            int f = 0;

                            for (int j = 0; j < z; j++)
                            {
                                if (files[j][i] != files[0][i])
                                {
                                    f = 1;
                                    break;
                                }
                            }
                            if (f == 0)
                            {
                                char temp[2];
                                temp[0] = files[0][i];
                                temp[1] = 0;
                                strcat(inp, temp);
                            }
                            else
                            {
                                break;
                            }
                        }
                        printf("\n");
                        for (int i = 0; i < z; i++)
                        {
                            printf("%s\n", files[i]);
                        }
                        pt = strlen(inp);
                        goto there;
                    }
                }
                else if (c == 4)
                {
                    exit(0);
                }
                else
                {
                    printf("%d\n", c);
                }
            }
            else
            {
                inp[pt++] = c;
                printf("%c", c);
            }
        }
        printf("\n");
        disableRawMode();
        //*************************************************************************************************************************
        int si = strlen(inp);
        if (strcmp(inp, "\n") == 0)
            continue;
        else
        {
            if (strcmp(l_ele, inp) != 0)
            {
                if (elements < 20)
                {
                    FILE *h_file = fopen("History.txt", "a");
                    fprintf(h_file, inp, strlen(inp));
                    fprintf(h_file, "\n");
                    strcpy(l_ele, inp);
                    elements++;
                    fclose(h_file);
                }
                else
                {
                    FILE *h_file = fopen("History.txt", "r");
                    FILE *temp = fopen("temp.txt", "a");
                    int count = 1;
                    while (fscanf(h_file, "%[^\n]s", buffer) != EOF)
                    {
                        fseek(h_file, 1, 1);
                        if (count != 1)
                        {
                            fprintf(h_file, buffer, strlen(buffer));
                            fprintf(h_file, "\n");
                        }
                        count++;
                    }
                    fprintf(temp, inp, strlen(inp));
                    // fprintf(temp, "\n");
                    strcpy(l_ele, inp);
                    elements++;
                    rename("temp.txt", "History.txt");
                }
            }
            char *t;
            t = strtok(inp, d);
            com[0] = t;
            // printf("%s\n", com[0]);
            int i = 1;
            while (t != NULL)
            {
                t = strtok(NULL, d);
                if (t != NULL)
                    com[i++] = t;
            }
            i = 0;
            // printf("%s\n", com[0]);
            while (com[i] != NULL)
            {
                int fdo = 0, oo = 0, fdi = 0, oi = 0, pip = 0;
                char *to[100] = {NULL};
                char *token;
                char comma[100] = {'\0'};
                strcpy(comma, com[i]);
                token = strtok(com[i], del);
                to[0] = token;
                int j = 1;
                while (token != NULL)
                {
                    // printf("%s\n", to[j - 1]);
                    token = strtok(NULL, del);
                    if (strcmp(to[j - 1], "<") == 0)
                    {
                        fdi = open(token, O_RDONLY, 0644);
                        oi = dup(STDIN_FILENO);
                        dup2(fdi, STDIN_FILENO);
                    }
                    if (strcmp(to[j - 1], ">") == 0)
                    {
                        fdo = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        oo = dup(STDOUT_FILENO);
                        dup2(fdo, STDOUT_FILENO);
                    }
                    if (strcmp(to[j - 1], ">>") == 0)
                    {
                        fdo = open(token, O_WRONLY | O_CREAT | O_APPEND, 0644);
                        oo = dup(STDOUT_FILENO);
                        dup2(fdo, STDOUT_FILENO);
                    }
                    if (strcmp(to[j - 1], "|") == 0)
                        pip++;
                    if (token != NULL)
                        to[j++] = token;
                }
                // printf("%s\n", com[0]);
                if (pip > 0)
                    pip_ing(comma, pip);
                else if (strcmp(to[0], "echo") == 0)
                    print_echo(to);
                else if (strcmp(to[0], "pwd") == 0)
                    print_pwd();
                else if (strcmp(to[0], "cd") == 0)
                {
                    getcwd(sting, 100);
                    print_cd(to, prev_dir, init_path, sting);
                }
                else if (strcmp(to[0], "ls") == 0)
                    print_ls(to, username, hostname);
                else if (strcmp(to[0], "pinfo") == 0)
                    print_pinfo(to);
                else if (strcmp(to[0], "discover") == 0)
                    print_discover(to, init_path, size);
                else if (strcmp(to[0], "jobs") == 0)
                    jobs();
                else if (strcmp(to[0], "fg") == 0)
                    fg(atoi(to[1]));
                else if (strcmp(to[0], "bag") == 0)
                    fg(atoi(to[1]));
                else if (strcmp(to[0], "sig") == 0)
                    sig(atoi(to[1]), atoi(to[2]));
                else if (strcmp(to[0], "history") == 0)
                {
                    char fi[100] = {'\0'};
                    strcpy(fi, init_path);
                    strcat(fi, "/History.txt");
                    print_history(fi);
                }
                else
                    command(comma);
                i++;
                printf("\n");
                if (oo != 0)
                    dup2(oo, STDOUT_FILENO);
                if (oi != 0)
                    dup2(oi, STDIN_FILENO);
            }
        }
    }
}