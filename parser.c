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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

static block_t* parse_block(parser_t* p);
static expression_t* parse_expression(parser_t* p);
static funcdef_t* parse_funcdef(parser_t* p);
static ifstatement_t* parse_if(parser_t* p)
static statement_t* parse_statement(parser_t* p);
static value_t* parse_value(parser_t* p);
static vardecl_t* parse_vardecl(parser_t* p);
static whilestatement_t* parse_while(parser_t* p);

static void expect(parser_t* p, int expected_token)
{
	if (p->t->token_type != expected_token) {
		fprintf(stderr, "expect failed on line %d, expecting %d found %d:%s\n", p->t->line_number, expected_token, p->t->token_type, (char*)(p->t->token_value));
		exit(EXIT_FAILURE);
	}
}

static int match(parser_t* p, token_type_t tok)
{
	get_token(p->t);
	if (p->t->token_type != tok)
		return 0;
	return 1;
}

void init_parser(parser_t* p, char* source)
{
	p->t = (tokenizer_t*)malloc(sizeof(tokenizer_t));
	init_tokenizer(p->t, source);
	p->ast = (ast_t*)malloc(sizeof(ast_t));
}

void release_parser(parser_t* p)
{
	free(p->ast);
	release_tokenizer(p->t);
	free(p->t);
}

static block_t* parse_block(parser_t* p)
{
	block_t* block = (block_t*)malloc(sizeof(block_t));
	token_type_t tok = get_token(p->t);
	while (tok != TT_END) {
		unget_token(p->t);
		parse_statement(p);
		tok = get_token(p->t);
	}
	unget_token(p->t);
	return block;
}

static expression_t* parse_expression(parser_t* p)
{
	expression_t* expression = (expression_t*)malloc(sizeof(expression_t));
	expression->values = create_list();
	expression->binaryops = create_list();
	expression->unaryops = create_list();
	while (1) {
		token_type_t tok = get_token(p->t);
		if (tok == TT_OP_POPEN) {
			value_t* value = (value_t*)malloc(sizeof(value_t));
			value->type = VT_EXPRESSION;
			value->value = parse_expression(p);
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
		} else {
			unget_token(p->t);
			break;
		}
	}
	return expression;
}

static funcdef_t* parse_funcdef(parser_t* p)
{
	funcdef_t* funcdef = (funcdef_t*)malloc(sizeof(funcdef_t));
	funcdef->line_number = p->t->line_number;
	match(p, TT_DEF);
	match(p, TT_IDENT);
	strcpy(funcdef->name, ((char*)(p->t->token_value)));
	match(p, TT_OP_POPEN);

	funcdef->parameters = create_list();

	token_type_t tok = get_token(p->t);
	if (TT_OP_PCLOSE != tok) {
		unget_token(p->t);
		do {
			list_insert(funcdef->parameters, parse_vardecl(p));
			get_token(p->t);
		} while (TT_OP_COMMA == p->t->token_type);
	}
	expect(p, TT_OP_PCLOSE);
	funcdef->block = parse_block(p);
	match(p, TT_END);
	return funcdef;
}

static ifstatement_t* parse_if(parser_t* p)
{
	ifstatement_t* ifstatement = (ifstatement_t*)malloc(sizeof(ifstatement_t));
	match(p, TT_IF);
	ifstatement->expression = parse_expression(p);
	ifstatement->block = parse_block(p);
	match(p, TT_END);
	return ifstatement;
}

static statement_t* parse_statement(parser_t* p)
{
	statement_t* statement = (statement_t*)malloc(sizeof(statement_t));
	statement->type = ST_EXPRESSION;
	statement->value = parse_expression(p);
	return statement;
}

static value_t* parse_value(parser_t* p)
{
	value_t* value = (value_t*)malloc(sizeof(value_t));
	token_type_t tok = get_token(p->t);
	if (tok = TT_NUMBER) {
		value->type = VT_NUMBER;
		value->value = (void*)(*(int*)(p->t->token_value));
	} else {
		fprintf(stderr, "unknown value type!!\n");
		exit(EXIT_FAILURE);
	}
	return value;
}

static vardecl_t* parse_vardecl(parser_t* p)
{
	vardecl_t* vardecl = (vardecl_t*)malloc(sizeof(vardecl_t));
	vardecl->type = get_token(p->t);
	match(p, TT_IDENT);
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
			parse_funcdef(p);
		} else {
			unget_token(p->t);
			statement_t* statement = parse_statement(p);
			list_insert(p->ast->statement_list, statement);
		}
		tok = get_token(p->t);
	}
}
// unit tests {{{
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
	int i;
	for (i = 1; i < list_get_item_count(e->values); i++) {
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
	p->ast->statement_list = create_list();
	parse(p);
	
	assert_int_equal(list_get_item_count(p->ast->statement_list), 1);
	
	statement_t* s = list_get_item(p->ast->statement_list, 0);
	expression_t* e = (expression_t*)s->value;
	assert_int_equal(300, tester_expr_eval(e));

	destroy_list(p->ast->statement_list);
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

// }}}

