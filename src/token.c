#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"

static int is_whitespace(char c)
{
    return ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'));
}

static int is_alpha(char c)
{
    if (c < 0)
    {
        return 1;
    }
    return (((c >= 'a') && (c <= 'z')) ||
            ((c >= 'A') && (c <= 'Z')) ||
            (c == '_'));
}

static int is_alphanum(char c)
{
    return (is_alpha(c) || isdigit(c));
}

static int is_digit(char c)
{
    return ((c >= '0') && (c <= '9'));
}

static struct
{
    char *str;
    token_type_t token_type;
} keywords[] = {
    {"and", TT_OP_AND},
    {"or", TT_OP_OR},
    {"if", TT_IF},
    {"else", TT_ELSE},
    {"while", TT_WHILE},
    {"end", TT_END},
    {"def", TT_DEF},
    {"return", TT_RETURN},
    {"print", TT_PRINT},
};

static struct
{
    char *op;
    token_type_t token;
} operators[] = {
    {"+", TT_OP_ADD},
    {"-", TT_OP_SUB},
    {"*", TT_OP_MUL},
    {"/", TT_OP_DIV},
    {".", TT_OP_DOT},
    {",", TT_OP_COMMA},
    {"(", TT_OP_POPEN},
    {")", TT_OP_PCLOSE},
    {"[", TT_OP_BOPEN},
    {"]", TT_OP_BCLOSE},
    {"{", TT_OP_COPEN},
    {"}", TT_OP_CCLOSE},
    {"==", TT_OP_EQUAL},
    {"<>", TT_OP_NOTEQUAL},
    {"=", TT_OP_ASSIGN},
    {">=", TT_OP_GTE},
    {">", TT_OP_GT},
    {"<=", TT_OP_LTE},
    {"<", TT_OP_LT},
    {":", TT_OP_COLON},
};

void init_tokenizer(tokenizer_t *t, char *source)
{
    t->source = duplicate_string(source);
    t->source_index = 0;
    t->index_stack = create_stack(sizeof(int));
    t->token_type = TT_NONE;
    t->line_number = 1;
}

void release_tokenizer(tokenizer_t *t)
{
    free(t->source);
    destroy_stack(t->index_stack);
    memset(t, 0, sizeof(tokenizer_t));
}

static void eatwhitespace(tokenizer_t *t)
{
eatwhitespace_begin:

    while (is_whitespace(t->source[t->source_index]))
    {
        if ('\n' == t->source[t->source_index])
        {
            t->line_number++;
        }
        t->source_index++;
    }

    // single line comments
    if (t->source[t->source_index] == '#')
    {
        while (t->source[t->source_index] != '\n')
        {
            // incase there is a comment on the last line
            // or the only line is a comment line then
            // we should check for an EOF
            if (t->source[t->source_index] == '\0')
            {
                printf("eol\n");
                break;
            }
            t->source_index++;
        }
        goto eatwhitespace_begin;
    }

    // multi line comments
    if (('/' == t->source[t->source_index]) &&
        ('*' == t->source[t->source_index + 1]))
    {
        t->source_index += 2;
        while (t->source[t->source_index] != '\0')
        {
            if (('*' == t->source[t->source_index]) &&
                ('/' == t->source[t->source_index + 1]))
            {
                t->source_index += 2;
                break;
            }
            t->source_index++;
        }
        goto eatwhitespace_begin;
    }
}

static void tokenize_number(tokenizer_t *t)
{
    t->token_type = TT_NUMBER;
    t->token_value.int_val = t->source[t->source_index++] - '0';
    while (is_digit(t->source[t->source_index]))
    {
        t->token_value.int_val *= 10;
        t->token_value.int_val += t->source[t->source_index] - '0';
        t->source_index++;
    }
}

static void tokenize_identifier(tokenizer_t *t)
{
    int i = 0;

    t->token_value.str_val[i++] = t->source[t->source_index++];
    while (is_alphanum(t->source[t->source_index]))
    {
        t->token_value.str_val[i++] = t->source[t->source_index++];
        if (MAX_IDENT_LENGTH == i)
        {
            fprintf(stderr, "MAX_IDENT_LENGTH reached :%d\n", t->line_number);
            exit(1);
        }
    }
    t->token_value.str_val[i] = '\0';
    for (i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i)
    {
        if (strcmp(t->token_value.str_val, keywords[i].str) == 0)
        {
            t->token_type = keywords[i].token_type;
            return;
        }
    }
    t->token_type = TT_IDENT;
}

static void tokenize_string(tokenizer_t *t)
{
    int i = 0;

    t->source_index++;
    while (t->source[t->source_index] != '"')
    {
        if (t->source[t->source_index] == '\\')
        {
            if (t->source[t->source_index + 1] == 'n')
            {
                t->token_value.str_val[i++] = '\n';
                t->source_index += 2;
            }
            else if (t->source[t->source_index + 1] == '"')
            {
                t->token_value.str_val[i++] = '\"';
                t->source_index += 2;
            }
            else if (t->source[t->source_index + 1] == '\\')
            {
                t->token_value.str_val[i++] = '\\';
                t->source_index += 2;
            }
            else
            {
                t->token_value.str_val[i++] = '\\';
                t->source_index++;
            }
        }
        else
        {
            t->token_value.str_val[i++] = t->source[t->source_index++];
        }
        if (i >= MAX_STRING_LENGTH)
        {
            fprintf(stderr, "max inline string length reached\n");
            exit(EXIT_FAILURE);
        }
    }
    t->token_value.str_val[i] = '\0';
    t->source_index++; // eat last "
    t->token_type = TT_STRING;
}

token_type_t get_token(tokenizer_t *t)
{
    eatwhitespace(t);

    stack_push(t->index_stack, (void *)&t->source_index);

    if ('\0' == t->source[t->source_index])
    {
        t->token_type = TT_EOF;
        return TT_EOF;
    }

    if (is_digit(t->source[t->source_index]))
    {
        tokenize_number(t);
        return TT_NUMBER;
    }

    if (is_alpha(t->source[t->source_index]))
    {
        tokenize_identifier(t);
        return t->token_type;
    }

    if ('"' == t->source[t->source_index])
    {
        tokenize_string(t);
        return TT_STRING;
    }

    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++)
    {
        if (strncmp(operators[i].op, &t->source[t->source_index], strlen(operators[i].op)) == 0)
        {
            t->source_index += strlen(operators[i].op);
            t->token_type = operators[i].token;
            return operators[i].token;
        }
    }

    return TT_UNKNOWN;
}

void unget_token(tokenizer_t *t)
{
    int prev_source_index = *(int *)stack_pop(t->index_stack);
    if (t->source_index <= prev_source_index)
    {
        t->source_index = prev_source_index;
        return;
    }
    do
    {
        t->source_index--;
        if ('\n' == t->source[t->source_index])
        {
            t->line_number--;
        }
    } while (t->source_index > prev_source_index);
}
