/*
 * yaw.c
 */

#include "yaw.h"

static volatile int32_t yaw_counter = 0;

void initYaw(void) {

    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);

    GPIOIntRegister(GPIO_PORTB_BASE, yawIntHandler);

    GPIOPinTypeGPIOInput (GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

int32_t getYawPos(void) {
    if (yaw_counter > WRAPSTEP) {
        yaw_counter = -WRAPSTEP + (yaw_counter - WRAPSTEP);
    }
    if (yaw_counter < -WRAPSTEP) {
        yaw_counter = WRAPSTEP - (yaw_counter + WRAPSTEP);
    }

    return yaw_counter;
}

void yawIntHandler (void) {
        // Hold the previous and current encoder states
       static uint8_t previous_state = 0;
       uint8_t current_state;

       // Identify which interrupts occurred and clear them on GPIO Port B
       uint32_t interrupt_status = GPIOIntStatus(GPIO_PORTB_BASE, true);
       GPIOIntClear(GPIO_PORTB_BASE, interrupt_status);

       // Capture the new state of the encoder pins (PB0 and PB1)
       current_state = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

       // Exit the handler early if no change in the encoder state is detected
       if (current_state == previous_state) return;

       // Determine the direction of rotation from the encoder state transitions
       // Each condition corresponds to a valid sequence for quadrature decoding
       if (((previous_state == 0x00) && (current_state == 0x01)) ||
           ((previous_state == 0x01) && (current_state == 0x03)) ||
           ((previous_state == 0x03) && (current_state == 0x02)) ||
           ((previous_state == 0x02) && (current_state == 0x00))) {
           // If moving in a counter-clockwise direction, decrement the yaw counter
           yaw_counter--;
       } else {
           // For clockwise movement, increment the yaw counter
           yaw_counter++;
       }

       // Keep the current state as a reference for the next encoder change
       previous_state = current_state;
}
