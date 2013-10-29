/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "kernel.h"
#include "user.h"
  
int main(void)
{
  InitKernel();//Initialize all kernel data structures
  
  USERMODE;    //Switch to user mode

  mymain();    //Now call what you would normally call main() 
  
  return 0;
}
