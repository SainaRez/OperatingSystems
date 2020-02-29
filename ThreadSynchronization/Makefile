all: main

clean:
	rm -f main main.o

main: main.c
	gcc -pthread -o main main.c queue.c thread_demo.c data.c probability.c -lm
