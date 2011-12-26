CFLAGS = -c -Wall -std=c99
OBJS = common.o main.o parser.o runtime.o token.o interpreter.o seatest.o

all: $(OBJS)
	$(CC) -o betik $(OBJS)

%o: %c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f betik

