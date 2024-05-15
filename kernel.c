#include "kernel.h"
#include "pid.h"


bool displayFlag = false;
bool controllerFlag = false;
bool buttonFlag = false;
unsigned long systemTime = 0;

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

//void initKernel(void) {
//
//}

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void SysTickIntHandler(void)
{
    static uint32_t counter = 0;

    ADCProcessorTrigger(ADC0_BASE, 3);

    if (counter % 1 == 0) {
        controllerFlag = true;
    }
    if (counter % 10 == 0) {
        displayFlag = true;
    }
    if (counter % 5 == 0) {
        buttonFlag = true;
    }

    counter++;

}

void runKernel(void) {

    while (1)
        {
    //        updateButtons();
            // Calculate and display the rounded mean of the buffer contents
            if (controllerFlag) {
                runController();
                controllerFlag = 0;
            }
            if (displayFlag) {
                updateDisplay();
                sendSerialData();
                displayFlag = 0;
            }
            if (buttonFlag) {
                moveButtons();
                buttonFlag = 0;
            }

    //        SysCtlDelay (SysCtlClockGet() / 200);  // Update display at ~ 2 Hz
        }

}
