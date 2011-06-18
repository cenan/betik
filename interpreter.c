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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "interpreter.h"
#include "runtime.h"


static variable_t* int_block(runtime_t* rt, block_t* b);
static variable_t* int_expression(runtime_t* rt, expression_t* e);
static variable_t*  int_funccall(runtime_t* rt, funccall_t* f);
static variable_t* int_if(runtime_t* rt, ifstatement_t* is);
static variable_t* int_statement(runtime_t* rt, statement_t* s);
static variable_t* int_value(runtime_t* rt, value_t* v);
static variable_t* int_while(runtime_t* rt, whilestatement_t* ws);

static variable_t* int_block(runtime_t* rt, block_t* b)
{
	variable_t* var = 0;
	for (int i = 0; i < list_get_item_count(b->statements); i++) {
		var = int_statement(rt, list_get_item(b->statements, i));
		if (var != 0) {
			return var;
		}
	}
	return var;
}

static variable_t* int_expression(runtime_t* rt, expression_t* e)
{
	value_t* v0 = (value_t*)list_get_item(e->values, 0);
	variable_t* var0 = int_value(rt, v0);
	variable_t* var = var0;

	for (int i = 1; i < list_get_item_count(e->values); i++) {
		value_t* v;
		variable_t* var1;
		v = (value_t*)list_get_item(e->values, i);
		var1 = int_value(rt, v);
		token_type_t tok = (token_type_t)list_get_item(e->binaryops, i-1);
		
		var = call_variable_op(rt, var, var1, tok);
		
	}
	return var;
}

static variable_t* call_funcdef(runtime_t* rt, funccall_t* f, funcdef_t* fd, scope_t* scope)
{
	variable_t* var = 0;

	scope_t* sc = create_scope();

	if (0 != scope) {
		for (int i = 0; i < list_get_item_count(scope->variables); i++) {
			list_insert(sc->variables, list_get_item(scope->variables, i));
		}
	}

	stack_push(rt->scopes, sc);
	scope_t* prevsc = rt->current_scope;
	rt->current_scope = sc;

	if (list_get_item_count(fd->parameters) != list_get_item_count(f->arguments)) {
		fprintf(stderr, "argument count mismatch\n");
		exit(EXIT_FAILURE);
	}
	for (int j = 0; j < list_get_item_count(fd->parameters); j++) {
		vardecl_t* vd = list_get_item(fd->parameters, j);
		variable_t* va = create_variable(rt, vd->name);
		
		scope_t* tmpscope;
		tmpscope = rt->current_scope;
		rt->current_scope = prevsc;
		variable_t* vtmp = int_expression(rt, list_get_item(f->arguments, j));
		rt->current_scope = tmpscope;

		va->obj = vtmp->obj;
	}
	var = int_block(rt, fd->block);

	sc->reference_count -= 1;
	if (sc->reference_count == 0) {
		destroy_scope(sc);
	}
	stack_pop(rt->scopes);
	rt->current_scope = prevsc;

	return var;
}

static void do_print(variable_t* var)
{
	if (OBJ_NUMBER == var->obj->type) {
		printf("%d\n", (int)var->obj->data);
	} else if (OBJ_STRING == var->obj->type) {
		printf("%s\n", (char*)var->obj->data);
	}
}

static variable_t* int_funccall(runtime_t* rt, funccall_t* f)
{
	variable_t* var;

	if (strcmp(f->function_name, "print") == 0) {
		var = int_expression(rt, list_get_item(f->arguments, 0));
		do_print(var);
		return var;
	}
	for (int i = 0; i < list_get_item_count(rt->ast->function_list); i++) {
		funcdef_t* fd = list_get_item(rt->ast->function_list, i);
		if (strcmp(f->function_name, fd->name) == 0) {
			return call_funcdef(rt, f, fd, 0);
		}
	}
	var = get_variable(rt, f->function_name);
	if (0 == var) {
		fprintf(stderr, "no such function: %s\n", f->function_name);
		exit(EXIT_FAILURE);
	} else {
		funcdef_t* fd = (funcdef_t*)var->obj->data;
		scope_t* scope = (scope_t*)var->obj->scope;
		return call_funcdef(rt, f, fd, scope);
	}
}

static variable_t* int_if(runtime_t* rt, ifstatement_t* is)
{
	variable_t* var = int_expression(rt, is->expression);
	variable_t* rv = 0;
	if ((int)var->obj->data) {
		rv = int_block(rt, is->block);
	}
	return rv;
}

static variable_t* int_statement(runtime_t* rt, statement_t* s)
{
	variable_t* var = 0;
	if (s->type == ST_EXPRESSION) {
		int_expression(rt, s->value);
	} else if (s->type == ST_WHILE) {
		var = int_while(rt, s->value);
	} else if (s->type == ST_IF) {
		var = int_if(rt, s->value);
	} else if (s->type == ST_RETURN) {
		var = int_expression(rt, s->value);
	} else if (s->type == ST_PRINT) {
		do_print(int_expression(rt, s->value));
	}
	return var;
}

static variable_t* int_value(runtime_t* rt, value_t* v)
{
	variable_t* var;

	if (VT_CNUMBER == v->type) {
		var = create_variable(rt, "#");
		var->obj = create_object(OBJ_NUMBER);
		var->obj->reference_count += 1;
		var->obj->data = v->value;
	} else if (VT_CSTRING == v->type) {
		var = create_variable(rt, "#");
		var->obj = create_object(OBJ_STRING);
		var->obj->reference_count += 1;
		var->obj->data = v->value;
	} else if (VT_FUNCCALL == v->type) {
		return int_funccall(rt, v->value);
	} else if (VT_INLINE_FUNC == v->type) {
		var = create_variable(rt, "#");
		var->obj = create_object(OBJ_FUNCTION);
		var->obj->reference_count = 1;
		var->obj->data = v->value;
		var->obj->scope = rt->current_scope;
		rt->current_scope->reference_count += 1;
		return var;
	} else if (VT_EXPRESSION == v->type) {
		return int_expression(rt, v->value);
	} else if (VT_IDENT == v->type) {
		var = get_variable(rt, (char*)v->value);
		if (0 == var) {
			for (int i = 0; i < list_get_item_count(rt->ast->function_list); i++) {
				funcdef_t* f = list_get_item(rt->ast->function_list, i);
				if (strcmp((char*)v->value, f->name) == 0) {
					var = create_variable(rt, (char*)v->value);
					var->obj = create_object(OBJ_FUNCTION);
					var->obj->reference_count = 1;
					var->obj->data = f;
					return var;
				}
			}
			var = create_variable(rt, (char*)v->value);
		}
	}
	return var;
}

static variable_t* int_while(runtime_t* rt, whilestatement_t* ws)
{
	variable_t* var = int_expression(rt, ws->expression);
	variable_t* rv = 0;

	while ((int)var->obj->data) {
		rv = int_block(rt, ws->block);
		if (rv != 0) {
			return rv;
		}
		var = int_expression(rt, ws->expression);
	}
	return rv;
}

void interpret(parser_t* p)
{
	runtime_t* rt = (runtime_t*)malloc(sizeof(runtime_t));

	rt->scopes = create_stack(sizeof(scope_t*));
	rt->global_scope = create_scope();
	rt->current_scope = rt->global_scope;
	stack_push(rt->scopes, rt->current_scope);
	rt->ast = p->ast;
	
	for (int i = 0; i < list_get_item_count(p->ast->statement_list); i++) {
		int_statement(rt, list_get_item(p->ast->statement_list, i));
	}
	while (stack_get_count(rt->scopes) > 0) {
		scope_t* sc = (scope_t*)stack_pop(rt->scopes);
		destroy_scope(sc);
	}
	free(rt);
}

