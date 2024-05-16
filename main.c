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
bool refKnown = false;
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

void sendSerialData()
{
    sendData(motorDuty, tailDuty, setPoint, yawPos, yawSetPoint, motorDuty * 0.8, flightState, refVal, meanVal);
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
    sendSerialData();
}

//void adcSample(void)
//{
//    ADCProcessorTrigger(ADC0_BASE, 3);
//}

void moveButtons()
{
    switch(flightState) {
    case FLYING :
        updateButtons();
        updateSwitch();
        if (checkButton(UP) == PUSHED) {
            increaseSetPoint(&setPoint);
        }
        if (checkButton(DOWN) == PUSHED) {
            decreaseSetPoint(&setPoint);
        }
        if (checkButton(RIGHT) == PUSHED) {
            increaseYawSetPoint(&yawSetPoint);
        }
        if (checkButton(LEFT) == PUSHED) {
            decreaseYawSetPoint(&yawSetPoint);
        }
        if (!getSwitchState()) {
            flightState = LANDING;
        }
        break;

    case LAUNCHING :
        enablePWM();
        updateReference();
        if(firstRefCycle) {
            increaseSetPoint(&setPoint);
            increaseYawSetPointRef(&yawSetPoint);
            firstRefCycle = false;
        }
        if(!refKnown) {
            findReferenceYaw(&yawPos, &yawSetPoint, &refVal, &flightState, &refKnown);
        } else if (refKnown) {
            goToRefYaw(&yawPos, &yawSetPoint, &refVal, &flightState);
        }
        break;

    case LANDED :
        updateSwitch();
        if(getSwitchState()) {
            flightState = LAUNCHING;
            firstRefCycle = true;
        }
        break;

    case LOCKED :
        updateSwitch();
        disablePWM();
        if(!getSwitchState()) {
            flightState = LANDED;
        }
        break;

    case LANDING :
        yawSetPoint = refVal;
        if (yawPos == refVal) {
            setPoint = min_Adc;
        }
        if (yawPos == refVal && (meanVal >= min_Adc || meanVal >= (min_Adc - 6))) {
            flightState = LANDED;
        }
    }

}

void init(void)
{
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
    IntMasterEnable();
    SysCtlDelay(SysCtlClockGet() / 5); // delay to populate buffer

    // get landed value
    initAltitude();
    min_Adc = getLandedAlt();
    initAdcLimits(min_Adc);
    setPoint = min_Adc;
    runController();
    yawSetPoint = yawPos;
    initDisplay ();
}

int main(void)
{
    init();

//    setKernelTask(adcSample, KERNEL_FREQ_HZ / ADC_SAMPLE_RATE, PRIO_0);
    setKernelTask(runController, KERNEL_FREQ_HZ / RUN_CONTROLLER_RATE, PRIO_0);
    setKernelTask(updateDisplay, KERNEL_FREQ_HZ / UPDATE_DISPLAY_RATE, PRIO_1);
    setKernelTask(moveButtons, KERNEL_FREQ_HZ / MOVE_BUTTONS_RATE, PRIO_2);
    runKernel();
}
