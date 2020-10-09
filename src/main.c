#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "interpreter.h"

static void run_buffer(char *buf)
{
    parser_t *p = (parser_t *)malloc(sizeof(parser_t));
    init_parser(p, buf);
    parse(p);
    interpret(p);
    release_parser(p);
    free(p);
}

static void run_file(char *filename)
{
    char *src;

    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    int filesize = ftell(f);
    fseek(f, 0, SEEK_SET);
    src = (char *)malloc(filesize + 1);
    if (fread(src, 1, filesize, f) != filesize)
    {
        return;
    }
    fclose(f);
    src[filesize] = '\0';
    run_buffer(src);
    free(src);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("usage: %s FILE\n", argv[0]);
        return 2;
    }
    run_file(argv[1]);
    return 0;
}
