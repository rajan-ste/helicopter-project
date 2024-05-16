/*
 * yaw.h
 */

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
#include "helistates.h"

#ifndef YAW_H_
#define YAW_H_
#define MAX_DEGREES 360
#define SCALE_100 100
#define NUM_SLITS 448

#define WRAPSTEP 224

void initYaw(void);
int32_t getYawPos(void);
void yawIntHandler (void);
int32_t getYawDeg(int32_t yaw_counter);
int32_t getYawInt(int32_t yaw_deg);
int32_t getYawDec(int32_t yaw_deg);
void findReferenceYaw (int32_t *yawPos, int16_t *yawSetPoint, int16_t *refVal, flightState_t *flightState, bool *refKnown);
void goToRefYaw(int32_t *yawPos, int16_t *yawSetPoint, int16_t *refVal, flightState_t *flightState);


#endif
