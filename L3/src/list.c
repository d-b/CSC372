/*
 * CSC372 - RTOS
 *
 * Thread list implementation
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "kernel.h"

//
// Thread descriptor management
//

static struct {
    int next_thread, // next thread to allocate
        next_free;   // next free thread (1-indexed)

    TD thread[MAX_THREADS];
    int freelist[MAX_THREADS];
} _threads = {0};

/*
 * AllocateTD
 *
 * Allocates a TD.
 */
static TD* AllocateTD() {
    // See if we have a free thread
    if(_threads.next_free) {
        int freetd = _threads.freelist[--_threads.next_free];
        return &_threads.thread[freetd];
    }

    // If we are out of threads bailout
    if(_threads.next_thread == MAX_THREADS)
        return NULL;

    // Otherwise return the next thread
    return &_threads.thread[_threads.next_thread++];
}

/*
 * DestroyTD
 *
 * Destroys a TD.
 */
RC DestroyTD(TD* td) {
    // Perform a linear search for the TD
    // TODO: add a hash table to speed this up
    int i;
    for(i = 0; i < MAX_THREADS; i++) {
        if(&_threads.thread[i] == td) {
            _threads.freelist[_threads.next_free++] = i;
            td->tid = 0;
            return 0;
        }
    }

    // Thread descriptor not found.
    return -1;
}

/*
 * CreateTD
 *
 * Allocate and prepare a thread descriptor for initialization.
 */
TD* CreateTD(ThreadId tid)
{
    TD* thread = AllocateTD();

    if(thread != NULL) {
        thread->link = NULL;
        thread->tid = tid;
        thread->state = 0;
        thread->priority = 0;
        thread->waittime = 0;
        thread->inlist = NULL;
        thread->returnCode = 0;

        thread->regs.pc = 0;
        thread->regs.sp = 0;
        thread->regs.sr = 0;
    } else {
        printk("Failed to allocate new thread\n");
    }

    return thread;
}

/*
 * InitTD
 *
 * Initialize a thread descriptor.
 */
void InitTD(TD* td, uval32 pc, uval32 sp, uval32 priority) 
{ 
    if(td != NULL) {
        td->regs.pc  = pc; 
        td->regs.sp = sp; 
        td->regs.sr  = DEFAULT_THREAD_SR; 
        td->priority = priority; 
    } else {
        printk("Tried to initialize NULL pointer\n");
    }
}

/*
 * InitList
 *
 * Initializes a list structure of the specified type.
 */
RC InitList(ListType type, LL* list) {
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
            return -1;
    }

    // Initialize and return it
    list->head = NULL;
    list->type = type;
    return 0;
}

/* 
 * DequeueHead
 * 
 * Dequeues the TD at the head of list and returns a pointer to it, or
 * else null
 */
TD* DequeueHead(LL* list) {
    // Empty list
    if(!list->head) return NULL;

    // Delink current head and return it
    TD* dequeued = list->head;
    list->head = list->head->link;
    dequeued->inlist = NULL;
    return dequeued;
}

/* 
 * OrderedEnqueue
 */
static RC OrderedEnqueue(TD* td, LL* list, int(*getorder)(TD* td)){
    // TD will be in the given list
    td->inlist = list;

    // See if we are the new head
    if(!list->head || getorder(td) < getorder(list->head)) {
        if(list->head) td->link = list->head;
        list->head = td;
        return 0;
    }

    // Search for a place to insert
    TD* prev;
    for(prev = list->head; prev->link; prev = prev->link)
        if(getorder(td) < getorder(prev->link)) break;

    // Perform the insertion    
    td->link = prev->link;
    prev->link = td;
    return 0;
}

/* 
 * PriorityEnqueue
 * 
 * If list is a priority list, then enqueues TD in its proper
 * location. Returns -1 if list is not a priority list and 0
 * otherwise.
 */
static int _order_priority(TD* td){return td->priority;}
RC PriorityEnqueue(TD* td, LL* list) {
    // Ensure list is a priority queue
    if(list->type != L_PRIORITY) return -1;

    // Perform the ordered enqueue
    return OrderedEnqueue(td, list, _order_priority);
}

/* 
 * EnqueueAtHead
 * 
 * Enqueues TD at the head of list if list is a LIFO list. Returns 0
 * if OK and -1 otherwise.
 */
RC EnqueueAtHead(TD* td, LL* list) {
    // Ensure list is a LIFO
    if(list->type != L_LIFO) return -1;

    // TD will be in the given list
    td->inlist = list;

    // Perform the enqueue
    td->link = list->head;
    list->head = td;
    return 0;
}

/*
 * WaitlistEnqueue
 *
 * If list is a waiting list, then inserts TD in its correct position
 * assuming it should wait for waittime. The waittime values of the
 * other elements in the list should be properly adjusted. Return -1
 * if list is not a waiting list and 0 otherwise.
 */
static int _order_waittime(TD* td){return td->waittime;}
RC WaitlistEnqueue(TD* td, int waittime, LL* list) {
    // Ensure list is a waitlist
    if(list->type != L_WAITING) return -1;

    // Perform ordered enqueue
    RC result = OrderedEnqueue(td, list, _order_waittime);
    if(result < 0) return result;

    //
    // Fix up wait time
    //
    TD* other = NULL;

    // Add our wait time to those we are ahead of
    for(other = td->link; other; other = other->link)
        other->waittime += waittime;

    // Accumulate wait time up to our inserted TD
    for(other = list->head->link; other && other != td; other = other->link)
        waittime += other->waittime;

    return 0;
}

/*
 * FindTD
 *
 * Searches list for a TD with a thread id tid, and returns a pointer
 * to it or null otherwise.
 */
TD* FindTD(ThreadId tid, LL *list) {
    TD* td;
    for(td = list->head; td; td = td->link)
        if(td->tid == tid) return td;
    return NULL;
}

/*
 * DequeueTD
 *
 * Dequeues TD from whatever list it might be in, if it is in one.
 */
RC DequeueTD(TD *td) {
    // Bailout if it does not belong to a list
    LL* list = td->inlist;
    if(!list) return -1;
    // Check if its the head
    if(list->head == td)
        return DequeueHead(list) != NULL ? 0 : -1;
    // Find the previous node and do the delink
    TD* prev = NULL;
    for(prev = list->head; prev->link != td; prev = prev->link);
    if(!prev) return -1; // Something very bad happened
    prev->link = td->link;
    td->inlist = NULL;
    return 0;
}

//
// Thread id management
//

static struct {
    unsigned int
        range_start,
        range_end;
} _tid = {1};

// Find largest range of free TIDs
static void RefreshTids() {
    // Candidate range
    unsigned int
        range_start = 0,
        range_end = 0;

    // The last found TID
    unsigned int lasttid = 0;
    
    // Scan threads
    int i;
    for(i = 0; i < MAX_THREADS; i++) {
        // Skip over invalid threads
        unsigned int tid = _threads.thread[i].tid;
        if(!tid) continue;

        // Current distance
        unsigned int olddist = range_end - range_start;
        unsigned int newdist = tid - lasttid - 1;
        if(newdist > olddist) {
            range_start = lasttid + 1;
            range_end = tid;
        }
    }

    // If no TID was found expand to entire range
    if(!lasttid) {
        range_start = 1;
        range_end = ((unsigned int) MAX_THREADID) + 1;
    }

    // Set new range
    _tid.range_start = range_start;
    _tid.range_end = range_end;
}

// Get free thread id
ThreadId GetTid() {
    // Refresh range if necessary
    if(_tid.range_start >= _tid.range_end) RefreshTids();
    if(_tid.range_start >= _tid.range_end) panic("No free thread ids!\n");

    // Return next free TID
    return _tid.range_start++;
}

//
// List initialization
//

void ListInit() {
    // Initialize TD management
    memset(&_threads, 0, sizeof(_threads));

    // Initialize TID management
    _tid.range_start = 1;
    _tid.range_end = 1;
}
