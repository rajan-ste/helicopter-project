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
#include "altitude.h"

//********************************************************
// Globals to module
//********************************************************
char statusStr[MAX_STR_LEN + 1];

void initSerial(void)
{
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);

    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}

void UARTSend (char *pucBuffer)
{
    while(*pucBuffer)
    {
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}

//**********************************************************************
// Transmit the current data values via serial
//**********************************************************************
void sendData(uint32_t motorDuty, uint32_t tailDuty, int16_t setPoint, int16_t yawPos,
              int16_t yawSetPoint, char *state, int16_t meanVal)
{
    int32_t deg = getYawDeg(yawPos);
    int32_t spdeg = getYawDeg(yawSetPoint);
    usprintf(statusStr, "Motor D: %3d%% Tail D: %3d%% Yaw: %3d YawSP: %3d Alt: %3d%% AltSP: %3d%% State: %16s \n\r", motorDuty, tailDuty,
             getYawInt(deg), getYawInt(spdeg), getPercentage(meanVal), getPercentage(setPoint), state);
    UARTSend (statusStr);

}
