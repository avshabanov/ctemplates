
/*
 * defines various helpers for unit testing
 *
 * A. Shabanov, 2009
 */

#pragma once

#include <stddef.h>

/*
 * initializes array given with the natural numbers from 1 to total
 */
void ut_init_ascending_naturals(int * arr, size_t total);

/*
 * performs 'perm_loop_count'-times permutation(s) on the array given
 */
void ut_permutate(int * arr, size_t total, size_t perm_loop_count);
