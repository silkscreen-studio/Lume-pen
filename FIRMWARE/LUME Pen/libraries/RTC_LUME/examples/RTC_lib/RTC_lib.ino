//////////////////////////////////////////////////
//                                              //
//   Minimal RX8111CE example code | LUME pen   //
//      not tested yet on the hardware          //
//                   Silkscreen Studio          //
//                                              //
//////////////////////////////////////////////////

#include <Wire.h>
#include "RTC_LUME.h"

void setup()
{
    Wire.begin();
    Wire.setClock(400000UL);   // 400 kHz

    bool first_init = rx8111_init();

    if (!first_init) {
        // First boot or clock data lost, set current time
        RTCTime now;
        now.second = 0;
        now.minute = 30;
        now.hour   = 14;
        now.day    = 20;
        now.month  =  5;
        now.year   = 26;
        rx8111_setTime(now);
    }
}

void loop()
{
    RTCTime t;
    rx8111_getTime(t);

    // Use t.hour, t.minute, t.second, t.day, t.month, t.year

    delay(1000);
}
