//********************************************************
//
// serial.c
//
// Support for displaying helicopter metrics via serial communication
// via 115200 baud rate
//
// Author:  P.J. Bones  UCECE

// Modified from uartDemo.c by: Tom Rizzi, Euan Robinson, Satwik Meravanage
// Last modified: 21 May 2021
//
//********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "serial.h"
#include "yaw.h"


//********************************************************
// Globals to module
//********************************************************
char statusStr[MAX_STR_LEN + 1];


//********************************************************
// initSerial - 8 bits, 1 stop bit, no parity
//********************************************************
void initSerial(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}


//**********************************************************************
// Transmit a string via UART0
//**********************************************************************
void UARTSend (char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}


//**********************************************************************
// Transmit the current data values via serial
//**********************************************************************
void sendData(uint32_t motorDuty, uint32_t tailDuty, int16_t setPoint, int16_t yawPos, int16_t yawSetPoint, int16_t offset, int16_t state)
{
    // Send a newline
//    usprintf(statusStr, "-----------------\n\r");
//    UARTSend (statusStr);

    // Send main duty cycle
    usprintf(statusStr, "M: %3d%% T: %3d%% Y: %3d YS: %3d SP: %3d O: %3d S: %3d \n\r", motorDuty, tailDuty, yawPos, yawSetPoint, setPoint, offset, state);
    UARTSend (statusStr);

}
