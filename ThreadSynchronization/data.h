#pragma once

struct visit {
    unsigned int wait_time;
    unsigned int shop_time;
};

/**
 * Calculates the cost of the visit based.
 * @param v Pointer to visit structure
 * @return Cost in gold of the visit baesd on wait time and shop time.
 */
int get_cost_of_visit(struct visit* v);

void print_visit(struct visit* v);