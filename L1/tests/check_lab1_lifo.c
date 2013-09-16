/*
 * CSC372 - Lab 1
 *
 * LIFO list test suite
 */

// Test platform includes
#include <stdlib.h>
#include <check.h>
#include "check_lab1.h"

// Project includes
#include "../src/process.h"
#include "../src/list.h"

START_TEST(test_lifo_create)
{
    struct LL* list = CreateList(L_LIFO);
    ck_assert(list != NULL);
    ck_assert(list->type == L_LIFO);
    ck_assert(DestroyList(list) == 0);
}
END_TEST

START_TEST(test_lifo_enqueue_once)
{
    struct LL* list = CreateList(L_LIFO);
    struct PD* pd = AllocatePD();
    pd->pid = 1;
    EnqueueAtHead(pd, list);
    ck_assert(list->head == pd);
    ck_assert(list->head->link == NULL);
    ck_assert(DestroyList(list) == 0);
}
END_TEST

START_TEST(test_lifo_enqueue_dequeue_once)
{
    struct LL* list = CreateList(L_LIFO);
    struct PD* pd = AllocatePD();
    pd->pid = 1;
    EnqueueAtHead(pd, list);
    ck_assert(DequeueHead(list) == pd);
    ck_assert(list->head == NULL);
    ck_assert(DestroyList(list) == 0);
}
END_TEST

START_TEST(test_lifo_enqueue_multiple)
{
    static const int count = 100;
    struct PD* entry; int i;
    struct LL* list = CreateList(L_LIFO);
    struct PD* pd[count];

    // Allocate and add the process descriptors
    for(i = 0; i < count; i++) {
        pd[i] = AllocatePD();
        pd[i]->pid = i;
        EnqueueAtHead(pd[i], list);
    }

    // Validate the constructed list
    for(entry = list->head, i = 0; entry; entry = entry->link, i++)
        ck_assert(pd[count - i - 1] == entry);

    // Cleanup
    for(i = 0; i < count; i++)
        DestroyPD(pd[i]);
    DestroyList(list);
}
END_TEST

START_TEST(test_lifo_enqueue_dequeue_multiple)
{
    static const int count = 100;
    struct PD* entry; int i;
    struct LL* list = CreateList(L_LIFO);
    struct PD* pd[count];

    // Allocate and add the process descriptors
    for(i = 0; i < count; i++) {
        pd[i] = AllocatePD();
        pd[i]->pid = i;
        EnqueueAtHead(pd[i], list);
    }

    // Dequeue all process descriptors
    for(i = 0; i < count; i++)
        ck_assert(DequeueHead(list) == pd[count - i - 1]);

    // Cleanup
    for(i = 0; i < count; i++)
        DestroyPD(pd[i]);
    DestroyList(list);
}
END_TEST

START_TEST(test_lifo_dequeue_empty)
{
    struct LL* list;

    list = CreateList(L_LIFO);
    ck_assert(DequeueHead(list) == NULL);
    ck_assert(DestroyList(list) == 0);

    list = CreateList(L_LIFO);
    struct PD* pd = AllocatePD();
    pd->pid = 1;
    EnqueueAtHead(pd, list);
    ck_assert(DequeueHead(list) == pd);
    ck_assert(DequeueHead(list) == NULL);
    DestroyPD(pd);
    DestroyList(list);
}
END_TEST

START_TEST(test_lifo_enqueue_nonlifo)
{
    struct LL* list = CreateList(L_PRIORITY);
    struct PD* pd = AllocatePD();
    pd->pid = 1;
    ck_assert(EnqueueAtHead(pd, list) == -1);
    DestroyPD(pd);
    DestroyList(list);
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
