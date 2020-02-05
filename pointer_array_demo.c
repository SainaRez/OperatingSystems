#include <stdio.h>

int main() {
	char this_is_kind_of_a_string[4];
	// Create an array of integers
	int test_array[10];
	test_array[0] = 12343432523141234;

	// create a character pointer??
	char *pointer;
	pointer = test_array;

	// Pointers are just arrays?
	printf("%u\n", test_array);
	printf("%u\n", pointer);

	// pointer 0 = some random stuff
	printf("pointer: %c\n", pointer[0]);
}
