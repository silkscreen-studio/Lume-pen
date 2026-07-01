// Minimal vibecoded library I'll use as a basis to create my own library once I have the hardware in hand. This just serves as a basis to test the AW933105 IC.

#ifndef AW9310X_MIN_H
#define AW9310X_MIN_H

#include <Arduino.h>
#include <Wire.h>

// 7-bit I2C address (ADDR pin tied LOW)
#define AW_ADDR       0x12

// Number of channels we care about
#define AW_NUM_CH     5

// Returns true if IC found and initialised, false otherwise.
// Call AFTER Wire.begin() + Wire.setClock().
bool aw_begin(void);

// Call in loop(). Reads the IC and updates internal state.
// Returns false on I2C bus error.
bool aw_update(void);

// Returns 1 if channel ch (0–4) is currently touched, 0 if not.
uint8_t aw_channel(uint8_t ch);

// Returns the raw 5-bit touch mask (bit N = channel N touched).
uint8_t aw_mask(void);

#endif
