#include "pid.h"

static int16_t minAdc = 0;
static int16_t maxAdc = 0;

void initAdcLimits(int16_t landedAdc)
{
    minAdc = landedAdc;
    maxAdc = minAdc - ONE_VOLT;
}

int16_t getminAdc()
{
    return minAdc;
}

int16_t getmaxAdc()
{
    return maxAdc;
}

int16_t increaseSetPoint (int16_t setPoint)
{
    return setPoint -= 124;
}

int16_t decreaseSetPoint (int16_t setPoint)
{
    return setPoint += 124;
}

int16_t increaseYawSetPoint(int16_t yawSetPoint)
{
    return yawSetPoint += 19;
}

int16_t decreaseYawSetPoint(int16_t yawSetPoint)
{
    return yawSetPoint -= 19;
}

int16_t controller (int16_t setPoint, int16_t meanVal, int16_t Kd, int16_t Kp, int16_t Ki, int16_t offset)
{
    static int16_t I = 0;
    static int16_t prev_sensor_reading = 0;
    int16_t error = setPoint - meanVal;
    int16_t P = Kp * error;
    int16_t dI = Ki * error * DELTA_T;
    int16_t D = Kd * (prev_sensor_reading - meanVal) / DELTA_T;

    int16_t control = offset - P - (I + dI) - D;

    I = (I + dI);
    prev_sensor_reading = meanVal;

    if(control < 5) {
        control = 5;
    }
    if(control > 95) {
        control = 95;
    }

    return control;
}


