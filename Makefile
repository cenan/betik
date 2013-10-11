CFLAGS = -c -Wall -std=c99 -Iretronym -Iinclude
OBJS = main.o parser.o runtime.o token.o interpreter.o seatest.o

all: $(OBJS)
	$(CC) -o betik $(OBJS) -L./retronym -lretronym
	@cp retronym/libretronym.so ./

%o: %c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f betik

