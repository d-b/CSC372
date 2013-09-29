/*
 * CSC372 - Lab 3
 *
 * LIFO list test suite
 */

// Test platform includes
#include <stdlib.h>
#include <check.h>
#include "check_lab3.h"

// Project includes
#include "../src/defines.h"
#include "../src/list.h"

// Global list used for tests
static LL test_list;
LL* init_test_list(ListType type){
    InitList(type, &test_list);
    return &test_list;
}

START_TEST(test_lifo_create)
{
    LL* list = init_test_list(L_LIFO);
    ck_assert(list != NULL);
    ck_assert(list->type == L_LIFO);
}
END_TEST

START_TEST(test_lifo_enqueue_once)
{
    LL* list = init_test_list(L_LIFO);
    TD* td = CreateTD(1);
    ck_assert(td != NULL);
    td->tid = 1;
    EnqueueAtHead(td, list);
    ck_assert(list->head == td);
    ck_assert(list->head->link == NULL);
}
END_TEST

START_TEST(test_lifo_enqueue_dequeue_once)
{
    LL* list = init_test_list(L_LIFO);
    TD* td = CreateTD(1);
    ck_assert(td != NULL);
    td->tid = 1;
    EnqueueAtHead(td, list);
    ck_assert(DequeueHead(list) == td);
    ck_assert(list->head == NULL);
}
END_TEST

START_TEST(test_lifo_enqueue_multiple)
{
    static const int count = MAX_THREADS;
    TD* entry; int i;
    LL* list = init_test_list(L_LIFO);
    TD* td[count];

    // Allocate and add the thread descriptors
    for(i = 0; i < count; i++) {
        td[i] = CreateTD(i + 1);
        ck_assert(td[i] != NULL);
        td[i]->tid = i;
        EnqueueAtHead(td[i], list);
    }

    // Validate the constructed list
    for(entry = list->head, i = 0; entry; entry = entry->link, i++)
        ck_assert(td[count - i - 1] == entry);

    // Cleanup
    for(i = 0; i < count; i++)
        DestroyTD(td[i]);
}
END_TEST

START_TEST(test_lifo_enqueue_dequeue_multiple)
{
    static const int count = MAX_THREADS;
    TD* entry; int i;
    LL* list = init_test_list(L_LIFO);
    TD* td[count];

    // Allocate and add the thread descriptors
    for(i = 0; i < count; i++) {
        td[i] = CreateTD(i + 1);
        ck_assert(td[i] != NULL);
        td[i]->tid = i;
        EnqueueAtHead(td[i], list);
    }

    // Dequeue all thread descriptors
    for(i = 0; i < count; i++)
        ck_assert(DequeueHead(list) == td[count - i - 1]);

    // Cleanup
    for(i = 0; i < count; i++)
        DestroyTD(td[i]);
}
END_TEST

START_TEST(test_lifo_dequeue_empty)
{
    LL* list;

    list = init_test_list(L_LIFO);
    ck_assert(DequeueHead(list) == NULL);

    list = init_test_list(L_LIFO);
    TD* td = CreateTD(1);
    td->tid = 1;
    EnqueueAtHead(td, list);
    ck_assert(DequeueHead(list) == td);
    ck_assert(DequeueHead(list) == NULL);
    DestroyTD(td);
}
END_TEST

START_TEST(test_lifo_enqueue_nonlifo)
{
    LL* list = init_test_list(L_PRIORITY);
    TD* td = CreateTD(1);
    td->tid = 1;
    ck_assert(EnqueueAtHead(td, list) == -1);
    DestroyTD(td);
}
END_TEST

// Construct the test suite
Suite* make_lifo_suite() {
    Suite* s = suite_create ("LIFO");

    // Core operation
    TCase* tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_lifo_create);
    tcase_add_test(tc_core, test_lifo_enqueue_once);
    tcase_add_test(tc_core, test_lifo_enqueue_dequeue_once);
    tcase_add_test(tc_core, test_lifo_enqueue_multiple);
    tcase_add_test(tc_core, test_lifo_enqueue_dequeue_multiple);
    suite_add_tcase(s, tc_core);

    // Invalid operation
    TCase* tc_invalid = tcase_create("Invalid");
    tcase_add_test(tc_invalid, test_lifo_dequeue_empty);
    tcase_add_test(tc_invalid, test_lifo_enqueue_nonlifo);
    suite_add_tcase(s, tc_invalid);

    // Return constructed suite
    return s;
}
