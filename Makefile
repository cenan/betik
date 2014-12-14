CFLAGS = -c -Wall -std=c99 -Iinclude -m32
OBJS = main.o parser.o runtime.o token.o interpreter.o seatest.o common.o

all: $(OBJS)
	$(CC) -o betik $(OBJS) -m32

%o: %c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f betik

