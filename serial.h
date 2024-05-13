#ifndef SERIAL_H_
#define SERIAL_H_

//********************************************************
//
// serial.h
//
// Author:  P.J. Bones  UCECE, Modified from uartDemo.c by Tom Rizzi, Euan Robinson, Satwik Meravanage
// Last modified: 21 May 2021
//
//********************************************************

//********************************************************
// Constants
//********************************************************
#define SLOWTICK_RATE_HZ 4
#define MAX_STR_LEN 105
//---USB Serial comms: UART0, Rx:PA0 , Tx:PA1
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX

//********************************************************
// Prototypes
//********************************************************
void initSerial(void);
void UARTSend(char *pucBuffer);
void sendData(uint32_t motorDuty, uint32_t tailDuty, int16_t setPoint, int16_t yawPos, int16_t yawSetPoint, int16_t offset, int16_t state);


#endif /* SERIAL_H_ */
