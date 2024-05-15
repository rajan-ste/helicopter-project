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
int16_t refVal = 0;
bool firstRefCycle = true;
flightState_t flightState = LOCKED;



//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
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
int16_t ctr2 = 0;

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
        if (!getState()) {
            flightState = LANDING;
        }
        break;

    case LAUNCHING :
        enablePWM();
        updateReference();
        if(firstRefCycle) {
            setPoint = increaseSetPoint(setPoint);
            yawSetPoint = increaseYawSetPointRef(yawSetPoint);
            firstRefCycle = false;
        }
        findReferenceYaw(&yawPos, &yawSetPoint, &refVal, &flightState);
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
        yawSetPoint = refVal;
        setPoint = 0;
        if (meanVal == 0 && yawPos == refVal) {
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
    initKernelSysTick();

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
    runKernel();

}
