all: main

clean:
	rm -f main main.o

main: main.c queue.c data.c probability.c
	gcc -w -pthread -o main main.c queue.c thread_demo.c data.c probability.c -lm

test: main.c queue.c data.c probability.c
	gcc -Wall -pthread -o main main.c queue.c thread_demo.c data.c probability.c -lm -fsanitize=address
