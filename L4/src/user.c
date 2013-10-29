/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h" 

RC SysCall(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2) 
{
  uval32 returnCode;

#ifdef NATIVE  
  uval32 sysMode = SYS_ENTER;  
  asm volatile("ldw r8, %0\n\t"
         "ldw r4, %1\n\t" 
         "ldw r5, %2\n\t"
         "ldw r6, %3\n\t"
         "ldw r7, %4\n\t" 
         "trap"
         : : "m" (sysMode), "m" (type), "m" (arg0), "m" (arg1), "m" (arg2)
         : "r4", "r5", "r6", "r7", "r8");  
  asm volatile("stw r2, %0" : "=m"(returnCode));
#else /* NATIVE */
  returnCode = U_VirtualSysCall(type, arg0, arg1, arg2);
#endif /* NATIVE */
  
  return returnCode; 
}

void ThreadExit(void) {
  uval32 tid = SysCall(SYS_GETCURRENTTHREADID, 0, 0, 0);
  printk("THREAD (tid=%d) DESTROYED\n", tid);
  SysCall(SYS_DESTROY, 0, 0, 0);
}

uval32 tid1,
       tid2,
       tid3,
       tid4;

void thread1() {
  uval32 tid = SysCall(SYS_GETCURRENTTHREADID, 0, 0, 0);
  printk("THREAD #1 (tid=%d)\n", tid);
  printk("THREAD #1 (tid=%d) DONE\n", tid);
  SysCall(SYS_RESUME, tid2, 0, 0);
}

void thread2() {
  uval32 tid = SysCall(SYS_GETCURRENTTHREADID, 0, 0, 0);
  printk("THREAD #2 (tid=%d)\n", tid);
  SysCall(SYS_CHANGEPRIORITY, tid1, 1, 0);
  SysCall(SYS_SUSPEND, 0, 0, 0);
  printk("THREAD #2 (tid=%d) DONE\n", tid);
  SysCall(SYS_RESUME, tid3, 0, 0);
}

void thread3() {
  uval32 tid = SysCall(SYS_GETCURRENTTHREADID, 0, 0, 0);
  printk("THREAD #3 (tid=%d)\n", tid);
  SysCall(SYS_CHANGEPRIORITY, tid2, 1, 0);
  SysCall(SYS_SUSPEND, 0, 0, 0);
  printk("THREAD #3 (tid=%d) DONE\n", tid);
}

void thread4() {
  uval32 tid = SysCall(SYS_GETCURRENTTHREADID, 0, 0, 0);
  printk("THREAD #4 (tid=%d)\n", tid);
  printk("THREAD #4 (tid=%d) DONE\n", tid);
}

void mymain() 
{ 
  tid1 = SysCall(SYS_CREATE, (uval32) &thread1, (uval32) malloc(STACKSIZE), 3);
  tid2 = SysCall(SYS_CREATE, (uval32) &thread2, (uval32) malloc(STACKSIZE), 2);
  tid3 = SysCall(SYS_CREATE, (uval32) &thread3, (uval32) malloc(STACKSIZE), 1);
  tid4 = SysCall(SYS_CREATE, (uval32) &thread4, (uval32) malloc(STACKSIZE), 1);
  SysCall(SYS_SUSPEND, 0, 0, 0);
}
