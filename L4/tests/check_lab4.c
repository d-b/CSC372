/*
 * CSC372 - RTOS
 *
 * Test runner
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "check_lab3.h"

int main(void) {
    int number_failed;
    SRunner *sr = srunner_create(make_lifo_suite());
    srunner_add_suite(sr, make_priority_suite());
    srunner_add_suite(sr, make_waitlist_suite());
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
