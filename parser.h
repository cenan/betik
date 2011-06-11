/*
 Betik
 Copyright (C) 2010 cenan ozen <cenan.ozen[at]gmail.com>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef parser_h
#define parser_h

#include "token.h" 
#include "common.h"

typedef enum {
	ST_EXPRESSION,
	ST_IF,
	ST_WHILE
} statement_type_t;

typedef enum {
	VT_EXPRESSION,
	VT_CNUMBER,
	VT_CSTRING,
	VT_FUNCCALL,
	VT_IDENT
} value_type_t;


typedef struct {
	list_t* statements;
} block_t;

typedef struct {
	list_t* values;
	list_t* binaryops;
	list_t* unaryops;
	int line_number;
} expression_t;

typedef struct {
	char function_name[MAX_IDENT_LENGTH];
	list_t* arguments;
} funccall_t;

typedef struct {
	char name[MAX_IDENT_LENGTH];
	list_t* parameters;
	block_t* block;
	int line_number;
} funcdef_t;

typedef struct {
	expression_t* expression;
	block_t* block;
} ifstatement_t;

typedef struct {
	statement_type_t type;
	void* value;
} statement_t;

typedef struct {
	value_type_t type;
	void* value;
} value_t;

typedef struct {
	token_type_t type;
	char name[MAX_IDENT_LENGTH];
} vardecl_t;

typedef struct {
	expression_t* expression;
	block_t* block;
} whilestatement_t;

typedef struct {
	list_t* statement_list;
	list_t* function_list;
} ast_t;

typedef struct {
	tokenizer_t* t;
	ast_t* ast;
} parser_t;

void init_parser(parser_t* p, char* source);
void release_parser(parser_t* p);
void parse(parser_t* p);

#endif // parser_h

