#ifndef PID_H_
#define PID_H_

#include "altitude.h"
#include <stdint.h>
#define DELTA_T 0.002
#define ALT_KP 0.517
#define ALT_KI 0.477
#define ALT_KD 0
#define YAW_KP 2.2
#define YAW_KI 0.3
#define YAW_KD 0
#define GRAVITY 33
#define ALT_STEP 124
#define YAW_STEP 19
#define WRAP_AROUND 224
#define FULL_ROTATION 448
#define EFFORT_CAP 30
#define YAW_EFFORT_CAP 45
#define CONTROL_LOWER 2
#define CONTROL_UPPER 98


void initAdcLimits(int16_t landedAdc);
int16_t getminAdc();
int16_t getmaxAdc();
void increaseSetPoint (int16_t *setPoint, int16_t step);
void decreaseSetPoint (int16_t *setPoint);
void increaseYawSetPoint(int16_t *yawSetPoint);
void increaseYawSetPointRef(int16_t *yawSetPoint);
void decreaseYawSetPoint(int16_t *yawSetPoint);
int16_t altController (int16_t setPoint, int16_t meanVal);
int16_t yawController (int16_t setPoint, int16_t yawPos, int16_t offset);


#endif /*PID_H_*/
