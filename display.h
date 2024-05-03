#ifndef DISPLAY_H_
#define DISPLAY_H_

void initDisplay (void);
void displayValues(int16_t meanPercentage, int32_t yaw_deg, int32_t yaw_int, int32_t yaw_dec);
void displayMeanVal (uint16_t meanVal);
void displayBlank();


#endif /*DISPLAY_H_*/
