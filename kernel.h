#ifndef KERNEL_H_
#define KERNEL_H_

#include "altitude.h"
#include "display.h"
#include "circBufT.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons.h"
#include "yaw.h"
#include "switch.h"
#include "pid.h"
#include "pwm.h"
#include "serial.h"
#include "reference.h"
#include "helistates.h"
#include "kernel.h"

#define KERNEL_FREQ_HZ 500
#define NUM_TASKS 3
#define ADC_SAMPLE_RATE 500
#define RUN_CONTROLLER_RATE 500
#define UPDATE_DISPLAY_RATE 4
#define MOVE_BUTTONS_RATE 100

/*
 * PRIO_0 is highest, priorities listed in descending order
 */
#define PRIO_0 0
#define PRIO_1 1
#define PRIO_2 2
#define PRIO_3 3

/*
 * Task object
 * @taskFunc the function to execute when the task is run
 * @cycles the cycle rate to execute the task at
 * @currCycles the number of cycles since the task was last executed
 * @ready true if task is ready to execute
 */
typedef struct {
    void (*taskFunc)(void);
    uint32_t cycles;
    uint32_t currCycles;
    bool ready;
} Task_t;

void initKernelSysTick(void);
void initKernel(void);
void SysTickIntHandler(void);
void runKernel(void);
void setKernelTask(void (*taskFunc)(void), uint32_t cycles, int16_t index);
void updateTaskState(void);

#endif /*KERNEL_H_ */
