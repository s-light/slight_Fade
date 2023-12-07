/******************************************

    slight_FaderLin__advanced2.ino
        minimal example for slight_Fader lib usage.
        debugport: serial interface 115200baud

    hardware:
        Arduino board of any typ.
        A3 --> Pushbutton closing to GND
        A4 --> Pushbutton closing to GND
        A5 --> Pushbutton closing to GND

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

#include "myinput.h"

// MyInput myinput = MyInput(Serial);
MyInput myinput = MyInput();

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
    Serial.println(F("slight_FaderLin__advanced2.ino"));
    Serial.println(F("example for library usage."));

    // ------------------------------------------
    myinput.begin(Serial);

    myinput.myFader.setDurationDefault(5* 1000);
    
    // ------------------------------------------
    Serial.println(F("Loop:"));
}


// ------------------------------------------
// main loop
// ------------------------------------------
void loop() {
    myinput.update();

    Serial.println();
    Serial.println("run tests:");
    // just do a bunch of test fades
    Serial.println("fadeTo 1");
    myinput.myFader.fadeTo(1.0, 5*1000);
    myinput.wait_with_update(10 * 1000);
    
    Serial.println("fadeTo 0 in 2s");
    myinput.myFader.fadeTo(0, 2 * 1000);
    myinput.wait_with_update(10 * 1000);

    // Serial.println("fadeUp");
    // myinput.myFader.fadeUp();
    // myinput.wait_with_update(6 * 1000);
    // Serial.println("fadePause");
    // myinput.myFader.fadePause();
    // myinput.wait_with_update(2 * 1000);
    // Serial.println("fadeDown");
    // myinput.myFader.fadeDown();
    // myinput.wait_with_update(3 * 1000);
    // Serial.println("fadePause");
    // myinput.myFader.fadePause();

    Serial.println("done.");
    Serial.println();
    Serial.println();
    Serial.println();
    delay(10 * 1000);
}


// ------------------------------------------
// THE END :-)
// ------------------------------------------
