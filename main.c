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

int main(int argc, char* argv[])
{
#ifdef UNITTESTS
	run_tests(all_unit_tests);
#else
	char* src;

	if (argc == 1) {
		printf("usage: %s FILE\n", argv[0]);
		return 2;
	}
	FILE* f = fopen(argv[1], "rb");
	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	src = (char*)malloc(filesize+1);
	fread(src, 1, filesize, f);
	fclose(f);
	src[filesize] = '\0';
	parser_t* p = (parser_t*)malloc(sizeof(parser_t));
	init_parser(p, src);
	parse(p);
	interpret(p);
	release_parser(p);
	free(p);

	free(src);
#endif
	return 0;
}

