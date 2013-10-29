/*
 * CSC372 - RTOS
 *
 * Interrupt handling
 * 
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _IRQ_H_
#define _IRQ_H_

// Helper macros
#define IRQL_RAISE_TO_HIGH \
    IRQL _irql_raise_to_high_oldirql; \
    irql_raise(IRQL_HIGH, &_irql_raise_to_high_oldirql);
#define IRQL_LOWER \
      irql_lower(&_irql_raise_to_high_oldirql);

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
