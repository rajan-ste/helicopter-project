#ifndef ALTITUDE_H_
#define ALTITUDE_H_

// *******************************************************
// altitude.h
//
//
// Oliver Butler, Rajan Stephens
// Last modified:  18.3.2024
//
// *******************************************************

#include <stdint.h>
#include <stdbool.h>
#include "circBufT.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 30
#define SAMPLE_RATE_HZ 500
#define ONE_VOLT 1280
#define ADC_SCALER 38

void SysTickIntHandler(void);
void ADCIntHandler(void);
void initClock (void);
void  initADC (void);
void initDisplay (void);
int16_t getPercentage(uint16_t meanVal);
uint16_t getMeanBufferVal(void);
void initAltitude(void);
uint16_t getLandedAlt (void);
#endif /*ALTITUDE_H_*/
