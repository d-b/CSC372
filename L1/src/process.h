/*
 * CSC372 - Lab 1
 *
 * Process structures
 */

#define MAX_PROCESSES 1024 

typedef int ProcessId;

struct PD {
    struct PD* link;
    ProcessId pid;
    int priority;
    int waittime;
    struct LL* inlist;
};

typedef enum {
    UNDEF, 
    L_PRIORITY,
    L_LIFO,
    L_WAITING
} ListType;

struct LL {
    struct PD* head;
    ListType type;
};

typedef int RC;
