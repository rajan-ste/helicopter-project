#include "kernel.h"
#include "pid.h"
#include <stdint.h>

bool displayFlag = false;
bool controllerFlag = false;
bool buttonFlag = false;
static Task_t tasks[NUM_TASKS];

void initKernelSysTick(void) {
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / KERNEL_FREQ_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

/*
 * Register a task in the kernel, lower index for higher priority
 * @taskFunc pointer to a function to be called by task object
 * @cycles the number of system ticks to wait before executing the task - 1 (cycles = 1 means every tick)
 * @index the priority of the task, lower index means it will be run earlier
 */
void setKernelTask(void (*taskFunc)(void), uint32_t cycles, int16_t index) {
    if (index < NUM_TASKS) {
        tasks[index].taskFunc = taskFunc;
        tasks[index].cycles = cycles;
        tasks[index].currCycles = 0;
        tasks[index].ready = true;
    }
}

/*
 * Update the current cycles and ready state of a task
 */
void updateTaskState(void) {
    int16_t i = 0;
    for (i=0; i < NUM_TASKS; i++) {
        tasks[i].currCycles++;
        if (tasks[i].currCycles == tasks[i].cycles) {
            tasks[i].ready = true;
            tasks[i].currCycles = 0;
        }
    }
}

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
// ADCProcessorTrigger must be called here rather than the kernel otherwise
// our first sampled adc value will be 0
//
//*****************************************************************************
void SysTickIntHandler(void)
{
    updateTaskState();
    ADCProcessorTrigger(ADC0_BASE, 3);
}

/*
 * Run the kernel, by checking if task is ready then executing it
 */
void runKernel(void) {
    while (1) {
       int16_t i = 0;
       for (i = 0; i < NUM_TASKS; i++) {
           if (tasks[i].ready) {
               tasks[i].ready = false;
               tasks[i].taskFunc();
               break;
           }
       }
   }
}
