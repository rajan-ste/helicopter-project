#include "switch.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"

/*
 * Initialise the SW1 switch
 */
void initSwitch1(void)
{
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
}

/*
 * Initialise the SW1 switch
 */
void initSwitch2(void)
{
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
}

/*
 * state of switch, false = down, true = up
 */
static bool state1 = false;
static bool state2 = true;

/*
 * read the switch1 pin to check for state change
 */
void updateSwitch1()
{
     state1 = (GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7) == GPIO_PIN_7);
}

/*
 * get the state of the switch1
 */
bool getSwitchState1()
{
    return state1;
}

/*
 * read the switch2 pin to check for state change
 */
void updateSwitch2()
{
     state2 = (GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_6) == GPIO_PIN_6);
}

/*
 * get the state of the switch2
 */
bool getSwitchState2()
{
    return state2;
}


