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

uint8_t button_state (uint8_t displayState) {
    if ((checkButton(UP) == PUSHED)) {
        displayState+=1;
        if (displayState == 3) {
            displayState=0;
        }
    }

    return displayState;
}

int main(void) {

    initClock ();
    initADC ();
    initButtons();
    initDisplay ();
    initYaw ();

    // Enable interrupts to the processor.
    IntMasterEnable();

    // get landed value
    initAltitude();

    int16_t meanVal; // Declare meanVal here
    uint8_t displayState = 0;
    int32_t yawPos;
    int32_t yawDeg;

    while (1)
    {
        updateButtons();
        if (checkButton(LEFT) == PUSHED) {
             initAltitude();
         }

        // Calculate and display the rounded mean of the buffer contents
        displayState = button_state(displayState);
        switch (displayState) {
        case 0 :
            meanVal = getMeanBufferVal();
            yawPos = getYawPos();
            yawDeg = getYawDeg(yawPos);
            displayValues (getPercentage(meanVal), yawDeg, getYawInt(yawDeg), getYawDec(yawDeg));
            break;
        case 1 :
            meanVal = getMeanBufferVal();
            displayMeanVal (meanVal);
            break;
        case 2 :
            displayBlank();
            break;
        }

        SysCtlDelay (SysCtlClockGet() / 100);  // Update display at ~ 2 Hz
    }
}
