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


//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 30
#define SAMPLE_RATE_HZ 5000
#define ONE_VOLT 1240
#define ADC_SCALER 38


void SysTickIntHandler(void);
void ADCIntHandler(void);
void initClock (void);
void  initADC (void);
void initDisplay (void);
void displayMeanVal(uint16_t meanVal, uint32_t count);
uint8_t getPercentage(uint16_t meanVal);
uint16_t getMeanAltitude(void);


#endif /*ALTITUDE_H_*/
