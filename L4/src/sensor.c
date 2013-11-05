/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h"

// Hardware configuration
#define SENSOR_SWITCHES *((volatile int *) 0x10000040)
#define SENSOR_GPIO_BASE_JP1 ((volatile int*) 0x10000060)
#define SENSOR_GPIO_BASE_JP2 ((volatile int*) 0x10000070)
#define SENSOR_GPIO_BASE SENSOR_GPIO_BASE_JP2
#define SENSOR_GPIO_DIRECTION_REGISTER *(SENSOR_GPIO_BASE + 1)
#define SENSOR_GPIO_DIRECTION_CONFIG 0x07F557FF

// Threshold configuration
#define SENSOR_THRESHOLD_VALUE 0xC

// Sensor modes
typedef enum {
    SENSOR_MODE_Value,
    SENSOR_MODE_State
} SensorMode;

// Sensor modes
typedef enum {
    SENSOR_Sensor0,
    SENSOR_Sensor1,
    SENSOR_Sensor2,
    SENSOR_Sensor3,
    SENSOR_Sensor4
} SensorNode;

// Lego GPIO configuration
typedef struct {
    unsigned int motor0_on:1;
    unsigned int motor0_fwd:1;
    unsigned int motor1_on:1;
    unsigned int motor1_fwd:1;
    unsigned int motor2_on:1;
    unsigned int motor2_fwd:1;
    unsigned int motor3_on:1;
    unsigned int motor3_fwd:1;
    unsigned int motor4_on:1;
    unsigned int motor4_fwd:1;
    unsigned int sensor0_on:1;
    unsigned int sensor0_ready:1;
    unsigned int sensor1_on:1;
    unsigned int sensor1_ready:1;
    unsigned int sensor2_on:1;
    unsigned int sensor2_ready:1;
    unsigned int sensor3_on:1;
    unsigned int sensor3_ready:1;
    unsigned int sensor4_on:1;
    unsigned int sensor4_ready:1;
    unsigned int unused:1;
    unsigned int mode:1;
    unsigned int threshold_load:1;
    unsigned int threshold_value:4;
    unsigned int sensor0_state:1;
    unsigned int sensor1_state:1;
    unsigned int sensor2_state:1;
    unsigned int sensor3_state:1;
    unsigned int sensor4_state:1;
} lego_gpio_t;

// Thread internals
static int sensor_thread_exit;

//
// Safe access to sensors
//
void sensor_direction_register_set(int value) {
    void* directionreg = (void*) &SENSOR_GPIO_DIRECTION_REGISTER;
    asm("ldw r2, %0" : : "m"(directionreg) : "r2");
    asm("ldw r3, %0" : : "m"(value) : "r3");
    asm("stwio r3, 0(r2)");
}
lego_gpio_t sensor_gpio_read(void) {
    uval32 value = 0;
    void* gpio = (void*) SENSOR_GPIO_BASE;
    asm("ldw r2, %0" : : "m"(gpio) : "r2");
    asm("ldwio r3, 0(r2)" ::: "r3", "r2");
    asm("stw r3, %0" : "=m"(value) :: "r3");
    value = ~value; return *(lego_gpio_t*)&value;
}
void sensor_gpio_write(lego_gpio_t value) {
    uval32 scalar = ~(*(uval32*) &value);
    void* gpio = (void*) SENSOR_GPIO_BASE;
    asm("ldw r2, %0" : : "m"(gpio) : "r2");
    asm("ldw r3, %0" : : "m"(scalar) : "r3");
    asm("stwio r3, 0(r2)");
}
int sensor_get_switch(int index) {
    uval32 value = 0;
    void* switches = (void*) &SENSOR_SWITCHES;
    if(index > 17) return -1;
    asm("ldw r2, %0" : : "m"(switches) : "r2");
    asm("ldwio r3, 0(r2)" ::: "r3", "r2");
    asm("stw r3, %0" : "=m"(value) :: "r3");
    return value & (1<< index);
}

/*
 * sensor_load_threshold
 *
 * Load a sensor threshold value
 */
int sensor_load_threshold(int sensor, int value) {
    // Prepare to load value
    lego_gpio_t state = sensor_gpio_read();
    int old_mode = state.mode;
    state.mode = SENSOR_MODE_Value;
    state.threshold_load = 1;
    
    // Set the correct sensor    
    switch(sensor) {
        case 0: state.sensor0_on = 1; break;
        case 1: state.sensor1_on = 1; break;
        case 2: state.sensor2_on = 1; break;
        case 3: state.sensor3_on = 1; break;
        case 4: state.sensor4_on = 1; break;
    }
    
    // Perform the switch to value mode
    sensor_gpio_write(state);

    // Now load threshold value
    state.threshold_value = ~(value & 0xF);
    sensor_gpio_write(state);
    
    // Disable loading
    state.sensor0_on = 0;
    state.sensor1_on = 0;
    state.sensor2_on = 0;
    state.sensor3_on = 0;
    state.sensor4_on = 0;
    state.threshold_load = 0;
    sensor_gpio_write(state);
}

/*
 * sensor_thread
 *
 * Manage the sensors and send sound accordingly
 */
void sensor_thread(void) {
    // State information
    lego_gpio_t state = {0};

    // Initialize sensors
    sensor_direction_register_set(SENSOR_GPIO_DIRECTION_CONFIG);
    
    // Set blank state
    sensor_gpio_write(state);
    
    // Set threshold values
    sensor_load_threshold(SENSOR_Sensor0, SENSOR_THRESHOLD_VALUE);
    sensor_load_threshold(SENSOR_Sensor1, SENSOR_THRESHOLD_VALUE);
    
    // Switch to state mode
    state = sensor_gpio_read();
    state.mode = SENSOR_MODE_State;
    sensor_gpio_write(state);
    
    // Setup the motor
    state = sensor_gpio_read();
    state.motor0_fwd = 1;
    sensor_gpio_write(state);
    
    // Start processing sensors
    while(!sensor_thread_exit) {
        // Read current state
        state = sensor_gpio_read();
        
        // Check motor switch
        state.motor0_on = sensor_get_switch(0);
        
        // Check sensors
        if(!state.sensor0_state)
            printk("Sensor 0 triggered!\n");
        if(!state.sensor1_state)
            printk("Sensor 1 triggered!\n");
            
        // Clear sensors
        state.sensor0_state = 1;
        state.sensor1_state = 1;
        sensor_gpio_write(state);
    
        // Yield to other threads
        SysCall(SYS_YIELD, 0, 0, 0);
    }
}

/*
 * sensor_init
 *
 * Initialize sensor subsystem
 */
int sensor_init(void) {
    // Set initial thread parameters
    sensor_thread_exit = 0;
    // Start the sensor processing thread
    SysCall(SYS_CREATE, (uval32) &sensor_thread, (uval32) malloc(STACKSIZE), 1);
    // Successful startup
    return 0;
}

/*
 * sensor_exit
 *
 * Shutdown the sensor subsystem
 */
int sensor_exit(void) {
    sensor_thread_exit = 1;
    return 0;
}
