/*
 * CSC372 - Lab 3
 *
 * Thread list declarations
 */

#ifndef _LIST_H_
#define _LIST_H_

#include "defines.h"

#ifndef NATIVE
#include <stdlib.h>
#include <asm/signal.h>
#include <ucontext.h>
#endif

typedef enum {UNDEF, L_PRIORITY, L_LIFO, L_WAITING} ListType ;
typedef enum {S_ACTIVE, S_READY, S_SLEEP} ThreadState ;

#define MIN_PRIORITY 100
#define MAX_THREADID 0x7FFFFFFF

typedef struct type_LL LL;
typedef struct type_TD TD;
typedef struct type_REGS Registers;

struct type_REGS
{
  uval32 sp;
  uval32 pc;
  uval32 sr;
};

struct type_LL
{
  TD *head;
  ListType type;
};

struct type_TD
{
  TD* link;
  ThreadId tid;
  Registers regs;
  ThreadState state;
  int priority;
  int waittime;
  RC returnCode;
  LL* inlist;
  
  // Non-native context switching support
  #ifndef NATIVE
    ucontext_t context;
    ucontext_t context_outer;
  #endif
};

TD* CreateTD(ThreadId tid);
void InitTD(TD* td, uval32 pc, uval32 sp, uval32 priority);
RC DestroyTD(TD* td);

RC InitList(ListType type, LL* list);
TD* DequeueHead(LL* list);
RC PriorityEnqueue(TD* td, LL* list);
RC EnqueueAtHead(TD* td, LL* list);
RC WaitlistEnqueue(TD* td, int waittime, LL* list);
TD* FindTD(ThreadId tid, LL *list);
RC DequeueTD(TD *td);

ThreadId GetTid();
void FreeTid(ThreadId);

#endif
