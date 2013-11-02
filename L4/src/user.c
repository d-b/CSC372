/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h"

#ifdef PLATFORM_RPI
#include "bcm_host.h"
#endif

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

void mymain() 
{
// Raspberry Pi initialization
#ifdef PLATFORM_RPI
  bcm_host_init();
#endif

  // Audio test routine
  audio_init();
  sample_t buffer[1024];
  wave_sequence_t wav_note_a4;
  wave_create(&wav_note_a4, NOTES_A4, NOTES_A4_SIZE);
  for(;;) {
    int samples;
    while((samples = audio_free()) > 0) {
      samples = wave_read(&wav_note_a4, buffer, 1024);
      audio_send(buffer, samples);
      if(!wave_remaining(&wav_note_a4)) wave_seek(&wav_note_a4, 0);
    }

    SysCall(SYS_YIELD, 0, 0, 0);
  }
  SysCall(SYS_SUSPEND, 0, 0, 0);
}
