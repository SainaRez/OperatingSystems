all: boring custom multi

clean:
	rm -f $(objects) custom.o boring.o multi.o
	echo Clean done

boring: boring.c
	 gcc -o boring boring.c

custom: custom.c
	gcc -o custom custom.c

multi: multi.c
	gcc -o multi multi.c
