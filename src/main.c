#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "builtin.h"

int main(void)
{
    char *line;
    token_list_t tokens;
    pipeline_t pipeline;

    printf("=====================================\n");
    printf("          Shellforge\n");
    printf("   A Unix Style Shell written in C\n");
    printf("=====================================\n");

    lexer_init();

    while (1)
    {
        line = readline("shellforge$ ");

        if (line == NULL)
        {
            printf("\nGoodbye!\n");
            break;
        }

        if (strlen(line) == 0)
        {
            free(line);
            continue;
        }

        add_history(line);

        token_list_init(&tokens);
        lexer_tokenize(line, &tokens);

        token_print(&tokens);

        if (parse(&tokens, &pipeline))
        {
            expand_variables(&pipeline);
            pipeline_print(&pipeline);

            if (pipeline.command_count == 1 &&
                is_builtin(pipeline.commands[0].argv[0]))
            {
                int result = builtin_execute(&pipeline.commands[0]);

                if (result == 2)
                {
                    pipeline_free(&pipeline);
                    free(line);
                    break;
                }
            }

            pipeline_free(&pipeline);
        }

        free(line);
    }

    return 0;
}
