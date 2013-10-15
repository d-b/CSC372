/*
 * CSC372 - Lab 3
 *
 * Test suite declarations
 */

// Testing platform
#include <stdlib.h>
#include <check.h>

// Project
#include "../src/kernel.h"
#include "../src/user.h"

Suite* make_lifo_suite();
Suite* make_priority_suite();
Suite* make_waitlist_suite();
