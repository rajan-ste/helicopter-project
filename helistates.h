#ifndef HELISTATES_H_
#define HELISTATES_H_

#include <stdint.h>

typedef enum  {
    FLYING = 0,
    LAUNCHING = 1,
    LANDED = 2,
    LOCKED = 3,
    LANDING = 4
}flightState_t;

char *getStringHeliState(flightState_t state);

#endif // HELISTATES_H_
