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
#include <stdlib.h>

#include "common.h"
#include "interpreter.h"

typedef enum {
	OBJ_BASE,
	OBJ_NUMBER,
} object_type_t;

typedef struct {
	list_t* variables;
} scope_t;

typedef struct {
	object_type_t type;
	int reference_count;
	void* data;
} object_t;

typedef struct {
	char* name;
	object_t* obj;
} variable_t;

typedef struct {
	scope_t* global_scope;
	scope_t* current_scope;
} runtime_t;


static scope_t* create_scope()
{
	scope_t* scope = (scope_t*)malloc(sizeof(scope_t));
	scope->variables = create_list();
	return scope;
}

static void destroy_scope(scope_t* s)
{
	destroy_list(s->variables);
	free(s);
}

static variable_t* get_variable(runtime_t* rt, char* variable_name)
{
	int i;
	for (i = 0; i < list_get_item_count(rt->current_scope->variables); i++) {
		variable_t* var = list_get_item(rt->current_scope->variables, i);
		if (strcmp(variable_name, var->name) == 0) {
			return var;
		}
	}
	return 0;
}

static variable_t* create_variable(runtime_t* rt, char* variable_name)
{
	variable_t* var = (variable_t*)malloc(sizeof(variable_t));
	var->name = variable_name;
	var->obj = 0;
	list_insert(rt->current_scope->variables, var);

	return var;
}

static object_t* create_object(object_type_t obj_type)
{
	object_t* obj = (object_t*)malloc(sizeof(object_t));

	obj->type = obj_type;
	obj->reference_count = 0;

	return obj;
}

static variable_t* call_variable_op(runtime_t* rt, variable_t* var1, variable_t* var2, token_type_t tok)
{
	variable_t* var = create_variable(rt, "#");
	var->obj = create_object(OBJ_NUMBER);
	var->obj->reference_count += 1;
	if (TT_OP_ADD == tok) {
		var->obj->data = (void*)((int)var1->obj->data + (int)var2->obj->data);
	} else if (TT_OP_SUB == tok) {
		var->obj->data = (void*)((int)var1->obj->data - (int)var2->obj->data);
	} else if (TT_OP_MUL == tok) {
		var->obj->data = (void*)((int)var1->obj->data * (int)var2->obj->data);
	} else if (TT_OP_DIV == tok) {
		if (0 == (int)var2->obj->data) {
			fprintf(stderr, "Division by zero\n");
			exit(EXIT_FAILURE);
		}
		var->obj->data = (void*)((int)var1->obj->data / (int)var2->obj->data);
	} else if (TT_OP_EQUAL == tok) {
		var1->obj = var2->obj;
		var2->obj->reference_count += 1;
	}
	return var;
}


static void int_block(runtime_t* rt, block_t* b);
static variable_t* int_expression(runtime_t* rt, expression_t* e);
static variable_t*  int_funccall(runtime_t* rt, funccall_t* f);
static void int_statement(runtime_t* rt, statement_t* s);
static variable_t* int_value(runtime_t* rt, value_t* v);

static void int_block(runtime_t* rt, block_t* b)
{
	int i;
	
	for (i = 0; i < list_get_item_count(b->statements); i++) {
		int_statement(rt, list_get_item(b->statements, i));
	}
}

static variable_t* int_expression(runtime_t* rt, expression_t* e)
{
	value_t* v0 = (value_t*)list_get_item(e->values, 0);
	variable_t* var0 = int_value(rt, v0);
	variable_t* var = var0;
	int i;
	for (i = 1; i < list_get_item_count(e->values); i++) {
		value_t* v;
		variable_t* var1;
		v = (value_t*)list_get_item(e->values, i);
		var1 = int_value(rt, v);
		token_type_t tok = (token_type_t)list_get_item(e->binaryops, i-1);
		
		var = call_variable_op(rt, var, var1, tok);
		
	}
	return var;
}

static variable_t* int_funccall(runtime_t* rt, funccall_t* f)
{
	variable_t* var;

	var = create_variable(rt, "#");
	var->obj = 0;
	if (strcmp(f->function_name, "print") == 0) {
		var = int_expression(rt, list_get_item(f->arguments, 0));
		printf("%d\n", (int)var->obj->data);
	}
	return var;
}

static void int_statement(runtime_t* rt, statement_t* s)
{
	if (s->type == ST_EXPRESSION) {
		int_expression(rt, s->value);
	}
}

static variable_t* int_value(runtime_t* rt, value_t* v)
{
	variable_t* var;

	if (VT_CNUMBER == v->type) {
		var = create_variable(rt, "#");
		var->obj = create_object(OBJ_NUMBER);
		var->obj->reference_count += 1;
		var->obj->data = v->value;
	} else if (VT_FUNCCALL == v->type) {
		return int_funccall(rt, v->value);
	} else if (VT_EXPRESSION == v->type) {
		return int_expression(rt, v->value);
	} else if (VT_IDENT == v->type) {
		var = get_variable(rt, (char*)v->value);
		if (0 == var) {
			var = create_variable(rt, (char*)v->value);
		}
	}
	return var;
}

void interpret(parser_t* p)
{
	runtime_t* rt = (runtime_t*)malloc(sizeof(runtime_t));

	rt->global_scope = create_scope();
	rt->current_scope = rt->global_scope;
	int i;
	for (i = 0; i < list_get_item_count(p->ast->statement_list); i++) {
		int_statement(rt, list_get_item(p->ast->statement_list, i));
	}
	destroy_scope(rt->global_scope);
	free(rt);
}

