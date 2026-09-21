#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include "builtin.h"

int is_builtin(const char *command)
{
    if (command == NULL)
        return 0;

    if (strcmp(command, "cd") == 0)
        return 1;

    if (strcmp(command, "pwd") == 0)
        return 1;

    if (strcmp(command, "echo") == 0)
        return 1;

    if (strcmp(command, "exit") == 0)
        return 1;

    return 0;
}

static int builtin_cd(command_t *command)
{
    int argc = 0;

    while (argc < MAX_ARGS && command->argv[argc] != NULL)
        argc++;

    char *dir;

    if (argc == 1)
    {
        dir = getenv("HOME");

        if (dir == NULL)
        {
            printf("cd: HOME not set\n");
            return 1;
        }
    }
    else if (argc == 2)
    {
        dir = command->argv[1];
    }
    else
    {
        printf("cd: too many arguments\n");
        return 1;
    }

    if (chdir(dir) != 0)
    {
        perror("cd");
        return 1;
    }

    return 1;
}

static int builtin_pwd(command_t *command)
{
    int argc = 0;

    while (argc < MAX_ARGS && command->argv[argc] != NULL)
        argc++;

    if (argc > 1)
    {
        printf("pwd: too many arguments\n");
        return 1;
    }

    char buffer[PATH_MAX];

    if (getcwd(buffer, sizeof(buffer)) == NULL)
    {
        perror("pwd");
        return 1;
    }

    printf("%s\n", buffer);
    printf("Ch. Nikhila(2500032254)\n");
    return 1;
}

static int builtin_echo(command_t *command)
{
    int i = 1;

    while (i < MAX_ARGS && command->argv[i] != NULL)
    {
        printf("%s", command->argv[i]);

        if (command->argv[i + 1] != NULL)
            printf(" ");

        i++;
    }

    printf("\n");

    return 1;
}

static int builtin_exit(command_t *command)
{
    int argc = 0;

    while (argc < MAX_ARGS && command->argv[argc] != NULL)
        argc++;

    if (argc > 1)
    {
        printf("exit: too many arguments\n");
        return 1;
    }

    return 2;
}

int builtin_execute(command_t *command)
{
    if (command == NULL || command->argv[0] == NULL)
        return 0;

    if (strcmp(command->argv[0], "cd") == 0)
        return builtin_cd(command);

    if (strcmp(command->argv[0], "pwd") == 0)
        return builtin_pwd(command);

    if (strcmp(command->argv[0], "echo") == 0)
        return builtin_echo(command);

    if (strcmp(command->argv[0], "exit") == 0)
        return builtin_exit(command);

    return 0;
}
