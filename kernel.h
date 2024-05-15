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
#define NUM_TASKS 4

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
