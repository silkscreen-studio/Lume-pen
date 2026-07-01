// Minimal vibecoded library example I'll use as a basis to create my own library once I have the hardware in hand. This just serves as a basis to test the AW933105 IC.

#include <Wire.h>
#include "AW9310X_min.h"

void setup()
{
    Wire.begin();
    Wire.setClock(400000UL);

    if (!aw_begin()) {
        // IC not found,  halt (or blink an error LED)
        while (true) {}
    }
}

void loop()
{
    aw_update();

    // aw_channel(n) => 1 if touched, 0 if not
    uint8_t ch0 = aw_channel(0);
    uint8_t ch1 = aw_channel(1);
    uint8_t ch2 = aw_channel(2);
    uint8_t ch3 = aw_channel(3);
    uint8_t ch4 = aw_channel(4);

    // Or get all five at once as a bitmask:
    // uint8_t mask = aw_mask();
    // bit 0 = CH0, bit 1 = CH1, … bit 4 = CH4

    delay(20);
}
