#include "defines.h"
#include "debug.h"
#include "list.h"
#include "user.h"
#include "kernel.h"
#include "main.h"

#include <assert.h>
#include <stdlib.h>

#ifndef NATIVE
#include <stdio.h>
#endif /* NATIVE */
  
int main(void)
{   
  InitKernel();//Initialize all kernel data structures
  
  USERMODE;    //Switch to user mode 

  mymain();    //Now call what you would normally call main() 
  
  return 0;
}
