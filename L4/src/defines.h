/*
 * CSC372 - RTOS
 *
 * Project wide defines
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _DEFINES_H_
#define _DEFINES_H_

//#define NATIVE
#define RASPI

#ifdef NATIVE
typedef unsigned char uval8;
typedef unsigned int uval32;
typedef uval32 ThreadId;
#else
#include <stdint.h>
typedef unsigned char uval8;
typedef intptr_t uval32;
typedef uval32 ThreadId;
#endif

typedef enum { SYS_CREATE, SYS_SUSPEND, SYS_RESUME, SYS_YIELD, SYS_CHANGEPRIORITY, SYS_DESTROY, SYS_GETCURRENTTHREADID } SysCallType;
typedef enum { SYS_ENTER, SYS_EXIT } SysCallDir; 

typedef enum { RC_SUCCESS, RC_FAILED } RC;

typedef int bool;

#ifndef TRUE
#define TRUE (bool)1
#endif

#ifndef FALSE
#define FALSE (bool)0
#endif

#define _SUCCESS(rc) ((rc)==RC_SUCCESS)

#ifndef NULL
#define NULL (int)0
#endif

//User-mode, Interrupts Enabled
#define DEFAULT_THREAD_SR 0x0003
//Interrupts Enabled
#define DEFAULT_KERNEL_SR 0x0001

#define MAX_THREADS 10

#define STACKSIZE 8192

//Depends on the stack variables of your system call handler - mine has one
#define SYS_HANDLER_OFFSET 4

#ifdef NATIVE

#define JTAG_UART_DATA ((volatile int*) 0x10001000) 
#define JTAG_UART_CONTROL ((volatile int*) (0x10001000+4))

#define MOVE_SP_TO_ACTIVE \
  asm volatile("stw r27, %0" : "=m"(Active->regs.sp) :: "r2", "r3", "r4", "r5", "r6", "r7", "r8")

#define MOVE_PC_TO_ACTIVE \
  asm volatile("stw r29, %0" : "=m" (Active->regs.pc) :: "r2", "r3", "r4", "r5", "r6", "r7", "r8") 

#define MOVE_SR_TO_ACTIVE \
  asm volatile("rdctl r10, ctl1\n\t" \
               "stw r10, %0" : : "m" (Active->regs.sr) : "r2", "r3", "r4", "r5", "r6", "r7", "r8")

#define MOVE_ACTIVE_TO_SP \
  asm volatile("ldw r11, %0" : : "m"(Active) : "r2", "r3", "r10"); \
  asm volatile("ldw r27, %0" : : "m"(Active->regs.sp) : "r2", "r3", "r10")

#define MOVE_ACTIVE_TO_PC \
  asm volatile("ldw r11, %0" : : "m"(Active) : "r2", "r3", "r10"); \
  asm volatile("ldw r29, %0" : : "m"(Active->regs.pc) : "r2", "r3", "r10")

#define MOVE_ACTIVE_TO_SR \
  asm volatile("ldw r11, %0" : : "m"(Active) : "r2", "r3", "r10"); \
  asm volatile("ldw r10, %0\n\t" \
               "wrctl ctl1, r10" : : "m" (Active->regs.sr) : "r2", "r3", "r10")

#define SET_KERNEL_SP \
  asm volatile("ldw r27, %0\n\t" \
               "subi r27, r27, %1" : : "m" (Kernel.regs.sp), "i" (SYS_HANDLER_OFFSET))

#define SET_KERNEL_FP \
  asm volatile("ldw r28, %0" : : "m" (Kernel.regs.sp) : "r2", "r3", "r4", "r5", "r6", "r7", "r8")

#define SET_KERNEL_PC \
  asm volatile("ldw r29, %0" : : "m" (Kernel.regs.pc) : "r2", "r3", "r4", "r5", "r6", "r7", "r8")

#define SET_KERNEL_SR \
  asm volatile("ldw r10, %0\n\t" \
               "wrctl ctl1, r10" : : "m" (Kernel.regs.sr) : "r2", "r3", "r4", "r5", "r6", "r7", "r8")

//Setting bit 1 to 1 in ctl0 sets processor to user mode
//Interrupt enabled by default
#define USERMODE \
  asm("movi r10, %0\n\t" \
      "wrctl ctl0, r10" : : "i" (DEFAULT_THREAD_SR))

//Setting bit 1 to 0 in ctl0 sets processor to supervisor mode
//Interrupts enabled by default
#define KERNELMODE \
  asm("movi r10, %0\n\t" \
      "wrctl ctl0, r10" : : "i" (DEFAULT_KERNEL_SR))

#define SAVE_REGS                 \
  asm("stw  r1,  4(sp)\n\t"       \
      "stw  r2,  8(sp)\n\t"       \
      "stw  r3,  12(sp)\n\t"      \
      "stw  r4,  16(sp)\n\t"      \
      "stw  r5,  20(sp)\n\t"      \
      "stw  r6,  24(sp)\n\t"      \
      "stw  r7,  28(sp)\n\t"      \
      "stw  r8,  32(sp)\n\t"      \
      "stw  r9,  36(sp)\n\t"      \
      "stw  r10, 40(sp)\n\t"      \
      "stw  r11, 44(sp)\n\t"      \
      "stw  r12, 48(sp)\n\t"      \
      "stw  r13, 52(sp)\n\t"      \
      "stw  r14, 56(sp)\n\t"      \
      "stw  r15, 60(sp)\n\t"      \
      "stw  r16, 64(sp)\n\t"      \
      "stw  r17, 68(sp)\n\t"      \
      "stw  r18, 72(sp)\n\t"      \
      "stw  r19, 76(sp)\n\t"      \
      "stw  r20, 80(sp)\n\t"      \
      "stw  r21, 84(sp)\n\t"      \
      "stw  r22, 88(sp)\n\t"      \
      "stw  r23, 92(sp)\n\t"      \
      "stw  r25, 96(sp)\n\t"      \
      "stw  r26, 100(sp)\n\t"     \
      "stw  r28, 104(sp)\n\t"     \
      "stw  r29, 108(sp)\n\t"     \
      "stw  r30, 112(sp)\n\t"     \
      "stw  r31, 116(sp)")


#define LOAD_REGS                 \
  asm("ldw  r1,  4(sp)\n\t"       \
      "ldw  r2,  8(sp)\n\t"       \
      "ldw  r3,  12(sp)\n\t"      \
      "ldw  r4,  16(sp)\n\t"      \
      "ldw  r5,  20(sp)\n\t"      \
      "ldw  r6,  24(sp)\n\t"      \
      "ldw  r7,  28(sp)\n\t"      \
      "ldw  r8,  32(sp)\n\t"      \
      "ldw  r9,  36(sp)\n\t"      \
      "ldw  r10, 40(sp)\n\t"      \
      "ldw  r11, 44(sp)\n\t"      \
      "ldw  r12, 48(sp)\n\t"      \
      "ldw  r13, 52(sp)\n\t"      \
      "ldw  r14, 56(sp)\n\t"      \
      "ldw  r15, 60(sp)\n\t"      \
      "ldw  r16, 64(sp)\n\t"      \
      "ldw  r17, 68(sp)\n\t"      \
      "ldw  r18, 72(sp)\n\t"      \
      "ldw  r19, 76(sp)\n\t"      \
      "ldw  r20, 80(sp)\n\t"      \
      "ldw  r21, 84(sp)\n\t"      \
      "ldw  r22, 88(sp)\n\t"      \
      "ldw  r23, 92(sp)\n\t"      \
      "ldw  r25, 96(sp)\n\t"      \
      "ldw  r26, 100(sp)\n\t"     \
      "ldw  r28, 104(sp)\n\t"     \
      "ldw  r29, 108(sp)\n\t"     \
      "ldw  r30, 112(sp)\n\t"     \
      "ldw  r31, 116(sp)")

#define SAVE_REGS_SOFTINT         \
  asm("stw  r1,  4(sp)\n\t"       \
      "stw  r3,  12(sp)\n\t"      \
      "stw  r4,  16(sp)\n\t"      \
      "stw  r5,  20(sp)\n\t"      \
      "stw  r6,  24(sp)\n\t"      \
      "stw  r7,  28(sp)\n\t"      \
      "stw  r8,  32(sp)\n\t"      \
      "stw  r9,  36(sp)\n\t"      \
      "stw  r10, 40(sp)\n\t"      \
      "stw  r11, 44(sp)\n\t"      \
      "stw  r12, 48(sp)\n\t"      \
      "stw  r13, 52(sp)\n\t"      \
      "stw  r14, 56(sp)\n\t"      \
      "stw  r15, 60(sp)\n\t"      \
      "stw  r16, 64(sp)\n\t"      \
      "stw  r17, 68(sp)\n\t"      \
      "stw  r18, 72(sp)\n\t"      \
      "stw  r19, 76(sp)\n\t"      \
      "stw  r20, 80(sp)\n\t"      \
      "stw  r21, 84(sp)\n\t"      \
      "stw  r22, 88(sp)\n\t"      \
      "stw  r23, 92(sp)\n\t"      \
      "stw  r25, 96(sp)\n\t"      \
      "stw  r26, 100(sp)\n\t"     \
      "stw  r28, 104(sp)\n\t"     \
      "stw  r29, 108(sp)\n\t"     \
      "stw  r30, 112(sp)\n\t"     \
      "stw  r31, 116(sp)" ::: "r2")

#define LOAD_REGS_SOFTINT         \
  asm("ldw  r1,  4(sp)\n\t"       \
      "ldw  r3,  12(sp)\n\t"      \
      "ldw  r4,  16(sp)\n\t"      \
      "ldw  r5,  20(sp)\n\t"      \
      "ldw  r6,  24(sp)\n\t"      \
      "ldw  r7,  28(sp)\n\t"      \
      "ldw  r8,  32(sp)\n\t"      \
      "ldw  r9,  36(sp)\n\t"      \
      "ldw  r10, 40(sp)\n\t"      \
      "ldw  r11, 44(sp)\n\t"      \
      "ldw  r12, 48(sp)\n\t"      \
      "ldw  r13, 52(sp)\n\t"      \
      "ldw  r14, 56(sp)\n\t"      \
      "ldw  r15, 60(sp)\n\t"      \
      "ldw  r16, 64(sp)\n\t"      \
      "ldw  r17, 68(sp)\n\t"      \
      "ldw  r18, 72(sp)\n\t"      \
      "ldw  r19, 76(sp)\n\t"      \
      "ldw  r20, 80(sp)\n\t"      \
      "ldw  r21, 84(sp)\n\t"      \
      "ldw  r22, 88(sp)\n\t"      \
      "ldw  r23, 92(sp)\n\t"      \
      "ldw  r25, 96(sp)\n\t"      \
      "ldw  r26, 100(sp)\n\t"     \
      "ldw  r28, 104(sp)\n\t"     \
      "ldw  r29, 108(sp)\n\t"     \
      "ldw  r30, 112(sp)\n\t"     \
      "ldw  r31, 116(sp)" ::: "r2")

#else /* NATIVE */

#define USERMODE
#define KERNELMODE

#endif /* NATIVE */

#endif
