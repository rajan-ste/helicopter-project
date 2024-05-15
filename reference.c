#include "reference.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"



void initReference (void)
{
    GPIOPinTypeGPIOInput (GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPadConfigSet (GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
}

static bool ref = false;

void updateReference()
{
     ref = (GPIOPinRead (GPIO_PORTC_BASE, GPIO_PIN_4));
}

bool getRef()
{
    return ref;
}
