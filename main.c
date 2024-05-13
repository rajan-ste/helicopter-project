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
#include "reference.h"

enum flightState_t {
    FLYING = 0,
    LAUNCHING = 1,
    LANDED = 2,
    LOCKED = 3,
    LANDING = 4
};


/******************************************************************************
 * GLOBALS
 *****************************************************************************/
int16_t meanVal = 0;
int16_t setPoint = 0;
int16_t yawSetPoint = 0;
int32_t yawPos = 0;
int32_t yawDeg = 0;
uint32_t motorDuty = 0;
uint32_t tailDuty = 0;
int16_t min_Adc = 0;
bool displayFlag = false;
bool controllerFlag = false;
bool buttonFlag = false;
enum flightState_t flightState = LOCKED;

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
   motorDuty = altController(setPoint, meanVal);
   tailDuty = yawController(yawSetPoint, yawPos, 0.8*motorDuty);
   setPWM(PWM_START_RATE_HZ, motorDuty);
   setPWM2(PWM_START_RATE_HZ, tailDuty);
}

void updateDisplay()
{
    displayValues(getPercentage(meanVal), yawDeg, getYawInt(yawDeg), getYawDec(yawDeg));
}

int16_t ctr = 0;

void moveButtons()
{
    switch(flightState) {
    case FLYING :
        updateButtons();
        updateSwitch();
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
//        if (getState()) {
//            yawSetPoint = 0;
//            flightState = LANDING;
//        }
        break;


    case LAUNCHING :
        enablePWM();
        updateReference();
        if(ctr == 0) {
            setPoint = increaseSetPoint(setPoint);
        }

        if (getRef()) {
            if (yawPos >= yawSetPoint) {
                yawSetPoint = increaseYawSetPoint(yawSetPoint);
            }
        }
        else if (!getRef()) {
            int16_t refVal = getYawPos();
            yawSetPoint = refVal;
            flightState = FLYING;
        }
        ctr++;
        break;

    case LANDED :
        updateSwitch();
        if(getState()) {
            flightState = LAUNCHING;
        }
        break;

    case LOCKED :
        updateSwitch();
        disablePWM();
        if(getState() == 0) {
            flightState = LANDED;
        }
        break;

    case LANDING :
        if (yawPos == 0) {
            setPoint = min_Adc;
        }
        if (meanVal = min_Adc) {
            flightState = LANDED;
        }
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
    initReference();

    // Enable interrupts to the processor.
    IntMasterEnable();
    SysCtlDelay(SysCtlClockGet() / 5);
    // get landed value
    int16_t min_Adc = 0;
    min_Adc = initAltitude();
    initAdcLimits(min_Adc);
    setPoint = min_Adc;
    runController();
    yawSetPoint = yawPos;
    initDisplay ();
}

void sendSerialData() {
    sendData(motorDuty, tailDuty, setPoint, yawPos, yawSetPoint, motorDuty * 0.8, flightState);
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
