
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//         Minimal Firmware V1.0  |  LUME pen | Silkscreen Studio             //
//                                                                            //
//     Not all the logic is implemented yet, it's just a rough basis.         //
//   managing touch events with the AW93105 will be implemented later on,     //
// once I have the 'debug-friendly board' to test and complete the libraries  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include <Wire.h>
#include "RTC_LUME.h"
#include "AW9310X_min.h"
#include <avr/sleep.h>

#define AW_NINT PIN_PB2

void touchInterrupt (void) {
}

bool stateTouch [5] = {0, 0, 0, 0, 0};
bool prevStateTouch [5] = {0, 0, 0, 0, 0};

const uint8_t FrameDELAY = 5;

uint8_t brightness = 255;

bool pomodoro = 0;
RTCTime PomTimer;
RTCTime PomStart;
uint8_t N = 1;

uint8_t touchEvent = 0; // when there is a new Touch event
// can be swipe R -> L [1], swipe L -> R then swipe R -> L [2], double tap CMD button [3]
//       Hold CMD button for > 5 sec [4], swipe L -> R [6], single tap R [7], single tap L [8], etc.

uint8_t displayStateMap [6] = {
  0b000000,
  0b000000,
  0b000000,
  0b000000,
  0b000000,
  0b000000
};


const uint8_t digitsStateMap [4][10][6] PROGMEM = {
  {
    { //digit 1 (far left), number 0
      0b000000,
      0b100000,
      0b100000,
      0b100000,
      0b010000,
      0b110000
    },
    { //digit 1 (far left), number 1
      0b000000,
      0b000000,
      0b000000,
      0b000000,
      0b010000,
      0b010000
    },
    { //digit 1 (far left), number 2
      0b000000,
      0b100000,
      0b000000,
      0b100000,
      0b110000,
      0b100000
    },
    { //digit 1 (far left), number 3
      0b000000,
      0b100000,
      0b000000,
      0b000000,
      0b110000,
      0b110000
    },
    { //digit 1 (far left), number 4
      0b000000,
      0b000000,
      0b100000,
      0b000000,
      0b110000,
      0b010000
    },
    { //digit 1 (far left), number 5
      0b000000,
      0b100000,
      0b100000,
      0b000000,
      0b100000,
      0b110000
    },
    { //digit 1 (far left), number 6
      0b000000,
      0b100000,
      0b100000,
      0b100000,
      0b100000,
      0b110000
    },
    { //digit 1 (far left), number 7
      0b000000,
      0b100000,
      0b000000,
      0b000000,
      0b010000,
      0b010000
    },
    { //digit 1 (far left), number 8
      0b000000,
      0b100000,
      0b100000,
      0b100000,
      0b110000,
      0b110000
    },
    { //digit 1 (far left), number 9
      0b000000,
      0b100000,
      0b100000,
      0b000000,
      0b110000,
      0b110000
    }
  },

  {
    { //digit 2 (middle left), number 0
      0b011000,
      0b001000,
      0b010000,
      0b000000,
      0b001000,
      0b001000
    },
    { //digit 2 (middle left), number 1
      0b011000,
      0b000000,
      0b000000,
      0b000000,
      0b000000,
      0b000000
    },
    { //digit 2 (middle left), number 2
      0b010000,
      0b001000,
      0b010000,
      0b001000,
      0b000000,
      0b001000
    },
    { //digit 2 (middle left), number 3
      0b011000,
      0b001000,
      0b010000,
      0b001000,
      0b000000,
      0b000000
    },
    { //digit 2 (middle left), number 4
      0b011000,
      0b000000,
      0b000000,
      0b001000,
      0b001000,
      0b000000
    },
    { //digit 2 (middle left), number 5
      0b001000,
      0b001000,
      0b010000,
      0b001000,
      0b001000,
      0b000000
    },
    { //digit 2 (middle left), number 6
      0b001000,
      0b001000,
      0b010000,
      0b001000,
      0b001000,
      0b001000
    },
    { //digit 2 (middle left), number 7
      0b011000,
      0b000000,
      0b010000,
      0b000000,
      0b000000,
      0b000000
    },
    { //digit 2 (middle left), number 8
      0b011000,
      0b001000,
      0b010000,
      0b001000,
      0b001000,
      0b001000
    },
    { //digit 2 (middle left), number 9
      0b011000,
      0b001000,
      0b010000,
      0b001000,
      0b001000,
      0b000000
    }
  },

  {
    { //digit 3 (middle right), number 0
      0b000100,
      0b000100,
      0b000010,
      0b000010,
      0b000100,
      0b000010
    },
    { //digit 3 (middle right), number 1
      0b000000,
      0b000100,
      0b000010,
      0b000000,
      0b000000,
      0b000000
    },
    { //digit 3 (middle right), number 2
      0b000000,
      0b000100,
      0b000100,
      0b000010,
      0b000100,
      0b000010
    },
    { //digit 3 (middle right), number 3
      0b000000,
      0b000100,
      0b000110,
      0b000010,
      0b000000,
      0b000010
    },
    { //digit 3 (middle right), number 4
      0b000000,
      0b000100,
      0b000110,
      0b000000,
      0b000100,
      0b000000
    },
    { //digit 3 (middle right), number 5
      0b000000,
      0b000100,
      0b000100,
      0b000010,
      0b000100,
      0b000010
    },
    { //digit 3 (middle right), number 6
      0b000100,
      0b000100,
      0b000100,
      0b000010,
      0b000100,
      0b000010
    },
    { //digit 3 (middle right), number 7
      0b000000,
      0b000100,
      0b000010,
      0b000010,
      0b000000,
      0b000000
    },
    { //digit 3 (middle right), number 8
      0b000100,
      0b000100,
      0b000110,
      0b000010,
      0b000100,
      0b000010
    },
    { //digit 3 (middle right), number 9
      0b000000,
      0b000100,
      0b000110,
      0b000010,
      0b000100,
      0b000010
    }
  },

  {
    { //digit 4 (far right), number 0
      0b000011,
      0b000010,
      0b000001,
      0b000001,
      0b000001,
      0b000000
    },
    { //digit 4 (far right), number 1
      0b000001,
      0b000000,
      0b000000,
      0b000001,
      0b000000,
      0b000000
    },
    { //digit 4 (far right), number 2
      0b000010,
      0b000011,
      0b000000,
      0b000001,
      0b000001,
      0b000000
    },
    { //digit 4 (far right), number 3
      0b000011,
      0b000001,
      0b000000,
      0b000001,
      0b000001,
      0b000000
    },
    { //digit 4 (far right), number 4
      0b000001,
      0b000011,
      0b000000,
      0b000001,
      0b000000,
      0b000000
    },
    { //digit 4 (far right), number 5
      0b000010,
      0b000011,
      0b000000,
      0b000001,
      0b000001,
      0b000000
    },
    { //digit 4 (far right), number 6
      0b000010,
      0b000011,
      0b000001,
      0b000001,
      0b000001,
      0b000000
    },
    { //digit 4 (far right), number 7
      0b000011,
      0b000000,
      0b000000,
      0b000001,
      0b000000,
      0b000000
    },
    { //digit 4 (far right), number 8
      0b000011,
      0b000011,
      0b000001,
      0b000001,
      0b000001,
      0b000000
    },
    { //digit 4 (far right), number 9
      0b000011,
      0b000011,
      0b000000,
      0b000001,
      0b000001,
      0b000000
    }
  }
};

const uint8_t rollingAnimationMap [12] PROGMEM = {    // upper bits L, lower bits C
  0x65, 0x56,  0x46, 0x16, 0x15, 0x45, 0x32, 0x21, 0x31, 0x41, 0x61, 0x23
};

const uint8_t linearAnimationMap [16] PROGMEM = {    // upper bits L, lower bits C
  0x31, 0x41, 0x52, 0x62, 0x53, 0x63, 0x12, 0x13, 0x54, 0x14, 0x35, 0x24, 0x25, 0x36, 0x16, 0x46
};


const uint8_t indexLEDbits [6] = {
  0b100000,
  0b010000,
  0b001000,
  0b000100,
  0b000010,
  0b000001
};

static const uint8_t displayPin [6] = {PIN_PA6, PIN_PB5, PIN_PB4, PIN_PA7, PIN_PA5, PIN_PA4};

void setup() {

  for (uint8_t pin = 0; pin < 6; pin ++)  {
    pinMode (displayPin [pin], INPUT);
  }

  pinMode (AW_NINT, INPUT_PULLUP);
  attachInterrupt (digitalPinToInterrupt (AW_NINT), touchInterrupt, FALLING);

  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  Wire.begin ();
  Wire.setClock (400000);

  Serial.begin (115200);

  if (!rx8111_init()) {
    RTCTime t;
    t.second = 0;
    t.minute = 0;
    t.hour = 12;
    t.day = 9;
    t.month = 6;
    t.year = 26;
    rx8111_setTime (t);
  }

  if (!aw_begin()) {
    Serial.println ("error: touch IC not initialized!");
    delay (4000);
  }

  delay (2000);

  /////// First init, just for testing ///////

  for (uint8_t i = 0; i < 9; i++) {
    displayNumber (i * 1111);
    for (uint8_t i = 0; i < 100; i++) {
      updateDisplay();
      delay (1);
    }
  }
  printTime ();
  displayTouchChanges ();
  delay  (100);

  ///////////////////////////////////////////

}

void loop() {

  if (touchEvent == 1) {   // swipe R -> L [1] // Display time for 3 sec
    touchEvent = 0;
    RTCTime t;
    rx8111_getTime (t);
    uint16_t displayableTime = t.hour * 100 + t.minute;
    for (uint8_t i = 0; i < 3; i++) {
      uint32_t previousTime = millis ();
      displayNumber (displayableTime);
      displayStateMap[3] = displayStateMap[3] | 0b010000; // add the dots
      displayStateMap[5] = displayStateMap[5] | 0b000100; // add the dots
      while ((millis() - previousTime) <= 500)  {
        updateDisplay ();
      }
      displayNumber (displayableTime);
      displayStateMap[3] = displayStateMap[3] & 0b101111;  // remove the dots
      displayStateMap[5] = displayStateMap[5] & 0b111011;  // remove the dots
      previousTime = millis ();
      while ((millis() - previousTime) <= 500)  {
        updateDisplay ();
      }
    }
  }

  if (touchEvent == 2) {  // swipe L -> R then swipe R -> L [2]   // Pomodoro Mode Settings
    touchEvent = 0;
    LEDAnimation (1600, linearAnimationMap, 16);
    PomTimer.minute = 25;
    uint16_t displayablePomTimer = PomTimer.hour * 100 + PomTimer.minute;
    uint32_t previousTime = millis ();
    while ((millis() - previousTime) <= 10000)  {
      // quit after 10s if the user is inactive

      displayNumber (displayablePomTimer);
      displayStateMap[3] = displayStateMap[3] | 0b010000; // add the dots
      displayStateMap[5] = displayStateMap[5] | 0b000100; // add the dots

      updateDisplay ();

      if (touchEvent == 7) {  // min -= 5
        touchEvent = 0;
        if (PomTimer.minute <= 0) {
          if (PomTimer.hour <= 0) {
            PomTimer.hour = 0;
            PomTimer.minute = 0;
          }
          else {
            PomTimer.hour -= 1;
            PomTimer.minute = 55;
          }
        }
        else {
          PomTimer.minute -= 5;
        }
        displayablePomTimer = PomTimer.hour * 100 + PomTimer.minute;
        previousTime = millis ();
      }

      if (touchEvent == 8) {  // min += 5
        touchEvent = 0;
        if (PomTimer.minute >= 55) {
          if (PomTimer.hour >= 24) {
            PomTimer.hour = 0;
          }
          else {
            PomTimer.hour += 1;
          }
          PomTimer.minute = 0;
        }
        else {
          PomTimer.minute += 5;
        }
        displayablePomTimer = PomTimer.hour * 100 + PomTimer.minute;
        previousTime = millis ();
      }

      if (touchEvent == 2) {      // Validated, duration set, next number of pomodoro sessions settings.
        touchEvent = 0;

        clearDisplay();

        uint32_t previousTime2 = millis ();
        while ((millis() - previousTime2) <= 10000)  {

          updateDisplay ();

          if (touchEvent == 7) {  // N --
            touchEvent = 0;
            if (N <= 1) {
              N = 1;
            }
            else {
              N -- ;
            }
            displayNumber (N);
            previousTime2 = millis ();
          }

          if (touchEvent == 8) {  // N ++
            touchEvent = 0;
            if (N >=  5) {
              N = 5;
            }
            else {
              N ++ ;
            }
            displayNumber (N);
            previousTime2 = millis ();
          }

          if (touchEvent == 2) {  // Validated, N number of sessions set, Quit setup
            touchEvent = 0;
            pomodoro = 1;
            goto END_POMODORO_SETUP;
          }
        }
      }

    }

    // millis quit, abort
    pomodoro = 0;

END_POMODORO_SETUP :
    rx8111_getTime (PomStart);
    LEDAnimation (1600, linearAnimationMap, 16);

  }

  if (touchEvent == 4) { // Hold CMD button for > 5 sec [4]
    touchEvent = 0;
    LEDAnimation (1200, rollingAnimationMap, 12);
    RTCTime t;
    rx8111_getTime (t);
    uint16_t displayableTime = t.hour * 100 + t.minute;
    uint32_t previousTime = millis ();
    while ((millis() - previousTime) <= 4000)  {
      // quit after 4s if the user is inactive

      displayNumber (displayableTime);
      displayStateMap[3] = displayStateMap[3] | 0b010000; // add the dots
      displayStateMap[5] = displayStateMap[5] | 0b000100; // add the dots

      updateDisplay ();

      if (touchEvent == 7) {
        touchEvent = 0;
        t.hour++;
        displayableTime = t.hour * 100 + t.minute;
        previousTime = millis ();   // resets the 4s timer
      }

      if (touchEvent == 8) {
        touchEvent = 0;
        t.minute++;
        displayableTime = t.hour * 100 + t.minute;
        previousTime = millis ();
      }
    }
    rx8111_setTime (t);

  }

  if (touchEvent == 3) { // double tap CMD button [3]
    touchEvent = 0;
SLEEP :
    sleep_cpu();

    // wakes up:
    uint32_t previousTime = millis();
    while (millis() - previousTime <= 6000) {
      if (touchEvent == 3) {
        touchEvent = 0;
        goto WAKING_UP;
      }

    }
    goto SLEEP;
  }
WAKING_UP :

  if (touchEvent == 6 && pomodoro == 1) { // swipe L -> R [6]
    RTCTime now;
    rx8111_getTime (now);

    uint16_t nowInt = now.minute * 60 + now.second;
    uint16_t PomStartInt = PomStart.minute * 60 + PomStart.second;

    int16_t diff = nowInt - PomStartInt;
    if (diff < 0) {
      diff = 0;
    }
    else if (diff > 5999) {  // 99 * 60 + 59 = 5999
      diff = 5999;
    }
    uint8_t diffSec = diff % 60;
    uint8_t diffMin = (diff - diffSec) / 60;

    uint16_t displayableDiff = diffMin * 100 + diffSec;
    for (uint8_t i = 0; i < 3; i++) {
      uint32_t previousTime = millis ();
      displayNumber (displayableDiff);
      displayStateMap[3] = displayStateMap[3] | 0b010000; // add the dots
      displayStateMap[5] = displayStateMap[5] | 0b000100; // add the dots
      while ((millis() - previousTime) <= 500)  {
        updateDisplay ();
      }
      clearDisplay();
      displayNumber (displayableDiff);
      displayStateMap[3] = displayStateMap[3] & 0b101111;  // remove the dots
      displayStateMap[5] = displayStateMap[5] & 0b111011;  // remove the dots
      previousTime = millis ();
      while ((millis() - previousTime) <= 500)  {
        updateDisplay ();
      }
    }

    
  }

  if (pomodoro == 1) {
    // checks if pomodoro time is elapsed
    // I will implement the pomodoro elapsed time later on
  }

}

void printTime () {
  RTCTime t;
  rx8111_getTime (t);

  Serial.print ("The time is :   ");
  Serial.print (t.hour);
  Serial.print (" : ");
  Serial.print (t.minute);
  Serial.print (" : ");
  Serial.println (t.second);

}

void displayTouchChanges () {
  aw_update ();
  for (uint8_t i = 0; i < 5; i++) {
    stateTouch [i] = aw_channel (i);
  }

  for (uint8_t i = 0; i < 5; i++) {
    if (stateTouch [i] != prevStateTouch [i]) {

      Serial.print ("CH");
      Serial.print (i);
      Serial.print ("  state  ");
      Serial.println (stateTouch[i]);

      delay (1);
    }
    prevStateTouch [i] = stateTouch [i];
  }

}

void updateDisplay () {
  for (uint8_t l = 0; l < 6; l++) {
    pinMode (displayPin [l], OUTPUT);
    analogWrite (displayPin [l], brightness);
    for (uint8_t c = 0; c < 6; c++) {
      if (l != c) {
        if (((displayStateMap [l] >> c) & 0x01) == 1) digitalWrite (displayPin[c], LOW);
        // LOW if the correct bit in the array (row l, col c) is 1
      }
    }
    delay (FrameDELAY);

    for (uint8_t c = 0; c < 6; c++) {
      pinMode (displayPin[c], INPUT);
    }
  }
}

void displayNumber (uint16_t num) {     // ex 1023
  if (num > 9999) num = 9999;
  uint8_t u = num % 10;  // 3
  uint16_t rm = num / 10; // 102
  uint8_t d = rm % 10;   // 2
  rm = rm / 10;          // 10
  uint8_t c = rm % 10;   // 0
  uint8_t m = rm / 10;   // 1

  for (uint8_t b = 0; b < 6; b++) {
    displayStateMap [b] = digitsStateMap[0][m][b] | digitsStateMap[1][c][b] | digitsStateMap[2][d][b] | digitsStateMap[3][u][b];
    // merges all the digits into one displayable array
  }
}

void LEDAnimation (uint16_t duration, const uint8_t *animationMap, uint8_t lengthByte) {
  for (uint8_t i = 0; i < lengthByte; i++) {
    for (uint8_t j = 0; j < 6; j++) {
      displayStateMap [j] = 0;
    }
    uint8_t animationByte = pgm_read_byte (&animationMap[i]);
    displayStateMap [(animationByte & 0xf0) >> 4] = indexLEDbits[animationByte & 0x0f];
    uint32_t previousTime = millis ();
    while ((millis() - previousTime) <= (duration / lengthByte))  {
      updateDisplay ();
    }
  }

}

void clearDisplay () {
  for (uint8_t l = 0; l < 6; l++) {
    displayStateMap[l] = 0;
  }
}
