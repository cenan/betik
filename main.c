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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "interpreter.h"

#ifdef UNITTESTS
#include "seatest.h"
void common_lib_test_fixture(void);
void tokenizer_test_fixture(void);
void parser_test_fixture(void);

void all_unit_tests(void)
{
	common_lib_test_fixture();
	tokenizer_test_fixture();
	parser_test_fixture();
}
#endif // UNITTESTS

static void run_buffer(char* buf)
{
	parser_t* p = (parser_t*)malloc(sizeof(parser_t));
	init_parser(p, buf);
	parse(p);
	interpret(p);
	release_parser(p);
	free(p);
}

static void run_file(char* filename)
{
	char* src;

	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	src = (char*)malloc(filesize+1);
	if (fread(src, 1, filesize, f) != filesize) {
		return;
	}
	fclose(f);
	src[filesize] = '\0';
	run_buffer(src);
	free(src);
}

int main(int argc, char* argv[])
{
#ifdef UNITTESTS
	run_tests(all_unit_tests);
#else // UNITTESTS

	if (argc == 1) {
		printf("usage: %s FILE\n", argv[0]);
		return 2;
	}
	run_file(argv[1]);

#endif // UNITTESTS
	return 0;
}

