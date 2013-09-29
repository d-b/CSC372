/*
 * CSC372 - Lab 3
 *
 * Interrupt handling
 */

#ifndef _IRQ_H_
#define _IRQ_H_

// Helper macros
#define IRQL_RAISE_TO_HIGH \
    { IRQL oldirql; \
      irql_raise(IRQL_HIGH, &oldirql);
#define IRQL_LOWER \
      irql_lower(&oldirql); }

// IRQ handler routine
typedef void (*irq_handler_routine)();

// IRQ processing levels
typedef unsigned char IRQL;
enum EIRQL{IRQL_LOW, IRQL_HIGH};

// IRQ initialization
void irq_init();

// IRQL level routines
void irql_raise(IRQL newirql, IRQL* oldirql);
void irql_lower(IRQL* oldirql);

// IRQ registration and processing
void irq_disable();
void irq_register(int irq, irq_handler_routine routine, int priority);
void irq_unregister(int irq);
void irq_dispatch(int irq_mask);
void irq_enable();

#endif
