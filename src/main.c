#include <stdio.h>
#include <string.h>
#include "../include/lexer.h"

int main(void)
{
    char input[256];
    token_list_t tokens;

    printf("==============================\n");
    printf("        Shellforge\n");
    printf("A Unix Style Shell written in C\n");
    printf("==============================\n");

    while (1)
    {
        printf("shellforge$ ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting..\n");
            break;
        }

        if (input[0] == '\0')
            continue;

        lexer_init();
        lexer_tokenize(input, &tokens);
        token_print(&tokens);
    }

    return 0;
}
