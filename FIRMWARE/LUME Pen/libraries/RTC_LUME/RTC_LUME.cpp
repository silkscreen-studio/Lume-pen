
/////////////////////////////////////////////////////////////
//                                                         //
//   Minimal RX8111CE library for attiny1616 | LUME pen    //
//      not tested yet on the hardware                     //
//                              Silkscreen Studio          //
//                                                         //
/////////////////////////////////////////////////////////////


#include "RTC_LUME.h"
#include <Wire.h>

static inline uint8_t bcd2dec(uint8_t b) {
  return (b >> 4) * 10 + (b & 0x0F);
}
static inline uint8_t dec2bcd(uint8_t d) {
  return ((d / 10) << 4) | (d % 10);
}

static void rx_write(uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(RX8111_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

static uint8_t rx_read(uint8_t reg)
{
  Wire.beginTransmission(RX8111_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)RX8111_ADDR, (uint8_t)1);
  return Wire.available() ? Wire.read() : 0xFF;
}

static void rx_read_burst(uint8_t reg, uint8_t *buf, uint8_t len)
{
  Wire.beginTransmission(RX8111_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)RX8111_ADDR, len);
  for (uint8_t i = 0; i < len; i++)
    buf[i] = Wire.available() ? Wire.read() : 0x00;
}

bool rx8111_init(void)
{
  delay(40);

  if (!(rx_read(RX_FLAG) & FLAG_VLF)) {
    return true;        // VLF = 0
  }

  uint16_t waited = 0;
  while (rx_read(RX_FLAG) & FLAG_VLF) {
    rx_write(RX_FLAG, 0x00);   //attempt to clear VLF
    delay(1000);
    waited += 1000;
    if (waited >= 20000) {
      // error, can't get VLF to be 0...
      break;   /* 20 s hard timeout */
    }
  }
 

  /* 0x32 | Power Switch Control | CHGEN | INIEN | z | z | SWSEL1 | SWSEL0 | SMPT1 | SMPT0

     Power-switch register (0x32): VDD-only : INIEN = 0, CHGEN = 0, SWSEL1,0 = 10b, SMPT1=0, SMPT0=0   -> SW1 always ON
     => 0b00001000 = 0x08
  */
  rx_write(RX_PWRSW, 0x08); // 0x32

  /* 0x1D | Extension Register | FSEL1 | FSEL0 | USEL | TE | WADA | ETS | TSEL1 | TSEL0
      FSEL1=1, FSEL0=1  -> FOUT OFF              (Table 43)
      WADA=1            -> DAY alarm mode         (§13.3.4)
      TSEL1=1, TSEL0=0  -> 1 Hz source (idle)     (§13.3.4)
      TE=0, USEL=0, ETS=0 -> timer/update/ts off
      => 0b11001010 = 0xCA
  */
  
  rx_write(RX_EXTENSION, 0xCA); // 0x1D
  rx_write(RX_CONTROL, 0x00);   // clear control register 0x1F
  rx_write(RX_FLAG, 0x00);   // clear flag register 0x1E

  // Time is NOT valid yet: must call rx8111_setTime() (in main code)
  return false;
}

void rx8111_setTime(const RTCTime &t)
{
  rx_write(RX_CONTROL, 0x01);

  Wire.beginTransmission(RX8111_ADDR);
  Wire.write(RX_SEC);
  Wire.write(dec2bcd(t.second));
  Wire.write(dec2bcd(t.minute));
  Wire.write(dec2bcd(t.hour));
  Wire.write(0x01);              // WEEK register: default to Sunday, even if not used
  Wire.write(dec2bcd(t.day));
  Wire.write(dec2bcd(t.month));
  Wire.write(dec2bcd(t.year));
  Wire.endTransmission();
  
  rx_write(RX_FLAG, 0x00);

  rx_write(RX_CONTROL, 0x00);
}

void rx8111_getTime(RTCTime &t)
{
  uint8_t buf[7];
  
  rx_read_burst(RX_SEC, buf, 7);

  t.second = bcd2dec(buf[0] & 0x7F);
  t.minute = bcd2dec(buf[1] & 0x7F);
  t.hour   = bcd2dec(buf[2] & 0x3F);
  t.day    = bcd2dec(buf[4] & 0x3F);
  t.month  = bcd2dec(buf[5] & 0x1F);
  t.year   = bcd2dec(buf[6]);
}

bool rx8111_isRunning(void)
{
  return !(rx_read(RX_FLAG) & FLAG_VLF);
}
