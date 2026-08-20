#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expand.h"

static char *copy_string(const char *value)
{
    if (value == NULL)
        return NULL;

    char *copy = malloc(strlen(value) + 1);

    if (copy == NULL)
        return NULL;

    strcpy(copy, value);

    return copy;
}

static char *expand_string(const char *value)
{
    if (value == NULL)
        return NULL;

    if (value[0] == '$')
    {
        const char *name = value + 1;

        if (*name == '\0')
            return copy_string(value);

        const char *env = getenv(name);

        if (env != NULL)
            return copy_string(env);
    }

    return copy_string(value);
}

void expand_variables(pipeline_t *pipeline)
{
    if (pipeline == NULL)
        return;

    for (int i = 0; i < pipeline->command_count; i++)
    {
        command_t *command = &pipeline->commands[i];

        for (int j = 0; j < MAX_ARGS && command->argv[j] != NULL; j++)
        {
            char *expanded = expand_string(command->argv[j]);

            if (expanded != NULL)
                command->argv[j] = expanded;
        }

        if (command->input != NULL)
        {
            char *expanded = expand_string(command->input);

            if (expanded != NULL)
                command->input = expanded;
        }

        if (command->output != NULL)
        {
            char *expanded = expand_string(command->output);

            if (expanded != NULL)
                command->output = expanded;
        }
    }
}
