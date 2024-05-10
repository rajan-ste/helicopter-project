#include "altitude.h"
#include "display.h"
#include "circBufT.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
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

/******************************************************************************
 * GLOBALS
 *****************************************************************************/
int16_t meanVal = 0;
int32_t yawPos = 0;
int32_t yawDeg = 0;
bool displayFlag = false;
bool controllerFlag = false;

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

    counter++;

}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


void controller(void)
{
   meanVal = getMeanBufferVal();
   yawPos = getYawPos();
   yawDeg = getYawDeg(yawPos);
}

void updateDisplay()
{
    displayValues (getPercentage(meanVal), yawDeg, getYawInt(yawDeg), getYawDec(yawDeg));
}

void init(void) {
    initClock ();
    initADC ();
    initButtons();
    initDisplay ();
    initYaw ();

    // Enable interrupts to the processor.
    IntMasterEnable();

    // get landed value
    initAltitude();
}

int main(void) {

    init();

    while (1)
    {
//        updateButtons();
        // Calculate and display the rounded mean of the buffer contents
        if (controllerFlag) {
            controller();
            controllerFlag = 0;
        }
        if (displayFlag) {
            updateDisplay();
            displayFlag = 0;
        }

//        SysCtlDelay (SysCtlClockGet() / 200);  // Update display at ~ 2 Hz
    }
}
