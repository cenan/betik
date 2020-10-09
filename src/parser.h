#ifndef parser_h
#define parser_h

#include "token.h"
#include "common.h"

typedef enum {
    ST_EXPRESSION,
    ST_IF,
    ST_WHILE,
    ST_RETURN,
    ST_PRINT
} statement_type_t;

typedef enum {
    VT_EXPRESSION,
    VT_CNUMBER,
    VT_CSTRING,
    VT_FUNCCALL,
    VT_IDENT,
    VT_INLINE_FUNC,
    VT_INLINE_OBJ,
    VT_LIST,
    VT_LISTINDEX,
} value_type_t;

typedef struct {
    list_t *statements;
} block_t;

typedef struct {
    list_t *values;
    list_t *binaryops;
    list_t *unaryops;
    int line_number;
} expression_t;

typedef struct {
    char function_name[MAX_IDENT_LENGTH];
    list_t *arguments;
} funccall_t;

typedef struct {
    char name[MAX_IDENT_LENGTH];
    list_t *parameters;
    block_t *block;
    int line_number;
} funcdef_t;

typedef struct {
    expression_t *expression;
    block_t *block;
    block_t *else_block;
} ifstatement_t;

typedef struct {
    char *name;
    expression_t *index;
} listindex_t;

typedef struct {
    list_t *keys;
    list_t *values;
} inlineobj_t;

typedef struct {
    statement_type_t type;
    void *value;
} statement_t;

typedef struct _value_t {
    value_type_t type;
    void *value;
    struct _value_t *subvalue;
} value_t;

typedef struct {
    char name[MAX_IDENT_LENGTH];
} vardecl_t;

typedef struct {
    expression_t *expression;
    block_t *block;
} whilestatement_t;

typedef struct {
    list_t *statement_list;
    list_t *function_list;
} ast_t;

typedef struct {
    tokenizer_t *t;
    ast_t *ast;
} parser_t;

void init_parser(parser_t *p, char *source);
void release_parser(parser_t *p);
void parse(parser_t *p);

#endif // parser_h
