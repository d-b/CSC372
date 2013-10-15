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
#else /* NATIVE */
  U_VirtualSysCall(type, arg0, arg1, arg2);
#endif /* NATIVE */
  
  returnCode = RC_SUCCESS; //Change this code to take the actual return value
  return returnCode; 
}

void thread1() {
  while(1) {
    printk("THREAD #1\n");
    SysCall(SYS_YIELD, 0, 0, 0);
  }
}

void thread2() {
  while(1) {
    printk("THREAD #2\n");
    SysCall(SYS_YIELD, 0, 0, 0);
  }
}

void thread3() {
  while(1) {
    printk("THREAD #3\n");
    SysCall(SYS_YIELD, 0, 0, 0);
  }
}

void mymain() 
{ 
  SysCall(SYS_CREATE, (uval32) &thread1, (uval32) malloc(STACKSIZE), 1);
  SysCall(SYS_CREATE, (uval32) &thread2, (uval32) malloc(STACKSIZE), 1);
  SysCall(SYS_CREATE, (uval32) &thread3, (uval32) malloc(STACKSIZE), 1);
  SysCall(SYS_SUSPEND, 0, 0, 0);
}
