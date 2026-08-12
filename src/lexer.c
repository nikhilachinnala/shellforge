#include <ctype.h>
#include <string.h>
#include "../include/lexer.h"

void lexer_init(void)
{
}

void lexer_tokenize(const char *input, token_list_t *list)
{
    token_list_init(list);

    int i = 0;

    while (input[i] != '\0')
    {
        if (isspace((unsigned char)input[i]))
        {
            i++;
            continue;
        }

        if (input[i] == '|')
        {
            token_add(list, TOKEN_PIPE, "|");
            i++;
            continue;
        }

        if (input[i] == '<')
        {
            token_add(list, TOKEN_INPUT, "<");
            i++;
            continue;
        }

        if (input[i] == '>')
        {
            if (input[i + 1] == '>')
            {
                token_add(list, TOKEN_APPEND, ">>");
                i += 2;
            }
            else
            {
                token_add(list, TOKEN_OUTPUT, ">");
                i++;
            }
            continue;
        }

        if (input[i] == '&')
        {
            token_add(list, TOKEN_BACKGROUND, "&");
            i++;
            continue;
        }

        char word[MAX_TOKEN_LEN];
        int j = 0;

        while (input[i] != '\0' &&
               !isspace((unsigned char)input[i]) &&
               input[i] != '|' &&
               input[i] != '<' &&
               input[i] != '>' &&
               input[i] != '&')
        {
            if (j < MAX_TOKEN_LEN - 1)
                word[j++] = input[i];

            i++;
        }

        word[j] = '\0';

        if (j > 0)
            token_add(list, TOKEN_WORD, word);
    }

    token_add(list, TOKEN_END, "END");
}
