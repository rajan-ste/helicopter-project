#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "display.h"
#include "altitude.h"

void initDisplay (void)
{
    // Initialize the Orbit OLED display
    OLEDInitialise ();
}

//*****************************************************************************
//
// Function to display the mean ADC value, sample count and yaw value
//
//*****************************************************************************
void displayValues(int16_t meanPercentage, int32_t yaw_deg, int32_t yaw_int, int32_t yaw_dec)
{
    bool negFlag = false;
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Altitude = %4d%%", meanPercentage);

    // Update line on display.
    OLEDStringDraw (string, 0, 1);

    if (yaw_dec < 0) {
        yaw_dec *= -1;
        negFlag = true;
    }

    usnprintf (string, sizeof(string), "Yaw = %d.%02d    " , yaw_int, yaw_dec);

    if (yaw_int == 0 && negFlag) {
            usnprintf (string, sizeof(string), "Yaw = -%d.%02d    " , yaw_int, yaw_dec);
        }

    // Update line on display.
    OLEDStringDraw (string, 0, 2);
}

void displayMeanVal (uint16_t meanVal)
{
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "ADC value = %4d%", meanVal);

    // Update line on display.
    OLEDStringDraw (string, 0, 1);
}

void displayBlank(void)
{
    char string[17];
    usnprintf (string, sizeof(string), "                ");
    OLEDStringDraw (string, 0, 0);
    OLEDStringDraw (string, 0, 1);
    OLEDStringDraw (string, 0, 2);
    OLEDStringDraw (string, 0, 3);
}


