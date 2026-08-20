#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

static void command_init(command_t *command)
{
    command->input = NULL;
    command->output = NULL;
    command->append = 0;
    command->background = 0;

    for (int i = 0; i < MAX_ARGS; i++)
        command->argv[i] = NULL;
}

void pipeline_init(pipeline_t *pipeline)
{
    pipeline->command_count = 0;

    for (int i = 0; i < MAX_COMMANDS; i++)
        command_init(&pipeline->commands[i]);
}

static int add_argument(command_t *command, char *value)
{
    int i = 0;

    while (i < MAX_ARGS - 1 && command->argv[i] != NULL)
        i++;

    if (i >= MAX_ARGS - 1)
    {
        printf("Error: too many arguments\n");
        return 0;
    }

    command->argv[i] = value;
    command->argv[i + 1] = NULL;

    return 1;
}

int parse(token_list_t *tokens, pipeline_t *pipeline)
{
    pipeline_init(pipeline);

    if (tokens == NULL || tokens->count == 0)
        return 0;

    pipeline->command_count = 1;

    command_t *command = &pipeline->commands[0];

    for (int i = 0; i < tokens->count; i++)
    {
        token *t = &tokens->tokens[i];

        switch (t->type)
        {
            case TOKEN_WORD:
                if (!add_argument(command, t->text))
                    return 0;
                break;

            case TOKEN_INPUT:
                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD)
                {
                    printf("Error: input file expected after <\n");
                    return 0;
                }

                command->input = tokens->tokens[++i].text;
                break;

            case TOKEN_OUTPUT:
                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD)
                {
                    printf("Error: output file expected after >\n");
                    return 0;
                }

                command->output = tokens->tokens[++i].text;
                command->append = 0;
                break;

            case TOKEN_APPEND:
                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD)
                {
                    printf("Error: output file expected after >>\n");
                    return 0;
                }

                command->output = tokens->tokens[++i].text;
                command->append = 1;
                break;

            case TOKEN_BACKGROUND:
                command->background = 1;
                break;

            case TOKEN_PIPE:
                if (command->argv[0] == NULL)
                {
                    printf("Error: command expected before pipe\n");
                    return 0;
                }

                if (pipeline->command_count >= MAX_COMMANDS)
                {
                    printf("Error: too many commands in pipeline\n");
                    return 0;
                }

                pipeline->command_count++;

                command =
                    &pipeline->commands[pipeline->command_count - 1];

                command_init(command);
                break;

            case TOKEN_END:
                break;

            default:
                printf("Error: unknown token\n");
                return 0;
        }
    }

    for (int i = 0; i < pipeline->command_count; i++)
    {
        if (pipeline->commands[i].argv[0] == NULL)
        {
            printf("Error: empty command in pipeline\n");
            return 0;
        }
    }

    return 1;
}

void pipeline_print(const pipeline_t *pipeline)
{
    printf("\n");
    printf("=========== PIPELINE ===========\n");

    for (int i = 0; i < pipeline->command_count; i++)
    {
        const command_t *command = &pipeline->commands[i];

        printf("\nCommand %d\n", i + 1);
        printf("--------------------------------\n");

        printf("Arguments\n");

        for (int j = 0; j < MAX_ARGS && command->argv[j] != NULL; j++)
        {
            printf("argv[%d] = %s\n", j, command->argv[j]);
        }

        printf("Input      : %s\n",
               command->input ? command->input : "None");

        printf("Output     : %s\n",
               command->output ? command->output : "None");

        printf("Append     : %s\n",
               command->append ? "Yes" : "No");

        printf("Background : %s\n",
               command->background ? "Yes" : "No");
    }

    printf("================================\n");
}

void pipeline_free(pipeline_t *pipeline)
{
    if (pipeline == NULL)
        return;

    pipeline->command_count = 0;
}
