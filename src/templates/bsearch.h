
/*
 * template implementation of the binary search algorithm over the particular array.
 *
 * this file comes under the MIT license that described at
 * http://www.opensource.org/licenses/mit-license.php.
 *
 * the template instantiation is controlled by the following macro definitions:
 *
 * required macros:
 *  BSEARCH_ARRAY_TYPE - defines array type
 *  BSEARCH_KEY_TYPE - defines key type which index is to be searched in an array
 *  BSEARCH_3W_COMPARE(array, index, key) - three-way comparison function -
 *                                          it shall return int value what is:
 *                                          <0 if a key is less than an array's element at the given position
 *                                          >0 if a key is greater than an array's element at the given position
 *                                          ==0 if a key is equal to an array's element at the given position
 *
 * optional macros:
 *  BSEARCH_NS - namespace macro
 *
 * unmasked types/functions:
 *  binary_search               performs binary search over the array given
 *
 *
 * Alexander Shabanov, 2009-2010
 * mailto:avshabanov@gmail.com
 * http://www.alexshabanov.com
 */


#ifndef BSEARCH_NS
#define BSEARCH_NS(name) name
#endif

#ifndef BSEARCH_ARRAY_TYPE
#error BSEARCH_ARRAY_TYPE is not defined
#endif

#ifndef BSEARCH_KEY_TYPE
#error BSEARCH_KEY_TYPE is not defined
#endif

#ifndef BSEARCH_3W_COMPARE
#error BSEARCH_3W_COMPARE(array, index, key) is not defined
#endif


/**
 * performs binary search over the array given
 * \param array         source array
 * \param count
 * \param key           key value to be searched
 * \returns int         non-negative index of the corresponding array element
 *                      if such exists, or negative value that represents
 *                      binary negation of the target index
 *                      before what the source element is to be inserted
 */
static int
BSEARCH_NS(binary_search)(BSEARCH_ARRAY_TYPE array, int count, BSEARCH_KEY_TYPE key)
{
    int begin = 0;
    int end = count - 1;
    int result;

    for (;;)
    {
        if (begin > end)
        {
            result = ~begin;
            break;
        }

        int mid = begin + (end - begin) / 2;

        // get comparison result, that is expected to be <0, ==0 or >0,
        // to reach three-way comparison result - http://en.wikipedia.org/wiki/Three-way_comparison
        int cmpret = BSEARCH_3W_COMPARE(array, mid, key);

        if (cmpret > 0)
        {
            begin = mid + 1;
        }
        else if (cmpret < 0)
        {
            end = mid - 1;
        }
        else
        {
            // cmpret==0 what indicates that a key has been found found
            result = mid;
            break;
        }
    }

    return result;
}


/*
 * undefine user macros
 */
#undef BSEARCH_NS
#undef BSEARCH_ARRAY_TYPE
#undef BSEARCH_KEY_TYPE
#undef BSEARCH_3W_COMPARE
