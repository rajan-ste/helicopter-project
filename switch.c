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
void initSwitch(void)
{
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
}

/*
 * state of switch, false = down, true = up
 */
static bool state = false;

/*
 * read the switch pin to check for state change
 */
void updateSwitch()
{
     state = (GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7) == GPIO_PIN_7);
}

/*
 * get the state of the switch
 */
bool getSwitchState()
{
    return state;
}
