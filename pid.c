#include "pid.h"

static int16_t minAdc = 0;
static int16_t maxAdc = 0;

/*
 * initiates the min and max ADC values
 */
void initAdcLimits(int16_t landedAdc)
{
    minAdc = landedAdc;
    maxAdc = minAdc - ONE_VOLT;
}

/*
 * returns the min ADC value
 */

int16_t getminAdc()
{
    return minAdc;
}

/*
 * returns the max ADC value
 */

int16_t getmaxAdc()
{
    return maxAdc;
}

/*
 * increases the altitude set point and does not allow the set point
 * to go under the max ADC value
 */

void increaseSetPoint (int16_t *setPoint, int16_t step)
{
    *setPoint -= step;
    if (*setPoint <= maxAdc) {
        *setPoint = maxAdc;
    }
}


/*
 * decreases the altitude set point and does not allow the set point
 * to go over the min ADC value
 */

void decreaseSetPoint (int16_t *setPoint)
{
    *setPoint += ALT_STEP;
    if (*setPoint >= minAdc) {
        *setPoint = minAdc;
    }
}

/*
 * increases the yaw set point and if the yaw set point goes past the
 * wrap point the helicopter changes direction to go the shortest path
 */

void increaseYawSetPoint(int16_t *yawSetPoint)
{
    *yawSetPoint += YAW_STEP;
    if (*yawSetPoint >= WRAP_AROUND) {
        *yawSetPoint = -WRAP_AROUND + (*yawSetPoint - WRAP_AROUND);
    }
}

void increaseYawSetPointRef(int16_t *yawSetPoint)
{
    *yawSetPoint += YAW_STEP;
    if (*yawSetPoint >= WRAP_AROUND) {
        *yawSetPoint = -WRAP_AROUND;
    }
}

void decreaseYawSetPoint(int16_t *yawSetPoint)
{
    *yawSetPoint -= YAW_STEP;
    if (*yawSetPoint <= -WRAP_AROUND) {
        *yawSetPoint = WRAP_AROUND + (*yawSetPoint + WRAP_AROUND);
    }
}

int16_t altController (int16_t setPoint, int16_t meanVal)
{
    static int16_t I = 0;
    static int16_t prev_sensor_reading = 0;
    int16_t error = setPoint - meanVal;
    int16_t P = ALT_KP * error;

    int16_t dI = ALT_KI * error * DELTA_T;
    int16_t D = ALT_KD * (prev_sensor_reading - meanVal) / DELTA_T;
    int16_t total = P + (I + dI) + D;
    if (total >= EFFORT_CAP) {
        total = EFFORT_CAP;
    }
    int16_t control = GRAVITY - (total);

    I = (I + dI);
    prev_sensor_reading = meanVal;

    if(control < CONTROL_LOWER) {
        control = CONTROL_LOWER;
    }
    if(control > CONTROL_UPPER) {
        control = CONTROL_UPPER;
    }

    return control;
}

int16_t yawController (int16_t setPoint, int16_t yawPos, int16_t offset)
{
    static int16_t I = 0;
    static int16_t prev_yaw_reading = 0;
    int16_t error = setPoint - yawPos;
    if (error < -WRAP_AROUND) {
        error = FULL_ROTATION + error;
    } else if (error > WRAP_AROUND) {
        error = -FULL_ROTATION + error;
    } else if ((error > -WRAP_AROUND) && (error < WRAP_AROUND)) {
        error = error;
    }
    int16_t P = YAW_KP * error;
    if (P > YAW_EFFORT_CAP) {
        P = YAW_EFFORT_CAP;
    }
    int16_t dI = YAW_KI * error * DELTA_T;
    int16_t D = YAW_KD * (prev_yaw_reading - yawPos) / DELTA_T;

    int16_t control = offset + P + (I + dI) + D;

    I = (I + dI);
    prev_yaw_reading = yawPos;

    if(control < CONTROL_LOWER) {
        control = CONTROL_LOWER;
    }
    if(control > CONTROL_UPPER) {
        control = CONTROL_UPPER;
    }

    return control;
}
//

