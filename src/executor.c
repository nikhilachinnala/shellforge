#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "executor.h"

static int execute_command(command_t *command)
{
    pid_t pid;
    int status;

    if (command == NULL || command->argv[0] == NULL)
        return 1;

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
                _exit(127);
            }

            if (dup2(fd, STDIN_FILENO) < 0)
            {
                perror("dup2");
                close(fd);
                _exit(127);
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
                _exit(127);
            }

            if (dup2(fd, STDOUT_FILENO) < 0)
            {
                perror("dup2");
                close(fd);
                _exit(127);
            }

            close(fd);
        }

        execvp(command->argv[0], command->argv);

        perror(command->argv[0]);
        _exit(127);
    }

    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return 1;
}

int execute_external(command_t *command)
{
    return execute_command(command);
}

int execute_pipeline(pipeline_t *pipeline)
{
    int command_count;
    int previous_read = -1;
    int pipefd[2];
    pid_t pids[MAX_COMMANDS];
    int status;
    int last_status = 0;

    if (pipeline == NULL || pipeline->command_count <= 0)
        return 1;

    command_count = pipeline->command_count;

    if (command_count == 1)
        return execute_command(&pipeline->commands[0]);

    for (int i = 0; i < command_count; i++)
    {
        if (i < command_count - 1)
        {
            if (pipe(pipefd) < 0)
            {
                perror("pipe");
                return 1;
            }
        }

        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("fork");

            if (i < command_count - 1)
            {
                close(pipefd[0]);
                close(pipefd[1]);
            }

            if (previous_read != -1)
                close(previous_read);

            return 1;
        }

        if (pids[i] == 0)
        {
            /* Input from previous pipe */
            if (previous_read != -1)
            {
                if (dup2(previous_read, STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    _exit(127);
                }
            }

            /* Output to current pipe */
            if (i < command_count - 1)
            {
                if (dup2(pipefd[1], STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    _exit(127);
                }
            }

            /* Close unused descriptors */
            if (previous_read != -1)
                close(previous_read);

            if (i < command_count - 1)
            {
                close(pipefd[0]);
                close(pipefd[1]);
            }

            execvp(pipeline->commands[i].argv[0],
                   pipeline->commands[i].argv);

            perror(pipeline->commands[i].argv[0]);
            _exit(127);
        }

        /* Parent closes previous pipe */
        if (previous_read != -1)
            close(previous_read);

        /* Keep current pipe's read end */
        if (i < command_count - 1)
        {
            close(pipefd[1]);
            previous_read = pipefd[0];
        }
    }

    /* Wait for all children */
    for (int i = 0; i < command_count; i++)
    {
        if (waitpid(pids[i], &status, 0) < 0)
        {
            perror("waitpid");
            continue;
        }

        if (i == command_count - 1 && WIFEXITED(status))
            last_status = WEXITSTATUS(status);
    }

    return last_status;
}
