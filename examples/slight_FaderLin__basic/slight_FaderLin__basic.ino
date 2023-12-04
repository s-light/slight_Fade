/******************************************

    slight_FaderLin__basic
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

byte led_1_pin = LED_BUILTIN;
byte led_2_pin = 9;


void myCallback_ValuesChanged(slight_FaderLin *pInstance,
                                       uint16_t *values, byte count) {
    uint8_t pin = pInstance->getID();
    // uint8_t pin = (*instance).getID();
    analogWrite(pin, values[0]);
}

void myCallback_onEvent(slight_FaderLin *instance) {
    // react on events:
    switch (instance->getEventLast()) {
    case slight_FaderLin::event_fading_Finished: {
        Serial.print(F("led on pin "));
        Serial.print((*instance).getID());
        Serial.print(F(": fading Finished."));
        Serial.println();
        } break;
        } // end switch
}

slight_FaderLin myFader1 =
    slight_FaderLin(led_1_pin,                // byte kID
                    1,                        // byte channelCount
                    myCallback_ValuesChanged, // tCallbackFunctionValuesChanged
                    myCallback_onEvent        // tCallbackFunction

    );
slight_FaderLin myFader2(
    led_2_pin,                         // byte kID
    1,                                 // byte channelCount
    myCallback_ValuesChanged, // tCallbackFunctionValuesChanged
    myCallback_onEvent                 // tCallbackFunction
);

void handle_serial_input() {
    if (Serial.available() > 0) {

        char input = Serial.read();

        // check what to do:
        switch (input) {
            case '1': {
                Serial.println("start fade 1 on");
                uint32_t duration = 2000; //ms
                uint16_t target_value = 255;
                myFader1.startFadeTo(duration, target_value);
            } break;
            case 'A': {
                Serial.println("start fade 1 off");
                uint32_t duration = 1000; //ms
                uint16_t target_value = 1;
                myFader1.startFadeTo(duration, target_value);
            } break;
            case 'a': {
                Serial.println("start fade 1 half");
                uint32_t duration = 1000; //ms
                uint16_t target_value = 100;
                myFader1.startFadeTo(duration, target_value);
            } break;
            case '2': {
                Serial.println("start fade 2 on");
                uint32_t duration = 4000; //ms
                uint16_t target_value = 150;
                myFader2.startFadeTo(duration, target_value);
            } break;
            case 'B': {
                Serial.println("start fade 2 off");
                uint32_t duration = 1000; //ms
                uint16_t target_value = 0;
                myFader2.startFadeTo(duration, target_value);
            } break;
            default: {
                Serial.println("type 1, A, 2, B to start one of the fades.");
            }
        }

        // clear input buffer
        while (Serial.available()) {
            Serial.read();
        }

    }
}

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
    Serial.println(F("slight_FaderLin__basic.ino sketch."));
    Serial.println(F("minimal example for library usage."));

    pinMode(led_1_pin, OUTPUT);
    digitalWrite(led_1_pin, HIGH);
    pinMode(led_2_pin, OUTPUT);
    digitalWrite(led_2_pin, HIGH);

    // ------------------------------------------
    // start myFaderOne
    Serial.println(F("setup slight_FaderLin:")); {
        Serial.println(F("  myFader.begin();"));
        myFader1.begin();
        myFader2.begin();
    }
    Serial.println(F("  finished."));

    // ------------------------------------------
    Serial.println(F("Loop:"));
}


// ------------------------------------------
// main loop
// ------------------------------------------
void loop() {
    myFader1.update();
    myFader2.update();
    handle_serial_input();
}


// ------------------------------------------
// THE END :-)
// ------------------------------------------
