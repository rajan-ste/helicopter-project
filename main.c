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
#include "switch.h"
#include "pid.h"
#include "pwm.h"
#include "serial.h"

enum flightState_t {
    FLYING = 0,
    LAUNCHING = 1,
    LANDED = 2
};


/******************************************************************************
 * GLOBALS
 *****************************************************************************/
const int16_t altKp = 1;
const int16_t altKi = 0;
const int16_t altKd = 0;
const int16_t yawKp = 1;
const int16_t yawKi = 0;
const int16_t yawKd = 0;
int16_t meanVal = 0;
int16_t setPoint = 0;
int16_t yawSetPoint = 0;
int32_t yawPos = 0;
int32_t yawDeg = 0;
uint32_t motorDuty = 0;
uint32_t tailDuty = 0;
bool displayFlag = false;
bool controllerFlag = false;
bool buttonFlag = false;
enum flightState_t flightState = FLYING;

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


void runController(void)
{
   meanVal = getMeanBufferVal();
   yawPos = getYawPos();
   yawDeg = getYawDeg(yawPos);
   motorDuty = controller(setPoint, meanVal, altKd, altKp, altKi, 33);
   tailDuty = controller(yawSetPoint, yawPos, yawKd, yawKp, yawKi, 0.8*motorDuty);
   setPWM(PWM_START_RATE_HZ, motorDuty);
   setPWM2(PWM_START_RATE_HZ, tailDuty);
}

void updateDisplay()
{
    displayValues(getPercentage(meanVal), yawDeg, getYawInt(yawDeg), getYawDec(yawDeg));

}

void moveButtons()
{
    switch(flightState) {
    case FLYING :
        updateButtons();
        if (checkButton(UP) == PUSHED) {
            setPoint = increaseSetPoint(setPoint);
        }
        if (checkButton(DOWN) == PUSHED) {
            setPoint = decreaseSetPoint(setPoint);
        }
        if (checkButton(RIGHT) == PUSHED) {
            yawSetPoint = increaseYawSetPoint(yawSetPoint);
        }
        if (checkButton(LEFT) == PUSHED) {
            yawSetPoint = decreaseYawSetPoint(yawSetPoint);
        }
        break;

    case LAUNCHING :
        increaseSetPoint(setPoint);
        flightState = FLYING;
        break;

    case LANDED :
        updateSwitch();
        if(getState()) {
            flightState = LAUNCHING;
        }
        break;
    }
}

void init(void) {
    initClock ();
    initADC ();
    initButtons();
    initYaw ();
    initialisePWM();
    initialisePWMTAIL();
    initSwitch();
    initSerial();

    // Enable interrupts to the processor.
    IntMasterEnable();
    SysCtlDelay(SysCtlClockGet() / 5);
    // get landed value
    int16_t min_Adc = 0;
    min_Adc = initAltitude();
    initAdcLimits(min_Adc);
    setPoint = min_Adc;
    initDisplay ();
}

void sendSerialData() {
    sendData(motorDuty, tailDuty, setPoint, yawSetPoint);
}

int main(void) {

    init();

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
