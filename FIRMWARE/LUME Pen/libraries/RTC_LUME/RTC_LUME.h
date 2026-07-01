/////////////////////////////////////////////////////////////
//                                                         //
//   Minimal RX8111CE library for attiny1616 | LUME pen    //
//      not tested yet on the hardware                     //
//                              Silkscreen Studio          //
//                                                         //
/////////////////////////////////////////////////////////////
    

#ifndef RTC_LUME_H
#define RTC_LUME_H


#include <stdint.h>
#include <stdbool.h>

#define RX8111_ADDR   0x32


#define RX_SEC        0x10
#define RX_MIN        0x11
#define RX_HOUR       0x12
#define RX_WEEK       0x13
#define RX_DAY        0x14
#define RX_MONTH      0x15
#define RX_YEAR       0x16
#define RX_EXTENSION  0x1D
#define RX_FLAG       0x1E
#define RX_CONTROL    0x1F
#define RX_PWRSW      0x32

#define FLAG_VLF    (1u<<1)  

struct RTCTime {
    uint8_t second;   // 0-59
    uint8_t minute;   // 0-59
    uint8_t hour;     // 0-23
    uint8_t day;      // 1-31
    uint8_t month;    // 1-12
    uint8_t year;     // 0-99
};

bool rx8111_init(void);

void rx8111_setTime(const RTCTime &t);

void rx8111_getTime(RTCTime &t);

bool rx8111_isRunning(void);

#endif
