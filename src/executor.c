#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "executor.h"

int execute_external(command_t *command)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        int fd;

        /* Input redirection */
        if (command->input != NULL)
        {
            fd = open(command->input, O_RDONLY);

            if (fd < 0)
            {
                perror(command->input);
                exit(EXIT_FAILURE);
            }

            if (dup2(fd, STDIN_FILENO) < 0)
            {
                perror("dup2");
                close(fd);
                exit(EXIT_FAILURE);
            }

            close(fd);
        }

        /* Output redirection */
        if (command->output != NULL)
        {
            if (command->append)
            {
                fd = open(command->output,
                          O_WRONLY | O_CREAT | O_APPEND,
                          0644);
            }
            else
            {
                fd = open(command->output,
                          O_WRONLY | O_CREAT | O_TRUNC,
                          0644);
            }

            if (fd < 0)
            {
                perror(command->output);
                exit(EXIT_FAILURE);
            }

            if (dup2(fd, STDOUT_FILENO) < 0)
            {
                perror("dup2");
                close(fd);
                exit(EXIT_FAILURE);
            }

            close(fd);
        }

        execvp(command->argv[0], command->argv);

        perror(command->argv[0]);
        exit(EXIT_FAILURE);
    }

    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return 1;
    }

    return 1;
}
