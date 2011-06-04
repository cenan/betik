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
#include "parser.h"

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

static void parse_expression(parser_t* p)
{
}

static statement_t* parse_statement(parser_t* p)
{
	statement_t* statement = (statement_t*)malloc(sizeof(statement_t));
	return statement;
}

static void parse_block(parser_t* p)
{
	token_type_t tok = get_token(p->t);
	while (tok != TT_END) {
		unget_token(p->t);
		parse_statement(p);
		tok = get_token(p->t);
	}
	unget_token(p->t);
}

static void parse_if(parser_t* p)
{
	match(p, TT_IF);
	parse_expression(p);
	parse_block(p);
}

static void parse_while(parser_t* p)
{
	match(p, TT_WHILE);
	match(p, TT_OP_POPEN);
	parse_expression(p);
	match(p, TT_OP_PCLOSE);
	match(p, TT_OP_COPEN);
	parse_block(p);
	match(p, TT_OP_CCLOSE);
}

void parse(parser_t* p)
{
	token_type_t tok = get_token(p->t);
	while (TT_EOF != tok) {
		unget_token(p->t);
		statement_t* statement = parse_statement(p);
	}
}

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

static void parser_test2()
{
	parser_t* p = (parser_t*)malloc(sizeof(parser_t));
	init_parser(p, "1+2");

	//assert_int_equal();

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


