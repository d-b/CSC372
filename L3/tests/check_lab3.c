/*
 * CSC372 - Lab 3
 *
 * Test runner
 */

#include <stdlib.h>
#include <check.h>
#include "check_lab3.h"

int main(void) {
    int number_failed;
    SRunner *sr = srunner_create(NULL);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
