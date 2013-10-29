/*
 * CSC372 - RTOS
 *
 * Test suite declarations
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
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
