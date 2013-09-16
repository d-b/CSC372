/*
 * CSC372 - Lab 1
 *
 * Process list implementation
 */

// Project includes
#include "process.h"
#include "list.h"

// Platform includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * CreateList
 *
 * Allocates and properly initializes a list structure and returns a
 * pointer to it or null
 */
struct LL* CreateList(ListType type) {
    // Validate given type
    switch(type){
        // Valid types
        case L_PRIORITY:
        case L_LIFO:
        case L_WAITING:
            break;

        // Invalid types
        case UNDEF:
        default:
            return NULL;
    }

    // Attempt to allocate a list
    struct LL* list = malloc(sizeof(struct LL));
    if(!list) return NULL;

    // Initialize and return it
    list->head = NULL;
    list->type = type;
    return list;
}

/*
 * DestroyList
 *
 * Destroys list, whose pointer is passed in as an argument.
 * Returns 0 if successful, and -1 otherwise.
 */
RC DestroyList(struct LL* list) {
    free(list);
    return 0;
}

/* 
 * DequeueHead
 * 
 * Dequeues the PD at the head of list and returns a pointer to it, or
 * else null
 */
struct PD* DequeueHead(struct LL* list) {
    // Empty list
    if(!list->head) return NULL;

    // Delink current head and return it
    struct PD* dequeued = list->head;
    list->head = list->head->link;
    dequeued->inlist = NULL;
    return dequeued;
}

/* 
 * PriorityEnqueue
 * 
 * If list is a priority list, then enqueues pd in its proper
 * location. Returns -1 if list is not a priority list and 0
 * otherwise.
 */
RC PriorityEnqueue(struct PD* pd, struct LL* list) {
    // Ensure list is a priority queue
    if(list->type != L_PRIORITY) return -1;

    // PD will be in the given list
    pd->inlist = list;

    // See if we are the new head
    if(!list->head || pd->priority < list->head->priority) {
        if(list->head) pd->link = list->head;
        list->head = pd;
        return 0;
    }

    // Search for a place to insert
    struct PD* prev;
    for(prev = list->head; prev->link; prev = prev->link)
        if(pd->priority < prev->link->priority) break;

    // Perform the insertion    
    pd->link = prev->link;
    prev->link = pd;
    return 0;
}

/* 
 * EnqueueAtHead
 * 
 * Enqueues pd at the head of list if list is a LIFO list. Returns 0
 * if OK and -1 otherwise.
 */
RC EnqueueAtHead(struct PD* pd, struct LL* list) {
    // Ensure list is a LIFO
    if(list->type != L_LIFO) return -1;

    // PD will be in the given list
    pd->inlist = list;    

    // Perform the enqueue
    pd->link = list->head;
    list->head = pd;
    return 0;
}

/*
 * WaitlistEnqueue
 *
 * If list is a waiting list, then inserts pd in its correct position
 * assuming it should wait for waittime. The waittime values of the
 * other elements in the list should be properly adjusted. Return -1
 * if list is not a waiting list and 0 otherwise.
 */
RC WaitlistEnqueue(struct PD* pd, int waittime, struct LL* list) {
    // Set wait time
    pd->waittime = waittime;

    // Perform the enqueue
    pd->link = list->head;
    list->head = pd;

    // PD will be in the given list
    pd->inlist = list;    

    // Add the time to the rest of the PDs
    for(pd = list->head->link; pd; pd = pd->link)
        pd->waittime += waittime;
    return 0;
}


/*
 * FindPD
 *
 * Searches list for a PD with a process id pid, and returns a pointer
 * to it or null otherwise.
 */
struct PD* FindPD(ProcessId pid, struct LL *list) {
    for(struct PD* pd = list->head; pd; pd = pd->link)
        if(pd->pid == pid) return pd;
    return NULL;
}

/*
 * DequeuePD
 *
 * Dequeues pd from whatever list it might be in, if it is in one.
 */
RC DequeuePD(struct PD *pd) {
    // Bailout if it does not belong to a list
    struct LL* list = pd->inlist;
    if(!list) return -1;
    // Check if its the head
    if(list->head == pd)
        return DequeueHead(list) != NULL ? 0 : -1;
    // Find the previous node and do the delink
    struct PD* prev = NULL;
    for(prev = list->head; prev->link != pd; prev = prev->link);
    if(!prev) return -1; // Something very bad happened
    prev->link = pd->link;
    pd->inlist = NULL;
    return 0;
}

/*
 * AllocatePD
 *
 * Allocates a PD.
 */
struct PD* AllocatePD() {
    struct PD* pd = malloc(sizeof(struct PD));
    if(!pd) return NULL;
    memset(pd, 0, sizeof(struct PD));
    return pd;
}

/*
 * DestroyPD
 *
 * Destroys a PD.
 */
RC DestroyPD(struct PD* pd) {
    if(!pd) return -1;
    free(pd); return 0;
}
