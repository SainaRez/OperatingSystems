#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * This function takes an average time/value and adds a variance
 * that is a generated random number with an approximate normal distribution
 * @param the average time or value
 * @return the average with the added variance
 */
int add_variance(const int avg_time) {
    // TODO
    srand(time(0));
    int seed = rand();
    srand48(seed);
    double a = drand48();
    double b = drand48();
    double variance = sqrt(-2 * log(a)) * cos(2 * M_PI * b);
    if (variance < 0 && (fabs(variance) >= avg_time)) {
        variance = fabs(variance);
    }
    double updated_time = avg_time + variance;
    return updated_time;
}


/**
 * Decides if a thread/pirate/ninja is coming back or not.
 * The probability of person coming back is 25%
 * @return bool False person is not coming back. True if person is coming back
 */
bool is_coming_back() {
    srand(time(0));
    double random_number = rand() % 4;
    if (random_number == 0) {
        return true;
    }
    return false;
}

/**
 * Generates the box_muller of a.
 *
 * @param a Is the value
 * @return Returns the Box muller
 */
double box_muller(int c) {
    double a = drand48();
    double b = drand48();
    return sqrt(-2.0 * log(a)) * cos(2.0 * 3.141592658 * b);
}

