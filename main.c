/**
 * Copyright 2010-2011 Cenan Ozen <cenan.ozen@gmail.com>
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

#ifdef _WIN32
#define SCRIPT_PACK
#endif

#ifdef SCRIPT_PACK

typedef struct {
	unsigned offset;
	unsigned signature;
} script_pack_t;

#endif

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

#ifdef SCRIPT_PACK
static unsigned char* read_file(char* filename, size_t* ofilesize)
{
	unsigned char* buffer;

	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = (unsigned char*)malloc(filesize);
	fread(buffer, 1, filesize, f);
	fclose(f);
	*ofilesize = filesize;
	return buffer;
}
#endif

static void run_file(char* filename)
{
	char* src;

	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	src = (char*)malloc(filesize+1);
	fread(src, 1, filesize, f);
	fclose(f);
	src[filesize] = '\0';
	run_buffer(src);
	free(src);
}

#ifdef SCRIPT_PACK
static int check_pack(char* self_filename)
{
	script_pack_t sp;
	FILE* f = fopen(self_filename, "rb");
	fseek(f, -sizeof(sp), SEEK_END);
	int filesize = ftell(f);
	fread(&sp, sizeof(sp), 1, f);
	if (sp.signature != 0xdeadc0de) {
		return 0;
	}
	fseek(f, sp.offset, SEEK_SET);
	int script_size = filesize - sp.offset;
	char* src = (char*)malloc(script_size + 1);
	fread(src, 1, script_size, f);
	fclose(f);
	src[script_size-2] = '\0';
	run_buffer(src);
	free(src);
	return 1;
}
#endif

int main(int argc, char* argv[])
{
#ifdef UNITTESTS
	run_tests(all_unit_tests);
#else // UNITTESTS

#ifdef SCRIPT_PACK
	if ((argc > 1) && (strncmp(argv[1], "-p", 2) == 0)) {
		size_t betik_size, script_size;
		unsigned char* betik = read_file(argv[0], &betik_size);
		unsigned char* script = read_file(&argv[1][2], &script_size);

		char destination[1024];
		strcpy(destination, &argv[1][2]);
		strcat(destination, ".exe");

		FILE* f = fopen(destination, "wb");
		fwrite(betik, betik_size, 1, f);
		fwrite(script, script_size, 1, f);

		script_pack_t sp;
		sp.offset = betik_size;
		sp.signature = 0xdeadc0de;
		fwrite(&sp, sizeof(sp), 1, f);

		fclose(f);
		free(script);
		free(betik);
		return 0;
	}
#endif // SCRIPT_PACK
	if (argc == 1) {
#ifdef SCRIPT_PACK
		if (!check_pack(argv[0])) {
			return 0;
		}
#endif
		printf("usage: %s FILE\n", argv[0]);
		return 2;
	}
	run_file(argv[1]);

#endif // UNITTESTS
	return 0;
}

