#ifndef PID_H_
#define PID_H_

#include "altitude.h"
#include <stdint.h>
#define DELTA_T 0.002

void initAdcLimits(int16_t landedAdc);
int16_t getminAdc();
int16_t getmaxAdc();
int16_t increaseSetPoint (int16_t setPoint);
int16_t decreaseSetPoint (int16_t setPoint);
int16_t increaseYawSetPoint(int16_t yawSetPoint);
int16_t decreaseYawSetPoint(int16_t yawSetPoint);
int16_t controller (int16_t setPoint, int16_t meanVal, int16_t Kd, int16_t Kp, int16_t Ki, int16_t offset);

#endif /*PID_H_*/
