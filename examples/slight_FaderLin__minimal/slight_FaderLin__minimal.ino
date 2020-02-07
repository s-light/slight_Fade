/******************************************

    slight_FaderLin__minimal
        minimal example for slight_Fade lib usage.
        debugport: serial interface 115200baud

    hardware:
        Arduino board of any type.
        D9 --> LED connected to R to VCC

    libraries used:
        ~ slight_Fade

    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************/
/******************************************
The MIT License (MIT)

Copyright (c) 2020 Stefan Krüger

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


#include <slight_FaderLin.h>


// ------------------------------------------
// slight_Fade things

byte bPin_LED = 9;

void myFaderOne_callback_OutputChanged(byte bID, uint16_t *wValues, byte bCount) {
    //invert output value..
    analogWrite(bPin_LED, 255 - wValues[0]);
}

void myCallback_onEvent(slight_FaderLin *pInstance, byte bEvent) {
    void;
}



const uint8_t myFaderOne__channel_count = 2;
slight_FaderLin myFaderOne(
    0, // byte cbID_New
    myFaderOne__channel_count, // byte cbChannelCount_New
    myFaderOne_callback_OutputChanged, // tCbfuncValuesChanged cbfuncValuesChanged_New
    myCallback_onEvent // tCbfuncStateChanged cbfuncStateChanged_New
);


// ------------------------------------------
// setup
// ------------------------------------------
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
    Serial.println(F("slight_FaderLin__minimal.ino sketch."));
    Serial.println(F("minimal example for library usage."));

    pinMode(bPin_LED, OUTPUT);
    digitalWrite(bPin_LED, HIGH);

    // ------------------------------------------
    // start myFaderOne
    Serial.println(F("setup slight_FaderLin:")); {
        Serial.println(F("  myFaderOne.begin();"));
        myFaderOne.begin();
    }
    Serial.println(F("  finished."));

    // ------------------------------------------
    Serial.println(F("Loop:"));
}


// ------------------------------------------
// main loop
// ------------------------------------------
void loop() {
    myFaderOne.update();
    // nothing else to do here...
}


// ------------------------------------------
// THE END :-)
// ------------------------------------------
