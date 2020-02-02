all: boring custom multi

clean:
	rm -f custom.o boring.o multi.o custom boring multi 
	echo clean done

boring: boring.c
	 gcc -o boring boring.c

custom: custom.c
	gcc -o custom custom.c

multi: multi.c
	gcc -o multi multi.c
