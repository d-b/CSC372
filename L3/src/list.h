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
#define MAX_THREADID 0x40000000

typedef struct type_LL LL;
typedef struct type_TD TD;
typedef struct type_REGS Registers;
typedef struct type_TRAPFRAME TrapFrame;

struct type_REGS
{
  uval32 sp;
  uval32 pc;
  uval32 sr;
};

struct type_TRAPFRAME
{
  uval32 r1;
  uval32 r2;
  uval32 r3;
  uval32 r4;
  uval32 r5;
  uval32 r6;
  uval32 r7;
  uval32 r8;
  uval32 r9;
  uval32 sr;
  uval32 r11;
  uval32 r12;
  uval32 r13;
  uval32 r14;
  uval32 r15;
  uval32 r16;
  uval32 r17;
  uval32 r18;
  uval32 r19;
  uval32 r20;
  uval32 r21;
  uval32 r22;
  uval32 r23;
  uval32 bt;
  uval32 gp;
  uval32 fp;
  uval32 ea;
  uval32 ba;
  uval32 ra;
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

void ListInit(void);

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
