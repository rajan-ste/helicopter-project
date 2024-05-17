#include "helistates.h"

char *getStringHeliState(flightState_t state) {

    switch(state) {
    case 0:
        return "FLYING";
    case 1:
        return "LAUNCHING";
    case 2:
        return "LANDED";
    case 3:
        return "LOCKED";
    case 4:
        return "LANDING";
    default:
        return "";
    }
}
