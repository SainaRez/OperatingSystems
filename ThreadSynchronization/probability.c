#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>


/**
 * Generates the box_muller of a.
 *
 * @param a Is the value
 * @return REturns the Box muller
 */
double box_muller(int a) {
	double a = drand48();
	double b = drand48();
	return sqrt(-2.0 * log(a)) * cos(2.0 * 3.141592658 * b);
}


int main() {
	srand(time(0));
	int seed = rand();
	srand48(seed);
	for (int i = 0; i < 100; ++i)
		printf("%f\n", box_muller());
}
