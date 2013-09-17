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
 * OrderedEnqueue
 */
static RC OrderedEnqueue(struct PD* pd, struct LL* list, int(*getorder)(struct PD* pd)){
    // PD will be in the given list
    pd->inlist = list;

    // See if we are the new head
    if(!list->head || getorder(pd) < getorder(list->head)) {
        if(list->head) pd->link = list->head;
        list->head = pd;
        return 0;
    }

    // Search for a place to insert
    struct PD* prev;
    for(prev = list->head; prev->link; prev = prev->link)
        if(getorder(pd) < getorder(prev->link)) break;

    // Perform the insertion    
    pd->link = prev->link;
    prev->link = pd;
    return 0;
}

/* 
 * PriorityEnqueue
 * 
 * If list is a priority list, then enqueues pd in its proper
 * location. Returns -1 if list is not a priority list and 0
 * otherwise.
 */
static int _order_priority(struct PD* pd){return pd->priority;}
RC PriorityEnqueue(struct PD* pd, struct LL* list) {
    // Ensure list is a priority queue
    if(list->type != L_PRIORITY) return -1;

    // Perform the ordered enqueue
    return OrderedEnqueue(pd, list, _order_priority);
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
static int _order_waittime(struct PD* pd){return pd->waittime;}
RC WaitlistEnqueue(struct PD* pd, int waittime, struct LL* list) {
    // Ensure list is a waitlist
    if(list->type != L_WAITING) return -1;

    // Perform ordered enqueue
    RC result = OrderedEnqueue(pd, list, _order_waittime);
    if(result < 0) return result;

    //
    // Fix up wait time
    //
    struct PD* other = NULL;

    // Add our wait time to those we are ahead of
    for(other = pd->link; other; other = other->link)
        other->waittime += waittime;

    // Accumulate wait time up to our inserted PD
    for(other = list->head->link; other && other != pd; other = other->link)
        waittime += other->waittime;

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

//
// Process descriptor management
//

static struct {
    int initialized;  // whether or not the manager is initialized

    int next_process, // next process to allocate
        next_free;    // next free process (1-indexed)

    struct PD process[MAX_PROCESSES];
    int freelist[MAX_PROCESSES];
} _processes = {0};

/*
 * AllocatePD
 *
 * Allocates a PD.
 */
struct PD* AllocatePD() {
    // See if we have a free process
    if(_processes.next_free) {
        _processes.next_free -= 1;
        int freepd = _processes.freelist[_processes.next_free - 1];
        return &_processes.process[freepd];
    }

    // If we are out of processes bailout
    if(_processes.next_process == MAX_PROCESSES)
        return NULL;

    // Otherwise return the next process
    return &_processes.process[_processes.next_process++];
}

/*
 * DestroyPD
 *
 * Destroys a PD.
 */
RC DestroyPD(struct PD* pd) {
    // Perform a linear search for the PD
    // TODO: add a hash table to speed this up
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(&_processes.process[i] == pd) {
            _processes.freelist[_processes.next_free++] = i;
            return 0;
        }
    }

    // Process descriptor not found.
    return -1;
}
