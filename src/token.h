#ifndef token_h
#define token_h

#include "common.h"

#define MAX_IDENT_LENGTH 64
#define MAX_STRING_LENGTH 1024

typedef enum
{
    TT_NONE = 0,
    TT_IDENT = 1,
    TT_NUMBER = 2,
    TT_STRING = 3,
    TT_EOF = 4,
    TT_UNKNOWN = 5,

    // binary operators
    TT_OP_AND = 10,
    TT_OP_OR = 11,
    TT_OP_ADD = 12,
    TT_OP_SUB = 13,
    TT_OP_MUL = 14,
    TT_OP_DIV = 15,
    TT_OP_ASSIGN = 16,
    TT_OP_GT = 17,
    TT_OP_LT = 18,
    TT_OP_GTE = 19,
    TT_OP_LTE = 20,
    TT_OP_EQUAL = 21,
    TT_OP_NOTEQUAL = 22,
    TT_OP_DOT = 23,

    // unary operators
    TT_OP_UNARYSUB = 30,

    TT_OP_COMMA = 40,
    TT_OP_POPEN = 41,
    TT_OP_PCLOSE = 42,
    TT_OP_BOPEN = 43,
    TT_OP_BCLOSE = 44,
    TT_OP_COPEN = 45,
    TT_OP_CCLOSE = 46,
    TT_OP_COLON = 47,
    TT_NOP = 50,

    // keywords
    TT_END = 80,
    TT_IF = 81,
    TT_ELSE = 82,
    TT_WHILE = 83,
    TT_DEF = 84,
    TT_RETURN = 85,
    TT_PRINT = 86,
} token_type_t;

#define TOK_IS_BINARY_OP(t) (((t) >= 10) && ((t) < 30))
#define TOK_IS_UNARY_OP(t) ((((t) >= 30) && ((t) < 40)) || ((t) == TT_OP_SUB))

typedef struct
{
    char *source;
    int source_index;
    btk_stack_t *index_stack;
    union {
        int int_val;
        char str_val[MAX_STRING_LENGTH];
    } token_value;
    token_type_t token_type;
    int line_number;
} tokenizer_t;

void init_tokenizer(tokenizer_t *t, char *source);
void release_tokenizer(tokenizer_t *t);
token_type_t get_token(tokenizer_t *t);
void unget_token(tokenizer_t *t);

#endif // token_h
