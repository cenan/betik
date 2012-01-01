/**
 * Copyright 2010-2012 Cenan Ozen <cenan.ozen@gmail.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

#define IS_END_OF_BLOCK_TOKEN(t) ((t == TT_END) || (t == TT_ELSE) || (t == TT_DEF) || (t == TT_EOF))

static block_t* parse_block(parser_t* p);
static expression_t* parse_expression(parser_t* p);
static funcdef_t* parse_funcdef(parser_t* p, bool is_inline);
static ifstatement_t* parse_if(parser_t* p);
static list_t* parse_list(parser_t* p);
static inlineobj_t* parse_object(parser_t* p);
static statement_t* parse_statement(parser_t* p);
static value_t* parse_value(parser_t* p);
static vardecl_t* parse_vardecl(parser_t* p);
static whilestatement_t* parse_while(parser_t* p);

#define expect(a, b) _expect(a, b, __FILE__, __LINE__)
#define match(a, b) _match(a, b, __FILE__, __LINE__)

static void _expect(parser_t* p, int expected_token, const char* file, int line)
{
	if (p->t->token_type != expected_token) {
		fprintf(stderr,
				"expect failed on line %d, expecting %d found %d:%s\n",
				p->t->line_number,
				expected_token,
				p->t->token_type,
				(char*)(p->t->token_value));
		fprintf(stderr, "at %s:%d\n", file, line);
		exit(EXIT_FAILURE);
	}
}

static int _match(parser_t* p, token_type_t tok, const char* file, int line)
{
	get_token(p->t);
	if (p->t->token_type != tok) {
		_expect(p, tok, file, line);
		return 0;
	}
	return 1;
}

void init_parser(parser_t* p, char* source)
{
	p->t = (tokenizer_t*)malloc(sizeof(tokenizer_t));
	init_tokenizer(p->t, source);
	p->ast = (ast_t*)malloc(sizeof(ast_t));
	p->ast->statement_list = create_list();
	p->ast->function_list = create_list();
}

void release_parser(parser_t* p)
{
	destroy_list(p->ast->function_list);
	destroy_list(p->ast->statement_list);
	free(p->ast);
	release_tokenizer(p->t);
	free(p->t);
}


static block_t* parse_block(parser_t* p)
{
	block_t* block = (block_t*)malloc(sizeof(block_t));
	block->statements = create_list();
	while (1) {
		token_type_t tok;
		tok = get_token(p->t);
		if (IS_END_OF_BLOCK_TOKEN(tok)) {
			unget_token(p->t);
			break;
		} else {
			unget_token(p->t);
			list_insert(block->statements, parse_statement(p));
		}
	}
	return block;
}

static expression_t* parse_expression(parser_t* p)
{
	expression_t* expression = (expression_t*)malloc(sizeof(expression_t));
	expression->values = create_list();
	expression->binaryops = create_list();
	expression->unaryops = create_list();
	expression->line_number = p->t->line_number;
	while (1) {
		token_type_t tok = get_token(p->t);
		if (tok == TT_OP_POPEN) {
			value_t* value = (value_t*)malloc(sizeof(value_t));
			value->type = VT_EXPRESSION;
			value->value = parse_expression(p);
			value->subvalue = 0;
			list_insert(expression->values, value);
			match(p, TT_OP_PCLOSE);
		} else {
			if (TOK_IS_UNARY_OP(tok)) {
				if (tok == TT_OP_SUB) {
					tok = TT_OP_UNARYSUB;
				}
				list_insert(expression->unaryops, (void*)tok);
			} else {
				list_insert(expression->unaryops, (void*)TT_NOP);
				unget_token(p->t);
			}
			list_insert(expression->values, parse_value(p));
		}
		tok = get_token(p->t);
		if (TOK_IS_BINARY_OP(tok)) {
			list_insert(expression->binaryops, (void*)tok);
			if (TT_OP_ASSIGN == tok) {
				value_t* value = (value_t*)malloc(sizeof(value_t));
				value->type = VT_EXPRESSION;
				value->value = parse_expression(p);
				value->subvalue = 0;
				list_insert(expression->values, value);
				break;
			}
		} else {
			unget_token(p->t);
			break;
		}
	}
	return expression;
}

static funcdef_t* parse_funcdef(parser_t* p, bool is_inline)
{
	funcdef_t* funcdef = (funcdef_t*)malloc(sizeof(funcdef_t));
	funcdef->line_number = p->t->line_number;
	match(p, TT_DEF);
	if (!is_inline) {
		match(p, TT_IDENT);
		strcpy(funcdef->name, ((char*)(p->t->token_value)));
	}
	funcdef->parameters = create_list();
	token_type_t tok = get_token(p->t);
	if (TT_OP_POPEN == tok) {
		tok = get_token(p->t);
		if (TT_OP_PCLOSE != tok) {
			unget_token(p->t);
			do {
				list_insert(funcdef->parameters, parse_vardecl(p));
				get_token(p->t);
			} while (TT_OP_COMMA == p->t->token_type);
		}
		expect(p, TT_OP_PCLOSE);
	} else {
		unget_token(p->t);
	}
	funcdef->block = parse_block(p);
	match(p, TT_END);
	return funcdef;
}

static funccall_t* parse_funccall(parser_t* p)
{
	funccall_t* funccall = (funccall_t*)malloc(sizeof(funccall_t));
	funccall->arguments = create_list();

	match(p, TT_IDENT);
	strcpy(funccall->function_name, (char*)(p->t->token_value));
	match(p, TT_OP_POPEN);
	token_type_t tok = get_token(p->t);
	if (TT_OP_PCLOSE == tok) {
		// empty parameter list
		return funccall;
	}
	unget_token(p->t);
	do {
		list_insert(funccall->arguments, parse_expression(p));
		tok = get_token(p->t);
	} while (TT_OP_COMMA == tok);
	expect(p, TT_OP_PCLOSE);
	return funccall;
}

static ifstatement_t* parse_if(parser_t* p)
{
	ifstatement_t* ifstatement = (ifstatement_t*)malloc(sizeof(ifstatement_t));
	match(p, TT_IF);
	ifstatement->expression = parse_expression(p);
	ifstatement->block = parse_block(p);
	token_type_t tok = get_token(p->t);
	if (TT_ELSE == tok) {
		ifstatement->else_block = parse_block(p);
		match(p, TT_END);
	} else {
		ifstatement->else_block = 0;
		expect(p, TT_END);
	}
	return ifstatement;
}

static list_t* parse_list(parser_t* p)
{
	list_t* list = create_list();

	match(p, TT_OP_BOPEN);
	token_type_t tok = get_token(p->t);
	while (TT_OP_BCLOSE != tok) {
		unget_token(p->t);
		list_insert(list, parse_value(p));
		tok = get_token(p->t);
		if (TT_OP_BCLOSE == tok) {
			break;
		}
		expect(p, TT_OP_COMMA);
		tok = get_token(p->t);
	}
	return list;
}

static inlineobj_t* parse_object(parser_t* p) {
	inlineobj_t* obj = (inlineobj_t*)malloc(sizeof(inlineobj_t));

	obj->keys = create_list();
	obj->values = create_list();

	match(p, TT_OP_COPEN);

	token_type_t tok = get_token(p->t);
	while (tok == TT_STRING) {
		list_insert(obj->keys, duplicate_string((char*)(p->t->token_value)));
		match(p, TT_OP_COLON);
		list_insert(obj->values, parse_expression(p));
		tok = get_token(p->t);
		if (tok != TT_OP_COMMA) {
			expect(p, TT_OP_CCLOSE);
			break;
		}
		tok = get_token(p->t);
	}

	return obj;
}

static statement_t* parse_statement(parser_t* p)
{
	statement_t* statement = (statement_t*)malloc(sizeof(statement_t));

	token_type_t tok = get_token(p->t);
	unget_token(p->t);

	switch (tok) {
	case TT_IF:
		statement->type = ST_IF;
		statement->value = parse_if(p);
		break;
	case TT_WHILE:
		statement->type = ST_WHILE;
		statement->value = parse_while(p);
		break;
	case TT_RETURN:
		statement->type = ST_RETURN;
		match(p, TT_RETURN);
		statement->value = parse_expression(p);
		break;
	case TT_PRINT:
		statement->type = ST_PRINT;
		match(p, TT_PRINT);
		statement->value = parse_expression(p);
		break;
	default:
		statement->type = ST_EXPRESSION;
		statement->value = parse_expression(p);
	}
	return statement;
}

static value_t* parse_value(parser_t* p)
{
	value_t* value = (value_t*)malloc(sizeof(value_t));
	value->subvalue = 0;
	token_type_t tok = get_token(p->t);
	if (TT_NUMBER == tok) {
		value->type = VT_CNUMBER;
		value->value = (void*)(*(int*)(p->t->token_value));
	} else if (TT_STRING == tok) {
		value->type = VT_CSTRING;
		value->value = duplicate_string((char*)(p->t->token_value));
	} else if (TT_DEF == tok) {
		value->type = VT_INLINE_FUNC;
		unget_token(p->t);
		funcdef_t* fd = parse_funcdef(p, true);
		strcpy(fd->name, "#");
		value->value = fd;
	} else if (TT_OP_BOPEN == tok) {
		value->type = VT_LIST;
		unget_token(p->t);
		value->value = parse_list(p);
	} else if (TT_OP_COPEN == tok) {
		value->type = VT_INLINE_OBJ;
		unget_token(p->t);
		value->value = parse_object(p);
	} else if (TT_IDENT == tok) {
		tok = get_token(p->t);
		if (TT_OP_POPEN == tok) {
			unget_token(p->t); // (
			unget_token(p->t); // IDENT
			value->type = VT_FUNCCALL;
			value->value = parse_funccall(p);
		} else if (TT_OP_BOPEN == tok) {
			unget_token(p->t);
			unget_token(p->t);
			tok = get_token(p->t);
			listindex_t* listindex = (listindex_t*)malloc(sizeof(listindex_t));
			listindex->name = duplicate_string((char*)(p->t->token_value));
			value->type = VT_LISTINDEX;
			match(p, TT_OP_BOPEN);
			value->value = listindex;
			listindex->index = parse_expression(p);
			match(p, TT_OP_BCLOSE);
		} else if (TT_OP_DOT == tok) {
			unget_token(p->t); // .
			unget_token(p->t); // IDENT
			tok = get_token(p->t);
			value->type = VT_IDENT;
			value->value = duplicate_string((char*)(p->t->token_value));
			match(p, TT_OP_DOT);
			match(p, TT_IDENT);
			unget_token(p->t);
			value->subvalue = parse_value(p);
		} else {
			unget_token(p->t);
			unget_token(p->t);
			tok = get_token(p->t);
			value->type = VT_IDENT;
			value->value = duplicate_string((char*)(p->t->token_value));
		}
	} else {
		fprintf(stderr, "unknown value type:%d\n", tok);
		exit(EXIT_FAILURE);
	}
	return value;
}

static vardecl_t* parse_vardecl(parser_t* p)
{
	vardecl_t* vardecl = (vardecl_t*)malloc(sizeof(vardecl_t));
	get_token(p->t);
	strcpy(vardecl->name, (char*)(p->t->token_value));

	return vardecl;
}

static whilestatement_t* parse_while(parser_t* p)
{
	whilestatement_t* whilestmt = (whilestatement_t*)malloc(sizeof(whilestatement_t));
	match(p, TT_WHILE);
	whilestmt->expression = parse_expression(p);
	whilestmt->block = parse_block(p);
	match(p, TT_END);
	return whilestmt;
}

void parse(parser_t* p)
{
	token_type_t tok = get_token(p->t);
	while (TT_EOF != tok) {
		if (TT_DEF == tok) {
			unget_token(p->t);
			list_insert(p->ast->function_list, parse_funcdef(p, false));
		} else {
			unget_token(p->t);
			statement_t* statement = parse_statement(p);
			list_insert(p->ast->statement_list, statement);
		}
		tok = get_token(p->t);
	}
}

// unit tests {{{
#ifdef UNITTESTS
#include "seatest.h"

static void parser_test1()
{
	parser_t* p = (parser_t*)malloc(sizeof(parser_t));
	init_parser(p, "1+2");
	assert_int_equal(TT_NUMBER, get_token(p->t));
	assert_int_equal(TT_OP_ADD, get_token(p->t));
	assert_int_equal(TT_NUMBER, get_token(p->t));
	assert_int_equal(TT_EOF, get_token(p->t));
	release_parser(p);
	free(p);
}

static int tester_expr_eval(expression_t* e)
{
	value_t* v0 = (value_t*)list_get_item(e->values, 0);
	int ret = (int)v0->value;;

	for (int i = 1; i < list_get_item_count(e->values); i++) {
		value_t* v = (value_t*)list_get_item(e->values, i);
		int foo = (int)v->value;
		token_type_t tok = (token_type_t)list_get_item(e->binaryops, i-1);
		if (TT_OP_ADD == tok) {
			ret += foo;
		} else if (TT_OP_SUB == tok) {
			ret -= foo;
		}
	}
	return ret;
}

static void parser_test2()
{
	parser_t* p = (parser_t*)malloc(sizeof(parser_t));
	init_parser(p, "def asd() 100+200-600-20 end 100+321-121");
	parse(p);

	assert_int_equal(1, list_get_item_count(p->ast->statement_list));

	statement_t* s = list_get_item(p->ast->statement_list, 0);
	expression_t* e = (expression_t*)s->value;
	assert_int_equal(300, tester_expr_eval(e));

	release_parser(p);
	free(p);
}

void parser_test_fixture(void)
{
	test_fixture_start();
	run_test(parser_test1);
	run_test(parser_test2);
	test_fixture_end();
}
#endif // UNITTESTS
// }}}

