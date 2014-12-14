/**
 * Copyright 2010-2014 Cenan Ozen <betik@cenanozen.com>
 * This file is part of Betik.
 *
 * Betik is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Betik is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Betik. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef runtime_h
#define runtime_h

#include "common.h"
#include "parser.h"


typedef enum {
	OBJ_BASE,
	OBJ_NUMBER,
	OBJ_STRING,
	OBJ_FUNCTION,
	OBJ_LIST,
} object_type_t;

typedef struct {
	list_t* variables;
	int reference_count;
} scope_t;

typedef struct {
	object_type_t type;
	int reference_count;
	void* data;
	scope_t* scope;
	list_t* properties;
} object_t;

typedef struct {
	char* name;
	object_t* obj;
} variable_t;

typedef struct {
	btk_stack_t* scopes;
	scope_t* global_scope;
	scope_t* current_scope;
	ast_t* ast;
} runtime_t;

scope_t* create_scope(runtime_t* rt);
void destroy_scope(scope_t* s);
variable_t* get_variable(runtime_t* rt, char* variable_name);
variable_t* create_variable(runtime_t* rt, char* variable_name);
object_t* create_object(runtime_t* rt, object_type_t obj_type);
variable_t* get_property(runtime_t* rt, variable_t* var, char* property_name);
void set_property(runtime_t* rt, object_t* base, char* key, variable_t* value);
variable_t* call_variable_op(runtime_t* rt, variable_t* var1, variable_t* var2, token_type_t tok);

#endif // runtime_h

