/*
 Betik
 Copyright (C) 2010-2011 cenan ozen <cenan.ozen[at]gmail.com>

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

#ifndef token_h
#define token_h

#include "common.h"

#define MAX_IDENT_LENGTH   64
#define MAX_STRING_LENGTH  1024

typedef enum {
	TT_NONE       =  0,
	TT_IDENT      =  1,
	TT_NUMBER     =  2,
	TT_STRING     =  3,
	TT_EOF        =  4,
	TT_UNKNOWN    =  5,

	// binary operators
	TT_OP_AND     = 10,
	TT_OP_OR      = 11,
	TT_OP_ADD     = 12,
	TT_OP_SUB     = 13,
	TT_OP_MUL     = 14,
	TT_OP_DIV     = 15,
	TT_OP_EQUAL   = 16,
	TT_OP_GT      = 17,
	TT_OP_LT      = 18,

	// unary operators
	TT_OP_UNARYSUB= 20,

	TT_OP_COMMA   = 30,
	TT_OP_POPEN   = 31,
	TT_OP_PCLOSE  = 32,
	TT_OP_BOPEN   = 33,
	TT_OP_BCLOSE  = 34,
	TT_OP_COPEN   = 35,
	TT_OP_CCLOSE  = 36,
	TT_NOP        = 40,

	// keywords
	TT_END        = 50,
	TT_IF         = 51,
	TT_WHILE      = 52,
	TT_DEF        = 53,
	TT_RETURN     = 54,
	TT_PRINT      = 55,
} token_type_t;

#define TOK_IS_BINARY_OP(t) (((t) >= 10) && ((t) < 20))
#define TOK_IS_UNARY_OP(t)  ((((t) >= 20) && ((t) < 30)) || ((t) == TT_OP_SUB))

typedef struct {
	char* source;
	int source_index;
	stack_t* index_stack;
	void* token_value;
	token_type_t token_type;
	int line_number;
} tokenizer_t;

void init_tokenizer(tokenizer_t* t, char* source);
void release_tokenizer(tokenizer_t* t);
token_type_t get_token(tokenizer_t* t);
void unget_token(tokenizer_t* t);

#endif // token_h

