/*
 * CSC372 - Lab 3
 *
 * Interrupt handling
 */

#include "defines.h"
#include "debug.h"
#include "list.h"
#include "kernel.h"
#include "irq.h"

#include <string.h>

//
// IRQ handler structures & management
//

enum EIRQ{IRQ_MAX = 32, IRQ_INVALID};
typedef struct {
    int irq;
    int priority;
    irq_handler_routine routine;
} irq_handler;

typedef struct {
    int enabled;
    irq_handler handler;
} irq_handler_entry;

static irq_handler_entry irq_handlers[IRQ_MAX];

/*
 * first_irq
 *
 * Find the first IRQ in a given mask
 */
static int find_first_irq(int x) {
    if(!x) return 32;
    int n = 0;
    if(!(x & 0x0000FFFF)) { n += 16; x >>= 16; }
    if(!(x & 0x000000FF)) { n += 8;  x >>= 8;  }
    if(!(x & 0x0000000F)) { n += 4;  x >>= 4;  }
    if(!(x & 0x00000003)) { n += 2;  x >>= 2;  }
    if(!(x & 0x00000001)) { n += 1;            }
    return n;
}

/*
 * irq_init
 *
 * Initialize interrupt system
 */
void irq_init() {
    InterruptLevel = IRQL_LOW;
    memset(irq_handlers, 0, sizeof(irq_handlers));
}


/*
 * irq_disable
 *
 * Disable all interrupts on the current CPU
 */
void irq_disable() {
#ifdef NATIVE
    asm ( "rdctl r8, status");
    asm ( "andi r8, r8, 2" );
    asm ( "wrctl status, r8");
#endif
}

/*
 * irq_enable
 *
 * Enable all interrupts on the current CPU
 */
void irq_enable() {
#ifdef NATIVE
    asm ( "rdctl r8, status");
    asm ( "ori r8, r8, 1" );
    asm ( "wrctl status, r8");
#endif
}

/*
 * irql_raise
 *
 * Raise the interrupt level
 */
void irql_raise(IRQL newirql, IRQL* oldirql) {
    KASSERT(newirql == IRQL_HIGH);
    irq_disable();
    *oldirql = InterruptLevel;
    InterruptLevel = newirql;
}

/*
 * irql_register
 *
 * Register an interrupt handler
 */

void irq_register(int irq, irq_handler_routine routine, int priority) {    
    // Sanity check on IRQ
    if(irq >= IRQ_MAX) return;

    IRQL_RAISE_TO_HIGH;
        irq_handlers[irq].handler.irq = irq;
        irq_handlers[irq].handler.routine = routine;
        irq_handlers[irq].handler.priority = priority;
        irq_handlers[irq].enabled = TRUE;
    IRQL_LOWER;
}

/*
 * irql_dispatch
 *
 * Dispatch an interrupt
 */
void irq_dispatch(int irq_mask) {
    // TODO: use priorities
    int irq = find_first_irq(irq_mask);
    if(irq_handlers[irq].enabled)
        irq_handlers[irq].handler.routine();
}

/*
 * irql_lower
 *
 * Lower the interrupt level
 */
void irql_lower(IRQL* oldirql) {
    IRQL newirql = *oldirql;
    InterruptLevel = newirql;
    if(newirql == IRQL_LOW) irq_enable();
}
