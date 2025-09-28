#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

int main()
{
    int fd_history = open("history.txt", O_RDWR | O_TRUNC);
    int fd_env = open("env.txt", O_RDWR | O_TRUNC);
    char help[] = {"pwd, exit, cd, history, echo, setenv, unsetenv, chprompt, clear"};
    printf("Welcome to the Simple Shell\n");
    char prompt[] = {"my_shell>"};
    while (1)
    {
        printf("%s ", prompt);
        char command[1024] = {0};
        char for_history[1024] = {0};
        fgets(command, sizeof(command), stdin);
        strcpy(for_history, command);
        command[strcspn(command, "\n")] = 0;
        char **arr = malloc(sizeof(char *) * 4);
        for (int i = 0; i < 4; i++)
        {
            arr[i] = malloc(sizeof(char) * 10);
        }
        int i = 1;
        arr[0] = strtok(command, " ");
        while (arr[i - 1] != NULL)
        {
            arr[i] = strtok(NULL, " ");
            i++;
        }
        if (strcmp(arr[0], "cd") == 0)
        {
            if (i != 3)
            {
                printf("%d\n", i);
                printf("cd: too many arguments\n");
                continue;
            }
            if (chdir(arr[1]) == 0)
            {
                write(fd_history, for_history, strlen(for_history));
                continue;
            }
            printf("wrong input\n");
            continue;
        }
        else if (strcmp(arr[0], "pwd") == 0)
        {
            char cwd[1024];
            if (i != 2)
            {
                printf("wrong input\n");
                continue;
            }
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {

                printf("%s\n", cwd);
                write(fd_history, for_history, strlen(for_history));
                continue;
            }
            printf("getcwd() error\n");
            continue;
        }
        else if (strcmp(arr[0], "exit") == 0)
        {
            if (i != 2)
            {
                printf("wrong input\n");
                continue;
            }
            free(arr);
            write(fd_history, for_history, strlen(for_history));
            exit(0);
        }
        else if (strcmp(arr[0], "history") == 0)
        {
            if (i != 2)
            {
                printf("wrong input\n");
                continue;
            }
            int fd_h = open("history.txt", O_RDONLY);
            char ch[1024] = {0};
            read(fd_h, ch, sizeof(ch));
            ch[strlen(ch) - 1] = '\0';
            printf("%s\n", ch);
            // fflush(stdout);
            close(fd_h);
            write(fd_history, for_history, strlen(for_history));
            continue;
        }
        else if (strcmp(arr[0], "setenv") == 0)
        {
            if (i != 4)
            {
                printf("setenv: enter SETVAR NAME VALUE\n");
                continue;
            }
            int tmp = open("env.txt", O_WRONLY | O_APPEND);
            // printf("%s\n",for_history);
            char for_env[1024] = {0};
            strcat(for_env, arr[1]);
            strcat(for_env, "=");
            strcat(for_env, arr[2]);
            strcat(for_env, "\n");
            write(tmp, for_env, strlen(for_env));
            write(fd_history, for_history, strlen(for_history));
            continue;
        }
        else if (strcmp(arr[0], "unsetenv") == 0)
        {
            if (i != 3)
            {
                printf("unsetenv: enter UNSETENV NAME\n");
                continue;
            }
            int tmp = open("env.txt", O_RDWR);
            // printf("%c\n",arr[1][0]);
            char env[1024] = {0};
            char res_env[1024] = {0};
            read(tmp, env, sizeof(env));
            bool flag = false;
            int i = 0;
            int k = 0;
            while (env[i] != '\0')
            {
                // printf("%c %c\n",env[i],arr[1][0]);
                if (arr[1][0] == env[i])
                {
                    flag = true;
                    while (env[i] != '\n')
                    {
                        i++;
                    }
                }
                else
                {
                    res_env[k++] = env[i++];
                }
            }
            res_env[k] = '\0';
            if (!flag)
            {
                printf("unsetenv: variable not found\n");
                continue;
            }
            close(tmp);
            tmp = open("env.txt", O_TRUNC | O_WRONLY);
            write(tmp, res_env, strlen(res_env));
            write(fd_history, for_history, strlen(for_history));
            continue;
        }
        else if (strcmp(arr[0], "chprompt") == 0)
        {
            if (i != 3)
            {
                printf("chprompt: enter CHPROMPT NAME\n");
                continue;
            }
            memset(prompt, 0, sizeof(prompt));
            strcat(prompt, arr[1]);
            write(fd_history, for_history, strlen(for_history));
            continue;
        }
        else if (strcmp(arr[0], "help") == 0)
        {
            if (i != 2)
            {
                printf("wrong input\n");
                continue;
            }
            printf("%s\n", help);
            write(fd_history, for_history, strlen(for_history));
            continue;
        }
        else if (strcmp(arr[0], "echo") == 0)
        {
            if (i == 2)
            {
                printf("wrong input\n");
                continue;
            }
            if (arr[1][0] == '$')
            {
                char ch = arr[1][1];
                char env[1024] = {0};
                int fd_e = open("env.txt", O_RDONLY);
                read(fd_e, env, sizeof(env));
                close(fd_e);
                bool flag = false;
                int j = 0;
                while (env[j] != '\0')
                {
                    if (env[j] == ch)
                    {
                        char buf[1024] = {0};
                        for (int i = 1; arr[1][i] != '\0'; ++i)
                        {
                            buf[i - 1] = arr[1][i];
                        }
                        char for_env[1024] = {0};
                        int k = 0;
                        while (env[j] != '=')
                        {
                            for_env[k++] = env[j++];
                        }
                        ++j;
                        if (strcmp(buf, for_env) == 0)
                        {
                            flag = true;
                            while (env[j] != 10 && env[j] != 0)
                            {
                                printf("%c", env[j++]);
                            }
                            printf("\n");
                        }
                        // bool flag1 = true;
                        // for (int i = 1; env[j] != '\0' && arr[1][i]!='\0'; ++i, j++)
                        // {
                        //     printf("%c %c\n", env[j], arr[1][i]);
                        //     if (env[j] != arr[1][i])
                        //     {
                        //         flag1 = false;
                        //         break;
                        //     }
                        // }
                        // if (flag1)
                        // {
                        //     while (env[j] != '=')
                        //     {
                        //         ++j;
                        //     }
                        //     j++;
                        //     flag = true;
                        //     while (env[j] != '\0' && env[j] != '\n')
                        //     {
                        //         printf("%c", env[j++]);
                        //     }
                        //     printf("\n");
                        //     break;
                        // }
                        // continue;
                    }
                    ++j;
                }
                if (!flag)
                {
                    printf("%s not found\n", arr[1]);
                    continue;
                }
                continue;
            }
            for (int j = 1; j < i - 1; j++)
            {
                printf("%s ", arr[j]);
            }
            write(fd_history, for_history, strlen(for_history));
            printf("\n");
        }
        else if (strcmp(arr[0], "clear") == 0)
        {
            if (i != 2)
            {
                printf("wrong input\n");
                continue;
            }
            printf("\033[H\033[J");
            write(fd_history, for_history, strlen(for_history));
            continue;
        }
        else
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                execvp(arr[0], arr);
                printf("command not found\n");
                exit(1);
            }
            wait(NULL);
        }
    }
}
