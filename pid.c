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
    setPoint -= 124;
    if (setPoint <= maxAdc) {
        setPoint = maxAdc;
    }
    return setPoint;
}

int16_t decreaseSetPoint (int16_t setPoint)
{
    return setPoint += 124;
}

int16_t increaseYawSetPoint(int16_t yawSetPoint)
{
    yawSetPoint += 19;
    if (yawSetPoint >= 224) {
        yawSetPoint = -224 + (yawSetPoint - 224);
    }
    return yawSetPoint;
}

int16_t decreaseYawSetPoint(int16_t yawSetPoint)
{
    yawSetPoint -= 19;
    if (yawSetPoint <= -224) {
        yawSetPoint = 224 + (yawSetPoint + 224);
    }
    return yawSetPoint;
}

int16_t altController (int16_t setPoint, int16_t meanVal)
{
    static int16_t I = 0;
    static int16_t prev_sensor_reading = 0;
    int16_t error = setPoint - meanVal;
    int16_t P = ALT_KP * error;
    int16_t dI = ALT_KI * error * DELTA_T;
    int16_t D = ALT_KD * (prev_sensor_reading - meanVal) / DELTA_T;

    int16_t control = GRAVITY - P + (I + dI) + D;

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

int16_t yawController (int16_t setPoint, int16_t yawPos, int16_t offset)
{
    static int16_t I = 0;
    static int16_t prev_yaw_reading = 0;
    int16_t error = setPoint - yawPos;
    if (error < -224) {
        error = 448 + error;
    } else if (error > 224) {
        error = -448 + error;
    } else if ((error > -224) && (error < 224)) {
        error = error;
    }
    int16_t P = YAW_KP * error;
    if (P > 30) {
        P = 30;
    }
    int16_t dI = YAW_KI * error * DELTA_T;
    int16_t D = YAW_KD * (prev_yaw_reading - yawPos) / DELTA_T;

    int16_t control = offset + P + (I + dI) + D;

    I = (I + dI);
    prev_yaw_reading = yawPos;

    if(control < 5) {
        control = 5;
    }
    if(control > 95) {
        control = 95;
    }

    return control;
}
//

