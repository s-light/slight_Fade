/******************************************

    slight_FaderLin__advanced.ino
        minimal example for slight_Fader lib usage.
        debugport: serial interface 115200baud

    hardware:
        Arduino board of any typ
        LED_BUILTIN

    libraries used:
        ~ slight_Fader

    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************/
/******************************************

The MIT License (MIT)

Copyright (c) 2023 Stefan Krüger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
https://opensource.org/licenses/mit-license.php
******************************************/

#include "mysun.h"

MySun mysun = MySun(Serial);

uint32_t last_action = millis();
uint32_t update_last_action = millis();

void setup() {
    // ------------------------------------------
    // init serial
    // wait for arduino IDE to release all serial ports after upload.
    delay(1000);
    Serial.begin(115200);
    delay(500);
    Serial.println();

    // ------------------------------------------
    // print short welcome text
    Serial.println(F("slight_FaderLin__advanced.ino"));
    Serial.println(F("example for library usage."));

    // ------------------------------------------
    mysun.begin();

    mysun.myFader.setDurationDefault(5 * 1000);

    // ------------------------------------------
    Serial.println(F("Loop:"));
}



// ------------------------------------------
void loop() {
    mysun.update();

    if (millis() > last_action + 999) {
        last_action = millis();
        Serial.println(millis());
    }
}

// ------------------------------------------
// THE END :-)
// ------------------------------------------
