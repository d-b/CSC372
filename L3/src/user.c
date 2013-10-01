#include "defines.h"
#include "debug.h"
#include "list.h"
#include "user.h"
#include "main.h"

#ifndef NATIVE

#include "kernel.h" 

#endif /* NATIVE */

#include <stdlib.h>
#include <assert.h>

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

void thread() {
  printk("THREAD\n");
}

void mymain() 
{ 
  RC ret;

  ret = SysCall(SYS_CREATE, (uval32) &thread, 0, 0);
  
  printk("DONE\n");

  while(1);
}
