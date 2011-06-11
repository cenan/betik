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

#include <stdio.h>

#include "common.h"
#include "interpreter.h"

static void int_block(block_t* b);
static int int_expression(expression_t* e);
static int int_funccall(funccall_t* f);
static void int_statement(statement_t* s);
static int int_value(value_t* v);

static void int_block(block_t* b)
{
	int i;
	
	for (i = 0; i < list_get_item_count(b->statements); i++) {
		int_statement(list_get_item(b->statements, i));
	}
}

static int int_expression(expression_t* e)
{
	//printf("value count: %d\n", list_get_item_count(e->values));
	//printf("bop count: %d\n", list_get_item_count(e->binaryops));
	//printf("uop count: %d\n", list_get_item_count(e->unaryops));
	//int i;
	
	//for (i = 0; i < list_get_item_count(e->values); i++) {
	//	int_value(list_get_item(e->values, i));
	//}
	value_t* v0 = (value_t*)list_get_item(e->values, 0);
	int ret;
	if (VT_CNUMBER == v0->type) {
		ret = (int)v0->value;
	} else if (VT_FUNCCALL == v0->type) {
		ret = int_funccall(v0->value);
	}
	int i;
	for (i = 1; i < list_get_item_count(e->values); i++) {
		value_t* v = (value_t*)list_get_item(e->values, i);
		int foo;
		if (VT_CNUMBER == v->type) {
	   		foo = (int)v->value;
		} else if (VT_FUNCCALL == v->type) {
			foo = int_funccall(v->value);
		} else if (VT_EXPRESSION == v->type) {
			foo = int_expression(v->value);
		}
		token_type_t tok = (token_type_t)list_get_item(e->binaryops, i-1);
		if (TT_OP_ADD == tok) {
			ret += foo;
		} else if (TT_OP_SUB == tok) {
			ret -= foo;
		} else if (TT_OP_MUL == tok) {
			ret *= foo;
		} else if (TT_OP_DIV == tok) {
			ret /= foo;
		}
	}
	return ret;
}

static int int_funccall(funccall_t* f)
{
	int result;

	if (strcmp(f->function_name, "print") == 0) {
		result = int_expression(list_get_item(f->arguments, 0));
		printf("%d\n", result);
	}
}

static void int_statement(statement_t* s)
{
	if (s->type == ST_EXPRESSION) {
		int_expression(s->value);
	}
}

static int int_value(value_t* v)
{
	if (VT_FUNCCALL == v->type) {
		int_funccall(v->value);
	}
}

void interpret(parser_t* p)
{
	int i;
	
	for (i = 0; i < list_get_item_count(p->ast->statement_list); i++) {
		int_statement(list_get_item(p->ast->statement_list, i));
	}
}

